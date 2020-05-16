/*
 * Copyright (C) 2015 - 2020 by Stefan Rothe
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

#include "junxion.h"
#include "config.h"
#include "behaviour.h"
#include "storage.h"

#define ANALOG 'a'
#define DIGITAL 'd'
#define DIGITAL_RESOLUTION 1

#define HEADER 0xFF
#define START_DATA 'D'
#define STOP_DATA 'S'
#define DATA_RESPONSE 'd'
#define BOARD_ID_REQUEST 'B'
#define BOARD_ID_RESPONSE 'b'
#define JUNXION_ID_REQUEST 'J'
#define JUNXION_ID_RESPONSE 'j'
#define INPUT_CONFIG_REQUEST 'I'
#define INPUT_CONFIG_RESPONSE 'p'
#define JUNXION_ID 308

#define JUNXION_BAUD_RATE 115200
#define JUNXION_DIGITAL_INPUT_COUNT 10
#define JUNXION_ANALOG_INPUT_COUNT 12

#define MIN_STATE 1
#define MAX_STATE 15

/******************************************************************************
 * analog and own pin mapping
 *****************************************************************************/

const uint8_t ANALOG_PIN_COUNT = 4;

const uint8_t ANALOG_PIN_MAP[ANALOG_PIN_COUNT] = {
    SENSOR_FLEX_INDEX_FINGER,
    SENSOR_FLEX_MIDDLE_FINGER,
    SENSOR_FLEX_RING_FINGER,
    SENSOR_FLEX_LITTLE_FINGER
};

const uint8_t OWN_PIN_COUNT = 8;

#define JUNXION_STATE 255

const uint8_t OWN_PIN_MAP[OWN_PIN_COUNT] = {
    SENSOR_DISTANCE,
    SENSOR_ACCEL_X,
    SENSOR_ACCEL_Y,
    SENSOR_ACCEL_Z,
    SENSOR_GYRO_PITCH,
    SENSOR_GYRO_ROLL,
    SENSOR_GYRO_HEADING,
    JUNXION_STATE
};

/******************************************************************************
 * digital pin mapping
 *****************************************************************************/

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

// ----------------------------------------------------------------------------
// class Junxion
// ----------------------------------------------------------------------------

Junxion::Junxion(SmartDevice& device) :
    Behaviour(device),
    _boardId(1),
    _dataSize(0),
    _headerReceived(false),
    _sendData(false),
    _serialAvailable(false),
    _serialCheckMs(0),
    _state(MIN_STATE)
{
}

void Junxion::setup() {
    _dataSize = 2 * (analogPinCount() + ownPinCount() + (digitalPinCount() / 16) + 1);
    _boardId = Storage.readByte(STORAGE_BOARD_ID);
}

void Junxion::loop() {
    if (device.commandMenu()) {
        device.pushBehaviour(new MainMenu(device));
    }

    if (device.commandCalibrateIMU()) {
        device.resetIMU();
    }

    unsigned long now = millis();
    if (_serialCheckMs < now) {
        // Checking the Serial connection takes a long time, so it shouldn't be
        // done too often.
        _serialAvailable = static_cast<bool>(Serial);
        if (!_serialAvailable) {
            Serial.begin(JUNXION_BAUD_RATE);
            sendJunxionId();
            sendInputConfig();
        }

        _serialCheckMs = now + SERIAL_CHECK_INTERVAL_MS;
    }

    if (!_serialAvailable) {
        device.display().setTextAlign(ALIGN_LEFT);
        device.display().setFont(&HELVETICA_10);
        device.display().drawText(10, 8, "Waiting for");
        device.display().drawText(10, 22, "serial connection...");
        return;
    }

    if (!_headerReceived && Serial.available() > 2) {
        if (Serial.read() == HEADER) {
            if (Serial.read() == HEADER) {
                _headerReceived = true;
                _packageSize = Serial.read();
            }
        }
    }

    if (_headerReceived && Serial.available() > _packageSize) {
        handleCommand(Serial.read());
        _headerReceived = false;
    }

    if (_sendData) {
        sendData();
    }

    if (device.commandUp()) {
        if (_state < MAX_STATE) {
            ++_state;
        }
        else {
            _state = MIN_STATE;
        }
    }

    if (device.commandDown()) {
        if (_state > MIN_STATE) {
            --_state;
        }
        else {
            _state = MAX_STATE;
        }
    }

    char text[10];
    sprintf(text, "%i", _state);
    device.display().setFont(&SWISS_20_B);
    device.display().setTextAlign(ALIGN_CENTER);
    device.display().drawText(64, 12, text);
}

bool Junxion::analogPinAvailable(uint8_t pin) const {
    if (pin >= ANALOG_PIN_COUNT) {
        return false;
    }

    return device.sensorAvailable(ANALOG_PIN_MAP[pin]);
}

uint8_t Junxion::analogPinCount() const {
    uint8_t result = 0;
    for (uint8_t id = 0; id < ANALOG_PIN_COUNT; ++id) {
        if (analogPinAvailable(id)) {
            ++result;
        }
    }

    return result;
}

