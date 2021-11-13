/*
 * Copyright (C) 2018 - 2020 by Stefan Rothe
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
#include "max.h"
#include "storage.h"

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
 * class InitBehaviour
 *****************************************************************************/

InitBehaviour::InitBehaviour(SmartDevice& device) :
    Behaviour(device) {
}

void InitBehaviour::setup() {
}

void InitBehaviour::loop() {
    uint8_t protocol = Storage.readByte(STORAGE_PROTOCOL);
    switch (protocol) {
        case 0:
            device.pushBehaviour(new Junxion(device));
            break;
        case 1:
            device.pushBehaviour(new Max(device));
            break;
        default:
            Storage.writeByte(STORAGE_PROTOCOL, 0);
            break;
    }
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
 * class DebugSerialOption
 *****************************************************************************/

const uint8_t DebugSerialOption::ITEM_COUNT = 2;
const char* DebugSerialOption::ITEMS[DebugSerialOption::ITEM_COUNT] = {
    "No",
    "Yes"
};

DebugSerialOption::DebugSerialOption(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
    select(device.debugSerial() ? 1 : 0);
}

void DebugSerialOption::action(uint8_t selected) {
    device.setDebugSerial(selected == 1);
    device.popBehaviour();
}

void DebugSerialOption::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Serial Debug Output");
    device.display().drawText(10, 20, ITEMS[selected]);
}

/******************************************************************************
 * class DistanceTest
 *****************************************************************************/

const uint16_t DistanceTest::RANGE = 116;

DistanceTest::DistanceTest(SmartDevice& device) :
    Behaviour(device) {
}

