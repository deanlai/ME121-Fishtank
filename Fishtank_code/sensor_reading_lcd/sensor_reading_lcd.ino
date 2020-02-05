//--------------------------------------------------------
// Sensor readings program for conversion from analog output to salinity (wt%)
// Version 1.1
// 1/27/20
// The North American Council on Aquatic Housing and Development
// NOTE: To run this code you must have downloaded the LiquidCrystal_I2C library (and added it to the sketch from the IDE), 
//       which is in Whitman's folder in the main directory
//--------------------------------------------------------

// Setup LCD screen
#include <Wire.h> // Include wire (i2c comm) and liquidcrystal libraries 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // 20x4 LCD screen, 0x27 is the address

// delcare global pins
const int SALINITY_POWER_PIN = 8;

void setup(){
    // Setup pins and serial comms
    pinMode(SALINITY_POWER_PIN, OUTPUT);
    Serial.begin(9600);

    lcd.init();         // initiate LCD
    lcd.backlight();    // backlight on

}

void loop(){
    // declare local pins
    const int SALINITY_READING_PIN = A0;

    // declare constants from power law fit
    const float c1 = 944.3844;
    const float c2 = 0.1368;

    // setup variables
    int numReadings = 30;
    int salinityReading = 0;
    float salinityPercentage;

    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);
    
    // Convert from analog to salinity percentage using s = (a/c1)^(1/c2)
    salinityPercentage = pow(salinityReading/c1, 1/c2);

    // Print to Serial Monitor (for data analysis)
    Serial.print(salinityReading);
    Serial.print(" ");
    Serial.print(salinityPercentage, 4);
    Serial.println();

    // Print to LCD Screen
    lcd.setCursor(1,0); lcd.print("N.A.C.A.H.D.");
    lcd.setCursor(0,1);                 // Print to second row
    lcd.print("Percent salt: ");
    lcd.print(salinityPercentage, 4);   // to 4 decimal places accuracy
    lcd.setCursor(0, 2);                // Print to third row
    lcd.print("Analog read: ");
    lcd.print(salinityReading);         // Add spaces to clear trailing digits
    lcd.print("   ");
    delay(250);                         // delay between refresh
     
}

float takeReading(int powerPin, int readingPin, int numReadings){
    // input: powerPin (digital out), readingPin (analog in), numReadings (>0)
    // output: average of numReadings readings

    float sum = 0.0;
    digitalWrite(powerPin, 1);          // Turn sensor on
    delay(100);                         // Allow power to settle
    for(int i=0; i<numReadings; i++){   // Take readings and sum
        sum += analogRead(readingPin);
        delay(10);
    }
    digitalWrite(powerPin, 0);          // Turn sensor off
    sum /= numReadings;                 // Average sum over readings

    return sum;
}