uint16_t Junxion::analogPinValue(uint8_t pin) const {
    if (pin >= ANALOG_PIN_COUNT) {
        return 0;
    }

    return device.sensorValue(ANALOG_PIN_MAP[pin]);
}

bool Junxion::digitalPinActive(uint8_t pin) const {
    if (pin >= DIGITAL_PIN_COUNT) {
        return false;
    }

    uint8_t id = DIGITAL_PIN_MAP[pin];
    if (DIGITAL_PIN_BUTTON[pin]) {
        return device.buttonAvailable(id) && device.buttonPressed(id);
    }
    else {
        return device.gestureAvailable(id) && device.gestureDetected(id);
    }
}

bool Junxion::digitalPinAvailable(uint8_t pin) const {
    if (pin >= DIGITAL_PIN_COUNT) {
        return false;
    }

    uint8_t id = DIGITAL_PIN_MAP[pin];
    if (DIGITAL_PIN_BUTTON[pin]) {
        return device.buttonAvailable(id);
    }
    else {
        return device.gestureAvailable(id);
    }
}

uint8_t Junxion::digitalPinCount() const {
    uint8_t result = 0;
    for (uint8_t id = 0; id < DIGITAL_PIN_COUNT; ++id) {
        if (digitalPinAvailable(id)) {
            ++result;
        }
    }

    return result;
}

bool Junxion::ownPinAvailable(uint8_t pin) const {
    if (pin >= OWN_PIN_COUNT) {
        return false;
    }

    if (OWN_PIN_MAP[pin] == JUNXION_STATE) {
        return true;
    }

    return device.sensorAvailable(OWN_PIN_MAP[pin]);
}

uint8_t Junxion::ownPinCount() const {
    uint8_t result = 0;
    for (uint8_t id = 0; id < OWN_PIN_COUNT; ++id) {
        if (ownPinAvailable(id)) {
            ++result;
        }
    }
    return result;
}

uint16_t Junxion::ownPinValue(uint8_t pin) const {
    if (pin >= OWN_PIN_COUNT) {
        return 0;
    }

    if (OWN_PIN_MAP[pin] == JUNXION_STATE) {
        return _state * 65536 / 127;
    }

    return device.sensorValue(OWN_PIN_MAP[pin]);
}

void Junxion::handleCommand(char cmd) {
    switch (cmd) {
        case START_DATA:
            _sendData = true;
            break;
        case STOP_DATA:
            _sendData = false;
            break;
        case BOARD_ID_REQUEST:
            sendHeader(BOARD_ID_RESPONSE, 1);
            Serial.write(_boardId);
            break;
        case JUNXION_ID_REQUEST:
            sendJunxionId();
            break;
        case INPUT_CONFIG_REQUEST:
            sendInputConfig();
            break;
    }
}

void Junxion::sendData() const {
    sendHeader(DATA_RESPONSE, _dataSize);
    // Send digital input states
    uint16_t state = 0;
    uint8_t pos = 0;
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (digitalPinAvailable(i)) {
            if (digitalPinActive(i)) {
                state = state | (1 << pos);
            }

            ++pos;
            if (pos >= 16) {
                sendUInt16(state);
                state = 0;
                pos = 0;
            }
        }
    }

    if (pos > 0) {
        sendUInt16(state);
    }

    // Send analog pin states
    for (uint8_t i = 0; i < ANALOG_PIN_COUNT; ++i) {
        if (analogPinAvailable(i)) {
            sendUInt16(analogPinValue(i));
        }
    }

    for (uint8_t i = 0; i < OWN_PIN_COUNT; ++i) {
        if (ownPinAvailable(i)) {
            sendUInt16(ownPinValue(i));
        }
    }
}

void Junxion::sendHeader(char cmd, uint8_t dataSize) const {
    Serial.write(HEADER);
    Serial.write(HEADER);
    Serial.write(dataSize);
    Serial.print(cmd);
}

void Junxion::sendInputConfig() const {
    sendHeader(INPUT_CONFIG_RESPONSE, 3 * (analogPinCount() + ownPinCount() + digitalPinCount()));
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (digitalPinAvailable(i)) {
            Serial.write(DIGITAL);
            Serial.write(i);
            Serial.write(DIGITAL_RESOLUTION);
        }
    }

    for (uint8_t i = 0; i < ANALOG_PIN_COUNT; ++i) {
        if (analogPinAvailable(i)) {
            Serial.write('a');
            Serial.write(i);
            Serial.write(16);
        }
    }

    for (uint8_t i = 0; i < OWN_PIN_COUNT; ++i) {
        if (ownPinAvailable(i)) {
            Serial.write('o');
            Serial.write(i);
            Serial.write(16);
        }
    }
}

void Junxion::sendJunxionId() const {
    sendHeader(JUNXION_ID_RESPONSE, 2);
    sendUInt16(JUNXION_ID);
}

void Junxion::sendUInt16(uint16_t data) const {
    Serial.write(data / 256);
    Serial.write(data % 256);
}
