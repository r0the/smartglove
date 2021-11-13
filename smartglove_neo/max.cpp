/*
 * Copyright (C) 2021 by Stefan Rothe
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

#include "max.h"
#include "config.h"
#include "behaviour.h"

#define FIRMATA_BAUD_RATE 57600

#define START_SYSEX 0xF0
#define END_SYSEX 0xF7

#define REPORT_FIRMWARE 0x79 

#define RECEIVED_NOTHING 0
#define RECEIVED_HEADER  1
#define RECEIVED_TYPE    2

const uint8_t DIGITAL_PIN_COUNT = 16;
const bool DIGITAL_PIN_BUTTON[DIGITAL_PIN_COUNT] = {
    true, true, true, true, true, true, true, true,
    false, false, false, false, true, true, true, true
};

const uint8_t DIGITAL_PIN_MAP[DIGITAL_PIN_COUNT] = {
    BUTTON_THUMB_1,
    BUTTON_THUMB_2,
    BUTTON_THUMB_3,
    BUTTON_THUMB_4,
    BUTTON_INDEX_FINGER_1,
    BUTTON_MIDDLE_FINGER_1,
    BUTTON_RING_FINGER_1,
    BUTTON_LITTLE_FINGER_1,
    GESTURE_WAVE_LEFT,
    GESTURE_WAVE_RIGHT,
    GESTURE_WAVE_UP,
    GESTURE_WAVE_DOWN,
    BUTTON_INDEX_FINGER_2,
    BUTTON_MIDDLE_FINGER_2,
    BUTTON_RING_FINGER_2,
    BUTTON_LITTLE_FINGER_2
};


const uint8_t ANALOG_PIN_COUNT = 11;
const uint8_t ANALOG_PIN_MAP[ANALOG_PIN_COUNT] = {
    SENSOR_DISTANCE,
    SENSOR_ACCEL_X,
    SENSOR_ACCEL_Y,
    SENSOR_ACCEL_Z,
    SENSOR_GYRO_PITCH,
    SENSOR_GYRO_ROLL,
    SENSOR_GYRO_HEADING,
    SENSOR_FLEX_INDEX_FINGER,
    SENSOR_FLEX_MIDDLE_FINGER,
    SENSOR_FLEX_RING_FINGER,
    SENSOR_FLEX_LITTLE_FINGER
};

Max::Max(SmartDevice& device) :
    Behaviour(device),
    _receiveState(RECEIVED_NOTHING),
    _serialConnected(false),
    _serialCheckMs(0),
    _state(0)
{
}

void Max::setup() {
    device.display().setTextAlign(ALIGN_LEFT);
    device.display().setFont(&HELVETICA_10);
}

void Max::loop() {
    if (device.commandMenu()) {
        device.popBehaviour();
        device.pushBehaviour(new MainMenu(device));
    }

    if (device.commandCalibrateIMU()) {
        device.resetIMU();
    }

    unsigned long now = millis();
    if (_serialCheckMs < now) {
        // Checking the Serial connection takes a long time, so it shouldn't be
        // done too often.
        _serialConnected = static_cast<bool>(Serial);
        if (!_serialConnected) {
            Serial.begin(FIRMATA_BAUD_RATE);
            // sendInformation();
        }

        _serialCheckMs = now + SERIAL_CHECK_INTERVAL_MS;
    }

    if (!_serialConnected) {
        device.display().setTextAlign(ALIGN_LEFT);
        device.display().setFont(&HELVETICA_10);
        device.display().drawText(10, 8, "Waiting for");
        device.display().drawText(10, 22, "Max connection...");
        return;
    }

    char text[10];
    sprintf(text, "%i", _state);
    device.display().setFont(&SWISS_20_B);
    device.display().setTextAlign(ALIGN_CENTER);
    device.display().drawText(64, 12, text);
    receive();
    sendDigital();
    sendAnalog();
}

void Max::receive() {
    switch (_receiveState) {
    case RECEIVED_NOTHING:
        if (Serial.available() && Serial.read() == 'S') {
            _receiveState = RECEIVED_HEADER;
        }
        break;
    case RECEIVED_HEADER:
        if (Serial.available() && Serial.read() == 'S') {
            _receiveState = RECEIVED_TYPE;
        }
        else {
            _receiveState = RECEIVED_NOTHING;
        }
        break;
    case RECEIVED_TYPE:
        if (Serial.available() >= 3) {
            receiveState();
            _receiveState = RECEIVED_NOTHING;
        }
        break;
    }
}

void Max::receiveState() {
    Serial.read(); // length
    _state = Serial.read();
    Serial.read(); // 'E'
}

void Max::sendAnalog() {
    sendByte('S');
    sendByte('A');
    sendByte(4 + 2 * ANALOG_PIN_COUNT);
    for (uint8_t i = 0; i < ANALOG_PIN_COUNT; ++i) {
        sendSensor(ANALOG_PIN_MAP[i]);
    }
    sendByte('E');
}

void Max::sendDigital() {
    sendByte('S');
    sendByte('D');
    sendByte(4 + DIGITAL_PIN_COUNT);
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (DIGITAL_PIN_BUTTON[i]) {
            sendButton(DIGITAL_PIN_MAP[i]);
        }
        else {
            sendGesture(DIGITAL_PIN_MAP[i]);
        }
    }
    sendByte('E');
}

void Max::sendInformation() {
    sendByte('S');
    sendByte('I');
    sendByte(10);
    sendByte('G');
    sendByte(VERSION_MAJOR);
    sendByte(VERSION_MINOR);
    sendByte(0);
    sendByte(0);
    sendByte(0);
    sendByte('E');
}

void Max::sendSensor(uint8_t id) {
    uint16_t value = device.sensorValue(id);
    Serial.write(value >> 8);
    Serial.write(value & 0xFF);
}

void Max::sendButton(uint8_t button) {
    Serial.write(device.buttonPressed(button) ? 1 : 0);
}

void Max::sendGesture(uint8_t gesture) {
    Serial.write(device.gestureDetected(gesture) ? 1 : 0);
}

void Max::sendByte(uint8_t data) {
    Serial.write(data);
}
