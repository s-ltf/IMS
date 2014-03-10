// ================================================================
// ===                      Encoders Setup Function             ===
// ================================================================

void SetupEncoders()
{
  Serial.println("I_Initializing encoders...");
  pinMode(EncoderPin[LEFT] , INPUT);
  pinMode(EncoderPin[RIGHT] , INPUT);
  Serial.println("I_Encoders ready!");
}

// ================================================================
// ===                      Encoders Main Function              ===
// ================================================================

void GetEncoderData(int Wheel)    //wheel identifies if function is right or left
{
  //if the encoders don't change for 500 ms then the robot stoped! reset all values
  if((millis() - LastTickTime) > 500)
  {
    if(VERBOSE)
      Serial.println("Robot halted! Reset Encoders");
    WheelSpeed[RIGHT] = 0;
    WheelSpeed[LEFT] = 0;
    LastEncoderCount[RIGHT] = EncoderCount[RIGHT];
    LastEncoderCount[LEFT] = EncoderCount[LEFT];
    WheelSpeedTimer[RIGHT] = millis();
    WheelSpeedTimer[LEFT] = millis();
    EncoderCount[RIGHT] = EncoderCount[LEFT];
  }
  
  int CurrentEncoderPinValue = digitalRead(EncoderPin[Wheel]);
  if(LastEncoderPinValue[Wheel] != CurrentEncoderPinValue)
  {
    LastTickTime = millis();
    
    EncoderCount[Wheel]++;
    WheelDistance[Wheel] += EncoderTickDistance;

    if((EncoderCount[Wheel] - LastEncoderCount[Wheel]) >= 10)
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
