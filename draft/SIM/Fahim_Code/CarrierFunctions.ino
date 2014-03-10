//Motor Globals
int WheelSpeedUpdated[2] = {0,0};
int MotorPwmVal[2] = {0,0};
float WheelSpeedRatio = 0.98;
int reqPWM = 0;
int StdSpeed = 120;

void DriveCarrier()
{
  //Serial.print("Req Speed: "); Serial.println(reqSpeed);
    //translate driving speed
    if(reqSpeed == 0)
    {
      reqPWM = 0;
      if(reqAngle > 0)
        ChangeOrientation();
    }
    else if(reqSpeed>0)
    {
      //Serial.println("Straight");
      reqPWM = StdSpeed;
      if( (MotorDirPin[RIGHT] == LOW) || (MotorDirPin[LEFT] == LOW))
      {
        StopCarrier();
        MotorDirPin[RIGHT] = HIGH;
        MotorDirPin[LEFT] = HIGH;
      }
      DriveStraight();
    }
    else if(reqSpeed<0)
    {
      //broken for some reason
      //Serial.println("Backing");
      reqPWM = StdSpeed;
      if((MotorDirPin[RIGHT] == HIGH) || (MotorDirPin[LEFT] == HIGH));
      {
        Serial.println("Change Dir");
        StopCarrier();
        MotorDirPin[RIGHT] = LOW;
        MotorDirPin[LEFT] = LOW;
      }
      DriveStraight();
    } 
}

void StopCarrier()
{
    analogWrite(MotorPwmPin[LEFT], 0);
    analogWrite(MotorPwmPin[RIGHT], 0);
    digitalWrite(MotorBrkPin[LEFT], HIGH);
    digitalWrite(MotorBrkPin[RIGHT], HIGH);
    delay(10);
    digitalWrite(MotorBrkPin[LEFT], LOW);
    digitalWrite(MotorBrkPin[RIGHT], LOW);
}

// ================================================================
// ===                      Motors SETUP                          ===
// ================================================================

void SetupMotors()
{
    Serial.println("I_Initializing motors...");
    
    pinMode(MotorPwmPin[RIGHT], OUTPUT);
    pinMode(MotorDirPin[RIGHT], OUTPUT);
    pinMode(MotorBrkPin[RIGHT], OUTPUT);
    
    pinMode(MotorPwmPin[LEFT], OUTPUT);
    pinMode(MotorDirPin[LEFT], OUTPUT);
    pinMode(MotorBrkPin[LEFT], OUTPUT);
    
    //See the board markings for each pin function 
    //Set Left Wheel PWM pin
    digitalWrite(MotorBrkPin[LEFT], LOW);
    digitalWrite(MotorDirPin[LEFT], HIGH);
    analogWrite(MotorPwmPin[LEFT], 0);
    
    //Set Right Wheel PWM pin
    digitalWrite(MotorBrkPin[RIGHT], LOW);
    digitalWrite(MotorDirPin[RIGHT], HIGH);
    analogWrite(MotorPwmPin[RIGHT], 0);
    Serial.println("I_Done Initializing motors!");
}

// ================================================================
// ===                      Straight Drive Function             ===
// ================================================================

void DriveStraight()

