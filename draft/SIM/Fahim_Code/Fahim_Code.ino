// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "AK8975.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050_9Axis_MotionApps41.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu; 
//MPU6050 mpu(0x69); // <-- use for AD0 high

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_YAWPITCHROLL

// uncommednt "OUTPUT_MAG" to get Raw Mag data
// doesn't work for as far as I can tell :s
//#define OUTPUT_MAG

// uncommednt "OUTPUT_SONAR" to get Sonar distance data
//#define OUTPUT_SONAR

// uncommednt "OUTPUT_ENCODERS" to get Sonar distance data
#define OUTPUT_ENCODERS

//uncomment if the carrier is connected and you want to control it
#define CARRIER_ENABLED

//set to 1 to get extra print statements! be ware of a flodded serial port :)
int VERBOSE;

//define OUTPUT_SERIAL to output data on the serial bus as expected by the Communication module
#define OUTPUT_SERIAL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
int yprIter = 0;
float yprAgg[3] = {0.0,0.0,0.0};
int yprIterMax  = 1;

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

//Mag globals
AK8975 mag(0x0C);
int16_t mx, my, mz;

//Encoder globals
int LastEncoderPinValue[2] = {0,0};
float WheelDistance[2] = {0.0, 0.0};
unsigned long WheelSpeedTimer[2] = {0,0};
unsigned long LastTickTime = 0;
float WheelSpeed[2] = {0.0, 0.0};
float EncoderTickDistance = 84.0/205.0;

int EncoderCountTarget[2] = {0,0};
int EncoderCount[2] = {0,0};
int LastEncoderCount[2] = {0,0};

//Sonar globals
const int SonarCount = 3;
long SonarPulse[SonarCount] = {0,0,0};
long SonarDist[SonarCount] = {0,0,0};
unsigned long SonarLastTimer = 0;

//Hardware pins
int EncoderPin[2] = {7,2};
int MotorPwmPin[2] = {3,11};
int MotorDirPin[2] = {12,13};
int MotorBrkPin[2] = {9,8};
int CurrentSensePin[2] = {A0,A1};
int SonarPwmPin[SonarCount] = {4,5,6}; //array of pins that sonnars are connected to


//Misc Globals
float InchesToCm = 2.54;
int LEFT = 0;
int RIGHT = 1;
unsigned long EncoderPollingTimer = 0;
unsigned long AngleCheckTimer = 0;
float reqSpeed = 0;
float reqAngle = 0;

//flag
bool isAngleRequested = false;

// ================================================================
// ===                      MAIN SETUP                          ===
// ================================================================

void setup()
{
  Serial.begin(9600);
  #ifdef OUTPUT_READABLE_YAWPITCHROLL
    SetupMPU();
  #endif
  
  #ifdef OUTPUT_SONAR
    SetupSonar();
  #endif
  
  #ifdef CARRIER_ENABLED
    SetupMotors();
  #endif
  
  #ifdef OUTPUT_ENCODERS
  SetupEncoders();
  #endif

}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop()
{
    VERBOSE = 0; //global flag for serial printing
    
    #ifdef OUTPUT_SERIAL
    //VERBOSE = 1;
    receiveData();
    //VERBOSE = 0;
    #endif
    
    #ifdef OUTPUT_READABLE_YAWPITCHROLL
    VERBOSE? Serial.println("Get YPR") : 0;
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    GetGyroData();
    #endif
    
    #ifdef OUTPUT_MAG
    VERBOSE? Serial.println("Get Mag") : 0;
    GetMagHeading();
    #endif
    
    #ifdef OUTPUT_SONAR
    VERBOSE? Serial.println("Get Sonar") : 0;
    if(( (millis()) - SonarLastTimer) > 40)
    {
      GetSonarDist();
      SonarLastTimer = (millis());
    }
    #endif
    
    #ifdef OUTPUT_ENCODERS
    VERBOSE? Serial.println("Get Encoders") : 0;
    if(millis() - EncoderPollingTimer > 10)
    {
      GetEncoderData(RIGHT);
      GetEncoderData(LEFT);
      EncoderPollingTimer = millis();
    }
    #endif
    
    #ifdef CARRIER_ENABLED
      VERBOSE? Serial.println("Set Carrier"): 0;
    DriveCarrier();
    //DriveStraight();
    //ChangeOrientation(315);   
    #endif
}
