/**
 * Smart Greenhouse Controller - Phase 4: Full Joystick Control
 * Hardware: LDR (A0), Servo (Pin 3), Joy X (A1), Joy Y (A2), Joy MS (A3)
 * Last Updated: Jan 2026
 */

#include <Servo.h>

// Pin Definitions
const int LDR_PIN = A0;
const int JOY_X_PIN = A1;
const int JOY_Y_PIN = A2;
const int JOY_BTN_PIN = A3; // Moved to A3
const int SERVO_PIN = 3;

// Calibration & Thresholds
int lightThreshold = 500;
const int JOY_DEADZONE = 100; // Increased for stability
const int JOY_CENTER = 512;
int lastVentAngle = -1;

Servo greenhouseVent;

void setup() {
    Serial.begin(9600);
    
    // Treating A3 as a digital input with internal pullup
    pinMode(JOY_BTN_PIN, INPUT_PULLUP); 
    
    // Initial Servo Sync
    greenhouseVent.attach(SERVO_PIN);
    greenhouseVent.write(0);
    delay(500);
    greenhouseVent.detach(); 

    Serial.println("--- System Initialized ---");
    Serial.println("X (A1): Fast Adjust | Y (A2): Fine Adjust | MS (A3): Save");
}

void loop() {
    // 1. Read Joystick Inputs
    int joyX = analogRead(JOY_X_PIN);
    int joyY = analogRead(JOY_Y_PIN);
    bool btnPressed = (digitalRead(JOY_BTN_PIN) == LOW);

    // 2. Adjust Threshold based on Joystick
    // X-Axis: Fast steps (+/- 10)
    if (joyX > (JOY_CENTER + JOY_DEADZONE)) lightThreshold += 10;
    else if (joyX < (JOY_CENTER - JOY_DEADZONE)) lightThreshold -= 10;

    // Y-Axis: Fine steps (+/- 1)
    if (joyY > (JOY_CENTER + JOY_DEADZONE)) lightThreshold += 1;
    else if (joyY < (JOY_CENTER - JOY_DEADZONE)) lightThreshold -= 1;

    lightThreshold = constrain(lightThreshold, 0, 1023);

    // 3. Handle "Save" Button (MS)
    if (btnPressed) {
        Serial.print(">>> [SAVED] Threshold locked at: ");
        Serial.println(lightThreshold);
        delay(300); // Debounce
    }

    // 4. LDR Logic & Servo Movement
    int currentLight = analogRead(LDR_PIN);
    
    // Print Status
    Serial.print("LDR: "); Serial.print(currentLight);
    Serial.print(" | Threshold: "); Serial.print(lightThreshold);
    Serial.print(" | X: "); Serial.print(joyX);
    Serial.print(" | Y: "); Serial.println(joyY);

    if (currentLight > lightThreshold) {
        updateVent(90); // Open
    } else {
        updateVent(0);  // Close
    }

    delay(150); // Loop speed
}

/**
 * Moves servo only on state change to prevent 
 * constant electrical noise on the analog pins.
 */
void updateVent(int angle) {
    if (angle != lastVentAngle) {
        greenhouseVent.attach(SERVO_PIN);
        greenhouseVent.write(angle);
        delay(400); // Ensure movement completes
        greenhouseVent.detach(); 
        lastVentAngle = angle;
        
        Serial.print("--- VENT MOVED TO: ");
        Serial.print(angle);
        Serial.println(" deg ---");
    }
}