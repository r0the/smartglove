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

#define LED_OFF         0
#define LED_ON          1
#define LED_BLINK_SLOW  2
#define LED_BLINK_FAST  3

#define BUTTON_THUMB_1          0x0001
#define BUTTON_THUMB_2          0x0002
#define BUTTON_THUMB_3          0x0004
#define BUTTON_THUMB_4          0x0008
#define BUTTON_INDEX_FINGER_1   0x0010
#define BUTTON_MIDDLE_FINGER_1  0x0020
#define BUTTON_RING_FINGER_1    0x0040
#define BUTTON_LITTLE_FINGER_1  0x0080
#define BUTTON_INDEX_FINGER_2   0x0100
#define BUTTON_MIDDLE_FINGER_2  0x0200

#define BUTTON_COUNT 12

class LED {
public:
    LED();
    inline bool on() const { return _on; }
    void loop();
    void setMode(uint8_t mode);
private:
    unsigned long _blinkMillis;
    bool _on;
    unsigned long _timeout;
};


class SmartDevice;

class Behaviour {
public:
    virtual void setup(SmartDevice& device) = 0;
    virtual void loop(SmartDevice& device) = 0;
};

typedef Behaviour* BehaviourPtr;

class SmartDevice {
public:
    SmartDevice();
    void setup();
    void loop();
    bool buttonDown(uint16_t button) const;
    bool buttonPressed(uint16_t button) const;
    bool commandEnter() const;
    bool commandNext() const;
    bool commandPrev() const;
    inline SSD1306& display() { return _display; }
    void popBehaviour();
    void pushBehaviour(Behaviour* behaviour);
    void setBehaviour(Behaviour* behaviour);
protected:
    virtual void doSetup() = 0;
    virtual void doLoop() = 0;
    virtual uint16_t longPressButtons() const = 0;
    virtual uint16_t readButtonState() const = 0;
    virtual void setInfoLed(bool on) = 0;
private:
    void waitForFlash();
    BehaviourPtr* _behaviour;
    uint8_t _behaviourIndex;
    uint16_t _buttonsCurrent;
    uint16_t _buttonsLast;
    SSD1306 _display;
    LED _infoLed;
    bool _longPress;
    unsigned long _longPressEnd;
    uint16_t _longPressButtons;
};

#endif