void DistanceTest::setup() {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void DistanceTest::loop() {
    if (device.commandEnter()) {
        device.popBehaviour();
    }

    device.display().drawText(10, 8, "Distance Test");
    device.display().drawRectangle(10, 22, RANGE, 8);
    uint16_t val = device.sensorValue(SENSOR_DISTANCE) / 565; // 565 = 65535 / RANGE
    if (device.debugSerial()) {
        Serial.println(val);
    }

    device.display().fillRectangle(10, 22, 10 + val, 8);
}

/******************************************************************************
 * class FlexTest
 *****************************************************************************/

const uint8_t FlexTest::ITEM_COUNT = 4;
const char* FlexTest::ITEMS[FlexTest::ITEM_COUNT] = {
    "Index Finger", "Middle Finger", "Ring Finger", "Little Finger"
};
const uint8_t FlexTest::MAP[] = {
    SENSOR_FLEX_INDEX_FINGER, SENSOR_FLEX_MIDDLE_FINGER, SENSOR_FLEX_RING_FINGER, SENSOR_FLEX_LITTLE_FINGER
};

const uint16_t FlexTest::RANGE = 116;

FlexTest::FlexTest(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

void FlexTest::setup() {
    MenuBehaviour::setup();
}

void FlexTest::action(uint8_t selected) {
    device.popBehaviour();
}

void FlexTest::draw(uint8_t selected) {
    device.display().drawText(10, 8, ITEMS[selected]);
    if (device.flexReady()) {
        if (device.sensorActivity(MAP[selected])) {
            device.display().drawText(90, 8, "A");
        }

        device.display().drawRectangle(10, 22, RANGE, 8);
        uint16_t val = device.sensorValue(MAP[selected]) / 565; // 565 = 65535 / RANGE
        if (device.debugSerial()) {
            Serial.println(val);
        }

        if (val < RANGE/2) {
            device.display().fillRectangle(10 + val, 22, RANGE/2 - val, 8);
        }
        else {
            device.display().fillRectangle(10 + RANGE/2, 22, val - RANGE/2, 8);
        }
    }
    else {
        device.display().drawText(10, 22, "Flex not ready");
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
    "Gesture", "Left/Right", "Up/Down"
};
const uint8_t GestureTest::MAP[] = {
    0, SENSOR_ACCEL_Y, SENSOR_ACCEL_Z
};
GestureTest::GestureTest(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

void GestureTest::setup() {
    MenuBehaviour::setup();
}

void GestureTest::action(uint8_t selected) {
    device.popBehaviour();
}

void GestureTest::draw(uint8_t selected) {
    device.display().drawText(10, 8, ITEMS[selected]);
    if (!device.imuReady()) {
        device.display().drawText(10, 22, "IMU not ready");
        return;
    }

    if (selected == 0) {
        if (device.gestureDetected(GESTURE_WAVE_LEFT)) {
            device.display().drawText(10, 20, "LL");
        }
    
        if (device.gestureDetected(GESTURE_WAVE_RIGHT)) {
            device.display().drawText(30, 20, "RR");
        }

        if (device.gestureDetected(GESTURE_WAVE_UP)) {
            device.display().drawText(50, 20, "UU");
        }

        if (device.gestureDetected(GESTURE_WAVE_DOWN)) {
            device.display().drawText(70, 20, "DD");
        }
    }
    else {
        char text[20];
        sprintf(text, "%i", device.sensorMinValue(MAP[selected]));
        device.display().drawText(10, 20, text);
    
        sprintf(text, "%i", device.sensorMaxValue(MAP[selected]));
        device.display().drawText(70, 20, text);
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
        if (device.debugSerial()) {
            Serial.println(val);
        }

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
 * class MainMenu
 *****************************************************************************/

const uint8_t MainMenu::ITEM_COUNT = 10;
const char* MainMenu::ITEMS[MainMenu::ITEM_COUNT] = {
    "Protocol",
    "junXion Board ID",
    "Button Test",
    "LED Test",
    "Distance Test",
    "Gesture Test",
    "Gyroscope Test",
    "Flex Test",
    "Debug Serial",
    "Exit"
};

MainMenu::MainMenu(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
}

void MainMenu::action(uint8_t selected) {
    switch (selected) {
    case 0:
        device.pushBehaviour(new ProtocolSelect(device));
        break;
    case 1:
        device.pushBehaviour(new BoardIdSelect(device));
        break;
    case 2:
        device.pushBehaviour(new ButtonTest(device));
        break;
    case 3:
        device.pushBehaviour(new LEDTest(device));
        break;
    case 4:
        device.pushBehaviour(new DistanceTest(device));
        break;
    case 5:
        device.pushBehaviour(new GestureTest(device));
        break;
    case 6:
        device.pushBehaviour(new GyroscopeTest(device));
        break;
    case 7:
        device.pushBehaviour(new FlexTest(device));
        break;
    case 8:
        device.pushBehaviour(new DebugSerialOption(device));
        break;
    case 9:
        device.popBehaviour();
        break;
    }
}

void MainMenu::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Menu");
    device.display().drawText(10, 20, ITEMS[selected]);
    device.display().setTextAlign(ALIGN_RIGHT);
    device.display().drawText(120, 8, VERSION);
    device.display().setTextAlign(ALIGN_LEFT);
}

/******************************************************************************
 * class ProtocolSelect
 *****************************************************************************/

const uint8_t ProtocolSelect::ITEM_COUNT = 2;
const char* ProtocolSelect::ITEMS[ProtocolSelect::ITEM_COUNT] = {
    "JunXion",
    "Max"
};

ProtocolSelect::ProtocolSelect(SmartDevice& device) :
    MenuBehaviour(device, ITEM_COUNT) {
    select(Storage.readByte(STORAGE_PROTOCOL));
}

void ProtocolSelect::action(uint8_t selected) {
    Storage.writeByte(STORAGE_PROTOCOL, selected);
    device.popBehaviour();
}

void ProtocolSelect::draw(uint8_t selected) {
    device.display().drawText(10, 8, "Protocol");
    device.display().drawText(10, 20, ITEMS[selected]);
}
