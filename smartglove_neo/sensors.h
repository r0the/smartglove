/*
 * Copyright (C) 2016 - 2018 by Stefan Rothe
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

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

/******************************************************************************
 * class Buttons
 *****************************************************************************/

#define BUTTON_THUMB_1          0
#define BUTTON_THUMB_2          1
#define BUTTON_THUMB_3          2
#define BUTTON_THUMB_4          3
#define BUTTON_INDEX_FINGER_1   4
#define BUTTON_MIDDLE_FINGER_1  5
#define BUTTON_RING_FINGER_1    6
#define BUTTON_LITTLE_FINGER_1  7
#define BUTTON_INDEX_FINGER_2   8
#define BUTTON_MIDDLE_FINGER_2  9
#define BUTTON_RING_FINGER_2    10
#define BUTTON_LITTLE_FINGER_2  11

class Buttons {
public:
    static const uint8_t COUNT;
    Buttons();
    bool available(uint8_t id) const;
    bool down(uint8_t id) const;
    bool pressed(uint8_t id) const;
    void setAvailable(uint16_t mask);
    void updateState(uint16_t current);
private:
    Buttons(const Buttons&);
    Buttons& operator=(const Buttons&);

    uint16_t _available;
    uint16_t _current;
    uint16_t _last;
};

/******************************************************************************
 * class Sensors
 *****************************************************************************/

#define SENSOR_FLEX_INDEX_FINGER   0
#define SENSOR_FLEX_MIDDLE_FINGER  1
#define SENSOR_FLEX_RING_FINGER    2
#define SENSOR_FLEX_LITTLE_FINGER  3
#define SENSOR_DIST_HAND           4
#define SENSOR_DIST_GROUND         5
#define SENSOR_ACCEL_X             6
#define SENSOR_ACCEL_Y             7
#define SENSOR_ACCEL_Z             8
#define SENSOR_GYRO_ROLL           9
#define SENSOR_GYRO_PITCH         10
#define SENSOR_GYRO_HEADING       11

#define GESTURE_WAVE_LEFT 0
#define GESTURE_WAVE_RIGHT 1
#define GESTURE_WAVE_UP 2
#define GESTURE_WAVE_DOWN 3

class Sensor;

class Sensors {
public:
    static const uint8_t COUNT;
    static const uint8_t GESTURE_COUNT;
    Sensors();
    ~Sensors();
    bool activity(uint8_t id) const;
    void addMeasurement(unsigned long time, uint8_t id, double value);
    bool available(uint8_t id) const;
    void configure(uint8_t id, double min, double max, double minStdDev);
    bool gestureAvailable(uint8_t id) const;
    bool gestureDetected(uint8_t id) const;
    uint16_t maxValue(uint8_t id) const;
    uint16_t minValue(uint8_t id) const;
    void setAvailable(uint16_t mask);
    uint16_t value(uint8_t id) const;
private:
    Sensors(const Sensors&);
    Sensors& operator=(const Sensors&);

    uint16_t _available;
    uint8_t _gestures;
    Sensor* _sensors;
};
#endif
