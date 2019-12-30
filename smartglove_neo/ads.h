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

 #ifndef ADS_H
 #define ADS_H

 #include <Arduino.h>

 class ADS {
 public:
     ADS(uint8_t address);

     /**
      * Initializes the sensor.
      */
     void init() const;

     /**
      * Reads a value from the sensor.
      */
     float readInput() const;

     /**
      * Enables or disables the poll mode.
      */
     void setPollMode(bool enable) const;

     /**
      * Checks if the chip is responding.
      */
     bool ready() const;

 private:
     uint8_t _address;
     float decode(uint8_t low, uint8_t high) const;
     bool readBuffer(uint8_t* buffer, uint8_t len) const;
     bool writeCommand(uint8_t command, uint8_t param1, uint8_t param2 = 0) const;
 };

 #endif
