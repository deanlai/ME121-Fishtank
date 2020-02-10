//--------------------------------------------------------
// Sensor readings program for conversion from analog output to salinity (wt%)
// Version 1.2
// 2/3/20
// The North American Council on Aquatic Housing and Development
// Reimplments v1.1 with piecewise polynomial fits
// NOTE: To run this code you must have downloaded the LiquidCrystal_I2C library (and added it to the sketch from the IDE),
//       which is in Whitman's folder in the main directory
//--------------------------------------------------------

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address

    // delcare global pins
    const int SALINITY_POWER_PIN = 8;

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

    //button and transistor pins
    const int buttonin = 5;
    const int buttonLED = 6;
    const int transistorPin1 = 10; 
    const int transistorPin2 = 11; 
    const int transistorPin3 = 13; 

    //working vars for button toggling 
    int state = LOW;
    int reading;
    int previous=LOW;
    long timer = 0;
    long debounce = 150;

   

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

  
    // setup variables
    int numReadings = 30;
    int salinityReading = 0;
    float salinityPercentage;
    int bstate = buttonRead(buttonin); //check state of toggle

    //button toggle LEDs
    digitalWrite(buttonLED, bstate); //light to test button toggle. you just have to hold down the button for a second because of other delays built into the sketch
    relayTest(bstate); //controls transistor pins based on button press toggle function 
    
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
    lcd.print("   ");
    delay(80); // delay between refresh
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
  reading = digitalRead(buttonin);
  
  if(reading==HIGH && previous==LOW && (millis()-timer*1000)>debounce){
    if (state == LOW){
      state = HIGH;
    }
    else {
      state = LOW;
    }
    timer = millis()/1000;
  } 
  
  previous = reading;
  return state;
}

  
void relayTest(int relaystate){ //turns all transistors on or off based on relaystate
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
