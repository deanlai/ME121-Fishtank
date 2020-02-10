<<<<<<< HEAD
//--------------------------------------------------------
// Sensor readings program for conversion from analog output to salinity (wt%)
// Version 3.1
// 2/3/20
// The North American Council on Aquatic Housing and Development
// Reimplments v2.0 with code to toggle relays for testing and calibration
// NOTE: To run this code you must have downloaded the LiquidCrystal_I2C library (and added it to the sketch from the IDE),
//       which is in Whitman's folder in the main directory
//--------------------------------------------------------

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address

// delcare global pins
const int SALINITY_POWER_PIN = 8;
const int buttonin = 5;
const int buttonLED = 6;
const int transistorPin1 = 10; 
const int transistorPin2 = 11; 
const int transistorPin3 = 13; 

// working vars for button toggling 
int toggled = 0;        // State of machine that button controls: 0: off, 1: on
int currentState;       // Current state of button --> 1: pressed, 0: not pressed
int previousState = 0;  // Previous state of button --> 1: pressed, 0: not pressed
long timer = 0;         // Used to implement delay
long debounce = 150;    // Delay checking conditional for time of physical button press

   

void setup()
{
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    pinMode(transistorPin1, OUTPUT);
    pinMode(transistorPin2, OUTPUT);
    pinMode(transistorPin3, OUTPUT);
    pinMode(buttonin, INPUT);
    pinMode(buttonLED, OUTPUT);
    Serial.begin(9600);

    // Setup LCD
    lcd.init();      // initiate LCD
    lcd.backlight(); // backlight on
}

void loop()
{
    // declare local pins
    const int SALINITY_READING_PIN = A0;
    const int relaytest;

    // declare constants from polynomial fits
    // Note on using constants:
    // c   --> prefix to show variable is a constant
    // h,l --> used for high/low salinity line
    // 1,2 --> first or second constant in eq. y = c1*x + c2
    const float cl1 = 9.0881533e-05;
    const float cl2 = -6.9069865e-03;
    const float ch1 = 8.5798207e-04;
    const float ch2 = -4.8485778e-01;

    // breakpoints for conditionals to determine which fit line to used
    // b1 --> low point determined from DI water average reading
    // b2 --> mid point at 0.05 wt% to transition from one fit line to the next
    // b3 --> high point determined from 0.15 wt% water reading
    const int b1 = 76;
    const int b2 = 626;
    const int b3 = 737;

    // setup variables
    int numReadings = 30;
    int salinityReading = 0;
    float salinityPercentage;
    buttonRead(buttonin); //check state of toggle

    // button toggle LED
    digitalWrite(buttonLED, toggled); // you just have to hold down the button for a second because of other delays built into the sketch
    relayTest(toggled);
    
    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);

    // Convert from analog to salinity percentage using s = (a/c1)^(1/c2)
    salinityPercentage = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, salinityReading);

    // Print to Serial Monitor (for data analysis)
    Serial.print(salinityReading);
    Serial.print(" ");
    Serial.print(salinityPercentage, 4);
    Serial.println();

    // Print to LCD Screen
    lcd.setCursor(1, 0);
    lcd.print("N.A.C.A.H.D.");
    lcd.setCursor(0, 1); // Print to second row
    lcd.print("Percent salt: ");
    lcd.print(salinityPercentage, 4); // to 4 decimal places accuracy
    lcd.setCursor(0, 2);              // Print to third row
    lcd.print("Analog read: ");
    lcd.print(salinityReading);
    lcd.print("   ");                 // added empty characters to clear trailing digits
    delay(80);                        // delay between refresh
    if(salinityPercentage>0.1475) {
      lcd.setCursor(0, 3);
      lcd.print("Salinity Too High"); //model too high salinity with upper section calibration data for est
    }
    
}

float takeReading(int powerPin, int readingPin, int numReadings)
{
    // input: powerPin (digital out), readingPin (analog in), numReadings (>0)
    // output: average of numReadings readings

    float sum = 0.0;
    digitalWrite(powerPin, 1); // Turn sensor on
    delay(100);                // Allow power to settle
    for (int i = 0; i < numReadings; i++)
    { // Take readings and sum
        sum += analogRead(readingPin);
        delay(10);
    }
    digitalWrite(powerPin, 0); // Turn sensor off
    sum /= numReadings;        // Average sum over readings
    return sum;
}

