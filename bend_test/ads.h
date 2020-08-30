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

#ifndef ADS_H
#define ADS_H

#define ADS_100_HZ 163

#include "i2cdevice.h"

class ADS : public I2CDevice {
public:
    ADS(uint8_t address);

    /**
     * Calibrates the sensor.
     */
    void calibrateZero() const;

    /**
     * 
     */
    void calibrateX() const;

    /**
     * Initializes the sensor.
     */
    void init() const;

    /**
     * Reads a value from the sensor.
     */
    float readInput() const;

    /**
     * Resets the sensor.
     */
    void reset() const;

    /**
     * Enables or disables the poll mode.
     */
    void setPollMode(bool enable) const;

     /**
      * Sets the sample rate.
      */
    void setSampleRate(uint16_t sps) const;
private:
    bool writeCommand(uint8_t command, uint8_t param1 = 0, uint8_t param2 = 0) const;
 
    inline int16_t read16i() const {
        int16_t value = Wire.read();
        value |= static_cast<int16_t>(Wire.read()) << 8;
        return value;
    }
};

#endif
