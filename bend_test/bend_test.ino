#include <Wire.h>
#include "ads.h"

#define ADS_ONE_AXIS 1
#define ADS_TWO_AXIS 2

ADS myFlexSensor(0x12); //Create object of the ADS class

byte deviceType; //Keeps track of if this sensor is a one axis of two axis sensor

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println(F("SparkFun Displacement Sensor Example"));

  Wire.begin();
  myFlexSensor.init();
  if (!myFlexSensor.present())
  {
    Serial.println(F("No sensor detected. Check wiring. Freezing..."));
    while (1)
      ;
  }

  deviceType = ADS_ONE_AXIS;
}

void loop()
{

  if (myFlexSensor.present() == true)
  {
    Serial.print(myFlexSensor.readInput());
    Serial.println();
  }

  if (Serial.available())
  {
    byte incoming = Serial.read();

    if (incoming == 'c')
      calibrate();
  }

  delay(10);
}

void calibrate()
{
  Serial.println(F("Calibration routine"));

  while (Serial.available() > 0)
    Serial.read(); //Flush all characters
  Serial.println(F("Press a key when the sensor is flat and straight on a table"));
  while (Serial.available() == 0)
  {
    myFlexSensor.present();
    delay(10); //Wait for user to press character
  }

  myFlexSensor.calibrateZero(); //Call when sensor is straight on both axis

  while (Serial.available() > 0)
    Serial.read(); //Flush all characters
  Serial.println(F("Good. Now press a key when the sensor is flat on table but bent at 90 degrees (along X axis)."));
  while (Serial.available() == 0)
  {
    myFlexSensor.present();
    delay(10); //Wait for user to press character
  }

  myFlexSensor.calibrateX(); //Call when sensor is straight on Y axis and 90 degrees on X axis

  Serial.println(F("Calibration complete."));
}
