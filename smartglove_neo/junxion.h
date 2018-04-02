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

#ifndef JUNXION_H
#define JUNXION_H

#include <Arduino.h>
#include "smart_device.h"

class AnalogInput;
class DigitalInput;

enum ConnectionState {
    Disconnected, Connecting, Connected
};

/******************************************************************************
 * class Junxion
 *****************************************************************************/

class JunxionMode : public Behaviour {
public:
    JunxionMode(SmartDevice& device);
    ~JunxionMode();

    virtual void setup();
    virtual void loop();

    void setBoardId(uint8_t id);
private:
    uint8_t analogPinCount() const;
    void communicate();
    bool digitalPinActive(uint8_t pin) const;
    bool digitalPinAvailable(uint8_t pin) const;
    uint8_t digitalPinCount() const;
    void handleCommand(char cmd);
    void sendBoardId();
    void sendData() const;
    void sendHeader(char cmd, uint8_t dataSize) const;
    void sendInputConfig();
    void sendJunxionId();
    void sendUInt16(uint16_t data) const;
    void showConnecting();
    uint32_t _baudRate;
    uint8_t _boardId;
    ConnectionState _connectionState; 
    unsigned long _connectionTimeout;
    uint8_t _dataSize;
    bool _headerReceived;
    unsigned int  _packageSize;
    bool _sendData;
    uint8_t _state;
};

#endif

