//--------------------------------------------------------
// Main Control Code
// Version 1.0
// 2/9/20
// The North American Council on Aquatic Housing and Development
// Extends sensor_reading code base to actuate solenoids
// Usage: This program is intended to running live when our system control is being tested.
//        For debugging and testing use testing_code_v#.
// NOTE: To run this code you must have downloaded the LiquidCrystal_I2C library (and added it to the sketch from the IDE),
//       which is in Whitman's folder in the main directory
//--------------------------------------------------------

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address

// delcare global pins
const int SALINITY_POWER_PIN = 8;
const int saltyPin = 10; 
const int freshPin = 11; 

void setup()
{
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    pinMode(saltyPin, OUTPUT);
    pinMode(freshPin, OUTPUT);

    // Setup LCD
    lcdFancySetup();
}

void loop()
{
    // declare local pins
    const int SALINITY_READING_PIN = A0;

    // declare constants and breakpoints for polynomial fit
    // Note on using constants:
    // c   --> prefix to show variable is a constant
    // h,l --> used for high/low salinity line
    // 1,2 --> first or second constant in eq. y = c1*x + c2
    // b1 --> low point determined from DI water average reading
    // b2 --> mid point at 0.05 wt% to transition from one fit line to the next
    // b3 --> high point determined from 0.15 wt% water reading
    const float cl1 = 9.5754848e-05;
    const float cl2 = -7.7561327e-03;
    const float ch1 = 8.5607131e-04;
    const float ch2 = -4.6129742e-01;
    const int b1 = 81;
    const int b2 = 603;
    const int b3 = 708;

    // declare sigma(analog) and deadtime from calibration
    // note: s & t prefixes refer to salinity and temperature
    const float sSetpoint = 0;
    const float sSigma_analog = 0;
    const deadtime = 12000;
    // convert sigma_analog to wt %
    float sSigma = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, sigma_analog);

    // setup variables
    int numReadings = 30;     // number of readings per salinity reading
    int salinityReading;      // current analog reading from salinity sensor
    float salinityPercentage; // current salinity percentage

    // compute salinity UCL & LCL
    float sUCL = sSetpoint + 3*sSigma;
    float sLCL = sSetpoint - 3*sSigma;

    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);
    // Convert from analog to salinity percentage using s = (a/c1)^(1/c2)
    salinityPercentage = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, salinityReading);

    // Adjust salinity using solenoids
    adjustSalinity(salinityPercentage, sSetpoint, sUCL, sLCL, deadtime);

    // Update LCD screen
    lcdUpdate();
    }
    
}

