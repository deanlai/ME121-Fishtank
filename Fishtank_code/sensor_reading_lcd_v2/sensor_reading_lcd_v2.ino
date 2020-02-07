//--------------------------------------------------------
// Sensor readings program for conversion from analog output to salinity (wt%)
// Version 2.0
// 2/3/20
// The North American Council on Aquatic Housing and Development
// Reimplments v1.0 with piecewise polynomial fits
// NOTE: To run this code you must have downloaded the LiquidCrystal_I2C library (and added it to the sketch from the IDE),
//       which is in Whitman's folder in the main directory
//--------------------------------------------------------

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address

// delcare global pins
const int SALINITY_POWER_PIN = 8;

void setup()
{
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    Serial.begin(9600);

    // Setup LCD
    lcd.init();      // initiate LCD
    lcd.backlight(); // backlight on
}

void loop()
{
    // declare local pins
    const int SALINITY_READING_PIN = A0;

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

    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);

    // Convert from analog to salinity percentage using piecewise linear fits
    salinityPercentage = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, salinityReading);

    // Print to Serial Monitor (for debugging)
    Serial.print(salinityReading);
    Serial.print(" ");
    Serial.print(salinityPercentage, 4);
    Serial.println();

    // Print to LCD Screen
    lcd.setCursor(1, 0);
    lcd.print("N.A.C.A.H.D.");
    lcd.setCursor(0, 1);                // Print to second row
    lcd.print("Percent salt: ");
    lcd.print(salinityPercentage, 4);   // to 4 decimal places accuracy
    lcd.setCursor(0, 2);                // Print to third row
    lcd.print("Analog read: ");
    lcd.print(salinityReading);
<<<<<<< HEAD
    lcd.print("   ");
    delay(250); // delay between refresh
    if(salinityPercentage>0.1475) {
      lcd.setCursor(0, 3);
      lcd.print("Salinity too high. est:", evaluatePolynomial(reading, c3, c4); 
    }
    
=======
    lcd.print("   ");                   // Spaces to clear trailing digits
    delay(250);                         // delay between refresh
>>>>>>> 95e8e8a82bd48f5c52a15ef6c8a84b1178d3f807
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
