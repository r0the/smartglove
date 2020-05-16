/*
 * Copyright (C) 2016 - 2020 by Stefan Rothe
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
#include "config.h"

/******************************************************************************
 * class Buttons
 *****************************************************************************/

const uint8_t Buttons::COUNT = 12;

Buttons::Buttons() :
    _current(0),
    _last(0) {
}

bool Buttons::available(uint8_t id) const {
    if (id >= COUNT) {
        return false;
    }

    return _available & (1 << id);
}

bool Buttons::down(uint8_t id) const {
    if (id >= COUNT) {
        return false;
    }

    return pressed(id) && !(_last & (1 << id));
}

bool Buttons::pressed(uint8_t id) const {
    if (id >= COUNT) {
        return false;
    }

    return available(id) && (_current & (1 << id));
}

void Buttons::setAvailable(uint16_t mask) {
    _available = mask;
}

void Buttons::updateState(uint16_t current) {
    unsigned long now = millis();
    _last = _current;
    _current = _available & current;
}

/******************************************************************************
 * class Sensor
 *****************************************************************************/

#define GESTURE_UP 0x01
#define GESTURE_DOWN 0x02

class Sensor {
    friend class Sensors;
public:
    Sensor();
    bool activity() const;
    void addMeasurement(unsigned long time, double value);
    void configure(double min, double max, double minStdDev);
    bool gestureDetected(uint8_t gestureMask) const;
    uint16_t value() const;
private:
    Sensor(const Sensor&);
    Sensor& operator=(const Sensor&);

    static const uint16_t GESTURE_THRESOLD;
    static const uint16_t MAX_VALUE;
    static const uint16_t ZERO_VALUE;
    static const uint8_t VALUE_COUNT;
    bool _activity;
    uint64_t _activityThreshold;
    double _factor;
    uint8_t _gesture;
    unsigned long _gestureTimeout;
    uint64_t _mean;
    uint8_t _pos;
    double _rawMax;
    double _rawMin;
    unsigned long _timeMax;
    unsigned long _timeMin;
    char _type;
    uint16_t _value;
    uint16_t _valueMax;
    uint16_t _valueMin;
    uint16_t* _values;
    int64_t _variance;
};

const uint16_t Sensor::GESTURE_THRESOLD = 10;
const uint16_t Sensor::MAX_VALUE = 0xFFFF;
const uint16_t Sensor::ZERO_VALUE = 0x7FFF;
const uint8_t Sensor::VALUE_COUNT = 16;

Sensor::Sensor() :
    _activity(false),
    _activityThreshold(0),
    _factor(1.0),
    _gesture(0),
    _gestureTimeout(0),
    _pos(0),
    _timeMin(0),
    _timeMax(0),
    _rawMax(1.0),
    _rawMin(-1.0),
    _value(ZERO_VALUE),
    _valueMax(MAX_VALUE),
    _valueMin(0),
    _values(new uint16_t[VALUE_COUNT]) {
    for (int i = 0; i < VALUE_COUNT; ++i) {
        _values[i] = ZERO_VALUE;
    }
}

bool Sensor::activity() const {
    return _activity;
}

