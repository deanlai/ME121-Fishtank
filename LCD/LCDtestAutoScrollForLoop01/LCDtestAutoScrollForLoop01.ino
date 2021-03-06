/* I2C LCD with Arduino example code. More info: https://www.makerguides.com */
// Include the libraries:
// LiquidCrystal_I2C.h: https://github.com/johnrickman/LiquidCrystal_I2C

#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered) 

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,16,2) for 16x2 LCD.
void setup() {
  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
}
void loop() {
  // Print 'Hello World!' on the first line of the LCD:
  //lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
 //lcd.print("Hello World!hahaaa"); // Print the string "Hello World!"
  lcd.setCursor(4, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
  lcd.print("Don't Panic.");
  delay(1500);
  lcd.clear();
  delay(500);
  lcd.setCursor(6, 1);
  lcd.print("Yo...");
  for (int poscount=0; poscount<10; poscount++){
    lcd.scrollDisplayLeft();
    lcd.print(".");
    lcd.print(".");
    delay(400);
  }
  lcd.clear();
  delay(900);
 
  
}
