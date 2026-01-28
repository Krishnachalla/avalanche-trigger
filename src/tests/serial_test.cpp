/**
 * Serial Test - Minimal firmware to verify serial communication
 * 
 * Use this to verify USB serial is working before debugging other issues.
 * 
 * To use:
 *   1. Rename this to main.cpp (backup existing main.cpp first)
 *   2. pio run --target upload
 *   3. pio device monitor -b 115200
 *   4. Should see "Hello, World! 1", "Hello, World! 2", etc.
 * 
 * Created: 2025-01-28 during debug session
 */

#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    
    // Wait for serial connection (important for USB CDC)
    delay(2000);
    
    Serial.println();
    Serial.println("=== Serial Test ===");
    Serial.println("If you see this, serial is working!");
    Serial.println();
}

void loop() {
    static int count = 1;
    Serial.print("Hello, World! ");
    Serial.println(count++);
    delay(1000);
}
