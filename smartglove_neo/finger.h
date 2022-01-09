/*
 * Copyright (C) 2020 - 2022 by Stefan Rothe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, eithe4r version 3 of the License, or
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

#ifndef FINGER_H
#define FINGER_H

#include <Arduino.h>

class Adafruit_NeoPixel;

// ----------------------------------------------------------------------------
// class Finger
// ----------------------------------------------------------------------------

class Finger {
public:
    Finger(uint8_t flexSensorPin, uint8_t neoPixelPin);
    void init();
    float readFlex();
    void setNeoPixel(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
private:
    uint8_t _flexSensorPin;
    Adafruit_NeoPixel* _neopixel;
    void fill(uint32_t color);
};

#endif
