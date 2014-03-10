int SonarToPing = 0;

// ================================================================
// ===                      Sonar Setup Function                ===
// ================================================================

void SetupSonar()
{
  Serial.print("I_Initializing "); Serial.print(SonarCount); Serial.println(" Sonars ...");
  for(int i =0; i < SonarCount; i++)
  {
    pinMode(SonarPwmPin[i], INPUT);
  }
  Serial.println("I_Sonars Initialization complete!");
}

// ================================================================
// ===                      Main Sonar Function                 ===
// ================================================================

void GetSonarDist()
{
  //unsigned long bla = millis();
  
  SonarPulse[SonarToPing] = pulseIn(SonarPwmPin[SonarToPing], HIGH);
  //147uS per inch
  SonarDist[SonarToPing] = (SonarPulse[SonarToPing]/147)*InchesToCm;
  Serial.print("s");Serial.print(SonarToPing+1);Serial.print("_");Serial.println(SonarDist[SonarToPing]);
  //Serial.println( ((SonarToPing+1)*(1000)) + SonarDist[SonarToPing]);
  //Serial.print("Sonar Time    "); Serial.println(millis() - bla);
  SonarToPing = (SonarToPing + 1)%SonarCount;

}
