/**
 * Automatic Avalanche Airbag Trigger
 * 
 * This firmware detects avalanche burial conditions and automatically
 * triggers an airbag deployment when the user is incapacitated.
 * 
 * Detection criteria:
 * 1. Light level drops below threshold (burial)
 * 2. Recent motion detected (was skiing/moving)
 * 3. Currently still (incapacitated)
 * 4. Conditions persist for confirmation time
 * 
 * Safety features:
 * - Warning buzzer before trigger (can cancel)
 * - Requires explicit arming
 * - Never triggers while moving
 * 
 * Hardware:
 * - XIAO ESP32-C3
 * - BH1750 Light Sensor (I2C)
 * - LSM6DS3 IMU (I2C)
 * - MOSFET + Solenoid
 */

#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_LSM6DS3.h>

#include "config.h"

// =============================================================================
// STATE MACHINE
// =============================================================================

enum SystemState {
    STATE_DISARMED,     // System off, LED blue
    STATE_ARMED,        // Monitoring, LED green
    STATE_MONITORING,   // Dark detected, checking motion, LED yellow
    STATE_WARNING,      // About to trigger, buzzer on, LED red blink
    STATE_TRIGGERED,    // Solenoid fired, LED red solid
    STATE_DEPLOYED      // Airbag deployed, waiting for reset
};

// Current state
SystemState currentState = STATE_DISARMED;

// =============================================================================
// SENSOR OBJECTS
// =============================================================================

BH1750 lightSensor;
Adafruit_LSM6DS3 imu;

// =============================================================================
// MOTION DETECTION VARIABLES
// =============================================================================

float motionBuffer[MOTION_BUFFER_SIZE];
int bufferIndex = 0;
float lastMagnitude = 1.0f;  // Start at 1g (resting)
unsigned long lastMotionTime = 0;

// =============================================================================
// BURIAL DETECTION VARIABLES
// =============================================================================

unsigned long darkStillStartTime = 0;
unsigned long warningStartTime = 0;

// =============================================================================
// BUTTON DEBOUNCING
// =============================================================================

unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_MS = 200;

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

void initPins();
bool initSensors();
void updateLEDs();
void readButton();
uint16_t readLight();
bool isInMotion();
void checkBurialConditions();
void triggerSolenoid();
void debugPrint();

// =============================================================================
// SETUP
// =============================================================================

void setup() {
    // Initialize serial for debugging
    Serial.begin(SERIAL_BAUD);
    delay(1000);  // Wait for serial
    
    Serial.println("=====================================");
    Serial.println("  Avalanche Airbag Auto-Trigger");
    Serial.println("  Version 1.0");
    Serial.println("=====================================");
    
    // Initialize pins
    initPins();
    
    // Initialize I2C
    Wire.begin(PIN_SDA, PIN_SCL);
    
    // Initialize sensors
    if (!initSensors()) {
        Serial.println("ERROR: Sensor init failed!");
        // Blink red LED to indicate error
        while (true) {
            digitalWrite(PIN_LED_RED, HIGH);
            delay(200);
            digitalWrite(PIN_LED_RED, LOW);
            delay(200);
        }
    }
    
    // Initialize motion buffer
    for (int i = 0; i < MOTION_BUFFER_SIZE; i++) {
        motionBuffer[i] = 0;
    }
    
    // Start in disarmed state
    currentState = STATE_DISARMED;
    Serial.println("System ready - DISARMED");
    Serial.println("Press button to ARM");
    
    // Set initial LED state
    updateLEDs();
}

// =============================================================================
// MAIN LOOP
// =============================================================================

void loop() {
    static unsigned long lastLoop = 0;
    
    // Run at fixed interval
    if (millis() - lastLoop < LOOP_INTERVAL_MS) {
        return;
    }
    lastLoop = millis();
    
    // Always check button (arm/disarm or cancel)
    readButton();
    
    // Update LEDs based on state
    updateLEDs();
    
    // State-specific logic
    switch (currentState) {
        case STATE_DISARMED:
            // Just waiting for button press to arm
            break;
            
        case STATE_ARMED:
        case STATE_MONITORING:
            // Check for burial conditions
            checkBurialConditions();
            break;
            
        case STATE_WARNING:
            // Continue buzzer, check for timeout or cancel
            if (millis() - warningStartTime > WARNING_DURATION_MS) {
                // Time's up - trigger!
                currentState = STATE_TRIGGERED;
                triggerSolenoid();
            }
            break;
            
        case STATE_TRIGGERED:
            // Transition to deployed
            currentState = STATE_DEPLOYED;
            Serial.println("AIRBAG DEPLOYED");
            break;
            
        case STATE_DEPLOYED:
            // Waiting for manual reset (button press)
            break;
    }
    
    // Debug output
    #if DEBUG_ENABLED
    debugPrint();
    #endif
}

