/*
 * Copyright (C) 2015 - 2018 by Stefan Rothe
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

#define DEFAULT_BAUD_RATE 115200

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

/******************************************************************************
 * class AnalogInput
 *****************************************************************************/

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

/******************************************************************************
 * class DigitalInput
 *****************************************************************************/

class DigitalInput {
public:
    DigitalInput() :
        active(false),
        pin(0) {
    }

    bool active;
    int8_t pin;
};

/******************************************************************************
 * class Junxion
 *****************************************************************************/

Junxion::Junxion() :
    _analogInputCount(0),
    _analogInputs(NULL),
    _boardId(1),
    _baudRate(DEFAULT_BAUD_RATE),
    _dataSize(0),
    _digitalInputCount(0),
    _digitalInputs(NULL),
    _headerReceived(false),
    _sendData(false) {
}

Junxion::~Junxion() {
    delete[] _analogInputs;
    delete[] _digitalInputs;
}

void Junxion::setup(uint8_t digitalInputCount, uint8_t analogInputCount) {
    _analogInputCount = analogInputCount;
    _analogInputs = new AnalogInput[analogInputCount];
    _digitalInputCount = digitalInputCount;
    _digitalInputs = new DigitalInput[digitalInputCount];

    _dataSize = 2 * analogInputCount + 2 * ((digitalInputCount / 16) + 1);
}

void Junxion::configureAnalogInput(uint8_t index, char type, uint8_t pin, uint8_t resolution) {
    if (index < _analogInputCount) {
        _analogInputs[index].pin = pin;
        _analogInputs[index].resolution = resolution;
        _analogInputs[index].type = type;
    }
}

void Junxion::configureDigitalInput(uint8_t index, uint8_t pin) {
    if (index < _digitalInputCount) {
        _digitalInputs[index].pin = pin;
    }
}

void Junxion::setAnalogValue(uint8_t index, int16_t value) {
    if (index < _analogInputCount) {
        _analogInputs[index].value = value;
    }
}

void Junxion::setDigitalValue(uint8_t index, bool active) {
    if (index < _digitalInputCount) {
        _digitalInputs[index].active = active;
    }
}

void Junxion::setBoardId(uint8_t id) {
    _boardId = id;
}

void Junxion::loop() {
    switch (_state) {
    case Disconnected:
        Serial.begin(_baudRate);
        _state = Connecting;
        break;
    case Connecting:
        if (Serial) {
            sendJunxionId();
            sendInputConfig();
            _state = Connected;
        }

        break;
    case Connected:
        communicate();
        break;
    }
}

void Junxion::communicate() {
    if (!_headerReceived && Serial.available() > 2) {
        if (Serial.read() != HEADER) {
            return;
        }
        
        if (Serial.read() == HEADER) {
           _headerReceived = true;
            _packageSize = Serial.read() + 1;
        }
    }

    if (_headerReceived && Serial.available() >= _packageSize) {
        char cmd = Serial.read();
        handleCommand(cmd);
        _headerReceived = false;
    }
 
    if (_sendData) {
        sendData();
    }
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
            sendBoardId();
            break;
        case JUNXION_ID_REQUEST:
            sendJunxionId();
            break;
        case INPUT_CONFIG_REQUEST:
            sendInputConfig();
            break;
    }
}

void Junxion::sendBoardId() {
    sendHeader(BOARD_ID_RESPONSE, 1);
    Serial.write(_boardId);
}

void Junxion::sendData() {
    sendHeader(DATA_RESPONSE, _dataSize);
    // Send digital input states
    uint16_t state = 0;
    uint8_t pos = 0;
    for (uint8_t i = 0; i < _digitalInputCount; ++i) {
        if (_digitalInputs[i].active) {
            state = state | (1 << pos);
        }

        ++pos;
        if (pos >= 16) {
            sendInt16(state);
            state = 0;
            pos = 0;
        }
    }

    if (pos > 0) {    
        sendInt16(state);
    }

    // Send analog pin states
    for (uint8_t i = 0; i < _analogInputCount; ++i) {
        sendInt16(_analogInputs[i].value);
    }
}

void Junxion::sendHeader(char cmd, uint8_t dataSize) {
    Serial.write(HEADER);
    Serial.write(HEADER);
    Serial.write(dataSize);
    Serial.print(cmd);
}

void Junxion::sendInputConfig() {
    sendHeader(INPUT_CONFIG_RESPONSE, 3 * (_analogInputCount + _digitalInputCount));
    for (uint8_t i = 0; i < _digitalInputCount; ++i) {
        Serial.write(DIGITAL);
        Serial.write(_digitalInputs[i].pin);
        Serial.write(DIGITAL_RESOLUTION);
    }
 
    for (uint8_t i = 0; i < _analogInputCount; ++i) {
        Serial.write(_analogInputs[i].type);
        Serial.write(_analogInputs[i].pin);
        Serial.write(_analogInputs[i].resolution);
    }
}

void Junxion::sendInt16(int16_t data) {
    Serial.write(data / 256);
    Serial.write(data % 256);
}

void Junxion::sendJunxionId() {
    sendHeader(JUNXION_ID_RESPONSE, 2);
    sendInt16(JUNXION_ID);
}

void Junxion::sendUInt16(uint16_t data) {
    Serial.write(data / 256);
    Serial.write(data % 256);
}

