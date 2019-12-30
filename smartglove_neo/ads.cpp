/*
 * Copyright (C) 2019 by Stefan Rothe
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

#include "ads.h"
#include <Wire.h>

#define ADS_RUN           static_cast<uint8_t>(0x00)
#define ADS_SPS           0x01
#define ADS_RESET         0x02
#define ADS_DFU           0x03
#define ADS_SET_ADDRESS   0x04
#define ADS_POLL          0x05
#define ADS_GET_FW_VER    0x06
#define ADS_CALIBRATE     0x07
#define ADS_AXES_ENABLED  0x08
#define ADS_SHUTDOWN      0x09
#define ADS_GET_DEV_ID    0x0A

#define ADS_SAMPLE        0x00

#define DATA_SIZE         3
#define COMMAND_SIZE      3

inline int16_t ads_int16_decode(const uint8_t* p_encoded_data) {
    return ((((uint16_t)(p_encoded_data)[0])) | (((int16_t)(p_encoded_data)[1]) << 8));
}

ADS::ADS(uint8_t address) : _address(address) {
}

float ADS::readInput() const {
    uint8_t buffer[3];
    if (!readBuffer(buffer, DATA_SIZE)) {
        return 0;
    }

    if (buffer[0] == ADS_SAMPLE) {
        return (float) ads_int16_decode(buffer + 1) / 64.0f;
    }
    return 0;
}

void ADS::init() const {
    Wire.beginTransmission(_address);
    Wire.write(ADS_POLL);
    Wire.write(0x01); // enable poll mode
    Wire.write(0); // fill
    Wire.endTransmission();
}

bool ADS::ready() const {
    Wire.beginTransmission(_address);
    return Wire.endTransmission() == 0;
}

void ADS::setPollMode(bool enable) const {
    writeCommand(ADS_POLL, enable);
}

bool ADS::readBuffer(uint8_t* buffer, uint8_t len) const {
     Wire.requestFrom(_address, len);
     uint8_t pos = 0;
     while (Wire.available()) {
        if (pos < len) {
            buffer[pos] = Wire.read();
            ++pos;
        }
     }

     return pos == len;
}

bool ADS::writeCommand(uint8_t command, uint8_t param1, uint8_t param2) const {
    uint8_t buffer[COMMAND_SIZE];
    buffer[0] = command;
    buffer[1] = param1;
    buffer[2] = param2;
    Wire.beginTransmission(_address);
    uint8_t written = Wire.write(buffer, COMMAND_SIZE);
    return Wire.endTransmission() == 0 && written == COMMAND_SIZE;
}
