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

#ifndef JUNXION_H
#define JUNXION_H

#include <Arduino.h>
#include "smart_device.h"

// ----------------------------------------------------------------------------
// class Junxion
// ----------------------------------------------------------------------------

class Junxion : public Behaviour {
public:
    Junxion(SmartDevice& device);
    virtual void setup();
    virtual void loop();
private:
    Junxion(const Junxion&);
    Junxion& operator=(const Junxion&);

    bool analogPinAvailable(uint8_t pin) const;
    uint8_t analogPinCount() const;
    uint16_t analogPinValue(uint8_t pin) const;
    bool digitalPinActive(uint8_t pin) const;
    bool digitalPinAvailable(uint8_t pin) const;
    uint8_t digitalPinCount() const;
    bool ownPinAvailable(uint8_t pin) const;
    uint8_t ownPinCount() const;
    uint16_t ownPinValue(uint8_t pin) const;
    void handleCommand(char cmd);
    void sendData() const;
    void sendHeader(char cmd, uint8_t dataSize) const;
    void sendInputConfig() const;
    void sendJunxionId() const;
    void sendUInt16(uint16_t data) const;
    uint8_t _boardId;
    uint8_t _dataSize;
    bool _headerReceived;
    unsigned int  _packageSize;
    bool _sendData;
    bool _serialAvailable;
    unsigned long _serialCheckMs;
    uint8_t _state;
};

#endif
