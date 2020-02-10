

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // object declaration 20x4 LCD screen, 0x27 is the address

void setup() {
  Serial.begin(9600);
  lcdSetup();
}

void loop() {

}

void lcdSetup(){

  lcd.init();
  lcd.backlight();
  
}
