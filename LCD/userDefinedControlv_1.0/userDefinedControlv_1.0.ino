

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // object declaration 20x4 LCD screen, 0x27 is the address

void setup() {
  Serial.begin(9600);
  
}

void loop() {
  lcdSetup();
  
}

void lcdSetup(){
  int ppos = 4;
  int cpos = 4;
  int npos = 0;
  String nacahd = "N.A.C.A.H.D.";
  lcd.init();
  lcd.backlight();
  lcd.setCursor(13,1);
  for (int poscount=0; poscount<6; poscount++){
    lcd.setCursor(ppos, 1);
    lcd.print(" .");
    ppos += 2;
    delay(200);
    }
  for (int poscount=0; poscount<12; poscount++){
    //lcd.scrollDisplayLeft();
    lcd.setCursor(cpos, 1);
    lcd.print(nacahd.charAt(npos));
    npos += 1;
    cpos += 1;
    delay(200);
  }
}

void lcdUpdate () {


  
}
