// ================================================================
// ===               MPU INTERRUPT DETECTION ROUTINE            ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
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
// ===                      MPU SETUP                           ===
// ================================================================

void SetupMPU() {
     // join I2C bus (I2Cdev library doesn't do this automatically)
        Wire.begin();

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

        // enable Arduino interrupt detection
        Serial.println(F("I_Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("I_DMP ready! Waiting for first interrupt..."));
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

    // initialize Magnetometer
    Serial.println("I_Initializing Magnetometer...");
    mag.initialize();

    // verify connection
    Serial.println("I_Testing Magnetometer connections...");
    Serial.println(mag.testConnection() ? "I_Magnetometer: AK8975 connection successful" : "I_Magnetometer: AK8975 connection failed");
}

// ================================================================
// ===                      Gyro Main Function                  ===
// ================================================================


void GetGyroData()
{
  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  
  // get current FIFO count
  fifoCount = mpu.getFIFOCount();
  
  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      // reset so we can continue cleanly
      mpu.resetFIFO();
      Serial.println(F("FIFO overflow!"));
  
  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
      // wait for correct available data length, should be a VERY short wait
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
      // read a packet from FIFO
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      
      // track FIFO count here in case there is > 1 packet available
      // (this lets us immediately read more without waiting for an interrupt)
      fifoCount -= packetSize;
  
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      
      Serial.print("y_");
      Serial.println(ypr[0] * 90/M_PI);
      /*
      Serial.print("\t");
      Serial.print(ypr[1] * 90/M_PI);
      Serial.print("\t");
      Serial.println(ypr[2] * 90/M_PI);
      Serial.print("t ");Serial.println(millis());
      */
      
  } 
}
