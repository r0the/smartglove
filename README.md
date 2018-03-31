# SmartGlove NEO

## Usage



### I2C Addresses

| Chip    | Function                  | Fixed Address Part | Variable Address part | Address Binary | Address Hex |
| ------- | ------------------------- | ------------------ | --------------------- | -------------- | ----------- |
| PCA9557 | buttons and LED           | `0011`             | `001`                 | `B0001001`     | `0x19`      |
| BNO055  | inertial measurement unit | `010100`           | `1`                   | `B0101001`     | `0x29`      |
| SSD1306 | dot matrix display        | `011110`           | `0`                   | `B0111100`     | `0x3C`      |

### Buttons

SmartGlove devices support up to 12 buttons:

| Description     | Index | Bit Mask |
| --------------- | ----- | -------- |
| Thumb 1         | 0     | 0x0001   |
| Thumb 2         | 1     | 0x0002   |
| Thumb 3         | 2     | 0x0004   |
| Thumb 4         | 3     | 0x0008   |
| Index Finger 1  | 4     | 0x0010   |
| Middle Finger 1 | 5     | 0x0020   |
| Ring Finger 1   | 6     | 0x0040   |
| Little Finger 1 | 7     | 0x0080   |
| Index Finger 2  | 8     | 0x0100   |
| Middle Finger 2 | 9     | 0x0200   |
| Ring Finger 2   | 10    | 0x0400   |
| Little Finger 2 | 11    | 0x0800   |
