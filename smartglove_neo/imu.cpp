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

#include "imu.h"
#include "config.h"

#define WAVE_THRESHOLD 7.0
#define WAVE_TIMEOUT 400

IMU::IMU() :
    _acceleration(new imu::Vector<3>()),
    _event(new sensors_event_t()),
    _status(Uninitialized) {
}

IMU::~IMU() {
    delete _event;
    delete _acceleration;
}

void IMU::setup() {
    if (_bno.begin(Adafruit_BNO055::OPERATION_MODE_IMUPLUS)) {
        _status = Ready;
    }
    else {
        _status = Error;
    }
}

void IMU::loop() {
    uint32_t now = millis();
    if (_status == Ready) {
        _bno.getEvent(_event);
        *_acceleration = _bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        _heading = _event->orientation.x;
        if (_heading > 180) {
            _heading -= 360;
        }

        _gestureState = 0;
        double ax = _acceleration->x();
        if (ax < -WAVE_THRESHOLD && _waveLeftTimeout < now) {
            _waveRightTimeout = now + WAVE_TIMEOUT;
            _gestureState |= GESTURE_1;
        }

        if (ax > WAVE_THRESHOLD && _waveRightTimeout < now) {
            _waveLeftTimeout = now + WAVE_TIMEOUT;
            _gestureState |= GESTURE_2;
        }
    }
}

double IMU::ax() const {
    return _acceleration->x();
}

double IMU::ay() const {
    return _acceleration->y();
}

double IMU::az() const {
    return _acceleration->z();
}

uint8_t IMU::gestureState() const {
    return _gestureState;
}

double IMU::heading() const {
    return _heading;
}

double IMU::pitch() const {
    return _event->orientation.y;
}

double IMU::roll() const {
    return _event->orientation.z;
}


