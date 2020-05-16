/*
 * Copyright (C) 2017 - 2020 by Stefan Rothe
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

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

class StorageSingleton {
public:
    StorageSingleton();
    void writeByte(uint16_t address, uint8_t data) const;
    uint8_t readByte(uint16_t address) const;
private:
    StorageSingleton(const StorageSingleton&);
    StorageSingleton operator=(const StorageSingleton&);
};

extern StorageSingleton Storage;

#endif
