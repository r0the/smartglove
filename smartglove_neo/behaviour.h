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

/******************************************************************************
 * class InitBehaviour
 *****************************************************************************/

class InitBehaviour : public Behaviour {
public:
    explicit InitBehaviour(SmartDevice& device);
    virtual void setup();
    virtual void loop();
};

/******************************************************************************
 * class MenuBehaviour
 *****************************************************************************/

class MenuBehaviour : public Behaviour {
public:
    MenuBehaviour(SmartDevice& device, uint8_t itemCount);
    virtual void setup();
    virtual void loop();
protected:
    virtual void action(uint8_t selected) = 0;
    virtual void draw(uint8_t selected) = 0;
private:
    uint8_t _itemCount;
    uint8_t _selected;
};

/******************************************************************************
 * class ButtonTest
 *****************************************************************************/

class ButtonTest : public Behaviour {
public:
    explicit ButtonTest(SmartDevice& device);
    virtual void setup();
    virtual void loop();
};

/******************************************************************************
 * class GyroscopeTest
 *****************************************************************************/

class GyroscopeTest : public MenuBehaviour {
public:
    explicit GyroscopeTest(SmartDevice& device);
    virtual void setup();
    virtual void action(uint8_t selected);
    virtual void draw(uint8_t selected);
private:
    uint8_t _range;
};

/******************************************************************************
 * class MainMenu
 *****************************************************************************/

class MainMenu : public MenuBehaviour {
public:
    MainMenu(SmartDevice& device);
    virtual void action(uint8_t selected);
    virtual void draw(uint8_t selected);
};

#endif

