/*
 * Copyright (C) 2018 by Stefan Rothe
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

#define VERSION "v3.0"

#define I2C_DISPLAY_ADDRESS                  0x3C
#define I2C_IMU_ADDRESS                      0x29
#define I2C_SMART_BALL_BUTTONS_ADDRESS       0x19
#define I2C_SMART_GLOVE_SIDE_BUTTONS_ADDRESS 0x18
#define I2C_SMART_GLOVE_TIP_BUTTONS_ADDRESS  0x19
#define I2C_FLEX_INDEX_FINGER_ADDRESS        0x12
#define I2C_FLEX_MIDDLE_FINGER_ADDRESS       0x13
#define I2C_FLEX_RING_FINGER_ADDRESS         0x14
#define I2C_FLEX_LITTLE_FINGER_ADDRESS       0x15

#define LONG_PRESS_MS 5000
#define LED_BLINK_FAST_MS 100
#define LED_BLINK_SLOW_MS 500

#define MIN_STATE 1
#define MAX_STATE 15

#define BEHAVIOUR_STACK_CAPACITY 10

#define STORAGE_BOARD_ID 2
#define STORAGE_SHOW_FRAMERATE 3

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
