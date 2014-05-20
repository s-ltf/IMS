//Motor Globals
int WheelSpeedUpdated[2] = {0,0};
int MotorPwmVal[2] = {0,0};
float WheelSpeedRatio = 0.94;
int WheelSpeedInc = 0;
float LastWheelSpeedRatio = WheelSpeedRatio;
int reqPWM = 0;
int maxPWM = 200;
int minPWM = 30;
int initPWM = 70;
int StdPWM = initPWM;
float SlwPWMRatio = 0.75;
int StdSpeed = 250; // in mm/sec
float ratioIncrement = 0.0;
//unsigned long DriveStraightTimer = 0;
float AngleRatio = 64.0/9;
long int StableCarCount = 0;
long int StableCarMaxCount= 100;
unsigned long YawDriveTimer = 0;

//Variables for Orientation
#define ANGLE_DELAY 500
bool turningDirection = false;
int encoderOvershoot = 0;
unsigned long angleDelay = ANGLE_DELAY;
int minTurnPWM = 40;
float P = 0.02;
float I = 0;
int EncoderError[2] = {0,0};
int EncoderErrorLast[2] = {0,0};
int feedbackTime = 1;
long int EncoderCountTarget[2] = {0,0};
unsigned long angleTimer = 0;
bool isAngleRequested = true;
bool completedFlag = false;
float targetAngle = 0;
int errorC = 0;
int errorP = 0;
unsigned long angleStartDelayTimer = 0;
unsigned long angleEndDelayTimer = 0;
unsigned long speedCheckTimer[2] = {0, 0};
int MotorPwm[2] = {0, 0};

unsigned long EncYawTimer = 0;

float minPwm[2] = {55, 55};

bool speedMatchRecovery = false;
bool recoveringSpeed = false;
int UseSonarToDrive = 0, UseEncoderToDrive = 1, UseYawToDrive =  1;

void DriveCarrier()    //Driving state machine
{
  //Serial.print("Sp_"); Serial.println((WheelSpeed[RIGHT] + WheelSpeed[LEFT])/2);
  //Serial.print("Req Speed: "); Serial.println(reqSpeed);
    //translate driving speed
    (newDataFlag)?((targetYaw += reqAngle), newDataFlag=false, Serial.print("E_New Requested Angle: "), Serial.println(reqAngle)) : 0;
    (targetYaw>360)?(targetYaw -= 360):0;
    
    if(reqSpeed == 0)
      if(reqAngle > 0)
      {
        //if((WheelSpeed[LEFT] > 0) || (WheelSpeed[RIGHT] > 0))
        //   StopCarrier();
        //else
          ChangeOrientation();
      }
      else
          StopCarrier();
    else if(reqSpeed>0)
    {
      //Serial.println("Straight");
      (reqSpeed == 01)?(reqPWM = SlwPWMRatio * StdPWM): reqPWM = StdPWM;
      /*
      if( (MotorDir[RIGHT]) || (MotorDir[LEFT]))
      {
        StopCarrier();
        
        digitalWrite(MotorDirPin[RIGHT], HIGH);
        MotorDir[RIGHT] = 1;
        digitalWrite(MotorDirPin[LEFT], HIGH);
        MotorDir[LEFT] = 1;
        
      }
      */
      //*******
      DriveStraight();
     //SendWheelSpeed();
    }
    else if(reqSpeed<0)
    {
      //broken for some reason
      //Serial.println("Backing");
      reqPWM = StdPWM;
      /*
      if((MotorDir[RIGHT] == HIGH) || (MotorDir[LEFT] == HIGH));
      {
        VERBOSE? (Serial.println("Change Dir")) :0;
        StopCarrier();
        
        digitalWrite(MotorDirPin[RIGHT], LOW);
        MotorDir[RIGHT] = 0;
        digitalWrite(MotorDirPin[LEFT], LOW);
        MotorDir[LEFT] = 0;
        
      }
      */
      DriveStraight();
    } 
}

