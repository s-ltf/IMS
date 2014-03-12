
// ================================================================
// ===                      Sonar Setup Function                ===
// ================================================================

void SetupSonar()
{
  Serial.print("I_Initializing "); Serial.print(SonarCount); Serial.println(" Sonars ...");
  pinMode(SonarPwmPin[LEFT], INPUT);
  digitalWrite(SonarPwmPin[LEFT], HIGH);
  attachInterrupt(SonarIntrptPin[LEFT], LeftSonarISR, CHANGE);
  
  pinMode(SonarPwmPin[RIGHT], INPUT);
  digitalWrite(SonarPwmPin[RIGHT], HIGH);
  attachInterrupt(SonarIntrptPin[RIGHT], RightSonarISR, CHANGE);

  pinMode(SonarPwmPin[FRONT], INPUT);
  digitalWrite(SonarPwmPin[FRONT], HIGH);
  attachInterrupt(SonarIntrptPin[FRONT], FrontSonarISR, CHANGE);  
  
  Serial.println("I_Sonars Initialization complete!");
}

// ================================================================
// ===                      Sonar ISR Function                  ===
// ================================================================
unsigned long SonarTimer[SonarCount] = {0,0,0};

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
  {
   if(SonarUpdatedFlag[i])
   {
      msg = "s"; msg += i; msg += "_"; msg += SonarDist[i];
      
      if(fakePrint && i==0)
       {
          msg = ""; msg += SonarDist[i];
       }
       if(fakePrint && i==1)
       {
          msg = "\t"; msg += SonarDist[i];
       }
       if(fakePrint && i==2)
       {
          msg = "\t\t"; msg += SonarDist[i];
       }
       
       
      if(isAngleRequested)  //not turning
        Serial.println(msg);
      
      SonarUpdatedFlag[i] = false;
      //Serial.println(millis());
   }
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
