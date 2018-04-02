/*
 * Copyright (C) 2015 - 2016 by Stefan Rothe
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

#define JUNXION_BOARD_ID 51 // results in ArdId:3 in junXion
#define JUNXION_BAUD_RATE 115200
#define JUNXION_DIGITAL_INPUT_COUNT 10
#define JUNXION_ANALOG_INPUT_COUNT 12

#define MIN_STATE 1
#define MAX_STATE 15

/******************************************************************************
 * digital pin mapping
 *****************************************************************************/

const uint8_t DIGITAL_PIN_COUNT = 16;
const bool DIGITAL_PIN_BUTTON[DIGITAL_PIN_COUNT] = {
    true, true, true, true, true, true, true, true,
    false, false, false, false, true, true, true, true
};

const uint16_t DIGITAL_PIN_MAP[DIGITAL_PIN_COUNT] = {
    BUTTON_THUMB_1,
    BUTTON_THUMB_2,
    BUTTON_THUMB_3,
    BUTTON_THUMB_4,
    BUTTON_INDEX_FINGER_1,
    BUTTON_MIDDLE_FINGER_1,
    BUTTON_RING_FINGER_1,
    BUTTON_LITTLE_FINGER_1,
    0,
    0,
    0,
    0,
    BUTTON_INDEX_FINGER_2,
    BUTTON_MIDDLE_FINGER_2,
    BUTTON_RING_FINGER_2,
    BUTTON_LITTLE_FINGER_2
};

// ----------------------------------------------------------------------------
// class AnalogInput
// ----------------------------------------------------------------------------

class AnalogInput {
public:
    AnalogInput() :
        pin(0),
        resolution(10),
        type(ANALOG),
        value(0) {
    }

    int8_t pin;
    int8_t resolution;
    char type;
    int16_t value;
};

// ----------------------------------------------------------------------------
// class Junxion
// ----------------------------------------------------------------------------

Junxion::Junxion(SmartDevice& device) :
    Behaviour(device),
    _analogInputCount(JUNXION_ANALOG_INPUT_COUNT),
    _analogInputs(new AnalogInput[JUNXION_ANALOG_INPUT_COUNT]),
    _boardId(1),
    _dataSize(2 * JUNXION_ANALOG_INPUT_COUNT + 2 * ((JUNXION_DIGITAL_INPUT_COUNT / 16) + 1)),
    _headerReceived(false),
    _sendData(false),
    _state(MIN_STATE)
{
}

void Junxion::setup() {
    _dataSize = 2 * JUNXION_ANALOG_INPUT_COUNT + 2 * ((digitalPinCount() / 16) + 1);
    setBoardId(JUNXION_BOARD_ID);
    configureAnalogInput( 0, 'a', 0, 10);
    configureAnalogInput( 1, 'a', 1, 10);
    configureAnalogInput( 2, 'a', 2, 10);
    configureAnalogInput( 3, 'a', 3, 10);
    configureAnalogInput( 4, 'o', 0, 10);
    configureAnalogInput( 5, 'o', 1, 10);
    configureAnalogInput( 6, 'o', 2, 10);
    configureAnalogInput( 7, 'o', 3, 10);
    configureAnalogInput( 8, 'o', 4, 10);
    configureAnalogInput( 9, 'o', 5, 10);
    configureAnalogInput(10, 'o', 6, 10);
    configureAnalogInput(11, 'o', 7, 16);
    for (uint8_t i = 0; i < Sensors::MAX; ++i) {
        device.setSensorOutRange(i, 0, 1023);
    }

    showConnecting();
    Serial.begin(JUNXION_BAUD_RATE);
    while (!Serial) {
        delay(10);
    }

    // this delay is required, otherwise junXion fails to detect Arduino
    delay(1500);
    sendJunxionId();
    sendInputConfig();
}

void Junxion::loop() {
//    setDigitalValue(8, device.gestureDetected(GESTURE_1));
//    setDigitalValue(9, device.gestureDetected(GESTURE_2));
    setAnalogValue(0, device.sensorValue(SENSOR_BEND_1));
    setAnalogValue(1, device.sensorValue(SENSOR_BEND_2));
    setAnalogValue(2, device.sensorValue(SENSOR_BEND_3));
    setAnalogValue(3, device.sensorValue(SENSOR_BEND_4));
    setAnalogValue(4, device.sensorValue(SENSOR_DIST_HAND));
    setAnalogValue(5, device.sensorValue(SENSOR_ACCEL_X));
    setAnalogValue(6, device.sensorValue(SENSOR_ACCEL_Y));
    setAnalogValue(7, device.sensorValue(SENSOR_ACCEL_Z));
    setAnalogValue(8, device.sensorValue(SENSOR_GYRO_ROLL));
    setAnalogValue(9, device.sensorValue(SENSOR_GYRO_PITCH));
    setAnalogValue(10, device.sensorValue(SENSOR_GYRO_HEADING));
    setAnalogValue(11, _state * 65536 / 127);

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

    char text[10];
    sprintf(text, "%i", _state);
    device.display().setFont(&SWISS_20_B);
    device.display().setTextAlign(ALIGN_CENTER);
    device.display().drawText(64, 12, text);
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
}

void Junxion::configureAnalogInput(uint8_t index, char type, uint8_t pin, uint8_t resolution) {
    if (index < _analogInputCount) {
        _analogInputs[index].pin = pin;
        _analogInputs[index].resolution = resolution;
        _analogInputs[index].type = type;
    }
}

void Junxion::setAnalogValue(uint8_t index, int16_t value) {
    if (index < _analogInputCount) {
        _analogInputs[index].value = value;
    }
}

void Junxion::setBoardId(uint8_t id) {
    _boardId = id;
}

bool Junxion::digitalPinActive(uint8_t pin) const {
    uint16_t mask = DIGITAL_PIN_MAP[pin];
    if (DIGITAL_PIN_BUTTON[pin]) {
        return device.buttonAvailable(mask) && device.buttonPressed(mask);
    }
    else {
        return false;
    }
}

bool Junxion::digitalPinAvailable(uint8_t pin) const {
    uint16_t mask = DIGITAL_PIN_MAP[pin];
    if (DIGITAL_PIN_BUTTON[pin]) {
        return device.buttonAvailable(mask);
    }
    else {
        return false;
    }
}

uint8_t Junxion::digitalPinCount() const {
    return device.buttonCount();
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
    for (uint8_t i = 0; i < _analogInputCount; ++i) {
        sendUInt16(_analogInputs[i].value);
    }
}

void Junxion::sendHeader(char cmd, uint8_t dataSize) const {
    Serial.write(HEADER);
    Serial.write(HEADER);
    Serial.write(dataSize);
    Serial.print(cmd);
}

void Junxion::sendInputConfig() const {
    sendHeader(INPUT_CONFIG_RESPONSE, 3 * (_analogInputCount + digitalPinCount()));
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (digitalPinAvailable(i)) {
            Serial.write(DIGITAL);
            Serial.write(i);
            Serial.write(DIGITAL_RESOLUTION);
        }
    }
 
    for (uint8_t i = 0; i < _analogInputCount; ++i) {
        Serial.write(_analogInputs[i].type);
        Serial.write(_analogInputs[i].pin);
        Serial.write(_analogInputs[i].resolution);
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

void Junxion::showConnecting() const {
    device.display().setTextAlign(ALIGN_LEFT);
    device.display().setFont(&HELVETICA_10);
    device.display().drawText(10, 8, "Connecting");
    device.display().drawText(10, 22, "to junXion...");
}

