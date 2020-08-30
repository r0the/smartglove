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

#include "ads.h"

#define ADS_RUN           0x00
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

#define ADS_CALIBRATE_FIRST 0
#define ADS_CALIBRATE_FLAT 1


inline int16_t ads_int16_decode(const uint8_t* p_encoded_data) {
    return ((((uint16_t)(p_encoded_data)[0])) | (((int16_t)(p_encoded_data)[1]) << 8));
}

ADS::ADS(uint8_t address) :
    I2CDevice(address) {
}

void ADS::calibrateX() const {
    writeCommand(ADS_CALIBRATE,  ADS_CALIBRATE_FLAT, 90); 
}

void ADS::calibrateZero() const {
    writeCommand(ADS_CALIBRATE,  ADS_CALIBRATE_FIRST, 0); 
}

void ADS::init() const {
    reset();
    delay(50);
    setSampleRate(ADS_100_HZ);
    setPollMode(true);
}

float ADS::readInput() const {
    uint8_t buffer[2];
    requestData(3);
    uint8_t type = read();
    int16_t value = read16i();
    if (type == ADS_SAMPLE) {
        return value / 64.0f;
    }
    return 0;
}

void ADS::reset() const {
    writeCommand(ADS_RESET);  
}

void ADS::setPollMode(bool enable) const {
    writeCommand(ADS_POLL, enable);
}

void ADS::setSampleRate(uint16_t sps) const {
    uint8_t low = static_cast<uint8_t>(sps & 0xFF);
    uint8_t high = static_cast<uint8_t>((sps >> 8) & 0xFF); 
    writeCommand(ADS_SPS, low, high);
}

bool ADS::writeCommand(uint8_t command, uint8_t param1, uint8_t param2) const {
    beginTransmission();
    write(command);
    write(param1);
    write(param2);
    return endTransmission();
}
