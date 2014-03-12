 int charCount = 0;
float reqSpeed = 0;
float reqAngle = 0;
int SignMultiplier = 0;  

void sendFakeData()
{
  if(millis() - LastTimer > 100)
  {
    Serial.println("s1_76");
    Serial.println("s2_176");
    Serial.println("s3_355");
    Serial.println("y_13");
    Serial.println("d_50");
    LastTimer = millis();
  }
}

bool receiveData()
{
  char currentChar = ' ';
 
  int currentInt = 0;
  while(Serial.available())
    {
      Serial.print("CharCount: "); Serial.println(charCount);
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
        
        (reqSpeed>=0)? (LED_State = true) : (LED_State = false);
        
          SignMultiplier = 0; 
          charCount = 0;
        reqSpeed = 0;
        reqAngle = 0;
        return true;  //n signifies end of line
      }
      
      currentInt = float(currentChar - '0');
      if(charCount == 0)
      {
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
        if (VERBOSE)
        {
          Serial.print("reqAngle   ");Serial.println(reqAngle);
        }
      }
      charCount++;
    }
    return false;
}
