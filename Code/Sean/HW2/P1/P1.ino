//--------------------------------------------------------
// ME 120-001
// Homework 2, Problem 1, 1/19/2020
// Sean Lai
//--------------------------------------------------------

// delcare global pins
const int SALINITY_POWER_PIN = 8;


void setup(){
pinMode(SALINITY_POWER_PIN, OUTPUT); // Set salinity pin to OUTPUT
Serial.begin(9600);

}

void loop(){
    // Delcare local pins and variables
    const int SALINITY_READING_PIN = A0;
    int currentTime = millis();
    int numReadings = 15;
    float sum = 0.0;

    digitalWrite(SALINITY_POWER_PIN, 1); // Turn salinity sensor on
    delay(100);
    for(int i=0; i<numReadings; i++){ // Take and sum readings
        sum += analogRead(SALINITY_READING_PIN);
        delay(10);
    }
    digitalWrite(SALINITY_POWER_PIN, 0); // Turn salinity sensor off
    sum /= numReadings; // Average sum over readings
    sum *= 5/1023.0; // Convert to voltage

    //Print to Serial Monitor
    Serial.print("Time: ");
    Serial.print(currentTime);
    Serial.print("  |  Voltage: ");
    Serial.println(sum);
    
}