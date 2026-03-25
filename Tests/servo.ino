/**
 * Smart Greenhouse Controller - Phase 2: Automated Shading/Ventilation
 * Hardware: LDR on A0, Servo on Pin 3
 * Last Updated: Jan 2026
 */

#include <Servo.h>

// Pin Definitions
const int LDR_PIN = A0;
const int SERVO_PIN = 3;

// Thresholds & Configuration
const int LIGHT_THRESHOLD = 600; // Adjust based on your environment (0-1023)
const int VENT_OPEN = 90;        // Degrees
const int VENT_CLOSED = 0;       // Degrees
const int SAMPLE_DELAY = 1000;   // 1 second interval

Servo greenhouseVent;

void setup() {
    Serial.begin(9600);
    
    // Attach servo to digital pin 3
    greenhouseVent.attach(SERVO_PIN);
    
    // Initialize position
    greenhouseVent.write(VENT_CLOSED);
    
    Serial.println("--- Smart Greenhouse System: Phase 2 ---");
    Serial.println("Monitoring LDR for Vent Control...");
}

void loop() {
    // 1. Read LDR while Servo is "quiet"
    int lightValue = analogRead(LDR_PIN);
    Serial.print("Stable LDR Value: ");
    Serial.println(lightValue);

    // 2. Logic
    if (lightValue > LIGHT_THRESHOLD) {
        greenhouseVent.attach(SERVO_PIN); // Wake up servo
        greenhouseVent.write(90);
        delay(500);                       // Give it time to move
        greenhouseVent.detach();          // Cut the noise/power
    } else {
        greenhouseVent.attach(SERVO_PIN);
        greenhouseVent.write(0);
        delay(500);
        greenhouseVent.detach();
    }

    delay(1000); 
}