float takeReading(int powerPin, int readingPin, int numReadings) {
    // input: powerPin (digital out), readingPin (analog in), numReadings (>0)
    // output: average of numReadings readings

    float sum = 0.0;
    digitalWrite(powerPin, 1); // Turn sensor on
    delay(100);                // Allow power to settle
    for (int i = 0; i < numReadings; i++) { // Take readings and sum
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

void adjustSalinity(float currentSalinity, float setpoint, float UCL, float LCL, int deadtime)
{
    // input: current salinity and salinity setpoint
    // output: none
    // calls openSolenoid() to adjust salinity of system to a target salinity
    static int lastAdjustment = 0 - deadtime;

    // Check if 
    if (millis() > lastAdjustment + 12000){
    // check if salinity percentage is outside of control limits
        if (salinityPercentage > UCL || salinityPercentage < LCL) {
            // Set target salinity to 80% of the difference between current salinity and setpoint
            int targetSalinity = currentSalinity - (currentSalinity - setpoint) * 0.8;
            if (targetSalinity > currentSalinity) {
                addWater(targetSalinity, currentSalinity, 1, saltyPin); // add 1% salted water
            }
            else {
                addWater(targetSalinity, currentSalinity, 0, freshPin); // add 0% DI water
            }
        }
    }
    lastAdjustment = millis();
}

void addWater(float targetSalinity, float currentSalinity, int addedSalinity, int pin)
{
    // input: targetSalinity (of system),
    //        currentSalinity (of system),
    //        addedSalinity (% salinity of fluid to be added),
    //        pin (of solenoid used to adjust system salinity)
    // opens solenoid at <pin> for appropriate time to reach targetSalinity

    const float overflowFraction = .2; // Fraction of added water that overflows before mixing
    const float totalMass = .25;       // Total mass of water in a filled system (kg)
    const float flowRate = .01;        // Mass flow rate of solenoids (kg/s)
    // calculate mass of water to add
    float massToAdd = totalMass *
                      (currentSalinity - targetSalinity) /
                      (currentSalinity - addedSalinity) *
                      (1 / 1 - overflowFraction);
    // calculate time needed to add appropriate quantity of mass and open solenoid
    float time = massToAdd / flowRate * 1000; // x1000 to convert to ms
    toggleSolenoid(pin, time);
}

void toggleSolenoid(int pin, int time)
{
    // Opens solenoid at <pin> for <time> in ms.
    digitalWrite(pin, 1);
    delay(time);
    digitalWrite(pin, 0);
}

void lcdFancySetup()
{ //setup lcd with fancy display

    int ppos = 4;                   //counter position for cursor to print periods
    int cpos = 4;                   //counter pos for cursor to print string
    int npos = 0;                   //counter for position within string
    int i;                          //alternating incrementation
    String nacahd = "N.A.C.A.H.D."; //creates nacahd string to be referenced

    lcd.init();
    lcd.backlight();

    //this loop prints periods, notice total chars/2 because we print 2 chars each loop
    for (int poscount = 0; poscount < 6; poscount++)
    {
        lcd.setCursor(ppos, 1); //counter moves cursor
        lcd.print(" .");
        ppos += 2;  //cursor moves 2 spaces because there are 2 characters
        delay(150); //mess with this to best dramatic effect
    }

    delay(800);

    //this loop prints NACAHD with each individual letter printed separately using string position calling (npos)
    for (int poscount = 0; poscount < 6; poscount++)
    {
        //lcd.scrollDisplayLeft();
        lcd.setCursor(cpos, 1);         //counter moves cursor
        lcd.print(nacahd.charAt(npos)); //prints the character referenced by the position (npos) in nacahd string
        npos += 2;                      //iterate counters
        cpos += 2;
        delay(150);
    }

    delay(800);

    //this loop blinks the string "initiating..."
    for (int i = 0; i < 6; i++)
    { //
        if (i % 2 == 1)
        { //modulo to check whether i is odd to change between 2 states
            lcd.setCursor(3, 2);
            lcd.print("Initiating...");
            delay(500);
        }
        else
        {
            lcd.setCursor(3, 2);
            lcd.print("             ");
            delay(380);
        }
    }
    lcd.clear();
}

void lcdUpdate(float sLCL, float sSP, float sUCL,
               float tLCL, float tSP, float tUCL,
               float saltNow, float tempNow, int heater)
{

    //LCL is lower control limit
    //SP is set point
    //UCL is upper control limit
    // S and T in rows 1 and 2 are salinity and temp controls
    // saltNow, tempNow, and heater on the bottom are current salinity/temp values, heater current state of heater (0,1)

    //Current values
    String heaterState; // "on" or "off"

    //if/else block to modify heaterState based on whether the heater is on or not, arbitrary var names used here
    if (heater == 1)
    {
        heaterState = "ON";
    }
    else
    {
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
    lcd.setCursor(4, 2);
    lcd.print(tLCL);
    lcd.setCursor(10, 2);
    lcd.print(tSP);
    lcd.setCursor(16, 2);
    lcd.print(tSP);

    //fourth row, current states
    lcd.setCursor(0, 3);
    lcd.print("S=");
    lcd.setCursor(2, 3);
    lcd.print(saltNow);
    lcd.setCursor(8, 3);
    lcd.print("T=");
    lcd.setCursor(10, 3);
    lcd.print(tempNow);
    lcd.setCursor(15, 3);
    lcd.print("H=");
    lcd.setCursor(17, 3);
    lcd.print(heaterState);
}