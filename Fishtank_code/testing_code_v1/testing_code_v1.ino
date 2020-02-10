//--------------------------------------------------------
// Testing Code
// Version 1.0
// 2/9/20
// The North American Council on Aquatic Housing and Development
// Extends sensor_reading code base to actuate solenoids
// Usage: This program is intended for testing different features and debugging the system
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

// working vars for button toggling
int toggled = 0;       // State of machine that button controls: 0: off, 1: on
int currentState;      // Current state of button --> 1: pressed, 0: not pressed
int previousState = 0; // Previous state of button --> 1: pressed, 0: not pressed
long timer = 0;        // Used to implement delay
long debounce = 150;   // Delay checking conditional for time of physical button press

// for timing sensing
int currentTime = millis();

void setup()
{
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    pinMode(saltyPin, OUTPUT);
    pinMode(freshPin, OUTPUT);
    pinMode(buttonLED, OUTPUT);
    pinMode(buttonIn, INPUT_PULLUP);
    Serial.begin(9600);

    // Setup LCD
    lcd.init();      // initiate LCD
    lcd.backlight(); // backlight on
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
    const float cl1 = 9.0881533e-05;
    const float cl2 = -6.9069865e-03;
    const float ch1 = 8.5798207e-04;
    const float ch2 = -4.8485778e-01;
    const int b1 = 76;
    const int b2 = 626;
    const int b3 = 737;

    // declare setpoint(wt %) and sigma (a)
    float setpoint = 0;
    float sigma_analog = 0;
    // convert sigma_analog to wt %
    float sigma = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, sigma_analog);

    // setup variables
    int numReadings = 30;     // number of readings per salinity reading
    int salinityReading;      // current analog reading from salinity sensor
    float salinityPercentage; // current salinity percentage
    int deadtime;             // timer used to allow water to mix

    // compute UCL & LCL
    float UCL = setpoint + 3 * sigma;
    float LCL = setpoint - 3 * sigma;

    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);
    // Convert from analog to salinity percentage using s = (a/c1)^(1/c2)
    salinityPercentage = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, salinityReading);

    // check if salinity reading is within control limits
    // conditional checks if salinity reading is above UCL OR below LCL
    // AND if enough deadtime has passed to perform another adjustment
    if ((salinityPercentage > UCL || salinityPercentage < LCL) && millis() > deadtime + 12000) {
        adjustSalinity(salinityPercentage, setpoint); // Adjust salinity using solenoids
        deadtime = millis();                          // Set deadtime timer to current millis() value
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
    lcd.print("   "); // added empty characters to clear trailing digits
    delay(80);        // delay between refresh
    if (salinityPercentage > 0.1475) {
        lcd.setCursor(0, 3);
        lcd.print("Salinity Too High"); //model too high salinity with upper section calibration data for est
    }

    // ----- FOR TESTING AND DEBUGGING -----
    // -------------------------------------

    // read button to manually actuate solenoids using relayTest()
    // Usage: hold down button for a second to toggle relay state
    buttonRead(buttonIn); // read button state and swap toggle
    digitalWrite(buttonLED, toggled);
    relayTest(toggled); // actuate relays based on toggle state

    // Print to Serial Monitor (for data analysis)
    Serial.println(salinityReading);
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

float findSalinityPercentage(float c1, float c2, float c3, float c4, int b1, int b2, int b3, int reading)
{
    // input c1, c2, c3, c4 --> polynomial constants for two line fits
    //       b1, b2, b3     --> breakpoints to determine regin for evaluation
    //       reading        --> analog reading value from sensor
    // output: corresponding salinity in wt%

    // check breakpoints to determine function region
    if (reading < b1) {      // reading < DI water breakpoint
        return 0;
    }
    else if (reading < b2) { // reading < 0.05 wt% breakpoint
        return evaluatePolynomial(reading, c1, c2);
    }
    else if (reading < b3) { // reading < 0.15 wt% breakpoint
        return evaluatePolynomial(reading, c3, c4);
    }
    else {                   // reading > 0.15 wt% breakpoint
        return evaluatePolynomial(b3, c3, c4); // return value of polynomial at b3
    }
}

float evaluatePolynomial(int x, float c1, float c2)
{
    // evaluates y = c1*x + c2 and returns y
    return c1 * x + c2;
}

void adjustSalinity(float currentSalinity, float setpoint)
{
    // input: current salinity and salinity setpoint
    // output: none
    // calls openSolenoid() to adjust salinity of system to a target salinity

    // Set target salinity to 80% of the difference between current salinity and setpoint
    int targetSalinity = currentSalinity - (currentSalinity - setpoint) * 0.8;

    if (targetSalinity > currentSalinity) {
        openSolenoid(targetSalinity, currentSalinity, 1, saltyPin); // add 1% salted water
    }
    else {
        openSolenoid(targetSalinity, currentSalinity, 0, freshPin); // add 0% DI water
    }
}

float openSolenoid(float targetSalinity, float currentSalinity, int addedSalinity, int pin)
{
    // input: targetSalinity (of system),
    //        currentSalinity (of system),
    //        addedSalinity (% salinity of fluid to be added),
    //        pin (of solenoid used to adjust system salinity)
    // opens solenoid at <pin> for appropriate time to reach targetSalinity

    const float overflowFraction = 1; // Fraction of added water that overflows before mixing
    const float totalMass = 0;        // Total mass of water in a filled system
    const float flowRate = 0;         // Mass flow rate of solenoids
    // calculate mass of water to add
    massToAdd = totalMass *
                (currentSalinity - targetSalinity) /
                (currentSalinity - addedSalinity) *
                (1 / 1 - overflowFraction);
    // calculate time needed to add appropriate quantity of mass and open solenoid
    time = massToAdd / flowRate;
    digitalWrite(pin, 1);
    delay(time);
    digitalWrite(pin, 0);
}

int buttonRead(int buttonIn) {
    // input: button Pin
    // Reads button state and swaps state of toggled variable if button is pressed for sufficient time

    currentState = digitalRead(buttonIn);       // read current button state

    if (currentState == 1 &&                    // checks that button is pressed and previously was not
        previousState == 0 &&                   // and that a sort delay has passed between state changes
        (millis() - timer * 1000) > debounce) {
        if (toggled == 0) {         // swaps toggled state
            toggled = 1;
        } 
        else {
            toggled = 0;
        }
        timer = millis() / 1000;    // sets timer for button state delay
    }
    previousState = currentState;   // set previous button state to current button state
}

void relayTest(int toggled)
{
    // turns all transistors on or off based on toggled state
    if (toggled == 1) {
      digitalWrite(saltyPin), 1);
      digitalWrite(freshPin, 1);
    }
    else if (toggled == 0) {
      digitalWrite(saltyPin), 0);
      digitalWrite(freshPin, 0);
    }
}
