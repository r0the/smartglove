/*
 * Copyright (C) 2017 - 2022 by Stefan Rothe
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

#define STORAGE_BOARD_ID 2
#define STORAGE_SHOW_FRAMERATE 3
#define STORAGE_PROTOCOL 4

class StorageSingleton {
public:
    StorageSingleton();
    inline uint8_t boardId() const { return readByte(STORAGE_BOARD_ID); };
    void setBoardId(uint8_t value) { writeByte(STORAGE_BOARD_ID, value); }
    inline uint8_t protocol() const { return readByte(STORAGE_PROTOCOL); };
    void setProtocol(uint8_t value) { writeByte(STORAGE_PROTOCOL, value); }
    inline uint8_t showFramerate() const { return readByte(STORAGE_SHOW_FRAMERATE); };
    void setShowFramerate(uint8_t value) { writeByte(STORAGE_SHOW_FRAMERATE, value); }
private:
    StorageSingleton(const StorageSingleton&);
    StorageSingleton operator=(const StorageSingleton&);

    void writeByte(uint16_t address, uint8_t data) const;
    uint8_t readByte(uint16_t address) const;
};

extern StorageSingleton Storage;

#endif
