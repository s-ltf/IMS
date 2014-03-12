const int SonarPWPin = 5;
const int SonarTrigPin = 53;
float inches = 0;
unsigned long Timer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SonarPWPin, INPUT);
  pinMode(SonarTrigPin, OUTPUT);
  digitalWrite(SonarTrigPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Timer = millis();
  digitalWrite(SonarTrigPin, HIGH);

  while((millis() - Timer) < 20);
  
  Timer = millis();
  int pulse = CustompulseIn(SonarPWPin, HIGH);
  Timer = millis() - Timer;
  Serial.print("time: "); Serial.print(Timer);
  
  inches = (pulse)/147;
  Serial.print("     dist: "); Serial.println(inches);
  digitalWrite(SonarTrigPin, LOW);
  delay(100);
}

String msg = "";

int CustompulseIn(int pin, uint8_t state)
{
  if(digitalRead(pin) == state)
  {
   // we missed it!!
   Serial.println("Too late for the pulse");
    return 0; 
  }
  
  unsigned long TempTimer = micros();
  while(digitalRead(pin) != state);
  //Serial.print("wasted   "); Serial.println(micros() - TempTimer);
  
  TempTimer = micros();
  while(digitalRead(pin) == state);
  return(micros() - TempTimer);
}
