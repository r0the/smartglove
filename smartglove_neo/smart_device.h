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

#ifndef SMART_DEVICE_H
#define SMART_DEVICE_H

#include <Adafruit_BNO055.h>
#include <ssd1306.h>
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
    void setup();
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
    bool buttonCombination(uint8_t id1, uint8_t id2) const;
    inline bool buttonDown(uint8_t id) const { return _buttons.down(id); }
    inline bool buttonPressed(uint8_t id) const { return _buttons.pressed(id); }
    virtual bool commandCalibrateIMU() const = 0;
    virtual bool commandDown() const = 0;
    virtual bool commandEnter() const = 0;
    virtual bool commandMenu() const = 0;
    virtual bool commandUp() const = 0;
    inline bool debugSerial() const { return _debugSerial; }
    inline SSD1306& display() { return _display; }
    virtual bool flexReady() const = 0;
    inline bool gestureAvailable(uint8_t id) const { return _sensors.gestureAvailable(id); }
    inline bool gestureDetected(uint8_t id) const { return _sensors.gestureDetected(id); }
    inline bool imuReady() const { return _imuReady; }
    void popBehaviour();
    void pushBehaviour(Behaviour* behaviour);
    bool resetIMU();
    bool sensorActivity(uint8_t id) const { return _sensors.activity(id); }
    bool sensorAvailable(uint8_t id) const { return _sensors.available(id); }
    int32_t sensorMaxValue(uint8_t id) const { return _sensors.maxValue(id); }
    int32_t sensorMinValue(uint8_t id) const { return _sensors.minValue(id); }
    int32_t sensorValue(uint8_t id) const { return _sensors.value(id); }
    void setLED(LED::Mode mode);
    void setDebugSerial(bool enable);
    void setShowFramerate(bool showFramerate);
    bool showFramerate() const { return _showFramerate; }
protected:
    virtual void doSetup() = 0;
    virtual void doLoop() = 0;
    virtual uint16_t availableButtonMask() const = 0;
    virtual uint16_t availableSensorMask() const = 0;
    virtual uint16_t readButtonState() const = 0;
    virtual void setInfoLED(bool on) = 0;
    void configureSensor(uint8_t index, double min, double max, double minStdDev);
    Sensors _sensors;
private:
    SmartDevice(const SmartDevice&);
    SmartDevice& operator=(const SmartDevice&);
    void waitForFlash();
    BehaviourStack _behaviour;
    Buttons _buttons;
    SSD1306 _display;
    bool _debugSerial;
    Adafruit_BNO055 _imu;
    imu::Vector<3> _imuAcceleration;
    sensors_event_t _imuEvent;
    bool _imuReady;
    bool _flexReady;
    LED _infoLED;
    unsigned long _lastMs;
    bool _showFramerate;
};

#endif
