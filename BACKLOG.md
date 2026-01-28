# Avalanche Trigger - Improvement Backlog

**Last Updated**: 2026-01-28
**Status**: Phase 0 (70% complete) → Phase 1 (Reliability improvements)

---

## 📋 **Summary**

This document tracks all identified improvements, organized by priority and phase. Use this to guide development, prioritize fixes, and measure progress toward Phase 1 completion.

**Legend:**
- 🔴 **CRITICAL** - Blocks functionality, safety risk, or deployment
- 🟠 **HIGH** - Core reliability/functionality
- 🟡 **MEDIUM** - Improves quality, maintainability
- 🟢 **LOW** - Nice-to-have, Phase 2+
- ✅ **DONE** - Completed
- 🚧 **IN PROGRESS** - Currently being worked on

---

## 🔴 **CRITICAL (Blocking)**

### CRIT-001: Firmware Not Buildable
**Status**: 🚧 IN PROGRESS
**Impact**: Project cannot compile; no executable firmware
**Risk**: Cannot test or deploy
**Effort**: 5 minutes

**Issue:**
- Main implementation in `src/main.cpp.bak` (496 lines)
- No `src/main.cpp` exists for PlatformIO to compile
- GitHub Actions build will fail

**Resolution:**
1. Rename `src/main.cpp.bak` → `src/main.cpp`
2. Verify `pio run` compiles successfully
3. Test that serial output works

**Dependencies**: None
**Phase**: Phase 0 (should have been done)
**Assignee**: In-progress

---

### CRIT-002: Hardware-Software Mismatch
**Status**: 🟡 WAITING (hardware arrival)
**Impact**: Current library config incompatible with actual hardware
**Risk**: Cannot initialize sensors when they arrive
**Effort**: 20 minutes (documentation + code updates)

**Issue:**
- Docs reference "Seeed XIAO ESP32-S3 **Sense**" with onboard IMU
- Actual hardware: Base "XIAO ESP32-S3" (no onboard sensors)
- platformio.ini references `seeed-studio/Seeed Arduino LSM6DS3` (for onboard IMU)
- External sensors (VEML7700, LSM6DSOX) not yet arrived

**Resolution:**
1. When sensors arrive:
   - Run I2C scanner to verify addresses (VEML7700 @ 0x10, LSM6DSOX @ 0x6A)
   - Update platformio.ini dependencies:
     ```ini
     lib_deps =
         adafruit/Adafruit VEML7700 Library@^2.1.6
         adafruit/Adafruit LSM6DS@^4.7.2
     ```
   - Update main.cpp sensor initialization code
   - Test I2C communication

2. Update documentation:
   - Fix ENGINEERING_DOC.md (remove Sense references)
   - Fix AGENTS.md hardware section
   - Update BOM if needed

**Dependencies**: Hardware arrival
**Phase**: Phase 0 (blocking testing)
**Assignee**: Waiting on hardware

---

### CRIT-003: Unknown I2C Device at 0x51
**Status**: 🟡 INVESTIGATE
**Impact**: Unknown device on I2C bus; potential conflicts
**Risk**: May cause I2C communication issues or power drain
**Effort**: 30 minutes

**Issue:**
- I2C scanner found device at address 0x51
- No documentation of what this device is
- Not in BOM

**Resolution:**
1. Physically inspect circuit board
2. Check datasheets for common devices at 0x51:
   - OV2640 camera (if accidentally connected)
   - Battery management IC
   - External real-time clock
3. If device not needed: remove from circuit
4. If device needed: document and integrate into firmware
5. Update I2C_SCANNER notes in main.cpp

**Dependencies**: Hardware inspection
**Phase**: Phase 0 (before deployment)
**Assignee**: Manual inspection

---

## 🟠 **HIGH Priority (Safety & Reliability)**

### HIGH-001: No Error Recovery / Infinite Loops
**Status**: 🟡 PENDING
**Impact**: Safety-critical; solenoid could hang energized
**Risk**: Device cannot recover from sensor failure
**Effort**: 2-3 hours

**Issue:**
```cpp
if (!initSensors()) {
    Serial.println("ERROR: Sensor init failed!");
    while (true) {  // INFINITE LOOP - DANGEROUS
        digitalWrite(PIN_LED_RED, HIGH);
        delay(200);
    }
}
```
- No watchdog timer to recover from hang
- No graceful degradation
- Solenoid output pin stays in current state (potentially energized)

