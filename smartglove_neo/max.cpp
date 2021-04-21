/*
 * Copyright (C) 2021 by Stefan Rothe
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

#include "max.h"
#include "config.h"
#include "behaviour.h"

#define FIRMATA_BAUD_RATE 57600

#define START_SYSEX 0xF0
#define END_SYSEX 0xF7

#define REPORT_FIRMWARE 0x79 

Max::Max(SmartDevice& device) :
    Behaviour(device),
    _serialConnected(false),
    _serialCheckMs(0)
{
}

void Max::setup() {
    device.display().setTextAlign(ALIGN_LEFT);
    device.display().setFont(&HELVETICA_10);
}

void Max::loop() {
    if (device.commandMenu()) {
        device.pushBehaviour(new MainMenu(device));
    }

    if (device.commandCalibrateIMU()) {
        device.resetIMU();
    }

    unsigned long now = millis();
    if (_serialCheckMs < now) {
        // Checking the Serial connection takes a long time, so it shouldn't be
        // done too often.
        _serialConnected = static_cast<bool>(Serial);
        if (!_serialConnected) {
            Serial.begin(FIRMATA_BAUD_RATE);
            reportFirmware();
        }

        _serialCheckMs = now + SERIAL_CHECK_INTERVAL_MS;
    }

    if (!_serialConnected) {
        device.display().setTextAlign(ALIGN_LEFT);
        device.display().setFont(&HELVETICA_10);
        device.display().drawText(10, 8, "Waiting for");
        device.display().drawText(10, 22, "Firmata connection...");
        return;
    }

    if (Serial.available()) {
        receive();
    }

    sendDigital();
}

void Max::receive() {
    uint8_t command = Serial.read();
    switch (command) {
        case START_SYSEX:
            Max::sysex();
            break;
    }
}

void Max::sysex() {
    uint8_t command = Serial.read();
    switch (command) {
        case REPORT_FIRMWARE:
            sysexEnd();
            reportFirmware();
            break;
    }
}

void Max::sysexEnd() {
    while (Serial.available() > 0 && Serial.read() != END_SYSEX);
}

void Max::reportFirmware() {
    Serial.write(START_SYSEX);
    Serial.write(REPORT_FIRMWARE);
    Serial.write(VERSION_MAJOR);
    Serial.write(VERSION_MINOR);
    Serial.print("Smartglove");
    Serial.write(END_SYSEX);
}

void Max::sendDigital() {
    // Thumb buttons
    Serial.write(0x90);
    uint8_t data1 = device.buttonPressed(BUTTON_THUMB_1) | 
                    (device.buttonPressed(BUTTON_THUMB_2) << 1) |
                    (device.buttonPressed(BUTTON_THUMB_3) << 2) |
                    (device.buttonPressed(BUTTON_THUMB_4) << 3);
    Serial.write(data1);
    Serial.write(0x00);

    Serial.write(0x91);
    uint8_t data1 = device.buttonPressed(BUTTON_THUMB_1) | 
                    (device.buttonPressed(BUTTON_THUMB_2) << 1) |
                    (device.buttonPressed(BUTTON_THUMB_3) << 2) |
                    (device.buttonPressed(BUTTON_THUMB_4) << 3);
    Serial.write(data1);
    Serial.write(0x00);
}
