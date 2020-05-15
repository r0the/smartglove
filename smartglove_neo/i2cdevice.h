/*
 * Copyright (C) 2020 by Stefan Rothe
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

#ifndef I2CDEVICE_H
#define I2CDEVICE_H

#include <Arduino.h>
#include <Wire.h>

class I2CDevice {
public:
    I2CDevice(uint8_t address) : 
      _address(address) {
    }

    bool present() {
        beginTransmission();
        return endTransmission();
    }
protected:
    inline void beginTransmission() const {
        Wire.beginTransmission(_address);
    }
  
    inline void write(uint8_t data) const {
        Wire.write(data);
    }

    inline void write16(uint16_t data) const {
        Wire.write(static_cast<uint8_t>((data >> 8) & 0xFF));
        Wire.write(static_cast<uint8_t>(data & 0xFF));
    }

    inline void write32(uint32_t data) const {
        Wire.write(static_cast<uint8_t>((data >> 24) & 0xFF));
        Wire.write(static_cast<uint8_t>((data >> 16) & 0xFF));
        Wire.write(static_cast<uint8_t>((data >> 8) & 0xFF));
        Wire.write(static_cast<uint8_t>(data & 0xFF));
    }
    
    inline bool endTransmission() const {
        return Wire.endTransmission() == 0;
    }
    
    inline void requestData(uint8_t bytes) const {
        Wire.requestFrom(_address, bytes);
    }
    
    inline uint8_t read() const {
        return Wire.read();
    }
    
    inline uint16_t read16() const {
        uint16_t value  = static_cast<uint16_t>(Wire.read()) << 8;
        value |= Wire.read();
        return value;
    }
private:
    uint8_t _address;
};

#endif
