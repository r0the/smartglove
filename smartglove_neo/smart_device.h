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
    inline bool buttonAvailable(uint8_t id) const { return _buttons.available(id); }
    inline bool buttonDown(uint8_t id) const { return _buttons.down(id); }
    inline bool buttonLongPress() const { return _buttons.longPress(); }
    inline bool buttonPressed(uint8_t id) const { return _buttons.pressed(id); }
    bool commandDown() const;
    bool commandEnter() const;
    bool commandUp() const;
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
    virtual uint16_t availableButtonMask() const = 0;
    virtual uint16_t availableSensorMask() const = 0;
    virtual uint16_t longPressButtonMask() const = 0;
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