float findSalinityPercentage(float c1, float c2, float c3, float c4, int b1, int b2, int b3, int reading) {
    // input c1, c2, c3, c4 --> polynomial constants for two line fits
    //       b1, b2, b3     --> breakpoints to determine regin for evaluation
    //       reading        --> analog reading value from sensor
    // output: corresponding salinity in wt%

    // check breakpoints to determine function region
    if (reading < b1) {         // reading < DI water breakpoint
        return 0;
    }
    else if (reading < b2) {    // reading < 0.05 wt% breakpoint
        return evaluatePolynomial(reading, c1, c2);
    }
    else if (reading < b3) {    // reading < 0.15 wt% breakpoint
        return evaluatePolynomial(reading, c3, c4);
    }
    else{                       // reading > 0.15 wt% breakpoint
        return evaluatePolynomial(b3, c3, c4); // return value of polynomial at b3
    }
}

float evaluatePolynomial(int x, float c1, float c2) {
    // evaluates y = c1*x + c2 and returns y
    return c1*x + c2;
}

int buttonRead(int buttonin){
  //
  //
  currentState = digitalRead(buttonin);
  
  if(currentState==HIGH && previousState==LOW && (millis()-timer*1000)>debounce){
    if (toggled == LOW){
      toggled = HIGH;
    }
    else {
      toggled = LOW;
    }
    timer = millis()/1000;
  } 
  previousState = currentState;
}

  
void relayTest(int relaystate){ 
    //turns all transistors on or off based on relaystate
    //turn relays on for testing purposes
    if (relaystate==1){
      digitalWrite(transistorPin1, HIGH);
      digitalWrite(transistorPin2, HIGH);
      digitalWrite(transistorPin3, HIGH);
    }
    else if (relaystate==0){
      digitalWrite(transistorPin1, LOW);
      digitalWrite(transistorPin2, LOW);
      digitalWrite(transistorPin3, LOW);
      }
    
}
=======
//--------------------------------------------------------
// Sensor readings program for conversion from analog output to salinity (wt%)
// Version 3.0
// 2/3/20
// The North American Council on Aquatic Housing and Development
// Reimplments v2.0 with code to toggle relays for testing and calibration
// NOTE: To run this code you must have downloaded the LiquidCrystal_I2C library (and added it to the sketch from the IDE),
//       which is in Whitman's folder in the main directory
//--------------------------------------------------------

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address

// delcare global pins
const int SALINITY_POWER_PIN = 8;
const int buttonIn = 5;
const int buttonLED = 6;
const int saltyPin = 10; 
const int freshPin = 11; 
const int transistorPin3 = 13; 

// working vars for button toggling 
int toggled = 0;        // State of machine that button controls: 0: off, 1: on
int currentState;       // Current state of button --> 1: pressed, 0: not pressed
int previousState = 0;  // Previous state of button --> 1: pressed, 0: not pressed
long timer = 0;         // Used to implement delay
long debounce = 150;    // Delay checking conditional for time of physical button press

// for timing sensing
int currentTime = millis();

void setup()
{
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    pinMode(transistorPin1, OUTPUT);
    pinMode(transistorPin2, OUTPUT);
    pinMode(transistorPin3, OUTPUT);
    pinMode(buttonIn, INPUT_PULLUP);
    pinMode(buttonLED, OUTPUT);
    Serial.begin(9600);

    // Setup LCD
    lcd.init();      // initiate LCD
    lcd.backlight(); // backlight on
}

void loop()
{
    // declare local pins
    const int SALINITY_READING_PIN = A0;
    const int relaytest;

    // declare setpoint and sigma
    // Note: setpoint in wt%
    //       sigma given relative to analog reading values
    float setpoint = 0.09;
    float sigma = 2.00;

    // delcare variables for deadtime timer
    int deadtime;

    //compute setpoint(analog), LCL, UCL
    
    UCL = setpointAnalog + 3*sigma;
    LCL = setpointAnalog - 3*sigma;

    // declare constants from polynomial fits
    // Note on using constants:
    // c   --> prefix to show variable is a constant
    // h,l --> used for high/low salinity line
    // 1,2 --> first or second constant in eq. y = c1*x + c2
    const float cl1 = 9.0881533e-05;
    const float cl2 = -6.9069865e-03;
    const float ch1 = 8.5798207e-04;
    const float ch2 = -4.8485778e-01;

    // breakpoints for conditionals to determine which fit line to used
    // b1 --> low point determined from DI water average reading
    // b2 --> mid point at 0.05 wt% to transition from one fit line to the next
    // b3 --> high point determined from 0.15 wt% water reading
    const int b1 = 76;
    const int b2 = 626;
    const int b3 = 737;

    // setup variables
    int numReadings = 30;
    int salinityReading;
    float salinityPercentage;
    buttonRead(buttonIn); //check state of toggle

    // button toggle LED
    digitalWrite(buttonLED, toggled); // you just have to hold down the button for a second because of other delays built into the sketch
    relayTest(toggled);

    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);
    // Convert from analog to salinity percentage using s = (a/c1)^(1/c2)
    salinityPercentage = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, salinityReading);

    // Print to Serial Monitor (for data analysis)
    Serial.println(salinityReading);

    // check if salinity reading is within control limits
    if ((salinityReading > UCL || salinityReading < LCL) && millis() > deadtime + 12000){
      adjustSalinity();       // Adjust salinity using solenoids
      deadtime = millis();    // Set deadtime timer to current millis() value
    }

    // Print to LCD Screen
    lcd.setCursor(1, 0);
    lcd.print("N.A.C.A.H.D.");
    lcd.setCursor(0, 1); // Print to second row
    lcd.print("Percent salt: ");
    lcd.print(salinityPercentage, 4); // to 4 decimal places accuracy
    lcd.setCursor(0, 2);              // Print to third row
    lcd.print("Analog read: ");
    lcd.print(salinityReading);
    lcd.print("   ");                 // added empty characters to clear trailing digits
    delay(80);                        // delay between refresh
    if(salinityPercentage>0.1475) {
      lcd.setCursor(0, 3);
      lcd.print("Salinity Too High"); //model too high salinity with upper section calibration data for est
    }
    
}