**Resolution:**
1. **Implement watchdog timer:**
   ```cpp
   #include <esp_system.h>
   void enableWatchdog() {
       esp_task_wdt_init(WATCHDOG_TIMEOUT_SEC, true);
       esp_task_wdt_add(NULL);
   }
   void feedWatchdog() {
       esp_task_wdt_reset();
   }
   ```
2. **Safe error states:**
   - Call `disarmSolenoid()` on sensor init failure
   - Blink red LED, sound error tone
   - Go to SAFE_ERROR state (allow button reset)
   - Don't call watchdog feed in error loop (force reset)

3. **Add sensor validation:**
   ```cpp
   bool validateSensorsRuntime() {
       // Test I2C communication every 5 seconds
       // Return false if sensor unresponsive
   }
   ```

**Dependencies**: None
**Phase**: Phase 1 (before field testing)
**Files to modify**:
- `src/main.cpp` - add watchdog setup, error handlers
- `include/config.h` - add WATCHDOG_TIMEOUT_SEC

**Assignee**: Ready to implement

---

### HIGH-002: Missing Sensor Health Checks
**Status**: 🟡 PENDING
**Impact**: Silent sensor failures could cause missed triggers
**Risk**: Safety-critical (avalanche not detected)
**Effort**: 2 hours

**Issue:**
- System assumes sensors work correctly after initialization
- No periodic health checks during operation
- No I2C timeout/error handling
- If sensor stops responding mid-flight, device won't know

**Resolution:**
1. **Add periodic sensor tests in main loop:**
   ```cpp
   #define SENSOR_HEALTH_CHECK_MS 5000
   unsigned long lastHealthCheck = 0;

   if (millis() - lastHealthCheck > SENSOR_HEALTH_CHECK_MS) {
       if (!sensorHealthy()) {
           triggerWarning();  // Alert user
           logEvent("SENSOR_ERROR");
       }
       lastHealthCheck = millis();
   }
   ```

2. **Implement health check function:**
   - Read light sensor once, verify data is in reasonable range
   - Read IMU once, verify accelerometer values
   - If I2C error occurs, increment error counter
   - After N consecutive failures, enter error state

3. **Add telemetry:**
   - Log sensor error events to EEPROM (see HIGH-006)
   - Include timestamp, error type, last valid reading

**Dependencies**: HIGH-004 (timing fixes)
**Phase**: Phase 1
**Files to modify**:
- `src/main.cpp` - add health check function and calls
- `include/config.h` - add SENSOR_HEALTH_CHECK_MS, MAX_SENSOR_ERRORS

**Assignee**: Ready to implement

---

### HIGH-003: Timer Rollover Logic (48-day bug)
**Status**: 🟡 PENDING
**Impact**: Motion detection logic breaks after ~49 days of uptime
**Risk**: Could fail to trigger if avalanche occurs after 49 days running
**Effort**: 1-2 hours

**Issue:**
```cpp
// UNSAFE: If millis() overflows, this comparison breaks
if (millis() - lastMotionTime < MOTION_HISTORY_MS) {
    wasMovingRecently = true;
}
```
- `millis()` wraps to 0 after ~49.7 days
- Subtraction will give wrong result on overflow
- Motion history detection will fail

**Example Bug Scenario:**
```
At Day 49:
  lastMotionTime = 4,290,000,000 ms
  millis() = 4,294,900,000 ms (near overflow)
  Difference = 4,900,000 ms ✓ (correct)

At Day 50 (after overflow):
  lastMotionTime = 4,290,000,000 ms (old, pre-wrap value)
  millis() = 100,000 ms (wrapped, post-reset)
  Difference = 100,000 - 4,290,000,000 = NEGATIVE! ✗ (wraps in signed math)
  Comparison fails, wasMovingRecently = false
```

**Resolution:**
1. **Use safe timer comparison:**
   ```cpp
   bool isWithinTimeWindow(unsigned long startTime, unsigned long windowMs) {
       // Handles wraparound correctly
       return (millis() - startTime) < windowMs;
   }
   ```
   - This works because Arduino uses unsigned math for millis()
   - Subtraction handles overflow safely

2. **Audit all time comparisons:**
   - Search for all `millis() - lastXXX` patterns
   - Replace with safe comparisons
   - Document timing assumptions

3. **Add test case:**
   - Simulate overflow: set millis to 4,294,900,000, verify logic
   - Or create unit test with mock clock

**Dependencies**: None
**Phase**: Phase 1 (before long-term field testing)
**Files to modify**:
- `src/main.cpp` - audit and fix timing comparisons
- `src/tests/` - add overflow test case

**Assignee**: Ready to implement

---

