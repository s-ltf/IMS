//Motor Globals
int WheelSpeedUpdated[2] = {0,0};
int MotorPwmVal[2] = {0,0};
float WheelSpeedRatio = 0.94;
float LastWheelSpeedRatio = WheelSpeedRatio;
int reqPWM = 0;
int initSpeed = 100;
int StdSpeed = initSpeed;
float ratioIncrement = 0.0;
unsigned long DriveStraightTimer = 0;
//Angle PI Controller
int minSpeed = 50;
float P = 0.1;
float I = -0.08;
float AngleRatio = 64.0/9;
bool speedMatchRecovery = false;
bool recoveringSpeed = false;
const int UseSonarToDrive = 0, UseEncoderToDrive = 1;

void DriveCarrier()    //Driving state machine
{
  //Serial.print("Req Speed: "); Serial.println(reqSpeed);
    //translate driving speed
    if(reqSpeed == 0)
    {
      if(reqAngle > 0)
      {
        //if(isAngleRequested && WheelSpeed[RIGHT] != 0 && WheelSpeed[LEFT] != 0)
        //  StopCarrier();
        //else
          ChangeOrientation();
      }
      else
        StopCarrier();
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
      //*******
      DriveStraight();
     //SendWheelSpeed();
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
  VERBOSE? (Serial.println("Stopping carrier...")) : 0;
  /*
  if(WheelSpeed[LEFT] != 0)
  {
    reqPWM = max((reqPWM-2),0);
    analogWrite(MotorPwmPin[LEFT], reqPWM);
    analogWrite(MotorPwmPin[RIGHT], reqPWM);
  }
  else
  {
    analogWrite(MotorPwmPin[LEFT], 0);
    analogWrite(MotorPwmPin[RIGHT], 0);
    StdSpeed = initSpeed;
  }
  */
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

  //(VERBOSE)? (Serial.println("Going Straight")) : 0;
          
  if(WheelSpeed[RIGHT] == 0 || WheelSpeed[LEFT] == 0)    //motors are stopped
  {
    SendWheelSpeed();
  }
  else  //motors are running
  {
    if((millis() - DriveStraightTimer)> 2000)
    {
      DriveStraightTimer = millis();
      StdSpeed = min((StdSpeed+10),200);
    }
    //SendWheelSpeed();
    //return;
    //VERBOSE? Serial.println("Adjusting Speed") : 0;    
    if(WheelSpeedUpdated[RIGHT] && WheelSpeedUpdated[LEFT])  //new speeds in buffer
     {
       //print the distance traveled
        String msg = "D_";
        msg += (WheelDistance[RIGHT] + WheelDistance[LEFT])/2;
        Serial.println(msg);
        //Serial.print("D_"); Serial.println((WheelDistance[RIGHT] + WheelDistance[LEFT])/2);
        
        Serial.print("y_"); Serial.println(tempYaw);       
        
       WheelSpeedUpdated[RIGHT] = 0;  //reset flags
       WheelSpeedUpdated[LEFT] = 0;
       
       if(UseEncoderToDrive)
       {
         // ******* this code uses encoder reported speed to keep going strait
         float EncoderAngleOffset = 0.0;
         if(reqAngle > 0)
         {
           (reqAngle>180)? (reqAngle -= 360) : 0;
           EncoderAngleOffset = 2*reqAngle*AngleRatio;
           DriveEncoderCount[RIGHT] += EncoderAngleOffset;
           reqAngle = 0.0; 
           //Serial.println("-");
         }
         float encoderCountDiff = abs(DriveEncoderCount[RIGHT] - DriveEncoderCount[LEFT]);

         //Serial.println(DriveEncoderCount[RIGHT] - DriveEncoderCount[LEFT]);
         
         float tempSpeedRatio = 0.0;
         float ratioIncrement = 0.0;
         
         if(encoderCountDiff <20 && !speedMatchRecovery)
         {
           //Serial.println("*");
           tempSpeedRatio = WheelSpeed[RIGHT]/WheelSpeed[LEFT];  //calculate wheel speed ratio
           if(1.01<tempSpeedRatio || tempSpeedRatio<0.99)
             ratioIncrement = 0.01;
           if(1.02<tempSpeedRatio || tempSpeedRatio<0.98)
             ratioIncrement = 0.02;
           if(1.05<tempSpeedRatio || tempSpeedRatio<0.95)
             ratioIncrement = 0.04;
             
           (tempSpeedRatio>1)? (WheelSpeedRatio -= ratioIncrement) : (WheelSpeedRatio += ratioIncrement);
           LastWheelSpeedRatio = WheelSpeedRatio;
         }
         else
         {
           speedMatchRecovery = true;
           if(encoderCountDiff < 20 || recoveringSpeed)
           {
             //Serial.println("=");
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
             tempSpeedRatio =1+((DriveEncoderCount[RIGHT] - DriveEncoderCount[LEFT])/100.0);
             if(encoderCountDiff > 20)
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
      
     if(SonarUpdated && UseSonarToDrive)
     {
       // ******* this code uses sonar distance to keep going strait
       
       SonarUpdated = 0;
       float tempDistRatio = float(SonarDist[RIGHT])/60.0;  //calculate distance from wall ratio
       
       if(1.05<tempDistRatio || tempDistRatio<0.95)
         ratioIncrement = 0.01;
       if(1.2<tempDistRatio || tempDistRatio<0.8)
         ratioIncrement = 0.02;
       if (1.3<tempDistRatio || tempDistRatio<0.7)
         ratioIncrement = 0.03;
       /*if (1.4<tempDistRatio || tempDistRatio<0.6)
         ratioIncrement = 0.05;*/  
         
       if(VERBOSE)
       {
         Serial.print("SNR-Ratio: "); Serial.print(tempDistRatio); Serial.print("    Inc: "); Serial.println(ratioIncrement);
       }
       (tempDistRatio>1)? (WheelSpeedRatio -= ratioIncrement) : (WheelSpeedRatio += ratioIncrement);
 
       if(ratioIncrement != 0.0)
         SendWheelSpeed();
     }
  }
}



void SendWheelSpeed()
{
  MotorPwmVal[RIGHT] = min((max( (WheelSpeedRatio*reqPWM) ,0)), 255);
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
  
  if(VERBOSE)
  {
    Serial.print(WheelSpeedRatio); Serial.print(" * "); Serial.print("Actual: "); Serial.print(WheelSpeed[LEFT]); Serial.print(", "); Serial.print(WheelSpeed[RIGHT]); Serial.print(" Set: "); Serial.print(MotorPwmVal[LEFT]); Serial.print(", "); Serial.println(MotorPwmVal[RIGHT]);
  }
  //Serial.print(WheelSpeed[LEFT]); Serial.print(", "); Serial.print(WheelSpeed[RIGHT]); Serial.print(", "); Serial.print(MotorPwmVal[LEFT]); Serial.print(", "); Serial.println(MotorPwmVal[RIGHT]);
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
    //StopCarrier();
    tempYaw = (tempYaw + (int)reqAngle) % 360;
    if (reqAngle > 180 && reqAngle < 359.9)
    {
       digitalWrite(MotorDirPin[LEFT], LOW);
       digitalWrite(MotorDirPin[RIGHT], HIGH); 
       reqAngle = 360 - reqAngle; 
    }
    else
    {
       digitalWrite(MotorDirPin[LEFT], HIGH);
       digitalWrite(MotorDirPin[RIGHT], LOW);  
    }
    
    /*
    if (reqAngle > 179.9 && reqAngle < 180.1)
    { 
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 1280;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 1280; 
       tempYaw = (tempYaw + 180) %360;
    }
    else if ((reqAngle > 89 && reqAngle < 91) || (reqAngle > 269 && reqAngle < 271))
    { 
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 640;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 640;   
       tempYaw = (tempYaw + 90) %360;
    }
    else if ((reqAngle > 44 && reqAngle < 46) || (reqAngle > 314 && reqAngle < 316)){
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 320;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 320; 
       tempYaw = (tempYaw + 45) %360; 
    }
    else if ((reqAngle > 4 && reqAngle < 6) || (reqAngle > 354 && reqAngle < 356)){
       EncoderCountTarget[LEFT] = EncoderCount[LEFT] + 1;
       EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + 1;  
    }
    */
    
    EncoderCountTarget[LEFT] = EncoderCount[LEFT] + reqAngle * AngleRatio;
    EncoderCountTarget[RIGHT] = EncoderCount[RIGHT] + reqAngle * AngleRatio;
    
    
    isAngleRequested = false;
  }  
  
  EncoderErrorLast[LEFT] = EncoderError[LEFT];
  EncoderErrorLast[RIGHT] = EncoderError[RIGHT];
  EncoderError[LEFT] = EncoderCountTarget[LEFT] - EncoderCount[LEFT];
  EncoderError[RIGHT] = EncoderCountTarget[RIGHT] - EncoderCount[RIGHT];
  
  int MotorPwm[2] = {minSpeed + P * EncoderError[LEFT] + I * EncoderErrorLast[LEFT], 
                    minSpeed + P * EncoderError[RIGHT] + I * EncoderErrorLast[RIGHT]};
   
                   
                    
  if (EncoderCount[LEFT] > EncoderCountTarget[LEFT])
    MotorPwm[LEFT] = 0;
  if (EncoderCount[RIGHT] > EncoderCountTarget[RIGHT])
    MotorPwm[RIGHT] = 0;
  if ((MotorPwm[LEFT] == 0) && (MotorPwm[RIGHT] == 0))
  {
    reqAngle = 0;
    digitalWrite(MotorDirPin[LEFT], HIGH);
    digitalWrite(MotorDirPin[RIGHT], HIGH); 
    isAngleRequested = true;
  }    
  analogWrite(MotorPwmPin[LEFT], MotorPwm[LEFT]);
  analogWrite(MotorPwmPin[RIGHT], MotorPwm[RIGHT]);
}
