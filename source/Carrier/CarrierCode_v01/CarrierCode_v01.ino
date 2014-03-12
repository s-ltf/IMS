#define LEFT 0
#define RIGHT 1
#define INTERVAL 1000

unsigned int interruptCount[2] = {0, 0};
float wheelSpeed[2] = {0.0,0.0};
unsigned int intervalTimer = 0;
unsigned int lastInterruptTime[2] = {0, 0};
int pinValue[2] = {0, 0};
int lastPinValue[2] = {0, 0};
float carrierDistance = 0;
float interruptToInch = 84.0/205.0;
int speedRequested = 0;
int angleRequested = 0;
int newRequest = 0;
int wheelSpeedPWN[2] = {0,0};
int PWMincrement = 20;
int PWMpin[2] = {11,5};

void setup() {
  //Setup Serial Port
  Serial.begin(9600);
 
  //Attach interrupts to pin D2 and D3 
  attachInterrupt(LEFT, LeftPulseInterrupt, CHANGE);
  attachInterrupt(RIGHT, RightPulseInterrupt, CHANGE);

  //Set Initial pin 2 and 3 values
  pinValue[LEFT] = digitalRead(LEFT+2);
  lastPinValue[LEFT] = pinValue[LEFT];
  pinValue[RIGHT] = digitalRead(RIGHT+2);
  lastPinValue[RIGHT] = pinValue[RIGHT];
  
  //Set output pins
  pinMode(PWMpin[RIGHT], OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(PWMpin[LEFT], OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  //See the board markings for each pin function 
  //Set Left Wheel PWM pin
  digitalWrite(8, LOW);
  digitalWrite(13, HIGH);
  analogWrite(PWMpin[LEFT], 0);
  
  //Set Right Wheel PWM pin
  digitalWrite(9, LOW);
  digitalWrite(12, HIGH);
  analogWrite(PWMpin[RIGHT], 0);
}

void loop(){
  if (millis() - intervalTimer > INTERVAL){
      //Any periodic task goes here
      Serial.println("I am in Main");
      /*
      if(speedRequested == 0 && angleRequested == 0)
      {
          GetUserInput();    //Request User Speed and Angle
          Serial.println("Setting Wheel Speeds");
          SetWheelSpeeds(1, speedRequested, angleRequested);
      }
      */      
      /*else
      {
          Serial.print("Data on serial? "); Serial.println(Serial.available());
          if(Serial.available()!=0)                 // read data
            StopRobot();
      }*/
      carrierDistance = ((interruptCount[RIGHT]*interruptToInch ) +  (interruptCount[LEFT]*interruptToInch ) ) / 2;  
      //carrierDistance = (interruptCount[LEFT]*interruptToInch);
      
      //SendSerialDistance();  //Displays last calculated speed for left and right wheels
      //SendSerialSpeed();
      
      intervalTimer = millis();
  } 
}

void SetWheelSpeeds(int newRequest, int speedRequested, int angleRequested)
{
  Serial.println("I am in set Wheel Speeds");
  if(newRequest)  //Setting new wheel speeds
  {
      Serial.println("First time to set Wheel Speeds");
      if(angleRequested == 0) // No turning
      {
        wheelSpeedPWN[RIGHT] = speedRequested;
        wheelSpeedPWN[LEFT] = speedRequested;
      }
      if(angleRequested > 0) // Turn Right
      {
        wheelSpeedPWN[RIGHT] = speedRequested*0.75;
        wheelSpeedPWN[LEFT] = speedRequested;
      }
      if(angleRequested < 0) // Turn Left
      {
        wheelSpeedPWN[RIGHT] = speedRequested;
        wheelSpeedPWN[LEFT] = speedRequested*0.75;
      }
  }
  else  //make sure that the left wheel is follow  ing the right wheel
  {
      Serial.print("Adjust Wheel Speeds. Speed Right = "); Serial.print(wheelSpeed[RIGHT]); Serial.print("Speed Left = "); Serial.println(wheelSpeed[LEFT]);
       if(wheelSpeed[LEFT] < wheelSpeed[RIGHT])
       {
         wheelSpeedPWN[LEFT] += PWMincrement*(wheelSpeed[RIGHT]/wheelSpeed[LEFT]);
       }
       if(wheelSpeed[LEFT] > wheelSpeed[RIGHT])
       {
         wheelSpeedPWN[LEFT] -= PWMincrement*(wheelSpeed[LEFT]/wheelSpeed[RIGHT]);
       }
  }
  
  analogWrite(PWMpin[LEFT], wheelSpeedPWN[LEFT]);
  analogWrite(PWMpin[RIGHT], wheelSpeedPWN[RIGHT]);
  Serial.print("Set Wheel Speeds: Left: "); Serial.print(wheelSpeedPWN[LEFT]); Serial.print("Right: "); Serial.println(wheelSpeedPWN[RIGHT]);
  
}

void StopRobot()
{
  analogWrite(PWMpin[LEFT], 0);
  analogWrite(PWMpin[RIGHT], 0);
  Serial.print("ROBOT STOPED!");
  
}

void GetUserInput()
{ 
    Serial.println("\nPlease enter a speed (0-->255): ");    
    while (Serial.available()==0);                 // wait for data
    speedRequested = Serial.parseInt();
    Serial.print("got speed input = "); Serial.println(speedRequested);
    while (Serial.available() > 0) 
      Serial.read();      //clear buffer
        
    Serial.println("\nPlease enter an angle (-180 --> +180): ");
    while (Serial.available()==0);                 // wait for data
    angleRequested = Serial.parseInt();
    Serial.print("got angle input = "); Serial.println(angleRequested);
    while (Serial.available() > 0) 
      Serial.read();    //clear buffer
}

void SendSerialSpeed()
{
  Serial.print("Last Speed Right = ");
  Serial.print(wheelSpeed[RIGHT]);
  Serial.println("   mm/Sec");
  Serial.print("Last Speed Left = ");
  Serial.print(wheelSpeed[LEFT]);
  Serial.println("   mm/Sec");
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
    Serial.println("I am in left pulse interrupt");
    interruptCount[LEFT]++;
    
    //get the speed of the wheel
    int interruptInterval = millis() - lastInterruptTime[LEFT];
    wheelSpeed[LEFT] = ((interruptToInch*25.4) / interruptInterval)*1000;  //speed in mm/s
    lastInterruptTime[LEFT] = millis();
    //SendSerialLeft();
  }
  lastPinValue[LEFT] = pinValue[LEFT];
}

void RightPulseInterrupt(){
  pinValue[RIGHT] = digitalRead(RIGHT+2);
  if (pinValue[RIGHT] != lastPinValue[RIGHT])
  {
    //ISR
    Serial.println("I am in right pulse interrupt");
    interruptCount[RIGHT]++;
    
    //get the speed of the wheel
    int interruptInterval = millis() - lastInterruptTime[RIGHT];
    wheelSpeed[RIGHT] = ((interruptToInch*25.4) / interruptInterval)*1000; //speed in mm/s
    lastInterruptTime[RIGHT] = millis();
    //SendSerialRight();
    
    SetWheelSpeeds(0,0,0);  //adjust for any wheel slipage
  }
  lastPinValue[RIGHT] = pinValue[RIGHT];
}