// =============================================================================
// INITIALIZATION FUNCTIONS
// =============================================================================

void initPins() {
    // LEDs as outputs
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    
    // Buzzer as output
    pinMode(PIN_BUZZER, OUTPUT);
    
    // Solenoid as output (start LOW = safe)
    pinMode(PIN_SOLENOID, OUTPUT);
    digitalWrite(PIN_SOLENOID, LOW);
    
    // Button as input with internal pull-up
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // All LEDs off initially
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);
    digitalWrite(PIN_BUZZER, LOW);
}

bool initSensors() {
    bool success = true;
    
    // Initialize BH1750 light sensor
    Serial.print("Initializing BH1750... ");
    if (lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_ADDRESS)) {
        Serial.println("OK");
    } else {
        Serial.println("FAILED");
        success = false;
    }
    
    // Initialize LSM6DS3 IMU
    Serial.print("Initializing LSM6DS3... ");
    if (imu.begin_I2C(LSM6DS3_ADDRESS)) {
        Serial.println("OK");
        
        // Configure accelerometer
        imu.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
        imu.setAccelDataRate(LSM6DS_RATE_104_HZ);
        
        // Configure gyroscope
        imu.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);
        imu.setGyroDataRate(LSM6DS_RATE_104_HZ);
    } else {
        Serial.println("FAILED");
        success = false;
    }
    
    return success;
}

// =============================================================================
// LED AND BUZZER CONTROL
// =============================================================================

void updateLEDs() {
    // Turn off all LEDs first
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    
    switch (currentState) {
        case STATE_DISARMED:
            // Blue LED solid
            digitalWrite(PIN_LED_BLUE, HIGH);
            break;
            
        case STATE_ARMED:
            // Green LED solid
            digitalWrite(PIN_LED_GREEN, HIGH);
            break;
            
        case STATE_MONITORING:
            // Yellow (green + red) solid
            digitalWrite(PIN_LED_GREEN, HIGH);
            digitalWrite(PIN_LED_RED, HIGH);
            break;
            
        case STATE_WARNING:
            // Red LED blinking + buzzer
            if ((millis() / 100) % 2 == 0) {
                digitalWrite(PIN_LED_RED, HIGH);
            }
            digitalWrite(PIN_BUZZER, HIGH);
            break;
            
        case STATE_TRIGGERED:
        case STATE_DEPLOYED:
            // Red LED solid
            digitalWrite(PIN_LED_RED, HIGH);
            break;
    }
}

// =============================================================================
// BUTTON HANDLING
// =============================================================================

void readButton() {
    // Read button (active LOW)
    bool buttonPressed = (digitalRead(PIN_BUTTON) == LOW);
    
    // Debounce
    if (buttonPressed && (millis() - lastButtonPress > DEBOUNCE_MS)) {
        lastButtonPress = millis();
        
        switch (currentState) {
            case STATE_DISARMED:
                // Arm the system
                currentState = STATE_ARMED;
                lastMotionTime = millis();  // Reset motion history
                Serial.println(">>> SYSTEM ARMED <<<");
                break;
                
            case STATE_ARMED:
            case STATE_MONITORING:
                // Disarm the system
                currentState = STATE_DISARMED;
                Serial.println(">>> SYSTEM DISARMED <<<");
                break;
                
            case STATE_WARNING:
                // Cancel trigger!
                currentState = STATE_ARMED;
                Serial.println(">>> TRIGGER CANCELLED <<<");
                break;
                
            case STATE_DEPLOYED:
                // Reset to disarmed
                currentState = STATE_DISARMED;
                Serial.println(">>> SYSTEM RESET <<<");
                break;
                
            default:
                break;
        }
    }
}

// =============================================================================
// SENSOR READING
// =============================================================================

uint16_t readLight() {
    float lux = lightSensor.readLightLevel();
    if (lux < 0) {
        // Error reading sensor
        return 0xFFFF;
    }
    return (uint16_t)lux;
}

