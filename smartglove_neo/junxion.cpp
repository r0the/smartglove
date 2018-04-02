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

const uint8_t ANALOG_PIN_COUNT = 8;

/******************************************************************************
 * class Junxion
 *****************************************************************************/

JunxionMode::JunxionMode(SmartDevice& device) :
    Behaviour(device),
    _boardId(1),
    _baudRate(DEFAULT_BAUD_RATE),
    _connectionState(Disconnected),
    _dataSize(0),
    _headerReceived(false),
    _sendData(false),
    _state(MIN_STATE) {
}

JunxionMode::~JunxionMode() {
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

void JunxionMode::setBoardId(uint8_t id) {
    _boardId = id;
}

uint8_t JunxionMode::analogPinCount() const {
    return 1;
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
        delay(10);
    }

    char text[10];
    sprintf(text, "%i", _state);
    device.display().setFont(&SWISS_20_B);
    device.display().setTextAlign(ALIGN_CENTER);
    device.display().drawText(64, 12, text);
    if (_sendData) {
        device.display().drawText(10, 12, "S"); 
    }
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

bool JunxionMode::digitalPinActive(uint8_t pin) const {
    uint16_t mask = DIGITAL_PIN_MAP[pin];
    if (DIGITAL_PIN_BUTTON[pin]) {
        return device.buttonAvailable(mask) && device.buttonPressed(mask);
    }
    else {
        return false;
    }
}

bool JunxionMode::digitalPinAvailable(uint8_t pin) const {
    uint16_t mask = DIGITAL_PIN_MAP[pin];
    if (DIGITAL_PIN_BUTTON[pin]) {
        return device.buttonAvailable(mask);
    }
    else {
        return false;
    }
}

uint8_t JunxionMode::digitalPinCount() const {
    return device.buttonCount();
}

void JunxionMode::handleCommand(char cmd) {
    switch (cmd) {
        case START_DATA:
            PRINTLN("Request Start Data");
            _sendData = true;
            break;
        case STOP_DATA:
            PRINTLN("Request Stop Data");
            _sendData = false;
            break;
        case BOARD_ID_REQUEST:
            PRINTLN("Request Board ID");
            sendBoardId();
            break;
        case JUNXION_ID_REQUEST:
            PRINTLN("Request junXion ID");
            sendJunxionId();
            break;
        case INPUT_CONFIG_REQUEST:
            PRINTLN("Request Input Config");
            sendInputConfig();
            break;
        default:
            PRINT("Unknown command: ") PRINTLN(cmd)
            break;
    }
}

void JunxionMode::sendBoardId() {
    sendHeader(BOARD_ID_RESPONSE, 1);
    Serial.write(_boardId);
}

void JunxionMode::sendData() const {
    uint8_t dataSize = 2 * (analogPinCount() + digitalPinCount() / 16 + 1);
    PRINT("Sending Data, size=")
    PRINTLN(dataSize)
    sendHeader(DATA_RESPONSE, dataSize);
    // Send digital input states
    uint16_t state = 0;
    uint8_t pos = 0;
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (digitalPinAvailable(i)) {
            if (digitalPinActive(i)) {
                PRINT("Pin ") PRINT(i) PRINTLN(" active")
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

    sendUInt16(device.sensorValue(SENSOR_GYRO_ROLL));
}

void JunxionMode::sendHeader(char cmd, uint8_t dataSize) const {
    Serial.write(HEADER);
    Serial.write(HEADER);
    Serial.write(dataSize);
    Serial.print(cmd);
}

void JunxionMode::sendInputConfig() {
    sendHeader(INPUT_CONFIG_RESPONSE, 3 * (analogPinCount() + digitalPinCount()));
    for (uint8_t i = 0; i < DIGITAL_PIN_COUNT; ++i) {
        if (digitalPinAvailable(i)) {
            Serial.write(DIGITAL);
            Serial.write(i);
            Serial.write(DIGITAL_RESOLUTION);
        }
    }
 
    Serial.write('c'); // type
    Serial.write(1); // pin
    Serial.write(10); // resolution
}

void JunxionMode::sendJunxionId() {
    sendHeader(JUNXION_ID_RESPONSE, 2);
    sendUInt16(JUNXION_ID);
}

void JunxionMode::sendUInt16(uint16_t data) const {
    Serial.write(static_cast<int>(data / 256));
    Serial.write(static_cast<int>(data % 256));
}

void JunxionMode::showConnecting() {
    device.display().setTextAlign(ALIGN_LEFT);
    device.display().setFont(&HELVETICA_10);
    device.display().drawText(10, 8, "Connecting");
    device.display().drawText(10, 22, "to junXion...");
}

