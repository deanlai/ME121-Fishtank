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

// delcare global pins and heater state
const int SALINITY_POWER_PIN = 8;
const int TEMP_SENSPOWER_PIN = 9;
const int saltyPin = 10; 
const int freshPin = 11;
const int heaterPin = 12;
int heaterState = 0;

void setup()
{
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    pinMode(TEMP_SENSPOWER_PIN, OUTPUT);
    pinMode(saltyPin, OUTPUT);
    pinMode(freshPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);

    // Setup LCD
    lcdSimpleSetup();

    // Setup serial comms
    Serial.begin(9600);
    Serial.println("Initiating... PLEASE WAIT 5S");
    delay(5000);
    
    

    //comment this out unless you mean to do it pls
    //systemFlush();
}

void loop() //------------------- LOOP ----------------------------------------------------------------------------
{
    // declare local pins
    const int SALINITY_READING_PIN = A0;
    const int TEMPERATURE_READING_PIN = A1;
    const int S_SETPOINT_PIN = A2;
    const int T_SETPOINT_PIN = A3;
    

    // declare constants and breakpoints for polynomial fit of salinity calibration
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

    //declare constants for Temperature Calibration polynomial fit 
    //Notes: 
    // "t" prefix indicates that the constant is for the temperature poly fit
    // c indicates that it is a constant
    // tc1, tc2... is the order of the constants for temp in deg Celsius as a function of analog reading
    const float tc1 = 1.5464668e-04;
    const float tc2 = -6.8341165e-02;
    const float tc3 = 1.8217030e+01;
    const float cK1 = 0;      //slope of change in heat v time when heater is on
    const float cK2 = 0;      // second coeffecient from temp change calibration
    float tFrontDelay = 0;    //delay from heater to being on to when the system temp starts to change
    float tEndDelay = 0;      //time it takes residual heat in radiator to dissipate
    
    // setup variables
    int numReadings = 30;     // number of readings per salinity reading
    int salinityReading;      // current analog reading from salinity sensor
    float salinityPercentage; // current salinity percentage
    float tempReading;        // current analog reading from thermistor
    float systemTemp;         // current temperature of system (deg C)
    float heatTime = 0;       // time heater should be on
    float solTime = 0;        // time solenoid should be open
    int solPin;               // which solenoid should be open

    // declare sigma(analog) and deadtime from calibration
    // note: s & t prefixes refer to salinity and temperature
    float sSetpoint = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, 
                                             analogRead(S_SETPOINT_PIN)); // Compute setpoint from pot reading
    const float sSigma_analog = 2.265; // from calibration of salinity percentages
    const unsigned long deadtime = 15000;    // averaged 
    // convert sigma_analog to wt %

    // Compute setpoint from pot reading
    float tSetpoint = findTemp(analogRead(T_SETPOINT_PIN), tc1, tc2, tc3); 
    float tSigma_analog = 2;
    float tSigma = findTemp(tSigma_analog, tc1, tc2, tc3);

    // compute UCL & LCL for salinity and temperature
    float sUCL = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3,
                                        analogRead(S_SETPOINT_PIN) + 3*sSigma_analog);
    float sLCL = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3,
                                        analogRead(S_SETPOINT_PIN) - 3*sSigma_analog);
    
    float tUCL = findTemp(analogRead(T_SETPOINT_PIN)+3*tSigma_analog, tc1, tc2, tc3); 
    float tLCL = findTemp(analogRead(T_SETPOINT_PIN)-3*tSigma_analog, tc1, tc2, tc3); 

//------------------------------- ACTUAL CONTROL  --------------------------------------------------------------------------------------
//-----SENSOR READINGS    
// take a salinity reading and convert to percentage salt
    // * salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);
    // * salinityPercentage = findSalinityPercentage(cl1, cl2, ch1, ch2, b1, b2, b3, salinityReading);
//take temp reading and convert it to degrees for function 
    int tread = round( takeReading(TEMP_SENSPOWER_PIN, TEMPERATURE_READING_PIN, 3) );
    systemTemp = findTemp(tread,
                          tc1, tc2, tc3); 