void StopCarrier()
{
  VERBOSE? (Serial.println("Stopping carrier...")) : 0;
  /*
  while(WheelSpeed[LEFT] > 0.0)
  {
    if(WheelSpeedUpdated[RIGHT] && WheelSpeedUpdated[LEFT])
    {
      reqPWM = max((reqPWM-1),0);
      analogWrite(MotorPwmPin[LEFT], reqPWM);
      analogWrite(MotorPwmPin[RIGHT], WheelSpeedRatio*reqPWM);
      Serial.println(WheelSpeed[LEFT]);
    }
    else
    {
      GetEncoderData(LEFT);
      GetEncoderData(RIGHT);
    }
  }

  analogWrite(MotorPwmPin[LEFT], 0);
  analogWrite(MotorPwmPin[RIGHT], 0);
  StdSpeed = initSpeed;
  */
  analogWrite(MotorPwmPin[LEFT], 0);
  analogWrite(MotorPwmPin[RIGHT], 0);
  digitalWrite(MotorBrkPin[LEFT], HIGH);
  digitalWrite(MotorBrkPin[RIGHT], HIGH);
  //delay(10);
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
    MotorDir[LEFT] = 1;
    analogWrite(MotorPwmPin[LEFT], 0);
    
    //Set Right Wheel PWM pin
    digitalWrite(MotorBrkPin[RIGHT], LOW);
    digitalWrite(MotorDirPin[RIGHT], HIGH);
    MotorDir[RIGHT] = 1;
    analogWrite(MotorPwmPin[RIGHT], 0);
    Serial.println("I_Done Initializing motors!");
    #ifdef CALIBRATE_MOTOR_SPEED
    CalibrateMotorSpeed();
    #endif
}

/*
void CalibrateMotorSpeed()
{
  digitalWrite(MotorDirPin[LEFT], HIGH);
  MotorDir[LEFT] = 1;
  digitalWrite(MotorDirPin[RIGHT], LOW);  
  MotorDir[RIGHT] = 0;
  
  bool doneMotorCalib = false;
  firstRunFlag = true;
  int motorCalibCounter = 0;
  unsigned long motorCalibTimer = 0;
  unsigned long calibTimeOut = millis() + 10000; //calibration should timeout after 10 sec!
  
  float tempYaw = calibYaw;
  
  while(!doneMotorCalib)
  {
    (millis() > calibTimeOut)?(doneMotorCalib = true, Serial.println("I_Motor Speed Calibration FAIL!!!")) :0;
    
    MotorPwmVal[LEFT] = StdPWM;
    MotorPwmVal[RIGHT] = WheelSpeedRatio * StdPWM;
    analogWrite(MotorPwmPin[LEFT], MotorPwmVal[LEFT]);
    analogWrite(MotorPwmPin[RIGHT], MotorPwmVal[RIGHT]);
    
    while(!WheelSpeedUpdated[LEFT] || !WheelSpeedUpdated[RIGHT])
    {
      delay(100);
      GetEncoderData(LEFT);
      GetEncoderData(RIGHT);
    }
    
    WheelSpeedUpdated[LEFT] =0;
    WheelSpeedUpdated[RIGHT] =0;
    Serial.print("\t\t");Serial.println(motorCalibCounter);
    if ( (WheelSpeed[LEFT] > (StdSpeed * 0.95)) && (WheelSpeed[LEFT] < (StdSpeed * 1.05)))
    {
      motorCalibCounter++;
      if(motorCalibCounter > 10)
      {
        //wonderful wheelSpeed is within the expected ratio
        doneMotorCalib = true;
        firstRunFlag = false;
        minTurnPWM = StdPWM * 4 / 7;
        analogWrite(MotorPwmPin[LEFT], 0);
        analogWrite(MotorPwmPin[RIGHT], 0);
        digitalWrite(MotorDirPin[LEFT], HIGH);
        MotorDir[LEFT] = 1;
        digitalWrite(MotorDirPin[RIGHT], HIGH);
        MotorDir[RIGHT] = 1;
        for(int i = 0; i<50; i++)
        {
          while(!mpuInterrupt);
          GetGyroData();
        } 
        reqAngle = tempYaw - calibYaw;
        reqAngle < 0 ? reqAngle += 360:0;
        Serial.println("I_Motor Calibration Successful!!!!");
        Serial.print("StdSpeed: "); Serial.print(StdSpeed); Serial.print("   StdPWM: "); Serial.print(StdPWM);Serial.print("   Angle: "); Serial.print(reqAngle); Serial.print("   TurnPWM: "); Serial.println(minTurnPWM);
      }
    }
    else
    {
      motorCalibCounter = 0;
      if(millis()-motorCalibTimer > 200)  //make a change to PWM every X ms to allow time for the motors to react to change
      {
        motorCalibTimer = millis();
        (WheelSpeed[LEFT] < StdSpeed)? (StdPWM += 3) : (StdPWM -= 3);
        Serial.print("LWhl-Sp_"); Serial.println(WheelSpeed[LEFT]);Serial.print("StdPWM_"); Serial.println(StdPWM);
        if((WheelSpeed[RIGHT] < (WheelSpeed[LEFT] * 0.95)) || (WheelSpeed[RIGHT] > (WheelSpeed[LEFT] * 1.05)))
        {
          (WheelSpeed[LEFT] > WheelSpeed[RIGHT])? (WheelSpeedRatio += 0.02) : (WheelSpeedRatio -= 0.02);
          //Serial.print("SR_");Serial.println(WheelSpeedRatio);
          Serial.print("\t\tRWhl-Sp_"); Serial.println(WheelSpeed[RIGHT]);
        }
      }
    }
  }
  
  
}
*/
// ================================================================
// ===                      Straight Drive Function             ===
// ================================================================

