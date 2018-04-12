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

#include "behaviour.h"
#include "config.h"
#include "junxion.h"

/******************************************************************************
 * class InitBehaviour
 *****************************************************************************/

InitBehaviour::InitBehaviour(SmartDevice& device) :
    Behaviour(device) {
}

void InitBehaviour::setup() {
}

void InitBehaviour::loop() {
    device.pushBehaviour(new MainMenu(device));
}

/******************************************************************************
 * class MenuBehaviour
 *****************************************************************************/

MenuBehaviour::MenuBehaviour(SmartDevice& device,uint8_t itemCount) :
    Behaviour(device),
    _itemCount(itemCount),
    _selected(0) {
}

void MenuBehaviour::setup() {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void MenuBehaviour::loop() {
    if (device.commandEnter()) {
        action(_selected);
        return;
    }

    if (device.commandDown()) {
        _selected = (_selected + 1) % _itemCount;
    }

    if (device.commandUp()) {
        _selected = (_selected + _itemCount - 1) % _itemCount;
    }

    draw(_selected);
}

/******************************************************************************
 * class ButtonTest
 *****************************************************************************/

ButtonTest::ButtonTest(SmartDevice& device) :
    Behaviour(device) {
}

void ButtonTest::setup() {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void ButtonTest::loop() {
    if (device.buttonLongPress()) {
        device.popBehaviour();
    }

    char text[20];
    device.display().drawText(10, 8, "Button Test");
    uint8_t x = 10;
    for (uint8_t id = 0; id < Buttons::COUNT; ++id) {
        if (device.buttonAvailable(id)) {
            device.display().drawRectangle(x, 22, 7, 10);
        }

        if (device.buttonPressed(id)) {
            device.display().fillRectangle(x, 22, 7, 10);
        }

        x += 9;
        if (id % 4 == 3) {
            x += 2;
        }
    }
}

/******************************************************************************
 * class GyroscopeTest
 *****************************************************************************/

const char* GYROSCOPE_MENU_ITEMS[] = { "Heading", "Pitch", "Roll" };
uint8_t GYROSCOPE_MENU_MAP[] = { SENSOR_GYRO_HEADING, SENSOR_GYRO_PITCH, SENSOR_GYRO_ROLL };

GyroscopeTest::GyroscopeTest(SmartDevice& device) :
    MenuBehaviour(device, 3) {
}

void GyroscopeTest::setup() {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
    _range = 115;
    device.setSensorOutRange(SENSOR_GYRO_HEADING, 0, _range);
    device.setSensorOutRange(SENSOR_GYRO_PITCH, 0, _range);
    device.setSensorOutRange(SENSOR_GYRO_ROLL, 0, _range);
    device.resetIMU();
}

void GyroscopeTest::action(uint8_t selected) {
    device.popBehaviour();
}

void GyroscopeTest::draw(uint8_t selected) {
    device.display().drawText(10, 8, GYROSCOPE_MENU_ITEMS[selected]);
    if (device.imuReady()) {
        device.display().drawRectangle(10, 22, _range, 8);
        uint16_t val = device.sensorValue(GYROSCOPE_MENU_MAP[selected]);
        if (val < _range/2) {
            device.display().fillRectangle(10 + val, 22, _range/2 - val, 8);
        }
        else {
            device.display().fillRectangle(10 + _range/2, 22, val - _range/2, 8);
        }
    }
    else {
        device.display().drawText(10, 22, "IMU not ready");
    }
}

/******************************************************************************
 * class MainMenu
 *****************************************************************************/

const char* MAIN_MENU_ITEMS[] = { "Button Test", "Gyroscope Test", "Exit" };

MainMenu::MainMenu(SmartDevice& device) :
    MenuBehaviour(device, 3) {
}

void MainMenu::action(uint8_t selected) {
    switch (selected) {
    case 0:
        device.pushBehaviour(new ButtonTest(device));
        break;
    case 1:
        device.pushBehaviour(new GyroscopeTest(device));
        break;
    case 2:
        device.pushBehaviour(new Junxion(device));
        break;            
    }
}

void MainMenu::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Menu");
    device.display().drawText(10, 20, MAIN_MENU_ITEMS[selected]);
}