//-----DO SOME MATH WITH SENSOR READINGS  
    //calculate duration for solenoids to be on based on readings
    // * setAdjustmentTimes(salinityPercentage, sSetpoint, sUCL, sLCL, deadtime, &solPin, &solTime);
    //calculate duration for heater to be on 
    // * setHeaterTime(systemTemp, tLCL, tSetpoint, cK1, cK2, tFrontDelay, tEndDelay);
//-----MAKE CHANGES BASED ON MATH     
    //turn solenoids on or off
    // * toggleSolenoids(solPin, solTime, deadtime);
    //turn heater on or off - HEATER PINS ARE CURRENTLY DEACTIVATED
    // * adjustTemp(heatTime);
<<<<<<< HEAD
    const long timeOffStart = 30000;
    const long timeOn = 300000;
    const long timeOffEnd = 300000;
=======
    const long timeOffStart = 60000;
    const long timeOn = 300000;
    const long timeOffEnd = 120000;
>>>>>>> bdb0e1c829ca3f305bb6f07c19f2da90be8d73e3
    static long startTime = 0;

    if (millis()<timeOffStart){
      
      digitalWrite(heaterPin, LOW);
      heaterState = 0;
      startTime = millis();
    }
    else if (millis()<(timeOffStart+timeOn)){
      //Serial.println("there");
      digitalWrite(heaterPin, HIGH);
      heaterState = 1;
      startTime = millis();
    }
    else {
      //Serial.println("everywhere");
      digitalWrite(heaterPin, LOW);
      heaterState = 0;
    }
    Serial.print(millis()); Serial.print("  "); Serial.print(heaterState); 
                                        Serial.print("  "); Serial.println(systemTemp); 

   /*                                     
    // time state temp
    long timeh = 120000;
    long timeoff = 120000;
    static long startTime = -timeoff;
  
    if (heaterState==0 && (millis()-startTime)>timeoff){
      digitalWrite(heaterPin, HIGH);
      heaterState = 1;
      startTime = millis();
    }
    else if (heaterState == 1 && (millis()-startTime)>= timeh) {
      digitalWrite(heaterPin, LOW);
      heaterState = 0;
      startTime = millis();
    }
    */
    
//-----UPDATE LCD
    lcdUpdate(sLCL, sSetpoint, sUCL, tLCL, tSetpoint, tUCL, salinityPercentage, systemTemp, heaterState);
    
} // <-this bracket ends loop


//-------------------------------- FUNCTIONS -------------------------------------------------------------------------------------------


