/*
 * Copyright (C) 2015 - 2018 by Stefan Rothe
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

#include "pca9557.h"

#include <Wire.h>

#define REGISTER_INPUT    static_cast<uint8_t>(0x00)
#define REGISTER_OUTPUT   0x01
#define REGISTER_POLARITY 0x02
#define REGISTER_CONFIG   0x03

PCA9557::PCA9557(uint8_t address) : _address(address) {
}

uint8_t PCA9557::readInput() const {
    Wire.beginTransmission(_address);
    Wire.write(REGISTER_INPUT);
    Wire.endTransmission();
    Wire.requestFrom(_address, static_cast<uint8_t>(1));
    if (Wire.available()) {
        return Wire.read();
    }
    else {
        return 0;
    }
}

bool PCA9557::ready() const {
    Wire.beginTransmission(_address);
    return Wire.endTransmission() == 0;
}

void PCA9557::writeConfig(uint8_t config) const {
    Wire.beginTransmission(_address);
    Wire.write(REGISTER_CONFIG);
    Wire.write(config);
    Wire.endTransmission();
}

void PCA9557::writeOutput(uint8_t data) const {
    Wire.beginTransmission(_address);
    Wire.write(REGISTER_OUTPUT);
    Wire.write(data);
    Wire.endTransmission();
}

void PCA9557::writePolarity(uint8_t polarity) const {
    Wire.beginTransmission(_address);
    Wire.write(REGISTER_POLARITY);
    Wire.write(polarity);
    Wire.endTransmission();
}

