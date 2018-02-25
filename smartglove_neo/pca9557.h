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

#ifndef PCA9557_H
#define PCA9557_H

#include <Arduino.h>

class PCA9557 {
public:
    PCA9557(uint8_t address);

    /**
     * Reads a byte from the input register.
     */
    uint8_t readInput() const;

    /**
     * Checks if the chip is responding.
     */
    bool ready() const;

    /**
     * Writes to the config register. The config register defines the input/output
     * direction of the I/O pins. A set bit configures a pin as input, a cleared 
     * bit as output.
     */
    void writeConfig(uint8_t config) const;

    /**
     * Writes a byte to the output register. Depending on the polarity register
     * a set bit either sets the output to VCC or to GND.
     */
    void writeOutput(uint8_t data) const;

    /**
     * Writes to the polarity register. The polarity register defines the polarity
     * of the output. If the corresponding polarity bit is set, the polarity
     * is inverted, i.e. the set bit sets the output to GND instead of VCC.
     */
    void writePolarity(uint8_t polarity) const;

private:
    uint8_t _address;
};

#endif

