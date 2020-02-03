#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

byte COMP[] = {
    B01110,
    B10101,
    B11011,
    B11011,
    B11011,
    B11011,
    B10101,
    B01110,
};
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.noDisplay();
  lcd.createChar(0, COMP);
}
void loop() {
  lcd.setCursor(9,1);
  lcd.write(0);
  lcd.display();
  
  delay(250);
  lcd.setCursor(5, 2);
  lcd.print("I made my song a coat");
  lcd.scrollDisplayLeft();
  delay(250);
  
  delay(100);
  
}
