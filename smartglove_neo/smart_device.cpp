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
#include "config.h"

#define LED_FAST_DELAY 100
#define LED_SLOW_DELAY 500

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
        _blinkMillis = LED_SLOW_DELAY;
        _on = true;
        _timeout = millis() + _blinkMillis;
        break;
    case LED_BLINK_FAST:
        _blinkMillis = LED_FAST_DELAY;
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


void ButtonTest::loop(SmartDevice& device) {
    char text[20];
    device.display().setFont(&HELVETICA_10);
    device.display().clear();
    device.display().drawText(10, 10, "Button Test");
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

    device.display().update();
}


void Menu::loop(SmartDevice& device) {
    
}


SmartDevice::SmartDevice() :
    _display(I2C_DISPLAY_ADDRESS) {
}

bool SmartDevice::buttonDown(uint16_t button) const {
    return buttonPressed(button) && ((_buttonsLast & button) == 0);
}

bool SmartDevice::buttonPressed(uint16_t button) const {
    return (_buttonsCurrent & button) == button;
}

void SmartDevice::setup() {
//    Serial.begin(9600);
//    while (!Serial) { delay(1); }
    Wire.begin();
    _display.begin();
    if (!_display.ready()) {
        _infoLed.setMode(LED_BLINK_FAST);
    }
    else {
        _infoLed.setMode(LED_OFF);
        _display.setFont(&HELVETICA_18);
    }

    setInfoLed(_infoLed.on());
    _behaviour = new ButtonTest;
    waitForFlash();
    doSetup();
}

void SmartDevice::loop() {
    _infoLed.loop();
    setInfoLed(_infoLed.on());
    // update buttons
    _buttonsLast = _buttonsCurrent;
    _buttonsCurrent = readButtonState();
    doLoop();
    _behaviour->loop(*this);
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

