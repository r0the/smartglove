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

#include "smart_ball.h"
#include "config.h"

const uint8_t BUTTON_COUNT = 7;
const uint8_t BUTTON_MAP[BUTTON_COUNT] = {
    BUTTON_THUMB_1,
    BUTTON_INDEX_FINGER_1,
    BUTTON_MIDDLE_FINGER_1,
    BUTTON_RING_FINGER_1,
    BUTTON_LITTLE_FINGER_1,
    BUTTON_INDEX_FINGER_2,
    BUTTON_MIDDLE_FINGER_2
};

SmartBall::SmartBall() :
    _buttons(I2C_SMART_BALL_BUTTONS_ADDRESS) {
}


void SmartBall::doSetup() {
    _buttons.writeConfig(0x7F);
    _buttons.writePolarity(0x7F);
}

void SmartBall::doLoop() {
}

uint16_t SmartBall::availableButtonMask() const {
    return
        (1 << BUTTON_THUMB_1) | 
        (1 << BUTTON_INDEX_FINGER_1) | 
        (1 << BUTTON_MIDDLE_FINGER_1) |
        (1 << BUTTON_RING_FINGER_1) |
        (1 << BUTTON_LITTLE_FINGER_1) |
        (1 << BUTTON_INDEX_FINGER_2) |
        (1 << BUTTON_MIDDLE_FINGER_2);
}

uint16_t SmartBall::availableSensorMask() const {
    return
        (1 << SENSOR_ACCEL_X) | 
        (1 << SENSOR_ACCEL_Y) | 
        (1 << SENSOR_ACCEL_Z) |
        (1 << SENSOR_GYRO_ROLL) |
        (1 << SENSOR_GYRO_PITCH) |
        (1 << SENSOR_GYRO_HEADING);
}

uint16_t SmartBall::longPressButtonMask() const {
    return
        (1 << BUTTON_THUMB_1) |
        (1 << BUTTON_LITTLE_FINGER_1);
}

uint16_t SmartBall::readButtonState() const {
    uint16_t result = 0;
    uint8_t buttons = _buttons.readInput();
    for (uint8_t bit = 0; bit < BUTTON_COUNT; ++bit) {
        if (buttons & (1 << bit)) {
            result |= (1 << BUTTON_MAP[bit]);
        }
    }

    return result;
}

void SmartBall::setInfoLed(bool on) {
    _buttons.writeOutput(on ? 0x80 : 0x00);
}

