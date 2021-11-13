/*
 * Copyright (C) 2021 by Stefan Rothe
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

#ifndef MAX_H
#define MAX_H

#include <Arduino.h>
#include "smart_device.h"

// ----------------------------------------------------------------------------
// class Maxx
// ----------------------------------------------------------------------------

class Max : public Behaviour {
public:
    Max(SmartDevice& device);
    virtual void setup();
    virtual void loop();
    
private:
    Max(const Max&);
    Max& operator=(const Max&);
    void receive();
    void sendAnalog();
    void sendDigital();
    void sendInformation();
    void sendButton(uint8_t button);
    void sendGesture(uint8_t gesture);
    void sendSensor(uint8_t id);
    void sendByte(uint8_t data);
    bool _serialConnected;
    unsigned long _serialCheckMs;
};

#endif
