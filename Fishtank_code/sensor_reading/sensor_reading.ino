//--------------------------------------------------------
// Sensor readings program
// Version 1.0
// 1/27/20
// The North American Council on Aquatic Housing and Development
//--------------------------------------------------------

// delcare global pins
const int SALINITY_POWER_PIN = 8;


void setup(){
pinMode(SALINITY_POWER_PIN, OUTPUT);
Serial.begin(9600);

}

void loop(){
    // declare local pins
    const int SALINITY_READING_PIN = A0;

    // setup variables
    int numReadings = 30;
    int salinityReading = 0;
    float voltage = 0.0;

    // take a salinity reading
    salinityReading = takeReading(SALINITY_POWER_PIN, SALINITY_READING_PIN, numReadings);
    voltage = salinityReading*5/1023.0;
    int currentTime = millis();

    /* Comment out whichever Serial dialog is not needed
    // Print to Serial Monitor (for humans)
    Serial.print("Time: ");
    Serial.print(currentTime);
    Serial.print("  |  Voltage: ");
    Serial.println(voltage);
    */

    // Print to Serial Monitor (for data analysis)
    Serial.println(salinityReading);
    
}

float takeReading(int powerPin, int readingPin, int numReadings){
    float sum = 0.0;
    digitalWrite(powerPin, 1); // Turn sensor on
    delay(100); // Allow power to settle
    for(int i=0; i<numReadings; i++){ // Take readings and sum
        sum += analogRead(readingPin);
        delay(10);
    }
    digitalWrite(powerPin, 0); // Turn sensor off
    sum /= numReadings; // Average sum over readings
    return sum;
}