#include <Wire.h>


void setup() {
    Wire.begin();
    Serial.begin(9600);
    while (!Serial) {
        delay(1);
    }

    Serial.println("\nI2C Scanner");
}


void loop() {
    uint8_t count = 0;
    Serial.println("Scanning...");
    for (uint8_t address = 1; address < 127; ++address) {
        Serial.print("0x");
        if (address < 16) Serial.print("0");
        Serial.print(address, HEX);
        Serial.print(": ");
        Wire.beginTransmission(address);
        uint8_t result = Wire.endTransmission();
        if (result == 0) {
            Serial.println("device found");
            ++count;
        }
        else if (result == 4) {
            Serial.println("unknown error");
        }
        else {
            Serial.println("no device");
        }    
    }
    Serial.print(count);
    Serial.println(" I2C devices found.");
    delay(5000);
}