void DriveStraight()
{

  //(VERBOSE)? (Serial.println("Going Straight")) : 0;
          
  if(WheelSpeed[RIGHT] == 0 || WheelSpeed[LEFT] == 0)    //motors are stopped
  {
    SendWheelSpeed();
  }
  else  //motors are running
  {
    /*we need to implement a stable robot counter here*/
    /*if(StableCarCount > StableCarMaxCount)
    {
      StableCarCount = 0;
      //StdPWM = min((StdPWM+5),200);
      float vehicleSpeed = (WheelSpeed[LEFT] + WheelSpeed[RIGHT])/2.0;
      float pwmInc = (StdSpeed - vehicleSpeed)/StdSpeed*10;
      StdPWM =  max((min((StdPWM + pwmInc),maxPWM)),minPWM);
      Serial.print("SpL_"); Serial.print(WheelSpeed[LEFT]); Serial.print("   SpR_"); Serial.print(WheelSpeed[RIGHT]);Serial.print("   StdSp_"); Serial.print(StdSpeed); Serial.print("   PWM_"); Serial.println(StdPWM);
      //Serial.print("---"); Serial.print(StableCarCount); Serial.print("  "); Serial.println(StableCarMaxCount);
    }*/
    
    //SendWheelSpeed();
    //return;
    //VERBOSE? Serial.println("Adjusting Speed") : 0;    
    if(WheelSpeedUpdated[RIGHT] && WheelSpeedUpdated[LEFT])  //new speeds in buffer
     {
       //print the distance traveled
        String msg = "D_";
        msg += ((WheelDistance[RIGHT] + WheelDistance[LEFT])/2)/10;
        
        if(stdVERBOSE)
          Serial.println(msg);
        
        //Serial.print("y_"); Serial.println(tempYaw);       
        
       WheelSpeedUpdated[RIGHT] = 0;  //reset flags
       WheelSpeedUpdated[LEFT] = 0;
          
       
       //if(UseYawToDrive && (millis() - YawDriveTimer) > 20)
       if(UseYawToDrive && newGyroDataFlag)
       {
         newGyroDataFlag = false;
         YawDriveTimer = millis();
         float YawDiff = calibYaw - targetYaw;
        (YawDiff>180)? (YawDiff-=360) :0;
        (YawDiff<-180)? (YawDiff+=360) :0;
        
        float inc = (YawDiff/2)*0.01;
        //(inc<0.005)? (inc = 0.0) :0;
        WheelSpeedRatio += inc;
        WheelSpeedRatio = max((min(WheelSpeedRatio, 1.2)),0.8);
        //Serial.print("Y_inc "); Serial.print(inc);
        //Serial.print("     Y_cur "); Serial.print(calibYaw);Serial.print("   Y_tar "); Serial.print(targetYaw);Serial.print("    Y_diff "); Serial.print(YawDiff);Serial.print("    SpRat "); Serial.println(WheelSpeedRatio);
        
        //Serial.print("\t"); Serial.print(targetYaw);Serial.print("\t"); Serial.print(YawDiff);Serial.print("\t\t"); Serial.println(WheelSpeedRatio);
        
        SendWheelSpeed();
       }
       
       if(UseEncoderToDrive)
       {
         
         /*********** This code uses encoder reported speed to keep going strait ******************
         float EncoderAngleOffset = 0.0;
        if(reqAngle > 0)
         {
           (reqAngle>180)? (reqAngle -= 360) : 0;
           EncoderAngleOffset = 2*reqAngle*AngleRatio;
           DriveEncoderCount[RIGHT] += EncoderAngleOffset;
           reqAngle = 0.0; 
           //Serial.println("-");
         }
         
         //acount for encoder overshoot from turning
         DriveEncoderCount[RIGHT] -= encoderOvershoot;
         encoderOvershoot = 0;
         ************************************************************************/
         
         float encoderCountDiff = abs(DriveEncoderCount[RIGHT] - DriveEncoderCount[LEFT]);
         //(encoderCountDiff < 30)?(StableCarCount++, Serial.print("**"), Serial.println(StableCarCount)) : (StableCarCount =0);
         //Serial.print("/t"); Serial.println(DriveEncoderCount[RIGHT] - DriveEncoderCount[LEFT]);
         
         float tempSpeedRatio = 0.0;
         float ratioIncrement = 0.0;
         
         int minEncDiff =0;
         (UseYawToDrive)?(minEncDiff=999):(minEncDiff=30);

         if(encoderCountDiff < minEncDiff && !speedMatchRecovery)
         {
           //Serial.println("\t\t=");
           tempSpeedRatio = WheelSpeed[RIGHT]/WheelSpeed[LEFT];  //calculate wheel speed ratio
           if(1.01<tempSpeedRatio || tempSpeedRatio<0.99)
             ratioIncrement = 0.005;
           if(1.02<tempSpeedRatio || tempSpeedRatio<0.98)
             ratioIncrement = 0.01;
           if(1.05<tempSpeedRatio || tempSpeedRatio<0.95)
             ratioIncrement = 0.03;
             
           (tempSpeedRatio>1)? (WheelSpeedRatio -= ratioIncrement) : (WheelSpeedRatio += ratioIncrement);
           LastWheelSpeedRatio = WheelSpeedRatio;
         }
         else
         {
           //Serial.println("\t\t-");
           speedMatchRecovery = true;
           if(encoderCountDiff < minEncDiff || recoveringSpeed)
           {
             DriveEncoderCount[RIGHT] = DriveEncoderCount[LEFT];
             
             recoveringSpeed = true;
             ratioIncrement = 0.005;
             tempSpeedRatio = WheelSpeedRatio/LastWheelSpeedRatio; 
             
             (tempSpeedRatio>1)? (WheelSpeedRatio -= ratioIncrement) : (WheelSpeedRatio += ratioIncrement);
             
             if((WheelSpeedRatio > (LastWheelSpeedRatio-0.02)) && (WheelSpeedRatio < (LastWheelSpeedRatio+0.02)))
             {
               
               speedMatchRecovery = false;
               recoveringSpeed = false;
             }
           }
           else
           {
             //Serial.println("\t\t*");
             tempSpeedRatio =1+((DriveEncoderCount[RIGHT] - DriveEncoderCount[LEFT])/100.0);
             if(encoderCountDiff > minEncDiff)
               ratioIncrement = 0.005;
             if(encoderCountDiff > 50)
               ratioIncrement = 0.008;
             if(encoderCountDiff > 70)
               ratioIncrement = 0.01;
             if(encoderCountDiff > 100)
               ratioIncrement = 0.015;
               
             (tempSpeedRatio>1)? (WheelSpeedRatio = max((WheelSpeedRatio-ratioIncrement),(LastWheelSpeedRatio-0.2))) : (WheelSpeedRatio = min((WheelSpeedRatio+ratioIncrement),(LastWheelSpeedRatio+0.2)));
           }
         }
         
         /*if (1.1<tempSpeedRatio || tempSpeedRatio<0.9)
           ratioIncrement = 0.03;
         if (1.2<tempSpeedRatio || tempSpeedRatio<0.8)
           ratioIncrement = 0.05; */ 
                  
         if(VERBOSE)
         {
           Serial.print("S-Ratio: "); Serial.print(WheelSpeedRatio); Serial.print("    Inc: "); Serial.println(ratioIncrement);
         }
         // ***** this code uses encoder count difference to try to keep the bot on the same line
        /* if(abs(EncoderCount[LEFT] - EncoderCount[RIGHT]) > 0)
         {
             if(VERBOSE)
             {
               Serial.print("Count Diff:   "); Serial.println(EncoderCount[LEFT] - EncoderCount[RIGHT]);
             }
             float temp = 0.00;//1;//*(abs(EncoderCount[LEFT] - EncoderCount[RIGHT]));
             ratioIncrement += temp;
             (EncoderCount[LEFT] > EncoderCount[RIGHT])? (WheelSpeedRatio += temp) : (WheelSpeedRatio -= temp);
         }*/
         
         if(ratioIncrement != 0.0)
           SendWheelSpeed();       
       }
     }
  }
}

