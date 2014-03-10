#define LEFT 0
#define RIGHT 1
#define INTERVAL 1000
#define POLLING_INTERVAL 5
#define VERBOSE 1
#define CONTROL_MODE 2 //modes are: 0 = keyboard control from processing; 1 = user input from arduino serial monitor; 2 = autonomous with set parameters

unsigned int interruptCount[2] = {0, 0};
float wheelSpeed[2] = {0.0,0.0};
unsigned long intervalTimer = 0;
unsigned long pollingIntervalTimer = 0;
unsigned long lastInterruptTime[2] = {0, 0};
int pinValue[2] = {0, 0};
int lastPinValue[2] = {0, 0};
float carrierDistance = 0;
float interruptToInch = 84.0/205.0;
int speedRequested = 0;
int angleRequested = 0;
int newRequest = 0;
int wheelSpeedPWN[2] = {0,0};
int PWMincrement = 25;
int PWMpin[2] = {11,5};
int rightWheelSpeedIter = 0;
int leftWheelSpeedIter = 0;
int defaultPWM = 170;
int lastInput = 0;
int inputIter =0;
int tempInput = 0;
int input =0;

void setup() {
  //Setup Serial Port
  Serial.begin(9600);

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
  if (millis() - pollingIntervalTimer > POLLING_INTERVAL)
  {
      //if (VERBOSE)
      //    Serial.println("Polling");
      if(CONTROL_MODE != 0)
      {
        RightPulseInterrupt();
        LeftPulseInterrupt();
      }
      pollingIntervalTimer = millis();
  }
  if ((millis() - intervalTimer) > INTERVAL)
  {
      //Any periodic task goes here
      if (VERBOSE)
        Serial.println("I am in Main");

      if(speedRequested == 0 && angleRequested == 0 && CONTROL_MODE == 1)
      {
          GetUserInput();    //Request User Speed and Angle
          if (VERBOSE)
            Serial.println("Setting Wheel Speeds");
          SetWheelSpeeds(1, speedRequested, angleRequested);
      }
      if(CONTROL_MODE == 2)
      {
          speedRequested = 170;
          angleRequested = 0;
          //SetWheelSpeeds(1, speedRequested, angleRequested);
      }
      //carrierDistance = ((interruptCount[RIGHT]*interruptToInch ) +  (interruptCount[LEFT]*interruptToInch ) ) / 2;  
      //carrierDistance = (interruptCount[LEFT]*interruptToInch);
      
      //SendSerialDistance();  //Displays last calculated speed for left and right wheels
      //SendSerialSpeed();
      
      intervalTimer = millis();
  } 
}

void SetWheelSpeeds(int newRequest, int speedRequested, int angleRequested)
{
  if (VERBOSE)
    Serial.println("I am in set Wheel Speeds");
  if(newRequest)  //Setting new wheel speeds
  {
      if (VERBOSE)
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
        if (VERBOSE)
        {
          Serial.print("Adjust Wheel Speeds. Speed Right = "); Serial.print(wheelSpeed[RIGHT]); Serial.print("Speed Left = "); Serial.println(wheelSpeed[LEFT]);
        }
       if(wheelSpeed[LEFT] < wheelSpeed[RIGHT])
       {
         Serial.print("I-");
         wheelSpeedPWN[LEFT] = min( (wheelSpeedPWN[LEFT] + (PWMincrement*(1-(wheelSpeed[LEFT]/wheelSpeed[RIGHT])))) , 255);
         if(wheelSpeedPWN[LEFT]==240)
         {
           wheelSpeedPWN[RIGHT] -= 10;
         }
       }
       if(wheelSpeed[LEFT] > wheelSpeed[RIGHT])
       {
         Serial.print("D-");
         wheelSpeedPWN[LEFT] = max( (wheelSpeedPWN[LEFT] - (PWMincrement*(1-(wheelSpeed[RIGHT]/wheelSpeed[LEFT])))) , 0);
       }
       if(wheelSpeed[LEFT] == wheelSpeed[RIGHT])
       {
         Serial.print("S-");
         wheelSpeedPWN[LEFT] = wheelSpeed[RIGHT];
       }
  }
  
  analogWrite(PWMpin[LEFT], wheelSpeedPWN[LEFT]);
  analogWrite(PWMpin[RIGHT], wheelSpeedPWN[RIGHT]);
  Serial.print("Actual: "); Serial.print(wheelSpeed[LEFT]); Serial.print(", "); Serial.print(wheelSpeed[RIGHT]); Serial.print(" Set: "); Serial.print(wheelSpeedPWN[LEFT]); Serial.print(", "); Serial.println(wheelSpeedPWN[RIGHT]);
  
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

int GetKeyboardInput()
{
  if(VERBOSE)
    Serial.println("waiting on processing input");
  while (Serial.available()==0);                 // wait for data
  while(Serial.available()>0)
  {
    int temp = Serial.read()-'0';
    //if(temp>=0)
    {
     if(inputIter>3)
     {
       inputIter=0;
       tempInput = 0;
     }
     tempInput = tempInput * 10;
     tempInput += temp;
     //Serial.print("iter");Serial.println(inputIter);
     //Serial.print("t");Serial.println(temp);
     //Serial.print("i");Serial.println(input);
     inputIter++;
    }
  }
  if(inputIter==3)
    return(tempInput);
  
  if(VERBOSE)
  {
    Serial.print("got speed input = "); Serial.println(input);
  } 
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
    if (VERBOSE)
      Serial.println("I am in left pulse interrupt");
    interruptCount[LEFT]++;
    
    //get the speed of the wheel
    int interruptInterval = millis() - lastInterruptTime[LEFT];
    
    
    if(leftWheelSpeedIter<20)
    {
      wheelSpeed[LEFT] = (wheelSpeed[LEFT] + (((interruptToInch*25.4) / interruptInterval)*1000) )/2; //speed in mm/s
      leftWheelSpeedIter++;
    }
    else
    {
      SetWheelSpeeds(0,0,0);  //adjust for any wheel slipage
      wheelSpeed[LEFT] = ((interruptToInch*25.4) / interruptInterval)*1000;  //speed in mm/s
      leftWheelSpeedIter = 0;
    }
    
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
    if (VERBOSE)
      Serial.println("I am in right pulse interrupt");
    interruptCount[RIGHT]++;
    
    //get the speed of the wheel
    int interruptInterval = millis() - lastInterruptTime[RIGHT];
    
    if(rightWheelSpeedIter<20)
    {
      wheelSpeed[RIGHT] = (wheelSpeed[RIGHT] + (((interruptToInch*25.4) / interruptInterval)*1000) )/2; //speed in mm/s
      rightWheelSpeedIter++;
    }
    else
    {
      SetWheelSpeeds(0,0,0);  //adjust for any wheel slipage
      wheelSpeed[RIGHT] = (((interruptToInch*25.4) / interruptInterval)*1000)/2; //speed in mm/s
      rightWheelSpeedIter = 0;
    }
    
    lastInterruptTime[RIGHT] = millis();
    //SendSerialRight();
  }
  lastPinValue[RIGHT] = pinValue[RIGHT];
}
