/*
 * Copyright (C) 2019 - 2022 by Stefan Rothe
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
    BUTTON_THUMB_4,
    BUTTON_THUMB_3,
    BUTTON_THUMB_2,
    BUTTON_THUMB_1
};

SmartGlove::SmartGlove() :
    _ads(false),
    _commandMenu(false),
    _distance(I2C_DISTANCE_ADDRESS),
    _indexFinger(INDEX_FINGER_FLEX_PIN, INDEX_FINGER_NEOPIXEL_PIN),
    _middleFinger(MIDDLE_FINGER_FLEX_PIN, MIDDLE_FINGER_NEOPIXEL_PIN),
    _ringFinger(RING_FINGER_FLEX_PIN, RING_FINGER_NEOPIXEL_PIN),
    _littleFinger(LITTLE_FINGER_FLEX_PIN, LITTLE_FINGER_NEOPIXEL_PIN),
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

void SmartGlove::setNeoPixel(uint8_t fingerIndex, uint8_t pixelIndex, uint8_t red, uint8_t green, uint8_t blue) {
    switch (fingerIndex) {
    case 2:
          _indexFinger.setNeoPixel(pixelIndex, red, green, blue);
          break;
    case 3:
        _middleFinger.setNeoPixel(pixelIndex, red, green, blue);
        break;
    case 4:
        _ringFinger.setNeoPixel(pixelIndex, red, green, blue);
        break;
    case 5:
        _littleFinger.setNeoPixel(pixelIndex, red, green, blue);
        break;
    }
}

void SmartGlove::doSetup() {
    configureSensor(SENSOR_FLEX_INDEX_FINGER, 0, 500, 2.0);
    configureSensor(SENSOR_FLEX_MIDDLE_FINGER, 0, 500, 2.0);
    configureSensor(SENSOR_FLEX_RING_FINGER, 0, 500, 2.0);
    configureSensor(SENSOR_FLEX_LITTLE_FINGER, 0, 500, 2.0);

    _sideButtons.writeConfig(0xF0);
    _sideButtons.writePolarity(0xF0);
    _tipButtons.writeConfig(0xF0);
    _tipButtons.writePolarity(0xF0);
    _indexFinger.init();
    _middleFinger.init();
    _ringFinger.init();
    _littleFinger.init();
    _distance.init();
}

void SmartGlove::doLoop() {
    unsigned long now = millis();
    _commandMenu = false;

    _sensors.addMeasurement(now, SENSOR_FLEX_INDEX_FINGER, _indexFinger.readFlex());
    _sensors.addMeasurement(now, SENSOR_FLEX_MIDDLE_FINGER, _middleFinger.readFlex());
    _sensors.addMeasurement(now, SENSOR_FLEX_RING_FINGER, _ringFinger.readFlex());
    _sensors.addMeasurement(now, SENSOR_FLEX_LITTLE_FINGER, _littleFinger.readFlex());

    if (_distance.dataReady()) {
        _sensors.addMeasurement(now, SENSOR_DISTANCE, _distance.readInput());
    }

    if (buttonCombination(BUTTON_THUMB_1, BUTTON_THUMB_2)) {
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