void SendWheelSpeed()
{
  MotorPwmVal[RIGHT] = min((max( ((WheelSpeedRatio*reqPWM) + WheelSpeedInc) ,0)), 255);
  MotorPwmVal[LEFT] = min((max(reqPWM,0)),255);
  
  analogWrite(MotorPwmPin[LEFT], MotorPwmVal[LEFT]);
  analogWrite(MotorPwmPin[RIGHT], MotorPwmVal[RIGHT]);
    
  /*
  //Current sense code
  float CurrentSenseVal[2] = {0.0,0.0};
  CurrentSenseVal[LEFT] = (analogRead(CurrentSensePin[LEFT])/.165);
  CurrentSenseVal[RIGHT] = (analogRead(CurrentSensePin[RIGHT])/.165);
  Serial.print(WheelSpeedRatio); Serial.print(" * "); Serial.print("Current: "); Serial.print(CurrentSenseVal[LEFT]); Serial.print(", "); Serial.println(CurrentSenseVal[RIGHT]);
  */
  
  //if(VERBOSE)
  {
    //Serial.print(WheelSpeedRatio); Serial.println(" * "); //Serial.print("Actual: "); Serial.print(WheelSpeed[LEFT]); Serial.print(", "); Serial.println(WheelSpeed[RIGHT]);// Serial.print(" Set: "); Serial.print(MotorPwmVal[LEFT]); Serial.print(", "); Serial.println(MotorPwmVal[RIGHT]);
  }
  //Serial.print(WheelSpeed[LEFT]); Serial.print(", "); Serial.print(WheelSpeed[RIGHT]); Serial.print(", "); Serial.print(MotorPwmVal[LEFT]); Serial.print(", "); Serial.println(MotorPwmVal[RIGHT]);
  //Serial.print("E_");Serial.print(MotorPwmVal[LEFT]); Serial.print("---"); Serial.println(MotorPwmVal[RIGHT]);
  String msg = "E_";
  msg+= MotorPwmVal[LEFT];
  msg += "---";
  msg += MotorPwmVal[RIGHT];
  //Serial.println(msg);
}

