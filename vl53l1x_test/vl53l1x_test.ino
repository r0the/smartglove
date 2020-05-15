#include "vl53l1x.h"
#include <Wire.h>


VL53L1X sensor(0x29);

void setup() {
    Serial.begin(9600);
    while (!Serial) delay(1);
    Serial.println("Welcome to VL53L1X test");
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C

    sensor.setTimeout(500);
    Serial.println("Initializing sensor...");
    if (!sensor.init()) {
        Serial.println("Failed to detect and initialize sensor!");
        while (1);
    }

  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.writeMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
 sensor.startContinuous(50);
  Serial.println("Initialized");
}

void loop()
{
    if (sensor.dataReady()) {
      sensor.updateDSS();
      sensor.readResults();
      sensor.getRangingData();
      Serial.println(sensor.ranging_data.range_mm);
    }
}
