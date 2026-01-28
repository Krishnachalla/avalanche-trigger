/**
 * config.h - Configuration constants for Avalanche Trigger
 * 
 * This file contains all tunable parameters. Modify these values
 * during calibration to adjust detection sensitivity.
 */

#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// PIN DEFINITIONS (XIAO ESP32-C3)
// =============================================================================

// I2C Bus (shared by BH1750 and LSM6DS3)
#define PIN_SDA         6   // D4 on XIAO
#define PIN_SCL         7   // D5 on XIAO

// Solenoid driver (MOSFET gate)
#define PIN_SOLENOID    4   // D2 on XIAO

// Status LEDs (active HIGH)
#define PIN_LED_GREEN   5   // D3 on XIAO
#define PIN_LED_RED     20  // D6 on XIAO
#define PIN_LED_BLUE    21  // D7 on XIAO

// Buzzer (active HIGH)
#define PIN_BUZZER      8   // D8 on XIAO

// ARM button (active LOW with pull-up)
#define PIN_BUTTON      9   // D9 on XIAO

// =============================================================================
// DETECTION THRESHOLDS
// =============================================================================

// Light threshold for burial detection (lux)
// Below this = considered dark/buried
// Phase 0: Start high (50 lux) - will false trigger in shadows, that's OK
// Real snow calibration will refine this value
// Reference: daylight=10000+, overcast=1000+, indoor=100-500, under snow<10
#define BURIAL_LUX_THRESHOLD    50

// Motion detection threshold (g)
// Acceleration change above this = moving
// Typical values: 0.2-0.5 g
#define MOTION_THRESHOLD        0.3f

// =============================================================================
// TIMING PARAMETERS (Phase 0 - Optimized for ≤2 second total trigger time)
// =============================================================================
// 
// Timeline from burial to trigger:
//   [Burial] → Light drops (instant)
//            → Stillness confirmed (500ms)
//            → Warning buzzer (500ms)
//            → TRIGGER (~1.0-1.5s total)
//

// How long must the device be still before considering user incapacitated (ms)
// Phase 0: Short (500ms) - prioritize speed over false-trigger prevention
#define STILLNESS_THRESHOLD_MS  500

// How far back to look for motion history (ms)
// Must have moved within this time window to trigger
// Phase 0: 60 seconds - generous window
#define MOTION_HISTORY_MS       60000

// Warning buzzer duration before auto-trigger (ms)
// Phase 0: Short (500ms) - prioritize speed, user can still cancel
#define WARNING_DURATION_MS     500

// Solenoid pulse duration (ms)
// How long to energize the solenoid to trigger valve
// 300ms should actuate most pilot valves
#define SOLENOID_PULSE_MS       300

// Main loop interval (ms)
// How often to check sensors
#define LOOP_INTERVAL_MS        100

// =============================================================================
// MOTION DETECTION BUFFER
// =============================================================================

// Number of samples for motion averaging
// Higher = smoother but slower response
#define MOTION_BUFFER_SIZE      10

// =============================================================================
// I2C ADDRESSES
// =============================================================================

#define BH1750_ADDRESS          0x23    // or 0x5C if ADDR pin is HIGH
#define LSM6DS3_ADDRESS         0x6A    // or 0x6B if SA0 pin is HIGH

// =============================================================================
// DEBUG SETTINGS
// =============================================================================

// Set to 1 to enable debug output to Serial
#define DEBUG_ENABLED           1

// Serial baud rate
#define SERIAL_BAUD             115200

#endif // CONFIG_H
