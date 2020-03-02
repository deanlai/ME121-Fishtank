

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


void setup() {
      // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    pinMode(TEMP_SENSPOWER_PIN, OUTPUT);
    pinMode(saltyPin, OUTPUT);
    pinMode(freshPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);

    Serial.begin(9600);
    delay(3000);
}

void loop() {

   // declare local pins
    const int SALINITY_READING_PIN = A0;
    const int TEMPERATURE_READING_PIN = A1;
    const int S_SETPOINT_PIN = A2;
    const int T_SETPOINT_PIN = A3;
    
    //declare constants for Temperature Calibration polynomial fit 
    //Notes: 
    // "t" prefix indicates that the constant is for the temperature poly fit
    // c indicates that it is a constant
    // tc1, tc2... is the order of the constants for temp in deg Celsius as a function of analog reading
    const float tc1 = 1.5464668e-04;
    const float tc2 = -6.8341165e-02;
    const float tc3 = 1.8217030e+01;

        // setup variables
    int numReadings = 30;     // number of readings per salinity reading
    int salinityReading;      // current analog reading from salinity sensor
    float salinityPercentage; // current salinity percentage
    float tempReading;        // current analog reading from thermistor
    float systemTemp;         // current temperature of system (deg C)
    float solTime = 0;
    int solPin;
    int offTime = 15000;
    unsigned long heatduration = 60000;

        // Compute setpoint from pot reading
    float tSetpoint = findTemp(analogRead(T_SETPOINT_PIN), tc1, tc2, tc3); 
    float tSigma_analog = 2;
    float tSigma = findTemp(tSigma_analog, tc1, tc2, tc3);

    float tUCL = findTemp(analogRead(T_SETPOINT_PIN)+3*tSigma_analog, tc1, tc2, tc3); 
    float tLCL = findTemp(analogRead(T_SETPOINT_PIN)-3*tSigma_analog, tc1, tc2, tc3); 

       //take temp reading and convert it to degrees for function 
    systemTemp = findTemp(takeReading(TEMP_SENSPOWER_PIN, TEMPERATURE_READING_PIN, 3),
                          tc1, tc2, tc3); 

    //turn heater on or off - HEATER PINS ARE CURRENTLY DEACTIVATED
    //adjustTemp(tLCL, tSetpoint, &heaterState, &systemTemp);

    
    heatertest(heatduration, offTime);
    Serial.println(systemTemp);
    Serial.println(heaterState);
    Serial.println(float(millis()/1000));
    
     // Update LCD screen
    //lcdUpdate(sLCL, sSetpoint, sUCL, tLCL, tSetpoint, tUCL, salinityPercentage, systemTemp, heaterState);

    
  
}

void heatertest(long heatduration, int offtime){
  static long heatstart = -offtime;
  if (heaterState==0 && ((millis()-heatstart)>offtime)) {
    Serial.println("on");
    digitalWrite(heaterPin, HIGH);
    heaterState = 1;
    heatstart = millis();
  }
    
  
  else if (((millis()-heatstart)>heatduration) && heaterState == 1) {
    Serial.println("here");
    digitalWrite(heaterPin, LOW);
    heaterState = 0;
    heatstart = millis();
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






// THIS EVALS SECOND DEGREE POLY
float evaluate2ndPolynomial(int x, float c1, float c2, float c3) {
    //evaluates y = c1*x^2 + c2*x + 3 and returns y
    return c1*x*x + c2*x + c3;
} 

//call with findTemp(analogRead(TEMPERATURE_READING_PIN)); for temperature in degrees Celsius 
float findTemp(int reading, float c1, float c2, float c3) {
 
   return evaluate2ndPolynomial(reading, c1, c2, c3);
   
}

void adjustTemp(float LCL, float setpoint, int* heaterState, float* temp) {

  if (*temp < LCL) {
    //digitalWrite(heaterPin, HIGH);
    *heaterState = 1;
  }
  else {
    //digitalWrite(heaterPin, LOW);
    *heaterState = 0;
  }
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
