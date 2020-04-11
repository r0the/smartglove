#include <Wire.h>
#define ADS_TRANSFER_SIZE 3
#define SENSORS_NUMBER 3

typedef enum{
    ADS_RUN = 0,
    ADS_SPS,
    ADS_RESET,
    ADS_DFU,
    ADS_SET_ADDRESS,
    ADS_POLL,
    ADS_GET_FW_VER,
    ADS_CALIBRATE,
    ADS_AXES_ENABLED,
    ADS_SHUTDOWN,
    ADS_GET_DEV_ID
} ADS_COMMAND_T;

//Identifier for packet received from ADS
typedef enum{
    ADS_SAMPLE = 0,
    ADS_FW_VER,
    ADS_DEV_ID
} ADS_PACKET_T;

//I2C Pins
const int SCLpin = 16;
const int SDApin = 15;

//I2C Sensor Addresses
uint8_t address[] = {0x13, 0x14, 0x15 };

uint8_t buffer[3];

volatile float currentSample[SENSORS_NUMBER]; //This is where the calculated angular values are stored pre and post filtering. 2 FINGERS NOW
static float prev_sample[SENSORS_NUMBER];
static float filter_samples[SENSORS_NUMBER][6];

void setup(){
    Serial.begin(115200);
    Serial.println("Bendlabs Custom I2C Lib");

    //I2C bus init
    Wire.begin(SCLpin, SDApin);
    Wire.setClock(100000);  

    //Sensors init check connection

    buffer[0] = ADS_POLL;//init polling mode config
    buffer[1] = true;

    for (int i = 0;i < SENSORS_NUMBER;i++) {
        Wire.beginTransmission(address[i]);
        if (Wire.endTransmission() != 0) {
            Serial.print("Problem with Sensor: ");Serial.println(i+1);
        }else {
            writeBuffer(buffer, ADS_TRANSFER_SIZE, address[i]);
            delay(10);
            Serial.print("OK Sensor: ");Serial.println(i + 1);
        }
    }
    Serial.println("Sensors Init complete!");
}

void loop(){
    for (int i = 0;i < SENSORS_NUMBER;i++) {
        if (readBuffer(buffer, ADS_TRANSFER_SIZE, address[i]) == true) {
            if (buffer[0] == ADS_SAMPLE) {
                currentSample[i] = (float)ads_int16_decode(&buffer[1]) / 64.0f;
                Serial.print(currentSample[i]);
                Serial.print(",");
            }
        }else {
            Serial.println("Data Problem");
        }
    }
    Serial.println();
    delay(100);
}

bool readBuffer(uint8_t *buffer, uint8_t len, uint8_t address){
    Wire.requestFrom(address, len);
    uint8_t i = 0;
    while (Wire.available()){
        if (i < len) {
            buffer[i++] = Wire.read();
        }
    }
    if (i == len) {
        return true;
    }
    return false; //Error
}

bool writeBuffer(uint8_t *buffer, uint8_t len, uint8_t address){
    Wire.beginTransmission(address);
    uint8_t bytesWritten = Wire.write(buffer, len);
    Wire.endTransmission();
    if (bytesWritten == len) {
        return true;
    }
    return false;
}

inline int16_t ads_int16_decode(const uint8_t* p_encoded_data) {
    return ((((uint16_t)(p_encoded_data)[0])) | (((int16_t)(p_encoded_data)[1]) << 8));
}
