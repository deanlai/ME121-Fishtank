
int tempPin = 5;
void setup() {
  
  Serial.begin(9600);
  pinMode(tempPin, INPUT);
}

void loop() {
  
  int reading;

  reading = analogRead(tempPin);
  Serial.println(reading);
  delay(25);
  
}
