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
    uint8_t _pin;
    uint16_t _outMax;
    uint16_t _outMin;
    uint8_t _pos;
    double _rawMax;
    double _rawMin;
    uint8_t _resolution;
    char _type;
    uint16_t* _values;
};

class Sensors {
public:
    Sensors(uint8_t count);
    ~Sensors();
    inline uint8_t count() const { return _count; }
    void configOutput(uint8_t index, uint8_t pin, uint8_t type, uint8_t resolution);
    uint8_t pin(uint8_t index) const;
    uint8_t resolution(uint8_t index) const;
    uint16_t value(uint8_t index) const;
    char type(uint8_t index) const;
private:  
    Sensors(const Sensors&);
    Sensors& operator=(const Sensors&);

    uint8_t _count;
    Sensor* _sensors;
};
#endif

