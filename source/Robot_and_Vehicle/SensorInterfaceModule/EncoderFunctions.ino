int EncYawCalib = 0;
// ================================================================
// ===                      Encoders Setup Function             ===
// ================================================================

void SetupEncoders()
{
  Serial.println("I_Initializing encoders...");
  pinMode(EncoderPin[LEFT] , INPUT);
  pinMode(EncoderPin[RIGHT] , INPUT);
  attachInterrupt(EncoderIntrptPin[LEFT], LeftEncoderISR, RISING);
  attachInterrupt(EncoderIntrptPin[RIGHT], RightEncoderISR, RISING);
  
  Serial.println("I_Encoders ready!");
}

// ================================================================
// ===                      Encoders ISR Function              ===
// ================================================================


void LeftEncoderISR()
{
  EncoderCount[LEFT]++;
  (MotorDir[LEFT])?(YawEncoderCount[LEFT]++):(YawEncoderCount[LEFT]--);
  //DriveEncoderCount[LEFT]++;
}

void RightEncoderISR()
{
  EncoderCount[RIGHT]++;
  (MotorDir[RIGHT])?(YawEncoderCount[RIGHT]++):(YawEncoderCount[RIGHT]--);
  //DriveEncoderCount[RIGHT]++;
}


// ================================================================
// ===                      Encoders Main Function              ===
// ================================================================
void GetEncoderData(int Wheel)
{
  //calculate the yaw based on encoder count difference
  int EncoderCountDiff = 0;
  
  #ifdef OUTPUT_ENCODER_YAW
  if ((millis()>EncYawTimer) && useEncoderYaw)
  {
    EncoderCountDiff = (YawEncoderCount[LEFT] - YawEncoderCount[RIGHT])/2;
    encYaw = EncoderCountDiff / AngleRatio;
    calibYaw = encYaw + EncYawCalib;
    while((calibYaw>360)||(calibYaw<0))
    {
      (calibYaw>360)?EncYawCalib-=360:EncYawCalib+=360;
      calibYaw = encYaw + EncYawCalib;
    }
    newGyroDataFlag = true;
    
    int temp = round(calibYaw/45) * 45;
    Serial.print("y_"); Serial.println(temp);
    EncYawTimer=millis()+100;
    
    //(millis()>EncYawTimer)?(Serial.print("y_"), Serial.println(temp), EncYawTimer=millis()+100):0;
  }
  #endif
  
  
  EncoderCountDiff = EncoderCount[Wheel] - LastEncoderCount[Wheel];
  if(EncoderCountDiff>0)  //the wheel has moved at least a bit
  {
     LastEncoderCount[Wheel] = EncoderCount[Wheel];
     //calc distance
     if(!firstRunFlag)    //not turning
     {
       WheelDistance[Wheel] =  WheelDistance[Wheel] + (EncoderCountDiff*EncoderTickDistance); //distance updated in mm
     }
     //calc speed    
     long int TickTime = micros() - WheelSpeedTimer[Wheel];
     TickTime = TickTime / EncoderCountDiff;
     //Serial.println(TickTime);
     WheelSpeed[Wheel] = ((EncoderTickDistance*1000000) / TickTime);  //speed in mm/s
     WheelSpeedTimer[Wheel] = micros();
     WheelSpeedUpdated[Wheel] = 1;
     //Serial.print("D_"); Wheel? Serial.print("R ") : Serial.print("L "); Serial.println(WheelDistance[Wheel]/25.4);
     //Serial.print("E_"); Wheel? Serial.print("R ") : Serial.print("L "); Serial.println(EncoderCount[Wheel]);
     //Serial.print("Sp_"); Wheel? Serial.print("R \t") : Serial.print("L \t"); Serial.println(WheelSpeed[Wheel]);
  }
  else  //the wheel is completely stopped
  {
    WheelSpeed[Wheel] = 0;
    WheelSpeedUpdated[Wheel] = 1;
  }
}
/*

void GetEncoderData(int Wheel)    //wheel identifies if function is right or left
{
  //if the encoders don't change for 500 ms then the robot stoped! reset all values
  if((millis() - LastTickTime[Wheel]) > 500)
  {
    if(VERBOSE)
    {
      Serial.print("Robot halted! Wheel: "); Serial.println(Wheel);
    }
    
    WheelSpeed[Wheel] = 0;
    //LastEncoderCount[Wheel] = EncoderCount[Wheel];
    WheelSpeedTimer[Wheel] = millis();
    WheelSpeedUpdated[Wheel] = 1;
    //EncoderCount[RIGHT] = EncoderCount[LEFT];
  }
  
  int CurrentEncoderPinValue = digitalRead(EncoderPin[Wheel]);
  if(LastEncoderPinValue[Wheel] != CurrentEncoderPinValue)
  {
    //Serial.print("Tick...   "); Serial.println(millis() - LastTickTime[Wheel]);
    
    LastTickTime[Wheel] = millis();
    
    EncoderCount[Wheel]++;
    WheelDistance[Wheel] += EncoderTickDistance;

    if((EncoderCount[Wheel] - LastEncoderCount[Wheel]) >= 2)
    {
       int TickTime = millis() - WheelSpeedTimer[Wheel];
       TickTime = TickTime / (EncoderCount[Wheel] - LastEncoderCount[Wheel]);
       WheelSpeed[Wheel] = ((EncoderTickDistance*InchesToCm*10) / TickTime) * 1000;  //speed in mm/s
       WheelSpeedTimer[Wheel] = millis();
       WheelSpeedUpdated[Wheel] = 1;
       if(VERBOSE)
       {
         Serial.print("Sp"); Wheel? Serial.print(" Right ") : Serial.print("Left "); Serial.println(WheelSpeed[Wheel]);
       }
       LastEncoderCount[Wheel] = EncoderCount[Wheel];
    }
    LastEncoderPinValue[Wheel] = CurrentEncoderPinValue;
    //Print Distance
    if(VERBOSE)
    {
      Serial.print("D"); Wheel? Serial.print(" Right ") : Serial.print("Left "); Serial.println(WheelDistance[Wheel]);
    }
    //Print Speed
  }
}
*/
