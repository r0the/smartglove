/*
 * Copyright (C) 2015 - 2018 by Stefan Rothe
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

#ifndef BNO055_H
#define BNO055_H

#include <Arduino.h>
#include <Adafruit_BNO055.h>

#define GESTURE_1 0x1
#define GESTURE_2 0x2
#define GESTURE_3 0x4
#define GESTURE_4 0x8

class Motion {
public:
    Motion();
    ~Motion();
    void begin();
    void loop();

    bool calibrating() const;
    bool error() const;
    bool ready() const;

    double ax() const;
    double ay() const;
    double az() const;
    uint8_t gestureState() const;
    double heading() const;
    double pitch() const;
    double roll() const;

private:
    Motion(const Motion&);
    Motion& operator=(const Motion&);

    imu::Vector<3>* _acceleration;
    Adafruit_BNO055 _bno;
    sensors_event_t* _event;
    uint8_t _gestureState;
    double _heading;
    byte _status;
    uint32_t _waveLeftTimeout;
    uint32_t _waveRightTimeout;
};

#endif

