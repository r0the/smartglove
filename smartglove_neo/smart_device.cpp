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
 * class Buttons
 *****************************************************************************/

Buttons::Buttons() :
    _current(0),
    _last(0),
    _longPress(false),
    _longPressButtons(0),
    _longPressMillis(5000) {
}

bool Buttons::available(uint16_t button) const {
    return (_available & button) == button;
}

bool Buttons::down(uint16_t button) const {
    return pressed(button) && ((_last & button) == 0);
}

bool Buttons::pressed(uint16_t button) const {
    return (_current & button) == button;
}

void Buttons::setAvailable(uint16_t available) {
    _available = available;
}

void Buttons::setLongPress(uint16_t buttons, uint16_t millis) {
    _longPressButtons = buttons;
    _longPressMillis = millis;
}

void Buttons::updateState(uint16_t current) {
    unsigned long now = millis();
    _last = _current;
    _current = _available & current;
    if ((_current & _longPressButtons) != _longPressButtons) {
        _longPressEnd = now + _longPressMillis;
    }

    if (_longPressEnd <= now) {
        _longPress = true;
        _longPressEnd = now + _longPressMillis;
    }
    else {
        _longPress = false;
    }
}

/******************************************************************************
 * class Behaviour
 *****************************************************************************/

Behaviour::Behaviour() {
}

/******************************************************************************
 * class SmartDevice
 *****************************************************************************/

SmartDevice::SmartDevice() :
    _behaviour(new BehaviourPtr[BEHAVIOUR_STACK_SIZE]),
    _behaviourIndex(0),
    _buttons(),
    _display(I2C_DISPLAY_ADDRESS),
    _imu(),
    _infoLed(),
    _sensors() {
}

bool SmartDevice::commandEnter() const {
    return buttonDown(BUTTON_THUMB_1);
}

bool SmartDevice::commandNext() const {
    return buttonDown(BUTTON_MIDDLE_FINGER_1);
}

bool SmartDevice::commandPrev() const {
    return buttonDown(BUTTON_INDEX_FINGER_1);
}

bool SmartDevice::imuReady() const {
    return _imu.status() == IMU::Ready;
}

void SmartDevice::setup() {
    Wire.begin();
    // initialize buttons
    _buttons.setAvailable(availableButtons());
    _buttons.setLongPress(longPressButtons(), LONG_PRESS_MS);

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

    setSensorRawRange(SENSOR_GYRO_ROLL, 180.0, -180.0);
    setSensorRawRange(SENSOR_GYRO_PITCH, 90.0, -90.0);
    setSensorRawRange(SENSOR_GYRO_HEADING, 180.0, -180.0);

    // initialize IMU
    _imu.setup();

    waitForFlash();
//    Serial.begin(9600);
//    while (!Serial) { delay(1); }
    setBehaviour(new MainMenu);
    doSetup();
}

void SmartDevice::loop() {
    unsigned long now = millis();
    _infoLed.loop();
    setInfoLed(_infoLed.on());
    // update buttons
    _buttons.updateState(readButtonState());
    if (_buttons.longPress()) {
        setBehaviour(new MainMenu);
    }

    // update sensor values from IMU
    _imu.loop();
    _sensors.addMeasurement(SENSOR_GYRO_HEADING, _imu.heading());
    _sensors.addMeasurement(SENSOR_GYRO_PITCH, _imu.pitch());
    _sensors.addMeasurement(SENSOR_GYRO_ROLL, _imu.roll());

    doLoop();
    _display.clear();
    if (_behaviourIndex > 0) {
        _behaviour[_behaviourIndex - 1]->loop(*this);
    }

    _display.update();
}

void SmartDevice::popBehaviour() {
    if (_behaviourIndex > 0) {
        delete _behaviour[_behaviourIndex - 1];
        _behaviour[_behaviourIndex - 1] = NULL;
        --_behaviourIndex;
        _behaviour[_behaviourIndex - 1]->setup(*this);
    }
}

void SmartDevice::pushBehaviour(Behaviour* behaviour) {
    if (_behaviourIndex < BEHAVIOUR_STACK_SIZE && behaviour != NULL) {
        ++_behaviourIndex;
        _behaviour[_behaviourIndex - 1] = behaviour;
        _behaviour[_behaviourIndex - 1]->setup(*this);
    }
}

void SmartDevice::resetIMU() {
    _imu.setup();
}

void SmartDevice::setBehaviour(Behaviour* behaviour) {
    while (_behaviourIndex > 0) {
        popBehaviour();
    }

    pushBehaviour(behaviour);
}


void SmartDevice::setSensorOutRange(uint8_t index, uint16_t min, uint16_t max) {
    _sensors.setOutRange(index, min, max);
}

void SmartDevice::setSensorRawRange(uint8_t index, double min, double max) {
    _sensors.setRawRange(index, min, max);
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

