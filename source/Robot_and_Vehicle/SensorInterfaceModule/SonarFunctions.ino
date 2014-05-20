//local globals
bool FrontSonarIntFlag = false;

// ================================================================
// ===                      Sonar Setup Function                ===
// ================================================================

void SetupSonar()
{
  Serial.print("I_Initializing "); Serial.print(SonarCount); Serial.println(" Sonars ...");
  
  //setup the sonar tiggering
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  
  digitalWrite(24, LOW);
  digitalWrite(22, HIGH);
  delay(1);
  digitalWrite(22, LOW);
  pinMode(22, INPUT);
  digitalWrite(22, HIGH);
  
  
  pinMode(SonarPwmPin[LEFT], INPUT);
  digitalWrite(SonarPwmPin[LEFT], HIGH);
  attachInterrupt(SonarIntrptPin[LEFT], LeftSonarISR, CHANGE);
  
  pinMode(SonarPwmPin[RIGHT], INPUT);
  digitalWrite(SonarPwmPin[RIGHT], HIGH);
  attachInterrupt(SonarIntrptPin[RIGHT], RightSonarISR, CHANGE);

  pinMode(SonarPwmPin[FRONT], INPUT);
  digitalWrite(SonarPwmPin[FRONT], HIGH);
  //Polling
  //pinMode(SonarTrigPin[FRONT], OUTPUT);
  //digitalWrite(SonarTrigPin[FRONT], LOW);
  //Interrupts
  //attachInterrupt(SonarIntrptPin[FRONT], FrontSonarISR, CHANGE);  
  //PinChange Interrupts
  attachPCInterrupt(SonarPwmPin[FRONT]);
  
  Serial.print("I_Sonars Initialization... ");
  //(SonarDist[LEFT] && SonarDist[RIGHT] && SonarDist[FRONT])? (Serial.println("Successful!")) : (Serial.println("Fail!"));
}

// ================================================================
// ===                      Sonar PCINT Function                ===
// ================================================================

void attachPCInterrupt(int Pin)
{
  //cli();
  bitSet(PCICR, PCIE1);
  bitSet(PCMSK1, PCINT10);    //Sonar
  bitSet(PCMSK1, PCINT9);      //MPU
  //sei();
}

ISR(PCINT1_vect)
{
  if(digitalRead(SonarPwmPin[FRONT]))
  {
    SonarTimer[FRONT] = micros();
    FrontSonarIntFlag = true;
  }
  if(!digitalRead(SonarPwmPin[FRONT]) && FrontSonarIntFlag)
  {
    SonarTimer[FRONT] = micros() - SonarTimer[FRONT];
    SonarDist[FRONT] = ((SonarTimer[FRONT])/147)*2.54;
    SonarUpdatedFlag[FRONT] = true;
    FrontSonarIntFlag = false;
  }
  if(digitalRead(MPUIntPin))
  {
    mpuInterrupt = true;
  }
  
  
}

// ================================================================
// ===                      Sonar ISR Function                  ===
// ================================================================

void LeftSonarISR()
{
  if(digitalRead(SonarPwmPin[LEFT]))
  {
    SonarTimer[LEFT] = micros();
  }
  else
  {
    SonarTimer[LEFT] = micros() - SonarTimer[LEFT];
    SonarDist[LEFT] = ((SonarTimer[LEFT])/147)*2.54;
    SonarUpdatedFlag[LEFT] = true;
  }
}

void RightSonarISR()
{
  if(digitalRead(SonarPwmPin[RIGHT]))
  {
    SonarTimer[RIGHT] = micros();
  }
  else
  {
    SonarTimer[RIGHT] = micros() - SonarTimer[RIGHT];
    SonarDist[RIGHT] = ((SonarTimer[RIGHT])/147)*2.54;
    SonarUpdatedFlag[RIGHT] = true;
  }
}
void FrontSonarISR()
{
  if(digitalRead(SonarPwmPin[FRONT]))
  {
    SonarTimer[FRONT] = micros();
  }
  else
  {
    SonarTimer[FRONT] = micros() - SonarTimer[FRONT];
    SonarDist[FRONT] = ((SonarTimer[FRONT])/147)*2.54;
    SonarUpdatedFlag[FRONT] = true;
  }
}

// ================================================================
// ===                      Main Sonar Function                 ===
// ================================================================

