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

#define SENSOR_BEND_1        0
#define SENSOR_BEND_2        1
#define SENSOR_BEND_3        2
#define SENSOR_BEND_4        3
#define SENSOR_DIST_HAND     4
#define SENSOR_DIST_GROUND   5
#define SENSOR_ACCEL_X       6
#define SENSOR_ACCEL_Y       7
#define SENSOR_ACCEL_Z       8
#define SENSOR_GYRO_ROLL     9
#define SENSOR_GYRO_PITCH   10
#define SENSOR_GYRO_HEADING 11

/******************************************************************************
 * class Sensor
 *****************************************************************************/

class Sensor {
    friend class Sensors;
public:
    Sensor();
    void addMeasurement(double value);

    void setOutRange(uint16_t min, uint16_t max);
    void setRawRange(double min, double max);
    uint16_t value() const;
private:
    Sensor(const Sensor&);
    Sensor& operator=(const Sensor&);

    double _factor;
    uint16_t _outMax;
    uint16_t _outMin;
    uint8_t _pos;
    double _rawMax;
    double _rawMin;
    char _type;
    uint16_t* _values;
};

/******************************************************************************
 * class Sensors
 *****************************************************************************/

class Sensors {
public:
    static const uint8_t MAX;
    Sensors();
    ~Sensors();
    void addMeasurement(uint8_t index, double value);
    void setOutRange(uint8_t index, uint16_t min, uint16_t max);
    void setRawRange(uint8_t index, double min, double max);
    uint16_t value(uint8_t index) const;
private:  
    Sensors(const Sensors&);
    Sensors& operator=(const Sensors&);

    Sensor* _sensors;
};
#endif