### HIGH-004: Motion Detection Sensor Issues
**Status**: 🟡 PENDING
**Impact**: Could trigger false positives or miss real avalanches
**Risk**: Safety-critical; core trigger logic
**Effort**: 4-6 hours (requires tuning and testing)

**Issue:**
1. **No gravity compensation:**
   - Accelerometer at rest reads ~9.81 m/s² (gravity)
   - Current code probably doesn't account for this
   - May need to subtract gravity vector before thresholding

2. **No sensor filtering:**
   - Raw accelerometer data is noisy
   - Single samples can spike above threshold (false motion detection)
   - Need high-pass filter or moving average

3. **Single fixed threshold:**
   - 0.3g threshold may not work in all snow conditions
   - Snow texture affects vibration response
   - Should calibrate per-user or per-region

4. **LSM6DSOX not yet available:**
   - Cannot test motion detection until hardware arrives
   - Sensor specs needed: range (±2g to ±16g), noise floor (~4 mg)

**Resolution:**
1. **When sensors arrive:**
   - Run motion profile tests (ski down slope, document acceleration ranges)
   - Measure actual avalanche data (if available) or use published research
   - Determine safe threshold that triggers on real avalanche but not normal skiing

2. **Implement filtering:**
   ```cpp
   const int FILTER_SAMPLES = 10;
   float accelBuffer[FILTER_SAMPLES];
   int bufferIndex = 0;

   float readFilteredAccel() {
       accelBuffer[bufferIndex] = readRawAccel();
       bufferIndex = (bufferIndex + 1) % FILTER_SAMPLES;

       float sum = 0;
       for (int i = 0; i < FILTER_SAMPLES; i++) {
           sum += accelBuffer[i];
       }
       return sum / FILTER_SAMPLES;
   }
   ```

3. **Add gravity compensation:**
   - Read resting acceleration to measure gravity vector
   - Subtract gravity from all motion readings
   - Improves detection accuracy in all orientations

4. **Add calibration procedure:**
   - On startup: measure resting acceleration for 1 second
   - Use as baseline for gravity
   - Store in EEPROM for persistence

5. **Make threshold tunable:**
   ```cpp
   // in config.h
   #define MOTION_THRESHOLD_G 0.3f    // Easy to adjust without recompiling
   #define MOTION_FILTER_SAMPLES 10
   ```

**Dependencies**: LSM6DSOX hardware arrival
**Phase**: Phase 1 (field testing)
**Files to modify**:
- `src/main.cpp` - add filtering, gravity compensation, calibration
- `include/config.h` - add tuneable parameters
- `docs/` - add calibration procedure

**Assignee**: Ready after hardware arrives

---

## 🟡 **MEDIUM Priority (Quality & Maintainability)**

### MED-001: No Test Framework
**Status**: 🟡 PENDING
**Impact**: Difficult to validate changes; no regression testing
**Risk**: Easy to introduce bugs
**Effort**: 3-4 hours (initial setup + first tests)

**Issue:**
- Only minimal serial test exists (`src/tests/serial_test.cpp`)
- No integration with build system
- No test runner
- Cannot validate state machine transitions

**Resolution:**
1. **Set up Arduino Test Library:**
   ```ini
   # platformio.ini
   [env:testing]
   platform = espressif32
   board = seeed_xiao_esp32s3
   framework = arduino
   lib_deps =
       Test-CI/Arduino-CI-Patch
       adafruit/Adafruit VEML7700 Library@^2.1.6
       adafruit/Adafruit LSM6DS@^4.7.2
   test_framework = catch2
   ```

2. **Create test suite structure:**
   ```
   test/
   ├── test_state_machine.cpp    (state transitions, edge cases)
   ├── test_motion_detection.cpp  (acceleration filtering, thresholds)
   ├── test_light_detection.cpp   (light level changes)
   ├── test_timing.cpp            (timer logic, edge cases)
   └── test_solenoid_safety.cpp   (output safety constraints)
   ```

3. **Write core tests:**
   - State transitions (DISARMED → ARMED → MONITORING, etc.)
   - Boundary conditions (light exactly at 50 lux)
   - Timing edge cases (rollover, 500ms stillness)
   - Safety invariants (solenoid starts LOW)

4. **Add to CI/CD:**
   ```yaml
   # .github/workflows/build.yml
   - name: Run tests
     run: pio test
   ```

**Dependencies**: None (can use mocks)
**Phase**: Phase 1
**Files to create**:
- `test/test_*.cpp` files
- Update `platformio.ini`
- Update GitHub Actions workflow

**Assignee**: Ready to implement

---