void GetSonarDist()
{
  int fakePrint=0;
  String msg = "";
  for (int i = 0; i < SonarCount; i++)
  //int i = 2;
  {
   if(SonarUpdatedFlag[i])
   {
      msg = "s"; msg += i; msg += "_"; msg += SonarDist[i];
      
      if(fakePrint && i==0)
       {
          msg = "\t\t"; msg += SonarDist[i];
       }
       if(fakePrint && i==1)
       {
          msg = "\t\t\t\t\t\t"; msg += SonarDist[i];
       }
       if(fakePrint && i==2)
       {
          msg = "\t\t\t\t"; msg += SonarDist[i];
       }
       
      
      if(!firstRunFlag && !SonarFilter[i])  //not turning
      {
        if(stdVERBOSE)
          Serial.println(msg);
        //long range filter
        (SonarDist[i]>700)? (SonarFilter[i] = true) :0;
      }
      else
        SonarFilter[i] = false;
      
      SonarUpdatedFlag[i] = false;
      //Serial.println(millis());
      /******
      if((SonarDist[i] < WallDist[i] * 1.02) && (SonarDist[i] > WallDist[i] * 0.98) && (WheelSpeed[LEFT] != 0) && (WheelSpeed[RIGHT] != 0) )
      {
        SonarDistCount[i] ++;
        WallDist[i] = (WallDist[i] + SonarDist[i])/2;
        Serial.print("\t\t\t\t\tI_Y_P2W"); Serial.println(WallDist[i]);
        if ((SonarDistCount[LEFT] > SonarDistCountMax) && (SonarDistCount[RIGHT] > SonarDistCountMax))
        {
          SonarDistCount[LEFT] = 0;
          SonarDistCount[RIGHT] = 0;
          
          GyroParllWallOffset = (calibYaw - targetYaw);
          GyroCalibOffset += GyroParllWallOffset;
          (GyroCalibOffset>360)?(GyroCalibOffset -= 360):0;
          (GyroCalibOffset<0)?(GyroCalibOffset += 360):0;
          Serial.print("\t\t\t\t\tI_Y_P2W"); Serial.println(calibYaw);
        }
      }
      else
      {
        SonarDistCount[i] -= 5;
        WallDist[i] = SonarDist[i];
        robotPrallelToWall = false;
      }
      ******/
   }
  }
  
}

void PollSonarDist(int SonarNumber)
{
  if(SonarTrigFlag[SonarNumber])
  {
    //Sonar has been triggered  
    //get distance
    unsigned long bla = millis();
    SonarPulse[SonarNumber] = pulseIn(SonarPwmPin[SonarNumber], HIGH);
    SonarDist[SonarNumber] = (SonarPulse[SonarNumber]/147)*InchesToCm;
    bla = millis() - bla;
    //Serial.println(bla);
    
    //reset flags
    //if(SonarDist[SonarNumber] != 0)
    {
      SonarUpdatedFlag[SonarNumber] = true;
      digitalWrite(SonarTrigPin[SonarNumber], LOW);
      SonarTimer[SonarNumber] = millis() + 120;
      SonarTrigFlag[SonarNumber] = false;
    }
  }
  else
  {
     //Trigger sonar 
     digitalWrite(SonarTrigPin[SonarNumber], HIGH);
     SonarTrigFlag[SonarNumber] = true;
     SonarTimer[SonarNumber] = millis() + 19;
  }
}

/*
void GetSonarDist()
{
    if(SonarToPing == LEFT)      //fake left sonar data
    {
      //****
      Serial.println("s1_200");// Serial.println(250 - SonarDist[RIGHT]);
      SonarToPing = (SonarToPing + 1)%SonarCount;
      return;
    }
    
    
   //Time to go for a reading!
    //unsigned long bla = millis();
    
    //SonarPulse[SonarToPing] = CustompulseIn(SonarPwmPin[SonarToPing], HIGH);
    SonarPulse[SonarToPing] = pulseIn(SonarPwmPin[SonarToPing], HIGH);
    
    //bla = millis() - bla;
    //Serial.println(bla);
    //Serial.print("Sonar Time    "); Serial.println(millis() - bla);
    //147uS per inch
    SonarDist[SonarToPing] = (SonarPulse[SonarToPing]/147)*InchesToCm;
    //if(SonarToPing == 1)
    {
      String msg = "s";
      msg += SonarToPing+1;
      msg += "_";
      msg += SonarDist[SonarToPing];
      Serial.println(msg);
    }
    
    (SonarToPing == RIGHT)? (SonarUpdated = 1) : (SonarUpdated = 0);
    
    SonarToPing = (SonarToPing + 1)%SonarCount;
    //Serial.print("Sonar "); Serial.print(SonarToPing); Serial.print(" Tick @ "); Serial.println(SonarTickTimer[SonarToPing]);
    //Serial.println( ((SonarToPing+1)*(1000)) + SonarDist[SonarToPing]);
}

int CustompulseIn(int pin, uint8_t state)
{
  if(digitalRead(pin) == state)
  {
   // we missed it!!
   //Serial.println("Too late for the pulse");
    return 0; 
  }
  
  unsigned long TempTimer = micros();
  while(digitalRead(pin) != state);
  //Serial.print("wasted   "); Serial.println(micros() - TempTimer);
  
  TempTimer = micros();
  while(digitalRead(pin) == state);
  return(micros() - TempTimer);
  
}*/
