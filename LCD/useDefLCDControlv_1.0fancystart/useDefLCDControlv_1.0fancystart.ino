

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
  int i; //alternating incrementation
  String nacahd = "N.A.C.A.H.D."; //creates nacahd string to be referenced
  lcd.init();
  lcd.backlight();
  //this loop prints periods, notice total chars/2 because we print 2 chars each loop
  for (int poscount=0; poscount<6; poscount++){ 
    lcd.setCursor(ppos, 1); //counter moves cursor 
    lcd.print(" .");
    ppos += 2; //cursor moves 2 spaces because there are 2 characters
    delay(150); //mess with this to best dramatic effect
    }
    delay(800);
  //this loop prints NACAHD with each individual letter printed separately using string position calling (npos)
  for (int poscount=0; poscount<6; poscount++){ 
    //lcd.scrollDisplayLeft();
    lcd.setCursor(cpos, 1); //counter moves cursor
    lcd.print(nacahd.charAt(npos)); //prints the character referenced by the position (npos) in nacahd string
    npos += 2; //iterate counters 
    cpos += 2;
    delay(150);
  }
  delay(800);
  //this loop blinks the word "initializing"
  for (int i=0; i<6; i++){ //
    if (i%2==1){ //modulo to check whether i is odd to change between 2 states
      lcd.setCursor(3,2);
      lcd.print("Initiating...");
      delay(500);
     }
     else{
      lcd.setCursor(3,2);
      lcd.print("             ");
      delay(380);
     }
     
  }
  lcd.clear();
}

void lcdUpdate () {

  //first row on LCD screen, this does not change
  lcd.setCursor(4,0);
  lcd.print("LCL");
  lcd.setCursor(11,0);
  lcd.print("SP");
  lcd.setCursor(16,0);
  lcd.print("UCL");

  // second row, this does change
  lcd.setCursor(0,1);
  lcd.print("S");
  lcd.setCursor(3,1);
  lcd.print(whatever goes under LCL on row S);
  lcd.setCursor(9,1);
  lcd.print(row s SP);
  lcd.setCursor(15,1);
  lcd.print(row s UCL);

  lcd.setCursor(0,2);
  lcd.print("T:");

  lcd.setCursor(0,3);
  lcd.print("S=");
  lcd.setCursor(8,3);
  lcd.print("T=");
  lcd.setCursor(15,3);
  lcd.print("H=");
}
