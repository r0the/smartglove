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

/******************************************************************************
 * class Buttons
 *****************************************************************************/

const uint8_t Buttons::MAX = 12;

Buttons::Buttons() :
    _current(0),
    _last(0),
    _longPress(false),
    _longPressButtons(0),
    _longPressMillis(5000) {
}

bool Buttons::available(uint8_t id) const {
    if (id >= MAX) {
        return false;
    }
    
    return _available & (1 << id);
}

bool Buttons::down(uint8_t id) const {
    if (id >= MAX) {
        return false;
    }

    return pressed(id) && !(_last & (1 << id));
}

bool Buttons::pressed(uint8_t id) const {
    if (id >= MAX) {
        return false;
    }

    return available(id) && (_current & (1 << id));
}

void Buttons::setAvailable(uint16_t mask) {
    _available = mask;
}

void Buttons::setLongPress(uint16_t mask, uint16_t millis) {
    _longPressButtons = mask;
    _longPressMillis = millis;
}

void Buttons::updateState(uint16_t current) {
    unsigned long now = millis();
    _last = _current;
    _current = _available & current;
    if ((_current & _longPressButtons) != _longPressButtons) {
        _longPressEnd = now + _longPressMillis;
    }

    if (_longPressEnd <= now) {
        _longPress = true;
        _longPressEnd = now + _longPressMillis;
    }
    else {
        _longPress = false;
    }
}

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

/******************************************************************************
 * class Sensors
 *****************************************************************************/

const uint8_t Sensors::MAX = 11;

Sensors::Sensors() :
    _sensors(new Sensor[MAX]) {
}

Sensors::~Sensors() {
    delete[] _sensors;
}

void Sensors::addMeasurement(uint8_t id, double value) {
    if (id >= MAX) {
        return;
    }

    _sensors[id].addMeasurement(value);
}

bool Sensors::available(uint8_t id) const {
    if (id >= MAX) {
        return false;
    }

    return _available & (1 << id);
}

void Sensors::setAvailable(uint16_t mask) {
    _available = mask;
}

void Sensors::setOutRange(uint8_t id, uint16_t min, uint16_t max) {
    if (id >= MAX) {
        return;
    }

    _sensors[id].setOutRange(min, max);
}

void Sensors::setRawRange(uint8_t id, double min, double max) {
    if (id >= MAX) {
        return;
    }

    _sensors[id].setRawRange(min, max);
}

uint16_t Sensors::value(uint8_t id) const {
    if (id >= MAX) {
        return 0;
    }

    if (id < MAX) {
        return _sensors[id].value();
    }
}

