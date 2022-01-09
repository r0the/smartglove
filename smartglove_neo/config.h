/*
 * Copyright (C) 2018 - 2022 by Stefan Rothe
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

//#define DEBUG

#define VERSION "v5.3"

#define VERSION_MAJOR 5
#define VERSION_MINOR 3

#define I2C_SMART_GLOVE_SIDE_BUTTONS_ADDRESS 0x18
#define I2C_SMART_GLOVE_TIP_BUTTONS_ADDRESS  0x19
#define I2C_SMART_BALL_BUTTONS_ADDRESS       0x19
#define I2C_IMU_ADDRESS                      0x28
#define I2C_DISTANCE_ADDRESS                 0x29
#define I2C_DISPLAY_ADDRESS                  0x3C

#define INDEX_FINGER_FLEX_PIN A0
#define MIDDLE_FINGER_FLEX_PIN A1
#define RING_FINGER_FLEX_PIN A2
#define LITTLE_FINGER_FLEX_PIN A3

#define INDEX_FINGER_NEOPIXEL_PIN 18
#define MIDDLE_FINGER_NEOPIXEL_PIN 19
#define RING_FINGER_NEOPIXEL_PIN 11
#define LITTLE_FINGER_NEOPIXEL_PIN 12

#define LONG_PRESS_MS 5000
#define LED_BLINK_FAST_MS 100
#define LED_BLINK_SLOW_MS 500

#define MIN_STATE 1
#define MAX_STATE 15

#define BEHAVIOUR_STACK_CAPACITY 10

#define STORAGE_BOARD_ID 2
#define STORAGE_SHOW_FRAMERATE 3
#define STORAGE_PROTOCOL 4

#define SERIAL_CHECK_INTERVAL_MS 500

#define GESTURE_TIMEOUT_MS 300

#ifdef DEBUG

void printByte(uint8_t data);


#define DEBUG_INIT() Serial1.begin(9600); while (!Serial) { delay(1); } Serial.println("Starting Debug Session...");
#define PRINTLN(MSG) Serial1.println(MSG);
#define PRINT(MSG) Serial1.print(MSG);
#define PRINT_HEX(B) Serial1.print(B, HEX);

#else

#define DEBUG_INIT()
#define PRINTLN(MSG)
#define PRINT(MSG)
#define PRINT_BYTE(B)

#endif
