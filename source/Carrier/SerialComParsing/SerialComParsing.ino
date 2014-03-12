bool LED_State = false;
#define VERBOSE 1

unsigned long LastTimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop()
{
  (VERBOSE && receiveData())? Serial.println("Got data!") : 0;
  //sendFakeData();
}
