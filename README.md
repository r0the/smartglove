# SmartGlove NEO

## Usage



### I2C Addresses

| Chip    | Function                  | Fixed Address Part | Variable Address part | Address Binary | Address Hex |
|:------- |:------------------------- |:------------------ |:--------------------- |:-------------- |:----------- |
| PCA9557 | buttons and LED           | `0011`             | `001`                 | `B0001001`     | `0x19`      |
| BNO055  | inertial measurement unit | `010100`           | `1`                   | `B0101001`     | `0x29`      |
| SSD1306 | dot matrix display        | `011110`           | `0`                   | `B0111100`     | `0x3C`      |

### Digital Outputs

SmartGlove devices support up to 16 digital outputs:

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

### Analog Outputs

| Device          | Description         |  ID | junXion Pin |
|:--------------- |:------------------- | ---:|:----------- |
| Bend Sensor     | Index Finger        |   0 | Analog 0    |
| Bend Sensor     | Middle Finger       |   1 | Analog 1    |
| Bend Sensor     | Ring Finger         |   2 | Analog 2    |
| Bend Sensor     | Little Finger       |   3 | Analog 3    |
| Distance Sensor | Hand-to-Hand        |   4 | Own Input 0 |
| IMU             | Raw Acceleration X  |   5 | Own Input 1 |
| IMU             | Raw Acceleration Y  |   6 | Own Input 2 |
| IMU             | Raw Acceleration Z  |   7 | Own Input 3 |
| IMU             | Orientation Roll    |   8 | Own Input 4 |
| IMU             | Orientation Pitch   |   9 | Own Input 5 |
| IMU             | Orientation Heading |  10 | Own Input 6 |
|                 | junXion State       |  11 | Own Input 7 |
| Distance Sensor | Hand-to-Ground      |  12 | Own Input 8 |
