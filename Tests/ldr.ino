/**
 * Smart Greenhouse Controller - Phase 1: Light Sensing
 * Last Updated: Jan 2026
 */

// Pin Definitions
const int LDR_PIN = A0;

// Configuration
const int SERIAL_BAUD = 9600;
const int SAMPLE_DELAY = 500; // ms

void setup() {
    // Initialize serial communication at 9600 bits per second
    Serial.begin(SERIAL_BAUD);
    
    // Explicitly set pin mode (though A0 is input by default)
    pinMode(LDR_PIN, INPUT);
    
    Serial.println("--- Smart Greenhouse System Initialized ---");
    Serial.println("Monitoring Light Levels (LDR on A0)...");
}

void loop() {
    // Read the analog value from the LDR (Range: 0 - 1023)
    int lightValue = analogRead(LDR_PIN);
    
    // Print the raw sensor data to the Serial Monitor
    Serial.print("Light Intensity (Raw): ");
    Serial.println(lightValue);

    // Logic placeholder: 
    // In a full system, we would map this to a percentage 
    // or trigger an LED/Grow Light relay if lightValue < THRESHOLD.

    delay(SAMPLE_DELAY); 
}