// ================================================================
// ===                      Turning Function                    ===
// ================================================================

void ChangeOrientation()
{
  if(VERBOSE)
    Serial.println("Turning");

  if (!firstRunFlag)
  {
    
    Serial.print("FRF--- ");     Serial.println(firstRunFlag);
    //minTurnPWM = StdPWM / 2;
    //tempYaw = (tempYaw + (int)reqAngle) % 360;
    //targetAngle = 
    
    angleStartDelayTimer = millis();
    StopCarrier();
    if (reqAngle > 180 && reqAngle < 359.9)
    {
       digitalWrite(MotorDirPin[LEFT], LOW);
       MotorDir[LEFT] = 0;
       digitalWrite(MotorDirPin[RIGHT], HIGH); 
       MotorDir[RIGHT] = 1;
       reqAngle = 360 - reqAngle; 
       turningDirection = true;
       Serial.print("CHANGING MOTOR DIR PIN --- LEFT --- Angle: "); Serial.println(reqAngle);
    }
    else
    {
       digitalWrite(MotorDirPin[LEFT], HIGH);
       MotorDir[LEFT] = 1;
       digitalWrite(MotorDirPin[RIGHT], LOW);  
       MotorDir[RIGHT] = 0;
       turningDirection = false;
       Serial.print("CHANGING MOTOR DIR PIN --- RIGHT --- Angle: "); Serial.println(reqAngle);

    }

    EncoderCountTarget[LEFT] = EncoderCount[LEFT] + reqAngle * AngleRatio;
    EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + reqAngle * AngleRatio;
      
    completedFlag = false;
    firstRunFlag = true;
  }  
  
  if (millis() - angleStartDelayTimer < 1000)
    return;
  
  if (millis() % feedbackTime == 0)
  {
    EncoderErrorLast[LEFT] = EncoderError[LEFT];
    EncoderErrorLast[RIGHT] = EncoderError[RIGHT];
    //Serial.println("-----------------EncoderErrorLast Updated------------------------------");
  }
  EncoderError[LEFT] = EncoderCountTarget[LEFT] - EncoderCount[LEFT];
  EncoderError[RIGHT] = EncoderCountTarget[RIGHT] - EncoderCount[RIGHT];

  //Serial.print("LeftLast = ");Serial.print(EncoderErrorLast[LEFT]);Serial.print(";    Left = ");Serial.println(EncoderError[LEFT]);

  int MotorPwm[2] = {minTurnPWM + P * EncoderError[LEFT] + I * (EncoderErrorLast[LEFT]/EncoderError[LEFT]), 
                    minTurnPWM + P * EncoderError[RIGHT] + I * (EncoderErrorLast[RIGHT]/EncoderError[RIGHT])};
                    
  if (EncoderCount[LEFT] > EncoderCountTarget[LEFT])
    MotorPwm[LEFT] = 0;
  if (EncoderCount[RIGHT] > EncoderCountTarget[RIGHT])
    MotorPwm[RIGHT] = 0;
  if ((MotorPwm[LEFT] == 0) && (MotorPwm[RIGHT] == 0) && !completedFlag)
  {
    completedFlag = true;
    MotorPwm[LEFT] = 0;
    MotorPwm[RIGHT] = 0;
    angleEndDelayTimer = millis();
  }    
  analogWrite(MotorPwmPin[LEFT], MotorPwm[LEFT]);
  analogWrite(MotorPwmPin[RIGHT], MotorPwm[RIGHT]);
  if (completedFlag)
  {
    
    
    encoderOvershoot = EncoderError[LEFT] + EncoderError[RIGHT];
    float PWMInc = (((abs(encoderOvershoot))/100.0)*5.0);
    //Serial.println(int(PWMInc));
    //if(abs(encoderOvershoot)>80)
    //  minTurnPWM = minTurnPWM - min(int(PWMInc),2);
    
    (!turningDirection) ? (encoderOvershoot *= -1) :0;
    
    
    
    //Restore Motor Direction Pin
    digitalWrite(MotorDirPin[LEFT], HIGH);
    MotorDir[LEFT] = 1;
    digitalWrite(MotorDirPin[RIGHT], HIGH);
    MotorDir[RIGHT] = 1;
     //Serial.println("###################################################");
  //======================Delay after Completed Turning===========================================
     if (millis() - angleEndDelayTimer < 500)
        return;
  //==============================================================================================
      Serial.print("EncoderOvershoot = ");Serial.print(encoderOvershoot); Serial.print(" - "); Serial.println(minTurnPWM);
        reqAngle = 0;
    firstRunFlag = false;
    
   
  }   
  
  /*
  //=====================================Check for Out Of Range Input Angle===================== 
  if (reqAngle < 0 || reqAngle > 360)
  {
    if(VERBOSE)
      Serial.println("E_Requested Angle Out of Range");
    return;
  }
  //============================================================================================
  
  //=======================================Debug================================================
  if(VERBOSE)
    Serial.println("Turning");
  //============================================================================================

  //=======================================First Run Code=======================================
  //This section only runs once if a new angle is requested. The rest of the code run
  if (!firstRunFlag)
  {
    angleStartDelayTimer = millis();                  //Record the starting time when an angle is requested
    speedCheckTimer[LEFT] = millis();
    speedCheckTimer[RIGHT] = millis();
    
    StopCarrier();
    
    targetAngle = int(calibYaw - reqAngle);           //Set the target absolute angle
    (targetAngle < 0) ? targetAngle += 360 : 0;
    
    if (reqAngle > 180 && reqAngle < 359.9)           //Setup the direction pin so that if the requested angle is between 180 and 359.9, the carrier will turn in the opposite direction
    {                                                 //The angle 360 is reserved for a full turn. This is most probably will not be used.
       digitalWrite(MotorDirPin[LEFT], LOW);
       MotorDir[LEFT] = 0;
       digitalWrite(MotorDirPin[RIGHT], HIGH);
       MotorDir[RIGHT] = 1;
    }
    else
    {
       digitalWrite(MotorDirPin[LEFT], HIGH);
       MotorDir[LEFT] = 1;
       digitalWrite(MotorDirPin[RIGHT], LOW);
       MotorDir[RIGHT] = 0;
    }
    
    completedFlag = false;
    firstRunFlag = true;
  }  
  //===========================================================================================
  
  //======================Delay before Start Turning===========================================
  if (millis() - angleStartDelayTimer < 1000)
    return;
  //===========================================================================================
  
  errorC = min(abs(calibYaw - targetAngle), 360 - abs(calibYaw - targetAngle));
 
  if (errorC > 2 && !completedFlag)
  {
    MotorPwm[LEFT] = minPwm[LEFT];
    MotorPwm[RIGHT] = minPwm[RIGHT];
  }
  else if (errorC <= 2 && !completedFlag)
  {
    completedFlag = true;
    MotorPwm[LEFT] = 0;
    MotorPwm[RIGHT] = 0;
    angleEndDelayTimer = millis();
  }
  
  analogWrite(MotorPwmPin[LEFT], MotorPwm[LEFT]);
  analogWrite(MotorPwmPin[RIGHT], MotorPwm[RIGHT]);
  
  if (completedFlag)
  {
  //======================Delay after Completed Turning===========================================
     if (millis() - angleEndDelayTimer < 5000)
        return;
  //==============================================================================================
    reqAngle = 0;
    
    //Restore Motor Direction Pin
    digitalWrite(MotorDirPin[LEFT], HIGH);
    MotorDir[LEFT] = 1;
    digitalWrite(MotorDirPin[RIGHT], HIGH);
    MotorDir[RIGHT] = 1;
    firstRunFlag = false;
  }   
  */
}
