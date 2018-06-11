/*
 * Copyright (C) 2018 by Stefan Rothe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Wire.h>
#include "smart_device.h"
#include "behaviour.h"
#include "config.h"
#include "storage.h"

/******************************************************************************
 * class LED
 *****************************************************************************/

LED::LED() :
    _blinkMillis(0),
    _on(false),
    _timeout(0) {
}

void LED::setMode(Mode mode) {
    switch (mode) {
    case Off:
        _blinkMillis = 0;
        _on = false;
        break;
    case On:
        _blinkMillis = 0;
        _on = true;
        break;
    case BlinkSlow:
        _blinkMillis = LED_BLINK_SLOW_MS;
        _on = true;
        _timeout = millis() + _blinkMillis;
        break;
    case BlinkFast:
        _blinkMillis = LED_BLINK_FAST_MS;
        _on = true;
        _timeout = millis() + _blinkMillis;
        break;
    }
}

void LED::loop() {
    unsigned long now = millis();
    if (_blinkMillis > 0 && now > _timeout) {
       _on = !_on;
        _timeout = now + _blinkMillis;
    }
}

/******************************************************************************
 * class Behaviour
 *****************************************************************************/

Behaviour::Behaviour(SmartDevice& device) :
    device(device) {
}

/******************************************************************************
 * class BehaviourStack
 *****************************************************************************/

BehaviourStack::BehaviourStack(SmartDevice& device, uint8_t capacity) :
    _behaviour(new BehaviourPtr[capacity]),
    _capacity(capacity),
    _index(0),
    _nextIndex(0) {
    _behaviour[_index] = new InitBehaviour(device);
}

BehaviourStack::~BehaviourStack() {
    delete[] _behaviour;
}

void BehaviourStack::loop() {
    _behaviour[_index]->loop();
    bool changed = false;
    while (_index > _nextIndex) {
        delete _behaviour[_index];
        --_index;
        changed = true;
    }

    if (_index < _nextIndex) {
        _index = _nextIndex;
        changed = true;
    }

    if (changed) {
        _behaviour[_index]->setup();
    }
}

void BehaviourStack::pop() {
    if (_nextIndex > 0) {
        --_nextIndex;
    }
}

void BehaviourStack::push(Behaviour* behaviour) {
    if (_nextIndex < _capacity - 1) {
        ++_nextIndex;
        _behaviour[_nextIndex] = behaviour;
    }
}

/******************************************************************************
 * class SmartDevice
 *****************************************************************************/

SmartDevice::SmartDevice() :
    _behaviour(*this, BEHAVIOUR_STACK_CAPACITY),
    _buttons(),
    _display(I2C_DISPLAY_ADDRESS),
    _imu(),
    _infoLed(),
    _sensors() {
}

bool SmartDevice::buttonCombination(uint8_t id1, uint8_t id2) const {
    return (buttonDown(id1) && buttonPressed(id2)) ||
           (buttonDown(id2) && buttonPressed(id1));
}

bool SmartDevice::commandDown() const {
    return buttonDown(BUTTON_MIDDLE_FINGER_1);
}

bool SmartDevice::commandEnter() const {
    return buttonDown(BUTTON_THUMB_1);
}

bool SmartDevice::commandUp() const {
    return buttonDown(BUTTON_INDEX_FINGER_1);
}

void SmartDevice::configureSensor(uint8_t index, double min, double max, double minStdDev) {
    _sensors.configure(index, min, max, minStdDev);
}

bool SmartDevice::imuReady() const {
    return _imu.status() == IMU::Ready;
}

void SmartDevice::setup() {
    Wire.begin();
    // initialize buttons
    _buttons.setAvailable(availableButtonMask());
    _sensors.setAvailable(availableSensorMask());

    // initialize display
    _display.begin();
    if (!_display.ready()) {
        _infoLed.setMode(LED::BlinkFast);
    }
    else {
        _infoLed.setMode(LED::Off);
        _display.setFont(&HELVETICA_18);
    }

    setInfoLed(_infoLed.on());

    _showFramerate = Storage.readByte(STORAGE_SHOW_FRAMERATE);

    configureSensor(SENSOR_ACCEL_X, -10.0, 10.0, 0.2);
    configureSensor(SENSOR_ACCEL_Y, 10.0, -10.0, 0.2);
    configureSensor(SENSOR_ACCEL_Z, 10.0, -10.0, 0.2);
    configureSensor(SENSOR_GYRO_ROLL, 180.0, -180.0, 2.0);
    configureSensor(SENSOR_GYRO_PITCH, 90.0, -90.0, 1.0);
    configureSensor(SENSOR_GYRO_HEADING, 180.0, -180.0, 2.0);

    // initialize IMU
    _imu.setup();

//    waitForFlash();
    doSetup();
}

unsigned long last;

void SmartDevice::loop() {
    _infoLed.loop();
    setInfoLed(_infoLed.on());
    // update buttons
    _buttons.updateState(readButtonState());
    // update sensor values from IMU
    _imu.loop();
    _sensors.addMeasurement(SENSOR_ACCEL_X, _imu.ax());
    _sensors.addMeasurement(SENSOR_ACCEL_Y, _imu.ay());
    _sensors.addMeasurement(SENSOR_ACCEL_Z, _imu.az());
    _sensors.addMeasurement(SENSOR_GYRO_HEADING, _imu.heading());
    _sensors.addMeasurement(SENSOR_GYRO_PITCH, _imu.pitch());
    _sensors.addMeasurement(SENSOR_GYRO_ROLL, _imu.roll());

    doLoop();
    unsigned long now = millis();
    _display.clear();
    _behaviour.loop();
    if (_showFramerate) {
        unsigned long now = millis();
        char text[6];
        sprintf(text, "%i", 1000 / (now - _lastMs));
        _lastMs = now;
        Font* oldFont = _display.font();
        _display.setFont(&HELVETICA_8);
        _display.setTextAlign(ALIGN_LEFT);
        _display.drawText(115, 7, text);
        _display.setFont(oldFont);
    }
    _display.updatePage();
    last = now;
}

void SmartDevice::popBehaviour() {
    _behaviour.pop();
}

void SmartDevice::pushBehaviour(Behaviour* behaviour) {
    _behaviour.push(behaviour);
}

void SmartDevice::resetIMU() {
    _imu.setup();
}

void SmartDevice::setShowFramerate(bool showFramerate) {
    if (_showFramerate != showFramerate) {
        _showFramerate = showFramerate;
        Storage.writeByte(STORAGE_SHOW_FRAMERATE, showFramerate);
    }
}

void SmartDevice::waitForFlash() {
    unsigned long end = millis() + 10000;
    signed long wait;
    char text[20];
    do {
        wait = end - millis();
        if (_display.ready()) {
            sprintf(text, "Wait %i", (wait / 1000));
            _display.clear();
            _display.setFont(&HELVETICA_10);
            _display.setTextAlign(ALIGN_LEFT);
            _display.drawText(10, 10, text);
            _display.update();
        }
        else {

        }

        delay(10);
    }
    while (wait > 0);
}

