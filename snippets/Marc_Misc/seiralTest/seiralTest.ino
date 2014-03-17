int i = 1;
void setup() {
  Serial.begin(9600);
}

void loop() {
 i = (i+1)%100;
 Serial.println(i); 
 delay(150);
}
