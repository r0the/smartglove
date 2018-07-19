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
        selected(_selected);
    }

    if (device.commandUp()) {
        _selected = (_selected + _itemCount - 1) % _itemCount;
        selected(_selected);
    }

    draw(_selected);
}

void MenuBehaviour::select(uint8_t index) {
    _selected = index;
}

void MenuBehaviour::selected(uint8_t selected) {
}

/******************************************************************************
 * class BoardIdSelect
 *****************************************************************************/

const uint8_t BoardIdSelect::ITEM_COUNT = 4;
const char* BoardIdSelect::ITEMS[BoardIdSelect::ITEM_COUNT] = {
    "ID:1",
    "ID:2",
    "ID:3",
    "ID:4"
};

BoardIdSelect::BoardIdSelect(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
    select(Storage.readByte(STORAGE_BOARD_ID) - 49);
}

void BoardIdSelect::action(uint8_t selected) {
    Storage.writeByte(STORAGE_BOARD_ID, selected + 49);
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
 * class LEDTest
 *****************************************************************************/

const uint8_t LEDTest::ITEM_COUNT = 4;
const char* LEDTest::ITEMS[LEDTest::ITEM_COUNT] = {
    "Off",
    "On",
    "Blink slow",
    "Blink fast"
};

LEDTest::LEDTest(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

void LEDTest::action(uint8_t selected) {
    device.setShowFramerate(selected == 1);
    device.popBehaviour();
}

void LEDTest::draw(uint8_t selected) {
    device.display().drawText(10, 8, "LED Test");
    device.display().drawText(10, 20, ITEMS[selected]);
}

void LEDTest::selected(uint8_t selected) {
    switch (selected) {
        case 0:
            device.setLED(LED::Off);
            break;
        case 1:
            device.setLED(LED::On);
            break;
        case 2:
            device.setLED(LED::BlinkSlow);
            break;
        case 3:
            device.setLED(LED::BlinkFast);
            break;
    }
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
        if (device.gestureDetected(GESTURE_WAVE_LEFT)) {
            device.display().drawText(90, 8, "L");
        }

        if (device.gestureDetected(GESTURE_WAVE_RIGHT)) {
            device.display().drawText(100, 8, "R");
        }

//        if (device.sensorActivity(MAP[selected])) {
//            device.display().drawText(90, 8, "A");
//        }

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

const uint8_t MainMenu::ITEM_COUNT = 7;
const char* MainMenu::ITEMS[MainMenu::ITEM_COUNT] = {
    "junXion Board ID",
    "Button Test",
    "LED Test",
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
        device.pushBehaviour(new LEDTest(device));
        break;
    case 3:
        device.pushBehaviour(new GestureTest(device));
        break;
    case 4:
        device.pushBehaviour(new GyroscopeTest(device));
        break;
    case 5:
        device.pushBehaviour(new FramerateOption(device));
        break;
    case 6:
        device.popBehaviour();
        break;
    }
}

void MainMenu::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Menu");
    device.display().drawText(10, 20, ITEMS[selected]);
}

