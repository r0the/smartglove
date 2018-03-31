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

#include <Wire.h>
#include "smart_device.h"
#include "behaviour.h"
#include "config.h"


LED::LED() :
    _blinkMillis(0),
    _on(false), 
    _timeout(0) {
}

void LED::setMode(uint8_t mode) {
    switch (mode) {
    case LED_OFF:
        _blinkMillis = 0;
        _on = false;
        break;
    case LED_ON:
        _blinkMillis = 0;
        _on = true;
        break;
    case LED_BLINK_SLOW:
        _blinkMillis = LED_BLINK_SLOW_MS;
        _on = true;
        _timeout = millis() + _blinkMillis;
        break;
    case LED_BLINK_FAST:
        _blinkMillis = LED_BLINK_FAST_MS;
        _on = true;
        _timeout = millis() + _blinkMillis;
        break;
    }
}

void LED::loop() {
    unsigned long now = millis();
    if (_blinkMillis > 0 && now > _timeout) {
       _on = !_on;
        _timeout = now + _blinkMillis;
    }
}

Behaviour::Behaviour() {
}

SmartDevice::SmartDevice() :
    _behaviour(new BehaviourPtr[BEHAVIOUR_STACK_SIZE]),
    _behaviourIndex(0),
    _buttons(),
    _display(I2C_DISPLAY_ADDRESS),
    _infoLed(),
    _sensors(NULL) {
}

bool SmartDevice::commandEnter() const {
    return buttonDown(BUTTON_THUMB_1);
}

bool SmartDevice::commandNext() const {
    return buttonDown(BUTTON_MIDDLE_FINGER_1);
}

bool SmartDevice::commandPrev() const {
    return buttonDown(BUTTON_INDEX_FINGER_1);
}

void SmartDevice::setup() {
//    Serial.begin(9600);
//    while (!Serial) { delay(1); }
    Wire.begin();
    _buttons.setupLongPress(longPressButtons(), LONG_PRESS_MS);
    _display.begin();
    if (!_display.ready()) {
        _infoLed.setMode(LED_BLINK_FAST);
    }
    else {
        _infoLed.setMode(LED_OFF);
        _display.setFont(&HELVETICA_18);
    }

    setInfoLed(_infoLed.on());
    waitForFlash();
    setBehaviour(new Menu);
    doSetup();
}

void SmartDevice::loop() {
    unsigned long now = millis();
    _infoLed.loop();
    setInfoLed(_infoLed.on());
    // update buttons
    _buttons.updateState(readButtonState());
    if (_buttons.longPress()) {
        setBehaviour(new Menu);
    }

    doLoop();
    _display.clear();
    if (_behaviourIndex > 0) {
        _behaviour[_behaviourIndex - 1]->loop(*this);
    }

    _display.update();
}

void SmartDevice::popBehaviour() {
    if (_behaviourIndex > 0) {
        delete _behaviour[_behaviourIndex - 1];
        _behaviour[_behaviourIndex - 1] = NULL;
        --_behaviourIndex;
    }
}

void SmartDevice::pushBehaviour(Behaviour* behaviour) {
    if (_behaviourIndex < BEHAVIOUR_STACK_SIZE && behaviour != NULL) {
        ++_behaviourIndex;
        _behaviour[_behaviourIndex - 1] = behaviour;
        _behaviour[_behaviourIndex - 1]->setup(*this);
    }
}

void SmartDevice::setBehaviour(Behaviour* behaviour) {
    while (_behaviourIndex > 0) {
        popBehaviour();
    }

    pushBehaviour(behaviour);
}

void SmartDevice::waitForFlash() {
    unsigned long end = millis() + 10000;
    signed long wait;
    char text[20];
    do {
        wait = end - millis();
        if (_display.ready()) {
            sprintf(text, "Wait %i", (wait / 1000));
            _display.clear();
            _display.drawText(10, 10, text);
            _display.update();
        }
        else {
            
        }

        delay(10);
    }
    while (wait > 0);
}

