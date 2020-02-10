

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

  int cpos = 17;
  int npos = 0;
  lcd.init();
  lcd.backlight();
  lcd.setCursor(16,1);
  lcd.print("N.A.C.A.H.D.");
  String nacahd = "N.A.C.A.H.D.";
  for (int poscount=0; poscount<5; poscount++){
    lcd.scrollDisplayLeft();
    lcd.setCursor(cpos, 1);
    lcd.print(nacahd.charAt(npos));
    npos += 1;
    cpos -= 1;
    delay(100);
  }
}
