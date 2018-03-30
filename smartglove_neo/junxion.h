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

#ifndef JUNXION_H
#define JUNXION_H

#include <Arduino.h>

class AnalogInput;
class DigitalInput;

// ----------------------------------------------------------------------------
// class Junxion
// ----------------------------------------------------------------------------

class Junxion {
public:
    Junxion();
    void init(uint8_t digitalInputCount, uint8_t analogInputCount);
    void configureAnalogInput(uint8_t index, char type, uint8_t pin, uint8_t resolution);
    void configureDigitalInput(uint8_t index, uint8_t pin);
    void loop();
    void setAnalogValue(uint8_t index, int16_t value);
    void setDigitalValue(uint8_t index, bool active);
    void setBoardId(uint8_t boardId);
private:
    Junxion(const Junxion&);
    Junxion& operator=(const Junxion&);

    void sendData();
    void sendHeader(char cmd, uint8_t dataSize);
    void sendInputConfig();
    void sendInt16(int16_t data);
    void sendJunxionId();
    void sendUInt16(uint16_t data);
    uint8_t _analogInputCount;
    AnalogInput* _analogInputs;
    uint8_t _boardId;
    uint8_t _dataSize;
    uint8_t _digitalInputCount;
    DigitalInput* _digitalInputs;
    bool _headerExpected;
    unsigned int  _packageSize;
    bool _sendData;
};

#endif