void Sensor::addMeasurement(unsigned long time, double value) {
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

    // scale raw measurement
    uint16_t currentValue = (value - _rawMin) * _factor;
    // add measurement to ring buffer
    _pos = (_pos + 1) % VALUE_COUNT;
    _values[_pos] = currentValue;

    // calculate variance for activity detection
    uint64_t sum = 0;
    uint64_t sumOfSquares = 0;
    for (uint8_t i = 0; i < VALUE_COUNT; ++i) {
        uint64_t val = _values[i];
        sum += val;
        sumOfSquares += val * val;
    }

    _mean = sum / VALUE_COUNT;
    _variance = sumOfSquares / VALUE_COUNT - _mean * _mean;
    _activity = _activityThreshold < _variance;
    if (!_activity) {
        _gesture = 0;
        _gestureTimeout = 0;
        return;
    }

    _value = currentValue;

    if (time < _gestureTimeout) {
        // still waiting for current gesture to time out
        return;
    }

    if (_gesture) {
        // last gesture has timed out, reset
        _gesture = 0;
        _timeMax = time;
        _timeMin = time;
        _valueMax = currentValue;
        _valueMin = currentValue;
    }
    else {
        // update min and max values
        if (currentValue < _valueMin) {
            _timeMin = time;
            _valueMin = currentValue;
        }

        if (_valueMax < currentValue) {
            _timeMax = time;
            _valueMax = currentValue;
        }
    }

    // check for gesture
    if (_valueMin < GESTURE_THRESOLD && _valueMax > MAX_VALUE - GESTURE_THRESOLD) {
        // gesture detected
        _gesture = _timeMin < _timeMax ? GESTURE_UP : GESTURE_DOWN;
        _gestureTimeout = time + GESTURE_TIMEOUT_MS;
    }
}

void Sensor::configure(double min, double max, double minStdDev) {
    _rawMin = min;
    _rawMax = max;
    _factor = static_cast<double>(MAX_VALUE) / (_rawMax - _rawMin);
    _activityThreshold = minStdDev * abs(_factor);
    _activityThreshold *= _activityThreshold;
}

bool Sensor::gestureDetected(uint8_t gestureMask) const {
    return (_gesture & gestureMask) == gestureMask;
}

uint16_t Sensor::value() const {
    return _value;
}

/******************************************************************************
 * class Sensors
 *****************************************************************************/

const uint8_t Sensors::COUNT = 11;
const uint8_t Sensors::GESTURE_COUNT = 4;

Sensors::Sensors() :
    _gestures(0),
    _sensors(new Sensor[COUNT]) {
}

Sensors::~Sensors() {
    delete[] _sensors;
}

bool Sensors::activity(uint8_t id) const {
    if (id >= COUNT) {
        return 0;
    }

    return _sensors[id].activity();
}

void Sensors::addMeasurement(unsigned long time, uint8_t id, double value) {
    if (id >= COUNT) {
        return;
    }

    _sensors[id].addMeasurement(time, value);
}

bool Sensors::available(uint8_t id) const {
    if (id >= COUNT) {
        return false;
    }

    return _available & (1 << id);
}

void Sensors::configure(uint8_t id, double min, double max, double minStdDev) {
    if (id >= COUNT) {
        return;
    }

    _sensors[id].configure(min, max, minStdDev);
}

bool Sensors::gestureAvailable(uint8_t id) const {
    return id < GESTURE_COUNT;
}

bool Sensors::gestureDetected(uint8_t id) const {
    switch (id) {
        case GESTURE_WAVE_LEFT:
            return _sensors[SENSOR_ACCEL_Y].gestureDetected(GESTURE_UP);
        case GESTURE_WAVE_RIGHT:
            return _sensors[SENSOR_ACCEL_Y].gestureDetected(GESTURE_DOWN);
        case GESTURE_WAVE_UP:
            return _sensors[SENSOR_ACCEL_Z].gestureDetected(GESTURE_UP);
        case GESTURE_WAVE_DOWN:
            return _sensors[SENSOR_ACCEL_Z].gestureDetected(GESTURE_DOWN);
        default:
            return false;
    }
}

uint16_t Sensors::maxValue(uint8_t id) const {
    if (id >= COUNT) {
        return 0;
    }

    return _sensors[id]._valueMax;
}

uint16_t Sensors::minValue(uint8_t id) const {
    if (id >= COUNT) {
        return 0;
    }

    return _sensors[id]._valueMin;
}

void Sensors::setAvailable(uint16_t mask) {
    _available = mask;
}

uint16_t Sensors::value(uint8_t id) const {
    if (id >= COUNT) {
        return 0;
    }

    return _sensors[id].value();
}
