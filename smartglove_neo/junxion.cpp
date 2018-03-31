/*
 * Copyright (C) 2015 - 2017 by Stefan Rothe
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

Junxion::Junxion() {
}

void Junxion::setup(const Buttons* buttons, const Sensors* sensors) {
    _buttons = buttons;
    _sensors = sensors;
    _boardId = 1;
    _dataSize = 2 * sensors->count() + 2 * ((buttons->count() / 16) + 1);
    _headerReceived = false;
}

void Junxion::setBoardId(uint8_t boardId) {
    _boardId = boardId;
}

void Junxion::loop() {
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
    for (uint8_t i = 0; i < _buttons->count(); ++i) {
        if (_buttons->pressed(i)) {
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
    for (uint8_t i = 0; i < _sensors->count(); ++i) {
        sendInt16(_sensors->value(i));
    }
}

void Junxion::sendHeader(char cmd, uint8_t dataSize) {
    Serial.write(HEADER);
    Serial.write(HEADER);
    Serial.write(dataSize);
    Serial.print(cmd);
}

void Junxion::sendInputConfig() {
    sendHeader(INPUT_CONFIG_RESPONSE, 3 * (_sensors->count() + _buttons->count()));
    for (uint8_t i = 0; i < _buttons->count(); ++i) {
        Serial.write(DIGITAL);
        Serial.write(i);
        Serial.write(DIGITAL_RESOLUTION);
    }
 
    for (uint8_t i = 0; i < _sensors->count(); ++i) {
        Serial.write(_sensors->type(i));
        Serial.write(_sensors->pin(i));
        Serial.write(_sensors->resolution(i));
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

