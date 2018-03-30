/*
 * Copyright (C) 2018 by Stefan Rothe
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

#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "smart_device.h"
#include "junxion.h"

class ButtonTest : public Behaviour {
public:
    virtual void setup(SmartDevice& device);
    virtual void loop(SmartDevice& device);
};

class JunxionMode : public Behaviour {
public:
    virtual void setup(SmartDevice& device);
    virtual void loop(SmartDevice& device);
private:
    Junxion _junxion;
    uint8_t _state;
};

class Menu : public Behaviour {
public:
    virtual void setup(SmartDevice& device);
    virtual void loop(SmartDevice& device);
private:
    uint8_t _selected;
};

class NumberInput : public Behaviour {
    
};

#endif

