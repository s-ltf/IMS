unsigned long FakeDataTimer = 0;
int charCount = 0;
int SignMultiplier = 0;

void sendFakeData()
{
  if(millis() - FakeDataTimer > 100)
  {
    Serial.println("s1_76");
    Serial.println("s2_176");
    Serial.println("s3_355");
    Serial.println("y_13");
    Serial.println("d_50");
    FakeDataTimer = millis();
  }
}

void receiveData()
{
  char currentChar = ' ';
  int currentInt = 0;
  
  while(Serial.available())
    {
      currentChar = (Serial.read());
      if (VERBOSE)
      {
        Serial.print("got   ");Serial.println(currentChar);
      }
      
      if(currentChar == 'n')
      {
        reqSpeed *= SignMultiplier;
        
        if (VERBOSE)
        {
          Serial.print("Speed:   "); Serial.print(reqSpeed); Serial.print("     Angle:   "); Serial.println(reqAngle);
        } 
        
        charCount = 0;
        return;  //n signifies end of line
      }
      
      currentInt = float(currentChar - '0');
      if(charCount == 0)
      {
        //receiving new values so reset old ones
        reqSpeed = 0;
        reqAngle = 0;
        
        (currentChar == '+')? (SignMultiplier = 1) : (SignMultiplier = -1);
      }
      else if(charCount < 3)
      {
        reqSpeed = reqSpeed + (currentInt*pow(10,2-charCount));
        if (VERBOSE)
        {
          Serial.print("reqSpeed   ");Serial.println(reqSpeed);
        }
      }
      else
      {
        reqAngle = reqAngle + (currentInt*pow(10,5-charCount));
        isAngleRequested = true;
        if (VERBOSE)
        {
          Serial.print("reqAngle   ");Serial.println(reqAngle);
        }
      }
      charCount++;
    }
    return;
}
