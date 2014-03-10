int MPU_dataRate = 25;
int Sonar_dataRate = 100;
int encoder_dataRate = 50;

unsigned long SonarTimer = 0, encoderTimer = 0, MPUTimer = 0, MPUDirTimer = 0;
int Distance = 0;
int Direction = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  ((millis() - MPUTimer) > MPU_dataRate)? sendMPUData() : DoNothing();
  
  ((millis() - SonarTimer) > Sonar_dataRate)? sendSonarData() : DoNothing();
  
  ((millis() - encoderTimer) > encoder_dataRate)? sendEncoderData() : DoNothing();
  
}

void DoNothing()
{
  return;
}

void sendSonarData ()
{
  Serial.println("S1_250");
  Serial.println("S2_100");
  Serial.println("S3_700");

  SonarTimer = millis();
}

void sendMPUData ()
{
 // if((millis() - MPUDirTimer)>15000)
 {
 //  Direction = (Direction + 1)%5;
 //  MPUDirTimer =  millis();
 }
  int yaw = 90*Direction;
  Serial.print("Y_"); Serial.println(yaw);
  MPUTimer = millis();
}

void sendEncoderData()
{
  Distance = (Distance + 2)%999999;
  Serial.print("D_"); Serial.println(Distance);
  encoderTimer = millis();
}
