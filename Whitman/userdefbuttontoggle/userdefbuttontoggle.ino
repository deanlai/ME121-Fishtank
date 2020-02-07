
const int buttonin=5;
const int LEDPin = 6;
int state = LOW;
int reading;
int previous = LOW;
int buttoncount;

long timer = 0;
long debounce = 200;

void setup() {
  
  Serial.begin(9600);
  pinMode(buttonin, INPUT);
  pinMode(LEDPin, OUTPUT);

}

void loop() {

  int bstate = buttonRead(buttonin);
  digitalWrite(LEDPin, bstate);
}

int buttonRead(int buttonin){
    reading = digitalRead(buttonin);
  
  if(reading==HIGH && previous==LOW && (millis()-timer*1000)>debounce){
    if (state == LOW){
      state = HIGH;
    }
    else {
      state = LOW;
    }
    timer = millis()/1000;
  } 
  digitalWrite(LEDPin, state);
  previous = reading;
  return state;
}
