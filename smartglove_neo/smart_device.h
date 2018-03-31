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

#ifndef SMART_DEVICE_H
#define SMART_DEVICE_H

#include <ssd1306.h>
#include "buttons.h"

#define LED_OFF         0
#define LED_ON          1
#define LED_BLINK_SLOW  2
#define LED_BLINK_FAST  3

class LED {
public:
    LED();
    inline bool on() const { return _on; }
    void loop();
    void setMode(uint8_t mode);
private:
    LED(const LED&);
    LED& operator=(const LED&);

    unsigned long _blinkMillis;
    bool _on;
    unsigned long _timeout;
};


class SmartDevice;

class Behaviour {
public:
    Behaviour();
    virtual void setup(SmartDevice& device) = 0;
    virtual void loop(SmartDevice& device) = 0;
private:
    Behaviour(const LED&);
    Behaviour& operator=(const LED&);
};

typedef Behaviour* BehaviourPtr;

class SmartDevice {
public:
    SmartDevice();
    void setup();
    void loop();
    inline bool buttonDown(uint16_t button) const { return _buttons.down(button); }
    inline bool buttonPressed(uint16_t button) const { return _buttons.pressed(button); }
    bool commandEnter() const;
    bool commandNext() const;
    bool commandPrev() const;
    inline SSD1306& display() { return _display; }
    void popBehaviour();
    void pushBehaviour(Behaviour* behaviour);
    uint8_t sensorCount() const;
    void setBehaviour(Behaviour* behaviour);
protected:
    virtual void doSetup() = 0;
    virtual void doLoop() = 0;
    virtual uint16_t longPressButtons() const = 0;
    virtual uint16_t readButtonState() const = 0;
    virtual void setInfoLed(bool on) = 0;
private:
    SmartDevice(const SmartDevice&);
    SmartDevice& operator=(const SmartDevice&);

    void waitForFlash();
    BehaviourPtr* _behaviour;
    Buttons _buttons;
    uint8_t _behaviourIndex;
    SSD1306 _display;
    LED _infoLed;
};

#endif

