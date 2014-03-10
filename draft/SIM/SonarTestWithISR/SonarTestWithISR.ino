//Feel free to use this code.
//Please be respectful by acknowledging the author in the code if you use or modify it.
//Author: Bruce Allen
//Date: 23/07/09

//Digital pin 7 for reading in the pulse width from the MaxSonar device.
//This variable is a constant because the pin will not change throughout execution of this code.
int pwPin= 0; 
unsigned long SonarTimer = 0;


void setup() {

  //This opens up a serial connection to shoot the results back to the PC console
  Serial.begin(9600);
  
  pinMode(pwPin, INPUT);
  //digitalWrite(pwPin[0], HIGH); //Internal Pullup Resistor
  
  
  //Used to read in the pulse that is being sent by the MaxSonar device.
  //Pulse Width representation with a scale factor of 147 uS per Inch.
  
  //digitalWrite(11, HIGH);
  pinMode(10, OUTPUT);
  analogWrite(10, 255);
//analogWrite(11, 150);
  
  //attachInterrupt(pwPin, SonarISR, RISING);
  attachInterrupt(pwPin, SonarISR, RISING);
  
  //attachInterrupt(pwPin[1], SonarISRLow, FALLING);
}

void loop() {
  //Serial.println(millis());
  //delay(1);
   // Serial.print("Time:  "); Serial.println(millis());
}
unsigned long pulse = 0;
float cm = 0.0;
unsigned long timer = 0;
unsigned long TotTime = 0;

void SonarISR()
{  
  //timer = micros();
  //
  TotTime = (millis() - timer);
  if(90 <= TotTime && TotTime <= 110)
  {
    Serial.println("Good");
  }
  else
    Serial.println(TotTime);
    
  timer = millis();
}

void SonarPulseISR ()
{
  
  pulse = pulseIn(pwPin, HIGH);
  cm = ((pulse / 147.00)*2.54);
  Serial.print(cm); Serial.println("  cm");
}
