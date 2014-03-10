#define LEFT 0
#define RIGHT 1
#define INTERVAL 1000

unsigned int interruptCount[2] = {0, 0};
unsigned int intervalTimer = 0;
int pinValue[2] = {0, 0};
int lastPinValue[2] = {0, 0};
float carrierDistance = 0;
int requestedSpeed =0;
void setup() {
  //Setup Serial Port
  Serial.begin(9600);
 
  //Attach interrupts to pin D2 and D3 
  attachInterrupt(LEFT, LeftPulseInterrupt, CHANGE);
  attachInterrupt(RIGHT, RightPulseInterrupt, CHANGE);

  //Set output pins
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  //See the board markings for each pin function 
  //Set Left Wheel PWM pin
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
  analogWrite(11, 0);
  
  //Set Right Wheel PWM pin
  digitalWrite(9, LOW);
  digitalWrite(12, LOW);
  analogWrite(5, 0);

  //Set Initial pin 2 and 3 values
  pinValue[LEFT] = digitalRead(LEFT+2);
  lastPinValue[LEFT] = pinValue[LEFT];
  pinValue[RIGHT] = digitalRead(RIGHT+2);
  lastPinValue[RIGHT] = pinValue[RIGHT];
}

void loop(){
  if (millis() - intervalTimer > INTERVAL){
      //Any periodic task goes here
      //carrierDistance = interruptCount[LEFT]*float(84)/205;  
      //SendSerialDistance();
      Serial.println("Enter the speed (0-255): "); 
      Serial.println(!Serial.available());
      Serial.println(!requestedSpeed);
      while(!Serial.available() || !requestedSpeed);
      requestedSpeed = Serial.read();
      analogWrite(5,requestedSpeed);
      analogWrite(11,requestedSpeed);
      intervalTimer = millis();
      
  } 
}

void SendSerialDistance()
{
  Serial.print("Distance = ");
  Serial.print(carrierDistance);
  Serial.println("inch");
}

void SendSerialLeft()
{
  Serial.print("Left Interrupt = ");
  Serial.println(interruptCount[LEFT]);
}

void SendSerialRight()
{
  Serial.print("Right Interrupt = ");
  Serial.println(interruptCount[RIGHT]);
}

void LeftPulseInterrupt(){
  pinValue[LEFT] = digitalRead(LEFT+2);
  if (pinValue[LEFT] != lastPinValue[LEFT])
  {
    //ISR
    interruptCount[LEFT]++;
    //SendSerialLeft();
  }
  lastPinValue[LEFT] = pinValue[LEFT];
}

void RightPulseInterrupt(){
  pinValue[RIGHT] = digitalRead(RIGHT+2);
  if (pinValue[RIGHT] != lastPinValue[RIGHT])
  {
    //ISR
    interruptCount[RIGHT]++;
    //SendSerialRight();
  }
  lastPinValue[RIGHT] = pinValue[RIGHT];
}
