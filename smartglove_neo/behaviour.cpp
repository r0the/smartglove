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

/******************************************************************************
 * class InitBehaviour
 *****************************************************************************/

void InitBehaviour::setup(SmartDevice& device) {
}

void InitBehaviour::loop(SmartDevice& device) {
    device.pushBehaviour(new MainMenu);
}

/******************************************************************************
 * class MenuBehaviour
 *****************************************************************************/

MenuBehaviour::MenuBehaviour(uint8_t itemCount) :
    _itemCount(itemCount),
    _selected(0) {
}

void MenuBehaviour::setup(SmartDevice& device) {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void MenuBehaviour::loop(SmartDevice& device) {
    if (device.commandEnter()) {
        action(device, _selected);
        return;
    }

    if (device.commandNext()) {
        _selected = (_selected + 1) % _itemCount;
    }

    if (device.commandPrev()) {
        _selected = (_selected + _itemCount + 1) % _itemCount;
    }

    draw(device, _selected);
}

/******************************************************************************
 * class ButtonTest
 *****************************************************************************/

void ButtonTest::setup(SmartDevice& device) {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void ButtonTest::loop(SmartDevice& device) {
    if (device.buttonLongPress()) {
        device.popBehaviour();
    }

    char text[20];
    device.display().drawText(10, 8, "Button Test");
    uint8_t x = 10;
    for (uint8_t i = 0; i < BUTTON_MAX; ++i) {
        if (device.buttonAvailable(1 << i)) {
            device.display().drawRectangle(x, 22, 7, 10);
        }

        if (device.buttonPressed(1 << i)) {
            device.display().fillRectangle(x, 22, 7, 10);
        }

        x += 9;
        if (i % 4 == 3) {
            x += 2;
        }
    }
}

/******************************************************************************
 * class GyroscopeTest
 *****************************************************************************/

const char* GYROSCOPE_MENU_ITEMS[] = { "Heading", "Pitch", "Roll" };
uint8_t GYROSCOPE_MENU_MAP[] = { SENSOR_GYRO_HEADING, SENSOR_GYRO_PITCH, SENSOR_GYRO_ROLL };

GyroscopeTest::GyroscopeTest() :
    MenuBehaviour(3) {
}

void GyroscopeTest::setup(SmartDevice& device) {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
    _range = 115;
    device.setSensorOutRange(SENSOR_GYRO_HEADING, 0, _range);
    device.setSensorOutRange(SENSOR_GYRO_PITCH, 0, _range);
    device.setSensorOutRange(SENSOR_GYRO_ROLL, 0, _range);
    device.resetIMU();
}

void GyroscopeTest::action(SmartDevice& device, uint8_t selected) {
    device.popBehaviour();
}

void GyroscopeTest::draw(SmartDevice& device, uint8_t selected) {
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
 * class JunxionMode
 *****************************************************************************/

void JunxionMode::setup(SmartDevice& device) {
    _state = MIN_STATE;
    for (uint8_t i = 0; i < BUTTON_MAX; ++i) {
        if (device.buttonAvailable(1 << i)) {
            _junxion.configureDigitalInput(i, i);
        }
    }

    _junxion.setBoardId(JUNXION_GLOVE_BOARD_ID);
}

void JunxionMode::loop(SmartDevice& device) {
    if (device.buttonLongPress()) {
        device.popBehaviour();
    }

    switch (_junxion.state()) {
    case Disconnected:
    case Connecting:
        device.display().setTextAlign(ALIGN_LEFT);
        device.display().setFont(&HELVETICA_10);
        device.display().drawText(10, 8, "Connecting");
        device.display().drawText(10, 22, "to junXion...");
        return;
    }

    char text[10];
    sprintf(text, "%i", _state);
    device.display().setFont(&SWISS_20_B);
    device.display().setTextAlign(ALIGN_CENTER);
    device.display().drawText(64, 12, text);


    if (device.commandNext()) {
        if (_state < MAX_STATE) {
            ++_state;
        }
        else {
            _state = MIN_STATE;
        }
    }

    if (device.commandPrev()) {
        if (_state > MIN_STATE) {
            --_state;
        }
        else {
            _state = MAX_STATE;
        }
    }
}

/******************************************************************************
 * class MainMenu
 *****************************************************************************/

const char* MAIN_MENU_ITEMS[] = { "Button Test", "Gyroscope Test", "Exit" };

MainMenu::MainMenu() :
    MenuBehaviour(3) {
}

void MainMenu::action(SmartDevice& device, uint8_t selected) {
    switch (selected) {
    case 0:
        device.pushBehaviour(new ButtonTest);
        break;
    case 1:
        device.pushBehaviour(new GyroscopeTest);
        break;
    case 2:
        device.pushBehaviour(new JunxionMode);
        break;            
    }
}

void MainMenu::draw(SmartDevice& device, uint8_t selected) {
    device.display().drawText(10, 8, "Menu");
    device.display().drawText(10, 20, MAIN_MENU_ITEMS[selected]);
}


