# SmartGlove NEO

## Usage SmartBall

The following buttons and button combinations have a special function:

| Button(s)                                      | Function             |
|:---------------------------------------------- |:-------------------- |
| Thumb 1                                        | Enter/Select in menu |
| Index Finger 1                                 | Menu navigation up   |
| Middle Finger 1                                | Menu navigation down |
| long press (> 5 s) Thumb 1 and Little Finger 1 | Open menu            |
| Index Finger 2 and Middle Finger 2             | Calibrate IMU        |

## Usage SmartGlove

The following buttons and button combinations have a special function:

| Button(s)                          | Function             |
|:---------------------------------- |:-------------------- |
| Thumb 1                            | Enter/Select in menu |
| Thumb 4                            | Menu navigation up   |
| Thumb 3                            | Menu navigation down |
| long press (> 5 s) Thumb 4         | Open menu            |
| Index Finger 2 and Middle Finger 2 | Calibrate IMU        |

## Hardware

### I2C Addresses SmartBall

| Chip    | Function                  | Fixed Address Part | Variable Address part | Address Hex |
|:------- |:------------------------- |:------------------ |:--------------------- |:----------- |
| PCA9557 | buttons and LED           | `0011`             | `001`                 | `0x19`      |
| BNO055  | inertial measurement unit | `010100`           | `1`                   | `0x29`      |
| SSD1306 | dot matrix display        | `011110`           | `0`                   | `0x3C`      |
| 24AA64  | EEPROM                    | `01010`            | `000`                 | `0x50`      |

### I2C Addresses SmartGlove

| Chip    | Function                  | Fixed Address Part | Variable Address part | Address Hex |
|:------- |:------------------------- |:------------------ |:--------------------- |:----------- |
|         | flex sensor index finger  |                    | `0010010`             | `0x12`      |
|         | flex sensor middle finger |                    | `0010011`             | `0x13`      |
|         | flex sensor ring finger   |                    | `0010100`             | `0x14`      |
|         | flex sensor little finger |                    | `0010101`             | `0x15`      |
| PCA9557 | side buttons and LED      | `0011`             | `000`                 | `0x18`      |
| PCA9557 | tip buttons and LED       | `0011`             | `001`                 | `0x19`      |
| BNO055  | inertial measurement unit | `010100`           | `0`                   | `0x28`      |
| VL52L1X | distance measurement      |                    |                       | `0x29`      |
| SSD1306 | dot matrix display        | `011110`           | `0`                   | `0x3C`      |
| 24AA64  | EEPROM                    | `01010`            | `000`                 | `0x50`      |

## IDE Setup

1. Install [the Arduino IDE][1].
2. In the Arduino IDE preferences, add the following **Additional Board Manager URL**:

   `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`

3. Install the **Adafruit SAMD Boards** boards manager (menu Tools/Board/Boards Manager).
4. Install the following libraries (menu Sketch/Include Library/Manage Libraries):
   - **Adafruit Unified Sensor**
   - **Adafruit BNO055**
   - **Adafruit NeoPixel**
   - **VL53L1X**
5. Select the board **Adafruit Feather M0**.

[1]: https://www.arduino.cc/en/Main/Software
