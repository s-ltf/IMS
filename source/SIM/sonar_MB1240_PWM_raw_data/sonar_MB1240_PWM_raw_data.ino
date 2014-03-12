//Feel free to use this code.
//Please be respectful by acknowledging the author in the code if you use or modify it.
//Author: Bruce Allen
//Date: 23/07/09

//Digital pin 7 for reading in the pulse width from the MaxSonar device.
//This variable is a constant because the pin will not change throughout execution of this code.
const int pwPin = 2; 
//#define AUX_SENSOR

//variables needed to store values
long inches, cm, pulseAux, inchesAux, cmAux;

unsigned long pulse;

void setup() {

  //This opens up a serial connection to shoot the results back to the PC console
  Serial.begin(9600);
  
  pinMode(2, INPUT);
  pinMode(18, INPUT);
  pinMode(19, INPUT);
  digitalWrite(2, HIGH); //Internal Pullup Resistor
  //Used to read in the pulse that is being sent by the MaxSonar device.
  //Pulse Width representation with a scale factor of 147 uS per Inch.
}

void loop() {
  pulse = pulseIn(2, HIGH);
  inches = pulse/147;
  cm = inches * 2.54;
  Serial.println(cm);
  /*
  pulse = pulseIn(18, HIGH);
  inches = pulse/147;
  cm = inches * 2.54;
  Serial.print("\t");Serial.println(cm);
    pulse = pulseIn(19, HIGH);
  inches = pulse/147;
  cm = inches * 2.54;
  Serial.print("\t\t");Serial.println(cm);*/
}
