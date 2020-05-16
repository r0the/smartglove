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
| Thumb 3                            | Menu navigation up   |
| Thumb 4                            | Menu navigation down |
| long press (> 5 s) Thumb 1         | Open menu            |
| Index Finger 2 and Middle Finger 2 | Calibrate IMU        |

## junXion Interface

### Digital Outputs

SmartGlove devices support up to 12 buttons:

| Description     |  ID | Bit Mask | junXion Pin |
|:--------------- | ---:|:-------- |:----------- |
| Thumb 1         |   0 | 0x0001   | Digital 0   |
| Thumb 2         |   1 | 0x0002   | Digital 1   |
| Thumb 3         |   2 | 0x0004   | Digital 2   |
| Thumb 4         |   3 | 0x0008   | Digital 3   |
| Index Finger 1  |   4 | 0x0010   | Digital 4   |
| Middle Finger 1 |   5 | 0x0020   | Digital 5   |
| Ring Finger 1   |   6 | 0x0040   | Digital 6   |
| Little Finger 1 |   7 | 0x0080   | Digital 7   |
| Index Finger 2  |   8 | 0x0100   | Digital 12  |
| Middle Finger 2 |   9 | 0x0200   | Digital 13  |
| Ring Finger 2   |  10 | 0x0400   | Digital 14  |
| Little Finger 2 |  11 | 0x0800   | Digital 15  |

SmartGlove devices support up to 4 gestures:

| Description     |  ID | Bit Mask | junXion Pin |
|:--------------- | ---:|:-------- |:----------- |
| Wave Left       |   0 | 0x1000   | Digital 8   |
| Wave Right      |   1 | 0x2000   | Digital 9   |
| Wave Up         |   2 | 0x1000   | Digital 10  |
| Wave Down       |   3 | 0x2000   | Digital 11  |

### Analog Outputs

| Device          | Description             |  ID | junXion Pin |
|:--------------- |:----------------------- | ---:|:----------- |
| Bend Sensor     | Index Finger            |   0 | Analog 0    |
| Bend Sensor     | Middle Finger           |   1 | Analog 1    |
| Bend Sensor     | Ring Finger             |   2 | Analog 2    |
| Bend Sensor     | Little Finger           |   3 | Analog 3    |
| Distance Sensor | Hand-to-Hand            |   4 | Own Input 0 |
| IMU             | Raw Acceleration X      |   5 | Own Input 1 |
| IMU             | Raw Acceleration Y      |   6 | Own Input 2 |
| IMU             | Raw Acceleration Z      |   7 | Own Input 3 |
| IMU             | Orientation Pitch       |   8 | Own Input 4 |
| IMU             | Orientation Roll        |   9 | Own Input 5 |
| IMU             | Orientation Yaw/Heading |  10 | Own Input 6 |
|                 | junXion State           |  11 | Own Input 7 |
| Distance Sensor | Hand-to-Ground          |  12 | Own Input 8 |

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
| SSD1306 | dot matrix display        | `011110`           | `0`                   | `0x3C`      |
| 24AA64  | EEPROM                    | `01010`            | `000`                 | `0x50`      |
| VL52L1X | distance measurement      | `1010010`          |                       | `0x52`      |

## IDE Setup

1. Install [the Arduino IDE][1].
2. In the Arduino IDE preferences, add the following **Additional Board Manager URL**:

   `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`

3. Install the **Adafruit SAMD Boards** boards manager (menu Tools/Board/Boards Manager).
4. Install the following libraries (menu Sketch/Include Library/Manage Libraries):
   - **Adafruit Unified Sensor**
   - **Adafruit BNO055**
   - **MonoGFX Arduino**
   - **SparkFun VL53L1X 4m Laser Distance Sensor**
5. Select the board **Adafruit Feather M0**.

[1]: https://www.arduino.cc/en/Main/Software
