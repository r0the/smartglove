/*
 * Copyright (C) 2017 - 2018 by Stefan Rothe
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

#include "storage.h"
#include <Wire.h>

#if defined(ARDUINO_SAMD_MKR1000)
#define NO_EEPROM
#endif

#if defined(ARDUINO_SAMD_FEATHER_M0)
#define NO_EEPROM
#endif

#if defined(NO_EEPROM)
#define EEPROM_ADDRESS 0x50
#else
#include <EEPROM.h>
#endif

StorageSingleton::StorageSingleton() {
}

void StorageSingleton::writeByte(uint16_t address, uint8_t data) const {
#if defined(NO_EEPROM)
    Wire.beginTransmission(EEPROM_ADDRESS);
    Wire.write(static_cast<uint8_t>(address >> 8));
    Wire.write(static_cast<uint8_t>(address & 0xFF));
    Wire.write(data);
    Wire.endTransmission();
    delay(5); // minimum delay required by 24AA64 EEPROM
#else
    EEPROM.update(address, data);
#endif
}

uint8_t StorageSingleton::readByte(uint16_t address) const {
#if defined(NO_EEPROM)
    Wire.beginTransmission(EEPROM_ADDRESS);
    Wire.write(static_cast<uint8_t>(address >> 8));
    Wire.write(static_cast<uint8_t>(address & 0xFF));
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_ADDRESS, static_cast<uint8_t>(1));
    if (Wire.available()) {
        return Wire.read();
    }
    else {
        return 0;
    }
#else
    return EEPROM.load(address);
#endif
}

StorageSingleton Storage;

