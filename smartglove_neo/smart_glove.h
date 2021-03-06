/*
 * Copyright (C) 2019 by Stefan Rothe
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

#ifndef SMART_GLOVE_H
#define SMART_GLOVE_H

#include "smart_device.h"
#include "SparkFun_Displacement_Sensor_Arduino_Library.h"
#include "pca9557.h"
#include "vl53l1x.h"

class SmartGlove : public SmartDevice {
public:
    SmartGlove();
    virtual bool commandCalibrateIMU() const;
    virtual bool commandDown() const;
    virtual bool commandEnter() const;
    virtual bool commandMenu() const;
    virtual bool commandUp() const;
    virtual bool flexReady() const;
protected:
    virtual void doSetup();
    virtual void doLoop();
    virtual uint16_t availableButtonMask() const;
    virtual uint16_t availableSensorMask() const;
    virtual uint16_t readButtonState() const;
    virtual void setInfoLED(bool on);
private:
    bool _ads;
    bool _commandMenu;
    VL53L1X _distance;
    ADS _flexIndexFinger;
    ADS _flexLittleFinger;
    ADS _flexMiddleFinger;
    ADS _flexRingFinger;
    unsigned long _menuTimeoutMs;
    PCA9557 _sideButtons;
    PCA9557 _tipButtons;
};

#endif
