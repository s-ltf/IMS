bool overFlowFlag = false;
float LastGoodCalibYaw = -999.0;
int GyroAxis = 0;

// ================================================================
// ===               MPU INTERRUPT DETECTION ROUTINE            ===
// ================================================================

void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                    Magnetometer                          ===
// ================================================================


void GetMagHeading()
{
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
        
    Serial.print("mag:\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.println(mz); 
}

// ================================================================
// ===                      MAG SETUP                           ===
// ================================================================

void SetupMag()
{
    // initialize Magnetometer
    Serial.println("I_Initializing Magnetometer...");
    mag.initialize();

    // verify connection
    Serial.println("I_Testing Magnetometer connections...");
    Serial.println(mag.testConnection() ? "I_Magnetometer: AK8975 connection successful" : "I_Magnetometer: AK8975 connection failed");
}


// ================================================================
// ===                      MPU SETUP                           ===
// ================================================================

void SetupMPU() {
     // join I2C bus (I2Cdev library doesn't do this automatically)
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // initialize device
    Serial.println(F("I_Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("I_Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("I_MPU6050 connection successful") : F("I_MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("I_Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("I_Enabling DMP..."));
        mpu.setDMPEnabled(true);
        
        /*
        // enable Arduino interrupt detection
        Serial.println(F("I_Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        */
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("I_DMP ready!"));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
        
        
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("I_DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

// ================================================================
// ===               Gyro Calibration Function                  ===
// ================================================================

void CalibrateGyro()
{
    GyroCalibOffset = 0.0;
    bool GyroOffsetFlag = false; 
    int GyroInitOffsetCounter = 0;
    while(!GyroOffsetFlag)
    {
     if(GetGyroData())
     {
       if( (ypr[GyroAxis] < (GyroCalibOffset + 1.0)) && (ypr[GyroAxis] > (GyroCalibOffset - 1.0)) )
       {
         //Serial.println("bla");
         GyroInitOffsetCounter++;
       }
       else
       {
        GyroInitOffsetCounter = 0;
        GyroCalibOffset = ypr[GyroAxis];
       }
       //Serial.println(GyroCalibOffset);
       Serial.print("I_"); Serial.println(GyroInitOffsetCounter);
     }
     
     if(GyroInitOffsetCounter > 50)
     {
        GyroOffsetFlag = true; 
        GyroCalibrated = true;
     }
    }
    Serial.print("I_Gyro calibarated, Offset = "); Serial.println(GyroCalibOffset);
    GyroCalibOffset = -GyroCalibOffset;
    
}
        
        
// ================================================================
// ===                      Gyro Main Function                  ===
// ================================================================


bool GetGyroData()
{
  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  
  // get current FIFO count
  fifoCount = mpu.getFIFOCount();
  bool result = false;
  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      Serial.println(F("E_FIFO overflow!"));
      overFlowFlag = true;
      result = false;
  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    
      // remember last Yaw for the quadrant fixing code bellow
      float lastYaw = ypr[GyroAxis];
      calibYaw = 0;
      
      
      
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;
  
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      //(insaneFlag?)(Serial.print("Quant    "),Serial.print(q[0]),Serial.println(q[1])):0;
      mpu.dmpGetGravity(&gravity, &q);
      //(insaneFlag)?(Serial.print("Grav    "),Serial.print(gravity)):0;
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      //(insaneFlag?)(Serial.print("YPR     "),Serial.print(q[0]),Serial.println(q[1])):0;
            
      ypr[GyroAxis] = (ypr[GyroAxis] * 90/M_PI)+90;
      //first implement the initial calibration
      
      /**********************THIS CODE CHECKS FOR RATIONAL GYRO BEHAVIOR AND TRIES TO MITIGATE FOR ERRORS*/
      float YawDiff = abs(lastYaw - ypr[GyroAxis]);
      if(YawDiff < 170 && YawDiff > 10 && !firstRunFlag && GyroCalibrated)
      {
        Serial.print("E_Something Bad happened with Gyro!\typr[GyroAxis]: "); Serial.print(ypr[0]); Serial.print(" - "); Serial.print(ypr[1]); Serial.print(" - "); Serial.print(ypr[2]); Serial.print("\tLast ypr[GyroAxis]:  "); Serial.println(lastYaw);
        insaneFlag = true;
        //stop the vhecle
        reqSpeed = 0;
        reqAngle = 0;
        newDataFlag = true;
        //stop printing sonar data
        firstRunFlag = true;
      }
            
      /******************************/
      
      //then track and correct the quadrant based on the last result
      if(!insaneFlag)
      {
        if(abs(lastYaw - ypr[GyroAxis]) > 100)
        {
           //(lastYaw<5)?(GyroCalibOffset+=180):0;
           //(lastYaw>175)?(GyroCalibOffset+=180):0;
           
           GyroCalibOffset+=180;
           (GyroCalibOffset >= 360)? (GyroCalibOffset -= 360) : 0;
           (GyroCalibOffset < 0)? (GyroCalibOffset += 360) : 0;
        }
             
        (GyroCalibrated)?(calibYaw = ypr[GyroAxis] + GyroCalibOffset) : (calibYaw = ypr[GyroAxis]);
       // Serial.println(GyroCalibOffset);
        (calibYaw >= 360)? (calibYaw -= 360) : 0;
        (calibYaw < 0)? (calibYaw += 360) : 0;
        calibYaw = 360 - calibYaw;
        
        Serial.print("y_");
        Serial.println(calibYaw);
        
        //Serial.print("\t\tEnc-y_");
        //Serial.println(encYaw);
        
        newGyroDataFlag = true;
      }
      else
      {
        (insaneCounter == -1)?(insaneCounter = 50):0;
        (insaneCounter<1)?(insaneFlag = false, firstRunFlag = false, insaneCounter = -1):(insaneCounter--, Serial.print("I_"), Serial.println(insaneCounter));
        
        
        /*
        (GyroCalibrated)?(calibYaw = ypr[GyroAxis] + GyroCalibOffset) : (calibYaw = ypr[GyroAxis]);
       // Serial.println(GyroCalibOffset);
        (calibYaw >= 360)? (calibYaw -= 360) : 0;
        (calibYaw < 0)? (calibYaw += 360) : 0;
        calibYaw = 360 - calibYaw;
       
        if(LastGoodCalibYaw==-999)
        {
          LastGoodCalibYaw = lastYaw + GyroCalibOffset;
          (LastGoodCalibYaw >= 360)? (LastGoodCalibYaw -= 360) : 0;
          (LastGoodCalibYaw < 0)? (LastGoodCalibYaw += 360) : 0;
          LastGoodCalibYaw = 360 - LastGoodCalibYaw;
          Serial.print("E_Last known yaw value: "); Serial.println(LastGoodCalibYaw);
        }
        
        for(int i = 0; i < 4; i++)
        {
        if((calibYaw < LastGoodCalibYaw*1.05)&&(calibYaw > LastGoodCalibYaw*0.95))
         { 
          insaneFlag = false;
          Serial.println("E_Gyro is stable again");
          LastGoodCalibYaw = -999;
         }
         GyroCalibOffset+=90;
         (GyroCalibOffset >= 360)? (GyroCalibOffset -= 360) : 0;
         (GyroCalibOffset < 0)? (GyroCalibOffset += 360) : 0;
         calibYaw = ypr[GyroAxis] + GyroCalibOffset;
         (calibYaw >= 360)? (calibYaw -= 360) : 0;
         (calibYaw < 0)? (calibYaw += 360) : 0;
        }
        */
      }
      
      
      /*
      Serial.print("\t");
      Serial.print(ypr[1] * 90/M_PI);
      Serial.print("\t");
      Serial.println(ypr[2] * 90/M_PI);
      Serial.print("t ");Serial.println(millis());
      */
      result = true;
  } 
  return(result);
}