### MED-002: No Logging / Telemetry
**Status**: 🟡 PENDING
**Impact**: Cannot diagnose failures after deployment
**Risk**: False trigger incidents hard to analyze
**Effort**: 3 hours

**Issue:**
- Debug output only via serial console
- No persistent event log
- If device is deployed, no record of what triggered it
- Crashes/hangs leave no trace

**Resolution:**
1. **Implement EEPROM event log:**
   ```cpp
   struct LogEntry {
       uint32_t timestamp;      // millis() when event occurred
       uint8_t eventType;       // ARMED, MOTION_DETECTED, TRIGGER, etc.
       float data1, data2;      // e.g., light level, accel magnitude
   };

   const int LOG_ENTRIES_MAX = 100;
   void logEvent(uint8_t type, float data1 = 0, float data2 = 0) {
       // Write to EEPROM in circular buffer
   }
   ```

2. **Define event types:**
   ```cpp
   #define LOG_EVENT_ARMED          1
   #define LOG_EVENT_DISARMED       2
   #define LOG_EVENT_MOTION_START   3
   #define LOG_EVENT_MOTION_END     4
   #define LOG_EVENT_DARK_DETECTED  5
   #define LOG_EVENT_WARNING        6
   #define LOG_EVENT_TRIGGERED      7
   #define LOG_EVENT_SENSOR_ERROR   8
   ```

3. **Add telemetry transmission:**
   - On button press: dump event log to serial
   - Or: send log via WiFi (future enhancement)
   - Include: timestamps, sensor readings, state transitions

4. **Use for debugging:**
   - False trigger analysis: review event log to see what caused it
   - Missed trigger analysis: replay sensor data
   - Performance tuning: analyze motion detection sensitivity

**Dependencies**: None
**Phase**: Phase 1
**Files to modify**:
- `src/main.cpp` - add logEvent() calls throughout
- `include/config.h` - add LOG_EVENT_* constants, buffer size

**Assignee**: Ready to implement

---

### MED-003: Power Management Missing
**Status**: 🟡 PENDING
**Impact**: Cannot monitor battery health; risk of over-discharge
**Risk**: LiPo damage, unexpected power loss
**Effort**: 2 hours

**Issue:**
- No battery voltage monitoring
- No low-battery warning
- Could discharge LiPo below 3.0V (damages cell)
- No current limiting on solenoid draw

**Resolution:**
1. **Add ADC battery monitoring:**
   ```cpp
   const int BATTERY_ADC_PIN = A0;  // GPIO pin with voltage divider
   const float MIN_SAFE_VOLTAGE = 3.2f;  // Volts
   const float MAX_VOLTAGE = 4.2f;

   float getBatteryVoltage() {
       int raw = analogRead(BATTERY_ADC_PIN);
       // Convert ADC reading to voltage (depends on voltage divider)
       return (raw / 4095.0) * 3.3 * (47 + 100) / 100;  // Adjust resistors as needed
   }
   ```

2. **Add low-battery checks:**
   - In main loop: check battery every 10 seconds
   - If battery < 3.2V: disable arming, display warning
   - Log battery voltage to EEPROM periodically

3. **Add current limiting:**
   - Measure current draw during solenoid operation
   - If exceeds 2A: shutdown solenoid, log error
   - Prevents overcurrent damage

4. **Estimate battery life:**
   - Measure: quiescent current (armed, monitoring)
   - Measure: peak current (solenoid firing)
   - Calculate: hours of operation before shutdown

**Dependencies**: None (but requires circuit modification for voltage divider)
**Phase**: Phase 1
**Files to modify**:
- `src/main.cpp` - add battery monitoring calls
- `include/config.h` - add battery thresholds
- Schematic: add voltage divider to ADC pin

**Assignee**: Ready to implement

---

### MED-004: Documentation Cleanup
**Status**: 🟡 PENDING
**Impact**: Confusion about hardware, outdated BOM
**Risk**: User purchases wrong components
**Effort**: 1 hour

**Issue:**
- Multiple docs reference "ESP32-S3 Sense" variant
- Actual hardware is base "ESP32-S3"
- BOM in `.ai/NEXT.md` lists old components (C3, Grove)
- I2C device addresses not documented

**Resolution:**
1. **Update ENGINEERING_DOC.md:**
   - Remove "Sense" references
   - Clarify: "Seeed XIAO ESP32-S3 (base variant, no onboard IMU)"
   - Update BOM with actual sensors (VEML7700, LSM6DSOX)
   - Add I2C address table

2. **Update AGENTS.md:**
   - Fix hardware section
   - Add note about external sensors

