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
#include "config.h"

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

const uint8_t DIGITAL_PIN_COUNT = 8;
const uint8_t DIGITAL_PIN_MAP[] = {
    BUTTON_THUMB_1,
    BUTTON_THUMB_2,
    BUTTON_THUMB_3,
    BUTTON_THUMB_4,
    BUTTON_INDEX_FINGER_1,
    BUTTON_MIDDLE_FINGER_1,
    BUTTON_RING_FINGER_1,
    BUTTON_LITTLE_FINGER_1
};

const uint8_t ANALOG_PIN_COUNT = 8;

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
 * class Junxion
 *****************************************************************************/

JunxionMode::JunxionMode(SmartDevice& device) :
    Behaviour(device),
    _analogInputCount(0),
    _analogInputs(NULL),
    _boardId(1),
    _baudRate(DEFAULT_BAUD_RATE),
    _connectionState(Disconnected),
    _dataSize(0),
    _headerReceived(false),
    _sendData(false),
    _state(MIN_STATE) {
}

JunxionMode::~JunxionMode() {
    delete[] _analogInputs;
}

void JunxionMode::setup() {
    setBoardId(JUNXION_GLOVE_BOARD_ID);
}

void JunxionMode::loop() {
    unsigned long now = millis();
    switch (_connectionState) {
    case Disconnected:
        showConnecting();
        Serial.begin(_baudRate);
        _connectionState = Connecting;
        _connectionTimeout = now + JUNXION_CONNECTION_TIMEOUT;
        break;
    case Connecting:
        showConnecting();
        if (_connectionTimeout < now) {
            Serial.end();
            _connectionState = Disconnected;
        }
        else if (Serial) {
            sendJunxionId();
            sendInputConfig();
            _connectionState = Connected;
        }

        break;
    case Connected:
        communicate();
        break;
    }

    if (device.buttonLongPress()) {
        device.popBehaviour();
    }
}

void JunxionMode::setup(uint8_t digitalInputCount, uint8_t analogInputCount) {
    _analogInputCount = analogInputCount;
    _analogInputs = new AnalogInput[analogInputCount];
    _dataSize = 2 * analogInputCount + 2 * ((digitalInputCount / 16) + 1);
}

void JunxionMode::configureAnalogInput(uint8_t index, char type, uint8_t pin, uint8_t resolution) {
    if (index < _analogInputCount) {
        _analogInputs[index].pin = pin;
        _analogInputs[index].resolution = resolution;
        _analogInputs[index].type = type;
    }
}

void JunxionMode::setAnalogValue(uint8_t index, int16_t value) {
    if (index < _analogInputCount) {
        _analogInputs[index].value = value;
    }
}

void JunxionMode::setBoardId(uint8_t id) {
    _boardId = id;
}

void JunxionMode::communicate() {
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

    char text[10];
    sprintf(text, "%i", _state);
    device.display().setFont(&SWISS_20_B);
    device.display().setTextAlign(ALIGN_CENTER);
    device.display().drawText(64, 12, text);
    if (device.commandNext()) {
        if (_state < MAX_STATE) {
            ++_state;
        }
        else {
            _state = MIN_STATE;
        }
    }

    if (device.commandPrev()) {
        if (_state > MIN_STATE) {
            --_state;
        }
        else {
            _state = MAX_STATE;
        }
    }
}

void JunxionMode::handleCommand(char cmd) {
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

void JunxionMode::sendBoardId() {
    sendHeader(BOARD_ID_RESPONSE, 1);
    Serial.write(_boardId);
}

void JunxionMode::sendData() {
    sendHeader(DATA_RESPONSE, _dataSize);
    // Send digital input states
    uint16_t state = 0;
    uint8_t pos = 0;
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        uint8_t button = DIGITAL_PIN_MAP[i];
        if (device.buttonAvailable(button)) {
            if (device.buttonPressed(button)) {
                state = state | (1 << pos);
            }
    
            ++pos;
            if (pos >= 16) {
                sendInt16(state);
                state = 0;
                pos = 0;
            }
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

void JunxionMode::sendHeader(char cmd, uint8_t dataSize) {
    Serial.write(HEADER);
    Serial.write(HEADER);
    Serial.write(dataSize);
    Serial.print(cmd);
}

void JunxionMode::sendInputConfig() {
    sendHeader(INPUT_CONFIG_RESPONSE, 3 * (_analogInputCount + device.buttonCount()));
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (device.buttonAvailable(DIGITAL_PIN_MAP[i])) {
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

void JunxionMode::sendInt16(int16_t data) {
    Serial.write(data / 256);
    Serial.write(data % 256);
}

void JunxionMode::sendJunxionId() {
    sendHeader(JUNXION_ID_RESPONSE, 2);
    sendInt16(JUNXION_ID);
}

void JunxionMode::sendUInt16(uint16_t data) {
    Serial.write(data / 256);
    Serial.write(data % 256);
}

void JunxionMode::showConnecting() {
    device.display().setTextAlign(ALIGN_LEFT);
    device.display().setFont(&HELVETICA_10);
    device.display().drawText(10, 8, "Connecting");
    device.display().drawText(10, 22, "to junXion...");
}

