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
#include "imu.h"
#include "sensors.h"


/******************************************************************************
 * class LED
 *****************************************************************************/

class LED {
public:
    enum Mode {
        Off, On, BlinkSlow, BlinkFast
    };

    LED();
    inline bool on() const { return _on; }
    void loop();
    void setMode(Mode mode);
private:
    LED(const LED&);
    LED& operator=(const LED&);

    unsigned long _blinkMillis;
    bool _on;
    unsigned long _timeout;
};

/******************************************************************************
 * class Buttons
 *****************************************************************************/

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
#define BUTTON_RING_FINGER_2    0x0400
#define BUTTON_LITTLE_FINGER_2  0x0800

class Buttons {
public:
    static const uint8_t MAX;
    Buttons();
    bool available(uint16_t button) const;
    uint8_t count() const { return _count; }
    bool down(uint16_t button) const;
    inline bool longPress() const { return _longPress; }
    bool pressed(uint16_t button) const;
    void setAvailable(uint16_t available);
    void setLongPress(uint16_t buttons, uint16_t millis);
    void updateState(uint16_t current);
private:
    uint16_t _available;
    uint8_t _count;
    uint16_t _current;
    uint16_t _last;
    bool _longPress;
    unsigned long _longPressMillis;
    unsigned long _longPressEnd;
    uint16_t _longPressButtons;
};

/******************************************************************************
 * class Behaviour
 *****************************************************************************/

class SmartDevice;

class Behaviour {
public:
    Behaviour(SmartDevice& device);
    virtual void setup() = 0;
    virtual void loop() = 0;
protected:
    SmartDevice& device;
private:
    Behaviour(const Behaviour&);
    Behaviour& operator=(const Behaviour&);
};

typedef Behaviour* BehaviourPtr;

/******************************************************************************
 * class BehaviourStack
 *****************************************************************************/

class BehaviourStack {
public:
    BehaviourStack(SmartDevice& device, uint8_t capacity);
    ~BehaviourStack();
    void loop();
    void pop();
    void push(Behaviour* behaviour);
private:
    BehaviourStack(const BehaviourStack&);
    BehaviourStack& operator=(const BehaviourStack&);

    BehaviourPtr* _behaviour;
    uint8_t _capacity;
    uint8_t _index;
    uint8_t _nextIndex;
};

/******************************************************************************
 * class SmartDevice
 *****************************************************************************/

class SmartDevice {
public:
    SmartDevice();
    void setup();
    void loop();
    inline bool buttonAvailable(uint16_t button) const { return _buttons.available(button); }
    inline uint8_t buttonCount() const { return _buttons.count(); }
    inline bool buttonDown(uint16_t button) const { return _buttons.down(button); }
    inline bool buttonLongPress() const { return _buttons.longPress(); }
    inline bool buttonPressed(uint16_t button) const { return _buttons.pressed(button); }
    bool commandEnter() const;
    bool commandNext() const;
    bool commandPrev() const;
    inline SSD1306& display() { return _display; }
    bool imuReady() const;
    void popBehaviour();
    void pushBehaviour(Behaviour* behaviour);
    void resetIMU();
    int32_t sensorValue(uint8_t index) const { return _sensors.value(index); }
    void setSensorOutRange(uint8_t index, uint16_t min, uint16_t max);
protected:
    virtual void doSetup() = 0;
    virtual void doLoop() = 0;
    virtual uint16_t availableButtons() const = 0;
    virtual uint16_t longPressButtons() const = 0;
    virtual uint16_t readButtonState() const = 0;
    virtual void setInfoLed(bool on) = 0;
    void setSensorRawRange(uint8_t index, double min, double max);
private:
    SmartDevice(const SmartDevice&);
    SmartDevice& operator=(const SmartDevice&);
    void waitForFlash();
    BehaviourStack _behaviour;
    Buttons _buttons;
    SSD1306 _display;
    IMU _imu;
    LED _infoLed;
    Sensors _sensors;
};

#endif

