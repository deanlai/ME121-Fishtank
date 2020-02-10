

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // object declaration 20x4 LCD screen, 0x27 is the address

void setup() {
  Serial.begin(9600);
  
}

void loop() {
  
  lcdSetup(); // ;)
}

void lcdSetup(){ //setup lcd with fancy display
  int ppos = 4; //counter position for cursor to print periods
  int cpos = 4; //counter pos for cursor to print string
  int npos = 0; //counter for position within string
  String nacahd = "N.A.C.A.H.D."; //creates nacahd string to be referenced
  lcd.init();
  lcd.backlight();
  for (int poscount=0; poscount<6; poscount++){ //this loop prints periods, notice total chars/2 because we print 2 chars each loop
    lcd.setCursor(ppos, 1); //counter moves cursor 
    lcd.print(" .");
    ppos += 2; //cursor moves 2 spaces because there are 2 characters
    delay(300); //mess with this to best dramatic effect
    }
  for (int poscount=0; poscount<12; poscount++){ //this loop prints "N.A.C.A.H.D.
    //lcd.scrollDisplayLeft();
    lcd.setCursor(cpos, 1); //counter moves cursor
    lcd.print(nacahd.charAt(npos)); //prints the character referenced by the position (npos) in nacahd string
    npos += 1; //iterate counters 
    cpos += 1;
    delay(200);
  }
  delay(250);
  lcd.clear();
}

void lcdUpdate () {


  
}
