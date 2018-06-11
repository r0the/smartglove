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

#ifndef SMART_BALL_H
#define SMART_BALL_H

#include "smart_device.h"
#include "pca9557.h"

class SmartBall : public SmartDevice {
public:
    SmartBall();
    virtual bool commandCalibrateIMU() const;
    virtual bool commandMenu() const;
protected:
    virtual void doSetup();
    virtual void doLoop();
    virtual uint16_t availableButtonMask() const;
    virtual uint16_t availableSensorMask() const;
    virtual uint16_t readButtonState() const;
    virtual void setInfoLED(bool on);
private:
    PCA9557 _buttons;
    bool _commandMenu;
    unsigned long _menuTimeoutMs;
};

#endif