float takeReading(int powerPin, int readingPin, int numReadings)
{
    // input: powerPin (digital out), readingPin (analog in), numReadings (>0)
    // output: average of numReadings readings

    float sum = 0.0;
    digitalWrite(powerPin, 1); // Turn sensor on
    delay(100);                // Allow power to settle
    for (int i = 0; i < numReadings; i++)
    { // Take readings and sum
        sum += analogRead(readingPin);
        delay(10);
    }
    digitalWrite(powerPin, 0); // Turn sensor off
    sum /= numReadings;        // Average sum over readings
    return sum;
}

float findSalinityPercentage(float c1, float c2, float c3, float c4, int b1, int b2, int b3, int reading) {
    // input c1, c2, c3, c4 --> polynomial constants for two line fits
    //       b1, b2, b3     --> breakpoints to determine regin for evaluation
    //       reading        --> analog reading value from sensor
    // output: corresponding salinity in wt%

    // check breakpoints to determine function region
    if (reading < b1) {         // reading < DI water breakpoint
        return 0;
    }
    else if (reading < b2) {    // reading < 0.05 wt% breakpoint
        return evaluatePolynomial(reading, c1, c2);
    }
    else if (reading < b3) {    // reading < 0.15 wt% breakpoint
        return evaluatePolynomial(reading, c3, c4);
    }
    else{                       // reading > 0.15 wt% breakpoint
        return evaluatePolynomial(b3, c3, c4); // return value of polynomial at b3
    }
}

float evaluatePolynomial(int x, float c1, float c2) {
    // evaluates y = c1*x + c2 and returns y
    return c1*x + c2;
}

int buttonRead(int buttonIn){
  // input: button Pin
  // Reads button state and swaps state of toggled variable
  currentState = digitalRead(buttonIn);
  
  if(currentState==HIGH && previousState==LOW && (millis()-timer*1000)>debounce){
    if (toggled == LOW){
      toggled = HIGH;
    }
    else {
      toggled = LOW;
    }
    timer = millis()/1000;
  } 
  previousState = currentState;
}

  
void relayTest(int relaystate){ 
    //turns all transistors on or off based on relaystate
    //turn relays on for testing purposes
    if (relaystate==1){
      digitalWrite(saltyPin), HIGH);
      digitalWrite(freshPin, HIGH);
      digitalWrite(transistorPin3, HIGH);
    }
    else if (relaystate==0){
      digitalWrite(saltyPin), LOW);
      digitalWrite(freshPin, LOW);
      digitalWrite(transistorPin3, LOW);
      }
    
}

int calculateSetpointAnalog() {
  return setpoint 
}

void adjustSalinity(salinityPercentage, setpoint){
  int targetSalinity = salinityPercentage - (salinityPercentage - setpoint)*0.8;
  if (targetSalinity < salinityPercentage){
    openSolenoid(targetSalinity, salinityPercentage, 1, saltyPin);
  }
  else{
    openSolenoid(targetSalinity, salinityPercentage, 0, freshPin);
  }
}

float openSolenoid(targetSalinity, currentSalinity, addedSalinity, pin) {
  const float overflowFraction = 1;
  const float totalMass = 0;
  const float flowRate = 0;
  massToAdd = totalMass * (currentSalinity - targetSalinity)/
                          (currentSalinity - addedSalinity)*
                          (1 / 1 - overflowFraction);
  time = massToAdd / flowRate;
  digitalWrite(pin, 1);
  delay(time);
  digitalWrite(pin, 0);
}
>>>>>>> 8d3dbc86a404e70170a8d4115d1d785b8092a1ad
