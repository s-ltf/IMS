
#include "Wire.h"
#include "I2Cdev.h"
#include "AK8975.h"
#include "MPU6050.h"
AK8975 mag(0x0C);

int16_t mx, my, mz;

void setup() {
    Wire.begin();
    Serial.begin(9600);

    // initialize devices
    Serial.println("Initializing I2C devices...");
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "AK8975 connection successful" : "AK8975 connection failed");
}

void loop() {
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
        
    Serial.print("mag:\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.println(mz);
}