void toggleSolenoids(int solPin, int solTime, int deadtime){
  //this function controls the solenoids, that is all it does
  static unsigned long startTime = (-1)*deadtime; //so that it doesnt freak out in the first 15 seconds of being on
  static int solStatus = 0;

  if (solStatus == 0 && solTime > 0 && (millis()-startTime) > deadtime) {
    digitalWrite(solPin, HIGH);
    solStatus = 1;
    startTime = millis();
  }
 
  else if ((millis()-startTime) > solTime){
    digitalWrite(solPin, LOW);
    solStatus = 0;
  }

  if ((millis()-startTime) < deadtime) {
    //this throws a little deadtime clock up in upper left corner
    int lcddeadDisplay = (((deadtime-(millis()-startTime))/1000));
    lcd.setCursor(0,0);
    lcd.print(lcddeadDisplay);
    lcd.print(" ");
    if(lcddeadDisplay == 0){
      lcd.setCursor(0,0);
      lcd.print("   ");
    }
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

float findSalinityPercentage(float c1, float c2, float c3, float c4, 
                             int b1, int b2, int b3, float reading) {
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

// THIS EVALS FIRST DEGREE POLY
float evaluatePolynomial(int x, float c1, float c2) {
    // evaluates y = c1*x + c2 and returns y
    return c1*x + c2;
}
// THIS EVALS SECOND DEGREE POLY
float evaluate2ndPolynomial(float x, float c1, float c2, float c3) {
    //evaluates y = c1*x^2 + c2*x + 3 and returns y
    return c1*x*x + c2*x + c3;
}


float setAdjustmentTimes(float currentSalinity, float setpoint, float UCL, float LCL, int deadtime, int* solPin, float* solTime)
{
    // input: current salinity and salinity setpoint
    // output: none
    // calls openSolenoid() to adjust salinity of system to a target salinity

    if (currentSalinity > UCL || currentSalinity < LCL) {
        // Set target salinity to 80% of the difference between current salinity and setpoint
        float targetSalinity = currentSalinity - (currentSalinity - setpoint) * 0.8;
        if (targetSalinity > currentSalinity) {
            *solPin = saltyPin; // sets salty pin to be adjusted
            setTime(targetSalinity, currentSalinity, 1, solTime); // sets time for salty solenoid
        }
        else {
            *solPin = freshPin; // sets fresh pin to be adjusted
            setTime(targetSalinity, currentSalinity, 0, solTime); // sets time for fresh solenoid
        }
    }
    return 0;
   
}

float setTime(float targetSalinity, float currentSalinity, int addedSalinity, float* time)
{
    // input: targetSalinity (of system),
    //        currentSalinity (of system),
    //        addedSalinity (% salinity of fluid to be added),
    //        pin (of solenoid used to adjust system salinity)
    // opens solenoid at <pin> for appropriate time to reach targetSalinity

    const float overflowFraction = .2; // Fraction of added water that overflows before mixing
    const float totalMass = .143;       // Total mass of water in a filled system (kg)
    float flowRate = 0;        // Mass flow rate of solenoids (kg/s)
    if (addedSalinity == 1) {
        flowRate = .003633; // Salty tank flow rate
    }
    else {
        flowRate = .004167; // Fresh tank flow rate
    }
    // calculate mass of water to add 
    float massToAdd = totalMass *
                      (currentSalinity - targetSalinity) /
                      (currentSalinity - addedSalinity) *
                      (1 / (1 - overflowFraction));
    // calculate time needed to add appropriate quantity of mass and open solenoid
    *time = ( massToAdd / flowRate ) * 1000; // x1000 to convert to ms. Sets solTime in loop() to calculated time
}

//calculate time heater should be on
float setHeaterTime(float temp, float LCL, float setPoint, float K, float K2, float tFrontDelay, float tEndDelay) {
  float error;
  float heaterTime;
  if (temp<=LCL){
    error = LCL - temp;
    heaterTime = tFrontDelay + (error-K2)/K - tEndDelay; //this should give time necessary to correct error
  }
  else {
    heaterTime = 0;
  }
  return heaterTime;

}
//turn heater on or off based on calculated time
void adjustTemp(float HeaterTime) {
  if (heaterState == 0 && HeaterTime>0){
    digitalWrite(heaterPin, HIGH);
    heaterState = 1;
  }
  else if (heaterState == 1 && HeaterTime == 0) {
    digitalWrite(heaterPin, LOW);
    heaterState = 0;
  }
}

//call with findTemp(analogRead(TEMPERATURE_READING_PIN)); for temperature in degrees Celsius 
float findTemp(float reading, float c1, float c2, float c3) {
   return evaluate2ndPolynomial(reading, c1, c2, c3);
   
}

void systemFlush(){
  lcd.setCursor(1, 2);
  lcd.print("SYSTEM FLUSH");
  delay(2000);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  delay(500000);
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
    lcd.print(sLCL,3);
    lcd.setCursor(9, 1);
    lcd.print(sSP, 3);
    lcd.setCursor(15, 1);
    lcd.print(sUCL, 3);

    //third row, Temp
    lcd.setCursor(0, 2);
    lcd.print("T:");
    lcd.setCursor(3, 2);
    lcd.print(tLCL, 1);
    lcd.setCursor(9, 2);
    lcd.print(tSP, 1);
    lcd.setCursor(15, 2);
    lcd.print(tUCL, 1);

    //fourth row, current states
    lcd.setCursor(0, 3);
    lcd.print("S=");
    lcd.setCursor(2, 3);
    lcd.print(saltNow);
    lcd.setCursor(7, 3);
    lcd.print("T=");
    lcd.setCursor(9, 3);
    lcd.print(tempNow, 1);
    lcd.setCursor(15, 3);
    lcd.print("H=");
    lcd.setCursor(17, 3);
    lcd.print(heaterState);
}
void lcdSimpleSetup()
{

    lcd.init();
    lcd.backlight();
}
