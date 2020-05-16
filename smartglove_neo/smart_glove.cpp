/*
 * Copyright (C) 2019 - 2020 by Stefan Rothe
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

#include "smart_glove.h"
#include "config.h"

const uint8_t BUTTON_COUNT = 8;
const uint8_t BUTTON_MAP[BUTTON_COUNT] = {
    BUTTON_INDEX_FINGER_1,
    BUTTON_MIDDLE_FINGER_1,
    BUTTON_RING_FINGER_1,
    BUTTON_LITTLE_FINGER_1,
    BUTTON_THUMB_1,
    BUTTON_THUMB_2,
    BUTTON_THUMB_3,
    BUTTON_THUMB_4
};

SmartGlove::SmartGlove() :
    _ads(false),
    _commandMenu(false),
    _distance(I2C_DISTANCE_ADDRESS),
    _flexIndexFinger(I2C_FLEX_INDEX_FINGER_ADDRESS),
    _flexLittleFinger(I2C_FLEX_LITTLE_FINGER_ADDRESS),
    _flexMiddleFinger(I2C_FLEX_MIDDLE_FINGER_ADDRESS),
    _flexRingFinger(I2C_FLEX_RING_FINGER_ADDRESS),
    _menuTimeoutMs(0),
    _sideButtons(I2C_SMART_GLOVE_SIDE_BUTTONS_ADDRESS),
    _tipButtons(I2C_SMART_GLOVE_TIP_BUTTONS_ADDRESS) {
}

bool SmartGlove::commandCalibrateIMU() const {
    return buttonCombination(BUTTON_INDEX_FINGER_2, BUTTON_MIDDLE_FINGER_2);
}

bool SmartGlove::commandDown() const {
    return buttonDown(BUTTON_THUMB_3);
}

bool SmartGlove::commandEnter() const {
    return buttonDown(BUTTON_THUMB_1);
}

bool SmartGlove::commandMenu() const {
    return _commandMenu;
}

bool SmartGlove::commandUp() const {
    return buttonDown(BUTTON_THUMB_4);
}

bool SmartGlove::flexReady() const {
    return true;
}

void SmartGlove::doSetup() {
    _sideButtons.writeConfig(0xF0);
    _sideButtons.writePolarity(0xF0);
    _tipButtons.writeConfig(0xF0);
    _tipButtons.writePolarity(0xF0);
    _flexIndexFinger.init();
    _flexMiddleFinger.init();
    _flexRingFinger.init();
    _flexLittleFinger.init();
    _distance.init();
}

void SmartGlove::doLoop() {
    unsigned long now = millis();
    _commandMenu = false;

    if (_flexIndexFinger.present()) {
        _sensors.addMeasurement(now, SENSOR_FLEX_INDEX_FINGER, _flexIndexFinger.readInput());
    }

    if (_flexMiddleFinger.present()) {
        _sensors.addMeasurement(now, SENSOR_FLEX_MIDDLE_FINGER, _flexMiddleFinger.readInput());
    }

    if (_flexRingFinger.present()) {
        _sensors.addMeasurement(now, SENSOR_FLEX_RING_FINGER, _flexRingFinger.readInput());
    }

    if (_flexLittleFinger.present()) {
        _sensors.addMeasurement(now, SENSOR_FLEX_LITTLE_FINGER, _flexLittleFinger.readInput());
    }

    if (_distance.dataReady()) {
        _sensors.addMeasurement(now, SENSOR_DISTANCE, _distance.readInput());
    }

    if (buttonCombination(BUTTON_THUMB_1, BUTTON_LITTLE_FINGER_1)) {
        _menuTimeoutMs = now + LONG_PRESS_MS;
    }

    if (buttonPressed(BUTTON_THUMB_1)) {
        if (_menuTimeoutMs < now) {
            _commandMenu = true;
            _menuTimeoutMs = now + LONG_PRESS_MS;
        }
    }
    else {
        _menuTimeoutMs = now + LONG_PRESS_MS;
    }
}

uint16_t SmartGlove::availableButtonMask() const {
    return
        (1 << BUTTON_THUMB_1) | 
        (1 << BUTTON_THUMB_2) | 
        (1 << BUTTON_THUMB_3) | 
        (1 << BUTTON_THUMB_4) | 
        (1 << BUTTON_INDEX_FINGER_1) | 
        (1 << BUTTON_MIDDLE_FINGER_1) |
        (1 << BUTTON_RING_FINGER_1) |
        (1 << BUTTON_LITTLE_FINGER_1);
}

uint16_t SmartGlove::availableSensorMask() const {
    return
        (1 << SENSOR_ACCEL_X) | 
        (1 << SENSOR_ACCEL_Y) | 
        (1 << SENSOR_ACCEL_Z) |
        (1 << SENSOR_DISTANCE) |
        (1 << SENSOR_GYRO_ROLL) |
        (1 << SENSOR_GYRO_PITCH) |
        (1 << SENSOR_GYRO_HEADING) |
        (1 << SENSOR_FLEX_INDEX_FINGER) |
        (1 << SENSOR_FLEX_MIDDLE_FINGER) |
        (1 << SENSOR_FLEX_RING_FINGER) |
        (1 << SENSOR_FLEX_LITTLE_FINGER);
}

uint16_t SmartGlove::readButtonState() const {
    uint16_t result = 0;
    uint8_t buttons = (_sideButtons.readInput() & 0xF0) | ((_tipButtons.readInput() & 0xF0) >> 4);
    for (uint8_t bit = 0; bit < BUTTON_COUNT; ++bit) {
        if (buttons & (1 << bit)) {
            result |= (1 << BUTTON_MAP[bit]);
        }
    }

    _sideButtons.writeOutput(~(buttons >> 4));
    _tipButtons.writeOutput(~(buttons & 0x0F));
    return result;
}

void SmartGlove::setInfoLED(bool on) {
}
