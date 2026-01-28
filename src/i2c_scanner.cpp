/**
 * I2C Scanner - Find all devices on the I2C bus
 * 
 * This will scan multiple I2C buses and pin combinations
 * to find where the IMU (or any sensor) might be.
 */

#include <Arduino.h>
#include <Wire.h>

void scanBus(TwoWire &wire, const char* name) {
    Serial.print("\n=== Scanning ");
    Serial.print(name);
    Serial.println(" ===");
    
    int deviceCount = 0;
    for (byte addr = 1; addr < 127; addr++) {
        wire.beginTransmission(addr);
        byte error = wire.endTransmission();
        
        if (error == 0) {
            Serial.print("  Device found at 0x");
            if (addr < 16) Serial.print("0");
            Serial.print(addr, HEX);
            
            // Identify known devices
            if (addr == 0x6A || addr == 0x6B) Serial.print(" (LSM6DS3/LSM6DSOX IMU)");
            if (addr == 0x10) Serial.print(" (VEML7700 Light)");
            if (addr == 0x29) Serial.print(" (VL53L0X ToF)");
            if (addr == 0x3C || addr == 0x3D) Serial.print(" (SSD1306 OLED)");
            if (addr == 0x48) Serial.print(" (ADS1115 ADC)");
            if (addr == 0x50) Serial.print(" (EEPROM)");
            if (addr == 0x68) Serial.print(" (MPU6050/DS3231)");
            if (addr == 0x76 || addr == 0x77) Serial.print(" (BME280/BMP280)");
            Serial.println();
            deviceCount++;
        }
    }
    
    if (deviceCount == 0) {
        Serial.println("  No devices found on this bus");
    } else {
        Serial.print("  Total: ");
        Serial.print(deviceCount);
        Serial.println(" device(s)");
    }
}

void setup() {
    Serial.begin(115200);
    
    // Wait for serial
    unsigned long start = millis();
    while (!Serial && (millis() - start < 5000)) {
        delay(10);
    }
    
    Serial.println();
    Serial.println("==========================================");
    Serial.println("        I2C BUS SCANNER");
    Serial.println("    XIAO ESP32-S3 Sense Debug");
    Serial.println("==========================================");
    
    // Try different I2C pin configurations
    
    // Configuration 1: Default Wire (no pins specified)
    Serial.println("\n--- Config 1: Wire.begin() [default pins] ---");
    Wire.begin();
    delay(100);
    scanBus(Wire, "Wire (default)");
    Wire.end();
    
    // Configuration 2: GPIO 5/6 (D4/D5 - external I2C header)
    Serial.println("\n--- Config 2: Wire.begin(5, 6) [D4/D5 external header] ---");
    Wire.begin(5, 6);
    delay(100);
    scanBus(Wire, "Wire (GPIO5/6)");
    Wire.end();
    
    // Configuration 3: GPIO 4/5 (alternate)
    Serial.println("\n--- Config 3: Wire.begin(4, 5) ---");
    Wire.begin(4, 5);
    delay(100);
    scanBus(Wire, "Wire (GPIO4/5)");
    Wire.end();
    
    // Configuration 4: GPIO 8/9 (another common option)
    Serial.println("\n--- Config 4: Wire.begin(8, 9) ---");
    Wire.begin(8, 9);
    delay(100);
    scanBus(Wire, "Wire (GPIO8/9)");
    Wire.end();
    
    // Configuration 5: GPIO 41/42 (could be internal)
    Serial.println("\n--- Config 5: Wire.begin(41, 42) ---");
    Wire.begin(41, 42);
    delay(100);
    scanBus(Wire, "Wire (GPIO41/42)");
    Wire.end();
    
    // Configuration 6: GPIO 21/22 (classic ESP32 I2C)
    // Note: May not be available on XIAO
    // Serial.println("\n--- Config 6: Wire.begin(21, 22) ---");
    // Wire.begin(21, 22);
    // delay(100);
    // scanBus(Wire, "Wire (GPIO21/22)");
    // Wire.end();
    
    Serial.println();
    Serial.println("==========================================");
    Serial.println("Scan complete.");
    Serial.println();
    Serial.println("If no IMU found, check:");
    Serial.println("1. Is the Sense expansion board attached?");
    Serial.println("2. The expansion board has camera + mic + IMU");
    Serial.println("3. Base XIAO ESP32-S3 has NO onboard IMU");
    Serial.println("==========================================");
}

void loop() {
    // Repeat scan every 10 seconds
    delay(10000);
    Serial.println("\n[Re-scanning...]");
    
    Wire.begin(5, 6);
    scanBus(Wire, "Wire (GPIO5/6)");
    Wire.end();
}
