#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Pin Definitions
#define LDR_PIN 0    // A0
#define JOY_X_PIN 1  // A1
#define JOY_BTN_PIN 17 // A3 is Digital 17 on many boards

// Bitmasks for System State (Stored in 1 byte)
#define STATE_LOCKED   0x01  // 00000001
#define STATE_VENT_OPN 0x02  // 00000010
#define STATE_BTN_LAST 0x04  // 00000100

uint8_t sysState = STATE_LOCKED; // Initial state: Locked, Closed, Button Up
int threshold = 512;
uint32_t tapTimer = 0;
Servo vent;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(JOY_BTN_PIN, INPUT_PULLUP);
  
  updateVent(0);
  lcd.print("BIT-READY");
  delay(800);
  lcd.clear();
}

void loop() {
  uint32_t now = millis();
  
  // 1. Bitwise Button Logic & Double Tap
  uint8_t btnPhysical = digitalRead(JOY_BTN_PIN);
  
  // Detect Falling Edge (Press) using bitwise comparison
  if (!(btnPhysical) && (sysState & STATE_BTN_LAST)) {
    if (sysState & STATE_LOCKED) {
      // Check Double Tap Timing
      if ((now - tapTimer) < 400) {
        sysState &= ~STATE_LOCKED; // Bitwise Clear: Unlock
        flashMessage("UNLOCKED");
      }
      tapTimer = now;
    } else {
      sysState |= STATE_LOCKED;    // Bitwise Set: Lock
      flashMessage("LOCKED  ");
    }
  }

  // Update Button Last State Bit
  if (btnPhysical) sysState |= STATE_BTN_LAST;
  else sysState &= ~STATE_BTN_LAST;

  // 2. Efficient Joystick Logic
  if (!(sysState & STATE_LOCKED)) {
    int x = analogRead(JOY_X_PIN);
    // Bit-shift deadzone: (512 >> 6) is 8. 
    // This compares the upper bits to see if moved significantly.
    if ((x >> 6) > 9) threshold += 15;      // Right
    else if ((x >> 6) < 7) threshold -= 15; // Left
    threshold = (threshold < 0) ? 0 : (threshold > 1023) ? 1023 : threshold;
  }

  // 3. LDR & Vent Logic
  int ldr = analogRead(LDR_PIN);
  if (ldr > threshold) updateVent(90);
  else updateVent(0);

  // 4. Low-Latency Display Update
  lcd.setCursor(0, 0);
  lcd.print("L:"); lcd.print(ldr); lcd.print("  T:"); lcd.print(threshold); lcd.print("    ");
  
  lcd.setCursor(0, 1);
  lcd.print((sysState & STATE_LOCKED) ? "[LCK] " : "[ADJ] ");
  lcd.print((sysState & STATE_VENT_OPN) ? "OPEN  " : "CLOSED");

  delay(80); 
}

void flashMessage(const char* m) {
  lcd.clear();
  lcd.print("SYSTEM "); lcd.print(m);
  delay(2000);
  lcd.clear();
}

void updateVent(uint8_t angle) {
  // Check if requested state differs from current bit state
  bool currentlyOpen = (sysState & STATE_VENT_OPN);
  if ((angle == 90) != currentlyOpen) {
    vent.attach(3);
    vent.write(angle);
    delay(400);
    vent.detach();
    
    // Toggle the Vent State Bit
    if (angle == 90) sysState |= STATE_VENT_OPN;
    else sysState &= ~STATE_VENT_OPN;
  }
}