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

#define I2C_DISPLAY_ADDRESS            0x3C
#define I2C_SMART_BALL_BUTTONS_ADDRESS 0x19

#define LONG_PRESS_MS 5000
#define LED_BLINK_FAST_MS 100
#define LED_BLINK_SLOW_MS 500

#define MIN_STATE 1
#define MAX_STATE 15

#define BEHAVIOUR_STACK_CAPACITY 10

#define JUNXION_GLOVE_BOARD_ID 51

#define JUNXION_CONNECTION_TIMEOUT 5000

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

