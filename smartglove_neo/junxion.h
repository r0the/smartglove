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
#include "buttons.h"
#include "sensors.h"

// ----------------------------------------------------------------------------
// class Junxion
// ----------------------------------------------------------------------------

class Junxion {
public:
    Junxion();
    void setup(const Buttons* buttons, const Sensors* sensors);
    void loop();
    void setBoardId(uint8_t boardId);
    void sendJunxionId();
    void sendInputConfig();
private:
    Junxion(const Junxion&);
    Junxion& operator=(const Junxion&);

    void handleCommand(char cmd);
    void sendBoardId();
    void sendData();
    void sendHeader(char cmd, uint8_t dataSize);
    void sendInt16(int16_t data);
    void sendUInt16(uint16_t data);
    uint8_t _boardId;
    const Buttons* _buttons;
    uint8_t _dataSize;
    bool _headerReceived;
    unsigned int  _packageSize;
    bool _sendData;
    const Sensors* _sensors;
};

#endif