{
  VERBOSE = 1;
  if(VERBOSE)
    //Serial.println("Going Straight");
  
  if(WheelSpeed[RIGHT] == 0 || WheelSpeed[LEFT] == 0)    //motors are stopped
  {
    SendWheelSpeed();
  }
  else  //motors are running
  {
    //VERBOSE? Serial.println("Adjusting Speed") : 0;
    if(WheelSpeedUpdated[RIGHT] && WheelSpeedUpdated[LEFT])  //new speeds in buffer
     {
       WheelSpeedUpdated[RIGHT] = 0;  //reset flags
       WheelSpeedUpdated[LEFT] = 0;
       
       float tempSpeedRatio = WheelSpeed[RIGHT]/WheelSpeed[LEFT];  //calculate wheel speed ratio
       float ratioIncrement = 0.0;
       
       if(1.02<tempSpeedRatio || tempSpeedRatio<0.98)
         ratioIncrement = 0.01;
       if(1.05<tempSpeedRatio || tempSpeedRatio<0.95)
         ratioIncrement = 0.05;
       if (1.1<tempSpeedRatio || tempSpeedRatio<0.9)
         ratioIncrement = 0.1;
         
       if(VERBOSE)
       {
         Serial.print("S-Ratio: "); Serial.print(tempSpeedRatio); Serial.print("    Inc: "); Serial.println(ratioIncrement);
       }
       (tempSpeedRatio>1)? (WheelSpeedRatio -= ratioIncrement) : (WheelSpeedRatio += ratioIncrement);
       
       if(abs(EncoderCount[LEFT] - EncoderCount[RIGHT]) > 0)
       {
           if(VERBOSE)
           {
             Serial.print("Count Diff:   "); Serial.println(EncoderCount[LEFT] - EncoderCount[RIGHT]);
           }
           float temp = 0.01 *(abs(EncoderCount[LEFT] - EncoderCount[RIGHT]));
           ratioIncrement += temp;
          (EncoderCount[LEFT] > EncoderCount[RIGHT])? (WheelSpeedRatio += temp) : (WheelSpeedRatio -= temp);
       }
       
       if(ratioIncrement != 0.0)
         SendWheelSpeed();
     } 
  }
}



void SendWheelSpeed()
{
  VERBOSE = 1;
  MotorPwmVal[RIGHT] = WheelSpeedRatio*reqPWM;
  MotorPwmVal[LEFT] = reqPWM;
 
  analogWrite(MotorPwmPin[LEFT], MotorPwmVal[LEFT]);
  analogWrite(MotorPwmPin[RIGHT], MotorPwmVal[RIGHT]);
  
  float CurrentSenseVal[2] = {0.0,0.0};
  
  CurrentSenseVal[LEFT] = (analogRead(CurrentSensePin[LEFT])/.165);
  CurrentSenseVal[RIGHT] = (analogRead(CurrentSensePin[RIGHT])/.165);
  
  //Serial.print(WheelSpeedRatio); Serial.print(" * "); Serial.print("Current: "); Serial.print(CurrentSenseVal[LEFT]); Serial.print(", "); Serial.println(CurrentSenseVal[RIGHT]);
  if(VERBOSE)
  {
    Serial.print(WheelSpeedRatio); Serial.print(" * "); Serial.print("Actual: "); Serial.print(WheelSpeed[LEFT]); Serial.print(", "); Serial.print(WheelSpeed[RIGHT]); Serial.print(" Set: "); Serial.print(MotorPwmVal[LEFT]); Serial.print(", "); Serial.println(MotorPwmVal[RIGHT]);
  }
}

// ================================================================
// ===                      Turning Function                    ===
// ================================================================

void ChangeOrientation()
{
  if(VERBOSE)
    Serial.println("Turning");
  
  if (isAngleRequested)
  {
    StopCarrier();
    if (reqAngle > 180)
    {
       digitalWrite(MotorDirPin[LEFT], LOW);
       digitalWrite(MotorDirPin[RIGHT], HIGH);  
    }
    else
    {
       digitalWrite(MotorDirPin[LEFT], HIGH);
       digitalWrite(MotorDirPin[RIGHT], LOW);  
    }
    
    if (reqAngle == 180)
    { 
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 18;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 18;  
    }
    else if (reqAngle == 90 || reqAngle == 270)
    { 
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 8;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 8;   
    }
    
    else if (reqAngle == 45 || reqAngle == 315)
    {
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 4;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 4;  
    }
    isAngleRequested = false;
  }
  
  int MotorPwm[2] = {90 + 1 * EncoderCountTarget[LEFT] - EncoderCount[LEFT] , 
                    90 + 1 * EncoderCountTarget[RIGHT] - EncoderCount[RIGHT]};
  
  if (EncoderCount[LEFT] > EncoderCountTarget[LEFT])
    MotorPwm[LEFT] = 0;
  if (EncoderCount[RIGHT] > EncoderCountTarget[RIGHT])
    MotorPwm[RIGHT] = 0;
  if ((MotorPwm[LEFT] == 0) && (MotorPwm[RIGHT] == 0))
    reqAngle = 0;
       
  analogWrite(MotorPwmPin[LEFT], MotorPwm[LEFT]);
  analogWrite(MotorPwmPin[RIGHT], MotorPwm[RIGHT]);
}
