/*
 * Copyright (C) 2016 - 2017 by Stefan Rothe
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

#include "sensors.h"

#define RAW_VALUE_COUNT 9

static uint8_t median3(uint16_t* data, uint8_t a, uint8_t b, uint8_t c) {
    if (data[a] < data[b]) {
        if (data[b] < data[c]) {
            return b; // a, b, c
        }
        else { // c < b
            if (data[a] < data[c]) {
                return c; // a, c, b
            }
            else { // c < a
                return a; // c, a, b
            }
        }
    }
    else { // b < a
        if (data[a] < data[c]) {
            return a; // b, a, c
        }
        else { // c < a
            if (data[b] < data[c]) {
                return c; // b, c, a
            }
            else { // c < b
                return b; // c, b, a
            }
        }
    }
}

Sensor::Sensor() :
    _factor(1.0),
    _outMax(65535),
    _outMin(0),
    _pos(0),
    _rawMax(1.0),
    _rawMin(-1.0),
    _values(new uint16_t[RAW_VALUE_COUNT]) {
    for (int i = 0; i < RAW_VALUE_COUNT; ++i) {
        _values[i] = (_outMax - _outMin) / 2;
    }
}

void Sensor::addMeasurement(double value) {
    if (_rawMin < _rawMax) {
        if (value < _rawMin) {
            value = _rawMin;
        }
    
        if (value > _rawMax) {
            value = _rawMax;
        }
    }
    else {
        if (value > _rawMin) {
            value = _rawMin;
        }
    
        if (value < _rawMax) {
            value = _rawMax;
        }        
    }

    _values[_pos] = _outMin + (value - _rawMin) * _factor;
    _pos = (_pos + 1) % RAW_VALUE_COUNT;
}

void Sensor::setOutRange(uint16_t min, uint16_t max) {
    _outMin = min;
    _outMax = max;
    _factor = static_cast<double>(_outMax - _outMin) / (_rawMax - _rawMin);
}

void Sensor::setRawRange(double min, double max) {
    _rawMin = min;
    _rawMax = max;
    _factor = static_cast<double>(_outMax - _outMin) / (_rawMax - _rawMin);
}

uint16_t Sensor::value() const {
    uint8_t index = _pos;
    uint8_t m1 = median3(_values, 0, 1, 2);
    uint8_t m2 = median3(_values, 3, 4, 5);
    uint8_t m3 = median3(_values, 6, 7, 8);
    index = median3(_values, m1, m2, m3);
    return _values[index];
}


Sensors::Sensors(uint8_t count) :
    _count(count),
    _sensors(new Sensor[count]) {
}

Sensors::~Sensors() {
    delete[] _sensors;
}

uint8_t Sensors::pin(uint8_t index) const {
    if (index < _count) {
        return _sensors[index]._pin;
    }
    else {
        return 0;
    }
}

char Sensors::type(uint8_t index) const {
    if (index < _count) {
        return _sensors[index]._type;
    }
    else {
        return 0;
    }
}


uint8_t Sensors::resolution(uint8_t index) const {
    if (index < _count) {
        return _sensors[index]._resolution;
    }
    else {
        return 0;
    }
}

uint16_t Sensors::value(uint8_t index) const {
    if (index < _count) {
        return _sensors[index].value();
    }
    else {
        return 0;
    }
}

void Sensors::configOutput(uint8_t index, uint8_t pin, uint8_t type, uint8_t resolution) {
    if (index < _count) {
        _sensors[index]._pin = pin;
        _sensors[index]._resolution = resolution;
        _sensors[index]._type = type;
    }
}

