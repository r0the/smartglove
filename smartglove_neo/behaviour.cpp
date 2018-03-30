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

#define MENU_COUNT 3

const char* MENU_ITEM[MENU_COUNT] = {
    "Set junXion ID", "Button Test", "Exit"
};

void ButtonTest::setup(SmartDevice& device) {
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void ButtonTest::loop(SmartDevice& device) {
    char text[20];
    device.display().drawText(10, 8, "Button Test");
    uint8_t x = 10;
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
        device.display().drawRectangle(x, 22, 7, 10);
        if (device.buttonPressed(1 << i)) {
            device.display().fillRectangle(x, 22, 7, 10);
        }

        x += 9;
        if (i % 4 == 3) {
            x += 2;
        }
    }
}

void JunxionMode::setup(SmartDevice& device) {
    _state = MIN_STATE;
    _junxion.setBoardId(JUNXION_GLOVE_BOARD_ID);
    _junxion.configureAnalogInput( 0, 'a', 0, 10);
    _junxion.configureAnalogInput( 1, 'a', 1, 10);
    _junxion.configureAnalogInput( 2, 'a', 2, 10);
    _junxion.configureAnalogInput( 3, 'a', 3, 10);
    _junxion.configureAnalogInput( 4, 'a', 4, 10);
    _junxion.configureAnalogInput( 5, 'a', 5, 10);
    _junxion.configureDigitalInput(0, 0);
    _junxion.configureDigitalInput(1, 1);
    _junxion.configureDigitalInput(2, 2);
    _junxion.configureDigitalInput(3, 3);
    _junxion.configureDigitalInput(4, 4);
    _junxion.configureDigitalInput(5, 5);
    Serial.begin(JUNXION_BAUD_RATE);
}

void JunxionMode::loop(SmartDevice& device) {
    char text[10];
    if (!Serial) {
        device.display().setTextAlign(ALIGN_LEFT);
        device.display().setFont(&HELVETICA_10);
        device.display().drawText(10, 8, "Connecting");
        device.display().drawText(10, 22, "to junXion...");
    }
    else {
        sprintf(text, "%i", _state);
        device.display().setFont(&SWISS_20_B);
        device.display().setTextAlign(ALIGN_CENTER);
        device.display().drawText(64, 12, text);
    }

    delay(1500);
//    _junxion.sendJunxionId();
//    _junxion.sendInputConfig();

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

void Menu::setup(SmartDevice& device) {
    _selected = 0;
    device.display().setFont(&HELVETICA_10);
    device.display().setTextAlign(ALIGN_LEFT);
}

void Menu::loop(SmartDevice& device) {
    device.display().drawText(10, 8, "Menu");
    device.display().drawText(10, 20, MENU_ITEM[_selected]);
    if (device.commandEnter()) {
        switch (_selected) {
            case 0:
                break;
            case 1:
                device.setBehaviour(new ButtonTest);
                break;
            case 2:
                device.setBehaviour(new JunxionMode);
                break;            
        }
    }

    if (device.commandNext()) {
        _selected = (_selected + 1) % MENU_COUNT;
    }

    if (device.commandPrev()) {
        _selected = (_selected + MENU_COUNT + 1) % MENU_COUNT;
    }
}

