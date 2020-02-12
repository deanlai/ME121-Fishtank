

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // object declaration 20x4 LCD screen, 0x27 is the address

void setup() {
  Serial.begin(9600);

}

void loop() {

  lcdFancySetup(); // ;)
}

void lcdsimple
void lcdFancySetup() { //setup lcd with fancy display
  int ppos = 4; //counter position for cursor to print periods
  int cpos = 4; //counter pos for cursor to print string
  int npos = 0; //counter for position within string
  int i; //alternating incrementation
  String nacahd = "N.A.C.A.H.D."; //creates nacahd string to be referenced
  lcd.init();
  lcd.backlight();
  //this loop prints periods, notice total chars/2 because we print 2 chars each loop
  for (int poscount = 0; poscount < 6; poscount++) {
    lcd.setCursor(ppos, 1); //counter moves cursor
    lcd.print(" .");
    ppos += 2; //cursor moves 2 spaces because there are 2 characters
    delay(150); //mess with this to best dramatic effect
  }
  delay(800);
  //this loop prints NACAHD with each individual letter printed separately using string position calling (npos)
  for (int poscount = 0; poscount < 6; poscount++) {
    //lcd.scrollDisplayLeft();
    lcd.setCursor(cpos, 1); //counter moves cursor
    lcd.print(nacahd.charAt(npos)); //prints the character referenced by the position (npos) in nacahd string
    npos += 2; //iterate counters
    cpos += 2;
    delay(150);
  }
  delay(800);
  //this loop blinks the word "initializing"
  for (int i = 0; i < 6; i++) { //
    if (i % 2 == 1) { //modulo to check whether i is odd to change between 2 states
      lcd.setCursor(3, 2);
      lcd.print("Initiating...");
      delay(500);
    }
    else {
      lcd.setCursor(3, 2);
      lcd.print("             ");
      delay(380);
    }

  }
  lcd.clear();
}

void lcdUpdate () {
  //LCL is lower control limit
  //SP is set point
  //UCL is upper control limit
  // S and T in rows 1 and 2 are salinity and temp controls 
  //S, T, and H on the bottom are the current values, H is whether the heater is on

  //Salinity Parameters 
  float sLCL, sSP, sUCL;

  //Temp parameters
  float tLCL, tSP, tUCL; 

  //Current values 
  int heater; //placeholder so that heaterState string control will compile
  float saltNow, tempNow; 
  String heaterState; // "on" or "off"

  //if/else block to modify heaterState based on whether the heater is on or not, arbitrary var names used here
  if (heater == 1) { 
    heaterState = "ON";
  }
  else {
    heaterState = "OFF";
  }
  
  
  
  //first row
  lcd.setCursor(4, 0);
  lcd.print("LCL    SP   UCL ");
  
  //second row, Salinity
  lcd.setCursor(0, 1);
  lcd.print("S: ");
  lcd.setCursor(3, 1);
  lcd.print(sLCL); 
  lcd.setCursor(9, 1);
  lcd.print(sSP);
  lcd.setCursor(15, 1);
  lcd.print(sUCL);
  
  //third row, Temp
  lcd.setCursor(0, 2);
  lcd.print("T:");
  lcd.setCursor(4,2);
  lcd.print(tLCL);
  lcd.setCursor(10,2);
  lcd.print(tSP);
  lcd.setCursor(16,2);
  lcd.print(tSP);
  
  //fourth row, current states
  lcd.setCursor(0, 3);
  lcd.print("S=");
  lcd.setCursor(2,3);
  lcd.print(saltNow);
  lcd.setCursor(8, 3);
  lcd.print("T=");
  lcd.setCursor(10,3);
  lcd.print(tempNow);
  lcd.setCursor(15, 3);
  lcd.print("H=");
  lcd.setCursor(17, 3);
  lcd.print(heaterState);
}
