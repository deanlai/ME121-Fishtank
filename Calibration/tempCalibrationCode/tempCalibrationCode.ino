// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address



int tempPin = A1;
void setup() {
  
  Serial.begin(9600);
  pinMode(tempPin, INPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  
  int reading;

  reading = analogRead(tempPin);
  Serial.println(reading);
  lcd.setCursor(1, 1);
  lcd.print("Analog Read: ");
  lcd.print(reading);
  delay(25);
  
}