3. **Update `.ai/NEXT.md`:**
   - Remove old shopping list
   - Add actual components ordered

4. **Add to ENGINEERING_DOC.md:**
   - I2C device address table
   - Pin assignment diagram
   - Schematic notes

**Dependencies**: None
**Phase**: Phase 0 (should be done)
**Files to modify**:
- `docs/ENGINEERING_DOC.md`
- `docs/AGENTS.md`
- `.ai/NEXT.md`

**Assignee**: Ready to implement

---

## 🟢 **LOW Priority (Phase 2+)**

### LOW-001: No State Persistence
**Status**: 🟢 LOW
**Impact**: On power loss, device resets to DISARMED
**Risk**: Low (user would notice and re-arm)
**Effort**: 1 hour

**Issue:**
- On restart, device goes to DISARMED state
- If buried and power is restored, device won't auto-arm
- May miss rescue window

**Resolution:**
- Add EEPROM boot flag: if device powered during motion, go to ARMED state
- Or: add user preference in config: "auto-arm on boot"

**Phase**: Phase 2

---

### LOW-002: Code Modularization
**Status**: 🟢 LOW
**Impact**: Maintainability, code reuse
**Risk**: Low (Phase 0 simplicity acceptable)
**Effort**: 4-6 hours

**Issue:**
- All code in single `main.cpp` file (496 lines)
- Difficult to test individual components
- Hard to reuse sensor code in other projects

**Resolution:**
- Separate into modules: `sensor_manager.cpp`, `state_machine.cpp`, `output_controller.cpp`
- Create header files with clear interfaces
- Easier to test, maintain, and extend

**Phase**: Phase 2+

---

### LOW-003: User Interface / Display
**Status**: 🟢 LOW
**Impact**: Better user feedback
**Risk**: Low
**Effort**: 2-3 hours

**Issue:**
- Only RGB LED indicators (limited feedback)
- No way to view state without serial connection
- No menu/configuration interface

**Resolution:**
- Add I2C OLED display (0x3C address already detected!)
- Show: current state, light level, motion status, battery voltage
- Add button menu for calibration, testing

**Phase**: Phase 2+

---

## 📊 **Implementation Roadmap**

### **Phase 1 (Reliability) - Target: 2-3 weeks**
```
Week 1:
- Fix CRIT-001 (firmware buildable) ✓
- Fix CRIT-002 (hardware docs) when sensors arrive
- Implement HIGH-001 (watchdog, error recovery)
- Implement HIGH-003 (timer fixes)

Week 2:
- Implement HIGH-002 (sensor health checks)
- Implement HIGH-004 (motion detection tuning)
- Implement MED-001 (test framework)

Week 3:
- Implement MED-002 (logging/telemetry)
- Implement MED-003 (power management)
- Field testing with real avalanche scenarios
```

### **Phase 2 (Usability) - Target: 3-4 weeks**
```
- Implement LOW-001 (state persistence)
- Implement LOW-003 (OLED display)
- Performance optimization
- User manual and safety documentation
```

### **Phase 3+ (Advanced)**
```
- Sensor fusion (combine multiple data sources)
- Machine learning (avalanche signature detection)
- WiFi connectivity and cloud logging
- Smartphone app integration
```

---

## 🎯 **Success Criteria**

| Criterion | Phase 0 | Phase 1 | Status |
|-----------|---------|---------|--------|
| Firmware builds | ✓ | ✓ | 🟡 Broken |
| Tests exist | ✗ | ✓ | ✗ Missing |
| Watchdog timer | ✗ | ✓ | ✗ Missing |
| Sensor health checks | ✗ | ✓ | ✗ Missing |
| Event logging | ✗ | ✓ | ✗ Missing |
| Field tested | ✗ | ✓ | 🟡 Pending hardware |
| Documentation current | ✓ | ✓ | 🟡 Outdated |
| No infinite loops | ✗ | ✓ | ✗ Present |

---

## 📝 **Notes**

- All effort estimates assume uninterrupted work by experienced developer
- Actual times may vary based on hardware availability and testing needs
- Phase 1 is prerequisite for field testing and deployment
- Critical items are safety-related and must be completed before use
- Use this document as living guide; update as work progresses

---

**Next Steps:**
1. Rename `main.cpp.bak` to `main.cpp` and verify build (TODAY)
2. When sensors arrive: test I2C communication (URGENT)
3. Implement watchdog timer (THIS WEEK)
4. Set up test framework (THIS WEEK)
5. Field testing once hardware is ready (NEXT PHASE)
