/*
 * Copyright (C) 2015 - 2020 by Stefan Rothe
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

#define REGISTER_INPUT    0x00
#define REGISTER_OUTPUT   0x01
#define REGISTER_POLARITY 0x02
#define REGISTER_CONFIG   0x03

PCA9557::PCA9557(uint8_t address) :
    I2CDevice(address) {
}

uint8_t PCA9557::readInput() const {
    beginTransmission();
    write(REGISTER_INPUT);
    endTransmission();
    requestData(1);
    return read();
}

void PCA9557::writeConfig(uint8_t config) const {
    beginTransmission();
    write(REGISTER_CONFIG);
    write(config);
    endTransmission();
}

void PCA9557::writeOutput(uint8_t data) const {
    beginTransmission();
    write(REGISTER_OUTPUT);
    write(data);
    endTransmission();
}

void PCA9557::writePolarity(uint8_t polarity) const {
    beginTransmission();
    write(REGISTER_POLARITY);
    write(polarity);
    endTransmission();
}
