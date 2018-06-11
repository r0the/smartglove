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
#include "storage.h"

/******************************************************************************
 * class InitBehaviour
 *****************************************************************************/

InitBehaviour::InitBehaviour(SmartDevice& device) :
    Behaviour(device) {
}

void InitBehaviour::setup() {
}

void InitBehaviour::loop() {
    device.pushBehaviour(new Junxion(device));
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

void MenuBehaviour::select(uint8_t index) {
    _selected = index;
}

/******************************************************************************
 * class BoardIdSelect
 *****************************************************************************/

const uint8_t BoardIdSelect::ITEM_COUNT = 3;
const char* BoardIdSelect::ITEMS[BoardIdSelect::ITEM_COUNT] = {
    "Arduino 51",
    "Arduino 52",
    "Arduino 53"
};

BoardIdSelect::BoardIdSelect(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
    switch (Storage.readByte(STORAGE_BOARD_ID)) {
        case 51:
            select(0);
            break;
        case 52:
            select(1);
            break;
        case 53:
            select(2);
            break;
    }
}

void BoardIdSelect::action(uint8_t selected) {
    uint8_t id = 0;
    switch (selected) {
        case 0:
            id = 51;
            break;
        case 1:
            id = 52;
            break;
        case 2:
            id = 53;
            break;
    }
    Storage.writeByte(STORAGE_BOARD_ID, id);
    device.popBehaviour();
}

void BoardIdSelect::draw(uint8_t selected) {
    device.display().drawText(10, 8, "junXion Board ID");
    device.display().drawText(10, 20, ITEMS[selected]);
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
    if (device.commandMenu()) {
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
 * class FramerateOption
 *****************************************************************************/

const uint8_t FramerateOption::ITEM_COUNT = 2;
const char* FramerateOption::ITEMS[FramerateOption::ITEM_COUNT] = {
    "No",
    "Yes"
};

FramerateOption::FramerateOption(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
    select(device.showFramerate() ? 1 : 0);
}

void FramerateOption::action(uint8_t selected) {
    device.setShowFramerate(selected == 1);
    device.popBehaviour();
}

void FramerateOption::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Show Framerate");
    device.display().drawText(10, 20, ITEMS[selected]);
}

/******************************************************************************
 * class GestureTest
 *****************************************************************************/

const uint8_t GestureTest::ITEM_COUNT = 3;
const char* GestureTest::ITEMS[GestureTest::ITEM_COUNT] = {
    "X-Axis", "Y-Axis", "Z-Axis"
};
const uint8_t GestureTest::MAP[] = {
    SENSOR_ACCEL_X, SENSOR_ACCEL_Y, SENSOR_ACCEL_Z
};
GestureTest::GestureTest(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

const uint16_t GestureTest::RANGE = 116;

void GestureTest::setup() {
    MenuBehaviour::setup();
}

void GestureTest::action(uint8_t selected) {
    device.popBehaviour();
}

void GestureTest::draw(uint8_t selected) {
    device.display().drawText(10, 8, ITEMS[selected]);
    if (device.imuReady()) {
        if (device.sensorActivity(MAP[selected])) {
            device.display().drawText(90, 8, "A");
        }

        device.display().drawRectangle(10, 22, RANGE, 8);
        uint16_t val = device.sensorValue(MAP[selected]) / 565; // 565 = 65535 / RANGE
        if (val < RANGE/2) {
            device.display().fillRectangle(10 + val, 22, RANGE/2 - val, 8);
        }
        else {
            device.display().fillRectangle(10 + RANGE/2, 22, val - RANGE/2, 8);
        }
    }
    else {
        device.display().drawText(10, 22, "IMU not ready");
    }
}

/******************************************************************************
 * class GyroscopeTest
 *****************************************************************************/

const uint8_t GyroscopeTest::ITEM_COUNT = 3;
const char* GyroscopeTest::ITEMS[GyroscopeTest::ITEM_COUNT] = {
    "Heading", "Pitch", "Roll"
};
const uint8_t GyroscopeTest::MAP[] = {
    SENSOR_GYRO_HEADING, SENSOR_GYRO_PITCH, SENSOR_GYRO_ROLL
};

const uint16_t GyroscopeTest::RANGE = 116;

GyroscopeTest::GyroscopeTest(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

void GyroscopeTest::setup() {
    MenuBehaviour::setup();
}

void GyroscopeTest::action(uint8_t selected) {
    device.popBehaviour();
}

void GyroscopeTest::draw(uint8_t selected) {
    device.display().drawText(10, 8, ITEMS[selected]);
    if (device.imuReady()) {
        if (device.sensorActivity(MAP[selected])) {
            device.display().drawText(90, 8, "A");
        }

        device.display().drawRectangle(10, 22, RANGE, 8);
        uint16_t val = device.sensorValue(MAP[selected]) / 565; // 565 = 65535 / RANGE
        if (val < RANGE/2) {
            device.display().fillRectangle(10 + val, 22, RANGE/2 - val, 8);
        }
        else {
            device.display().fillRectangle(10 + RANGE/2, 22, val - RANGE/2, 8);
        }
    }
    else {
        device.display().drawText(10, 22, "IMU not ready");
    }
}

/******************************************************************************
 * class MainMenu
 *****************************************************************************/

const uint8_t MainMenu::ITEM_COUNT = 6;
const char* MainMenu::ITEMS[MainMenu::ITEM_COUNT] = {
    "junXion Board ID",
    "Button Test",
    "Gesture Test",
    "Gyroscope Test",
    "Framerate",
    "Exit"
};

MainMenu::MainMenu(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

void MainMenu::action(uint8_t selected) {
    switch (selected) {
    case 0:
        device.pushBehaviour(new BoardIdSelect(device));
        break;
    case 1:
        device.pushBehaviour(new ButtonTest(device));
        break;
    case 2:
        device.pushBehaviour(new GestureTest(device));
        break;
    case 3:
        device.pushBehaviour(new GyroscopeTest(device));
        break;
    case 4:
        device.pushBehaviour(new FramerateOption(device));
        break;
    case 5:
        device.popBehaviour();
        break;
    }
}

void MainMenu::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Menu");
    device.display().drawText(10, 20, ITEMS[selected]);
}

