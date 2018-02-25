# Smartglove NEO

## Bedienung



### I2C Addresses

| chip    | function                  | fixed address part | variable address part | address binary | address hex |
| ------- | ------------------------- | ------------------ | --------------------- | -------------- | ----------- |
| PCA9557 | buttons and LED           | `0011`             | `001`                 | `B0001001`     | `0x19`      |
| BNO055  | inertial measurement unit | `010100`           | `1`                   | `B0101001`     | `0x29`      |
| SSD1306 | dot matrix display        | `011110`           | `0`                   | `B0111100`     | `0x3C`      |
