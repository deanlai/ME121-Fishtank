//Salinity sensor

//delcare pins
int sensorPowerPin = 8;
int sensorReadingPin = A0;


void setup(){
pinMode(sensorPowerPin, OUTPUT);
Serial.begin(9600);

}

void loop(){
    int numReadings = 15;
    float sum = 0.0;
    sum = takeReading(sensorPowerPin, sensorReadingPin, numReadings);
    
    //Print to Serial Monitor
    Serial.print("Time: ");
    Serial.print(currentTime);
    Serial.print("  |  Voltage: ");
    Serial.println(sum);
    
}

float takeReading(int powerPin, int readingPin, int numReadings){
    int currentTime = millis();
    float sum = 0.0;
    digitalWrite(powerPin, 1);
    delay(100);
    for(int i=0; i<numReadings; i++){
        sum += analogRead(readingPin);
        delay(10);
    }
    digitalWrite(powerPin, 0);
    sum /= numReadings; // Average sum over readings
    sum *= 5/1023.0; // Convert to voltage
    return sum;
}