bool isInMotion() {
    // Read accelerometer
    sensors_event_t accel, gyro, temp;
    imu.getEvent(&accel, &gyro, &temp);
    
    // Calculate acceleration magnitude
    float ax = accel.acceleration.x;
    float ay = accel.acceleration.y;
    float az = accel.acceleration.z;
    float magnitude = sqrt(ax*ax + ay*ay + az*az);
    
    // Calculate change from last reading
    float delta = abs(magnitude - lastMagnitude);
    lastMagnitude = magnitude;
    
    // Update circular buffer
    motionBuffer[bufferIndex] = delta;
    bufferIndex = (bufferIndex + 1) % MOTION_BUFFER_SIZE;
    
    // Calculate average motion
    float avgMotion = 0;
    for (int i = 0; i < MOTION_BUFFER_SIZE; i++) {
        avgMotion += motionBuffer[i];
    }
    avgMotion /= MOTION_BUFFER_SIZE;
    
    // Return true if motion exceeds threshold
    return (avgMotion > MOTION_THRESHOLD);
}

// =============================================================================
// BURIAL DETECTION LOGIC
// =============================================================================

void checkBurialConditions() {
    uint16_t lux = readLight();
    bool moving = isInMotion();
    
    // Update motion history
    if (moving) {
        lastMotionTime = millis();
    }
    
    // Check conditions
    bool isDark = (lux < BURIAL_LUX_THRESHOLD);
    bool wasRecentlyMoving = (millis() - lastMotionTime < MOTION_HISTORY_MS);
    bool isStill = !moving;
    
    // Evaluate burial state
    if (isDark) {
        // Dark detected - enter monitoring if not already
        if (currentState == STATE_ARMED) {
            currentState = STATE_MONITORING;
            Serial.println("Dark detected - monitoring");
        }
        
        // Check all conditions for trigger
        if (wasRecentlyMoving && isStill) {
            // Start timing stillness
            if (darkStillStartTime == 0) {
                darkStillStartTime = millis();
                Serial.println("Stillness timer started");
            }
            
            // Check if still long enough
            if (millis() - darkStillStartTime > STILLNESS_THRESHOLD_MS) {
                // Enter warning state
                currentState = STATE_WARNING;
                warningStartTime = millis();
                Serial.println("!!! WARNING - TRIGGER IN 2 SECONDS !!!");
            }
        } else {
            // Conditions not met - reset timer
            darkStillStartTime = 0;
        }
    } else {
        // Light detected - back to armed
        if (currentState == STATE_MONITORING) {
            currentState = STATE_ARMED;
            Serial.println("Light OK - back to armed");
        }
        darkStillStartTime = 0;
    }
}

// =============================================================================
// SOLENOID CONTROL
// =============================================================================

void triggerSolenoid() {
    Serial.println("!!! TRIGGERING SOLENOID !!!");
    
    // Pulse the solenoid
    digitalWrite(PIN_SOLENOID, HIGH);
    delay(SOLENOID_PULSE_MS);
    digitalWrite(PIN_SOLENOID, LOW);
    
    Serial.println("Solenoid pulse complete");
}

// =============================================================================
// DEBUG OUTPUT
// =============================================================================

void debugPrint() {
    static unsigned long lastDebug = 0;
    
    // Print every 500ms
    if (millis() - lastDebug < 500) {
        return;
    }
    lastDebug = millis();
    
    uint16_t lux = readLight();
    
    // State name
    const char* stateName;
    switch (currentState) {
        case STATE_DISARMED:  stateName = "DISARMED"; break;
        case STATE_ARMED:     stateName = "ARMED"; break;
        case STATE_MONITORING: stateName = "MONITORING"; break;
        case STATE_WARNING:   stateName = "WARNING"; break;
        case STATE_TRIGGERED: stateName = "TRIGGERED"; break;
        case STATE_DEPLOYED:  stateName = "DEPLOYED"; break;
        default:              stateName = "UNKNOWN"; break;
    }
    
    Serial.print("State: ");
    Serial.print(stateName);
    Serial.print(" | Lux: ");
    Serial.print(lux);
    Serial.print(" | Motion: ");
    Serial.print(isInMotion() ? "YES" : "no");
    Serial.print(" | LastMotion: ");
    Serial.print((millis() - lastMotionTime) / 1000);
    Serial.println("s ago");
}
