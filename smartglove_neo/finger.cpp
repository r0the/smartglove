/*
 * Copyright (C) 2020 - 2022 by Stefan Rothe
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

#include "finger.h"
#include "config.h"
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_COUNT 10

Finger::Finger(uint8_t flexSensorPin, uint8_t neoPixelPin) :
    _flexSensorPin(flexSensorPin),
    _neopixel(new Adafruit_NeoPixel(NEOPIXEL_COUNT, neoPixelPin, NEO_GRB + NEO_KHZ800))
{
}

void Finger::init() {
  _neopixel->begin();
  fill(Adafruit_NeoPixel::Color(0, 0, 0));
}

float Finger::readFlex() {
    return analogRead(_flexSensorPin);
}

void Finger::setNeoPixel(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t color = Adafruit_NeoPixel::Color(red, green, blue);
    if (index == 0) {
        fill(color);
    }
    else {
        --index;
        if (index < NEOPIXEL_COUNT) {
            _neopixel->setPixelColor(index, color);
        }
    }
    _neopixel->show();
}

void Finger::fill(uint32_t color) {
    for (uint8_t i = 0; i < NEOPIXEL_COUNT; ++i) {
        _neopixel->setPixelColor(i, color);
    }
    _neopixel->show();
}
