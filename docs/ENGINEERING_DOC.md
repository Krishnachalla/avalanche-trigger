# Engineering Documentation
## Automatic Avalanche Airbag Trigger

**Document Version:** 1.1  
**Project Status:** Phase 0 - "Does it work at all?"  
**Last Updated:** January 2025

> **Note:** This document provides technical specifications. For project philosophy,
> phases, and contributor rules, see `PROJECT_MANIFESTO.md`.

---

## Table of Contents

1. [System Overview](#1-system-overview)
2. [Requirements](#2-requirements)
3. [Hardware Design](#3-hardware-design)
4. [Software Architecture](#4-software-architecture)
5. [Detection Algorithm](#5-detection-algorithm)
6. [Safety Analysis](#6-safety-analysis)
7. [Testing Protocol](#7-testing-protocol)
8. [Bill of Materials](#8-bill-of-materials)
9. [Assembly Instructions](#9-assembly-instructions)
10. [Calibration Procedure](#10-calibration-procedure)
11. [Known Limitations](#11-known-limitations)
12. [Revision History](#12-revision-history)

---

## 1. System Overview

### 1.1 Purpose
This device automatically triggers an avalanche airbag when it detects that the user has been buried in snow and is incapacitated (unable to manually trigger the airbag).

### 1.2 Operating Principle
1. Light sensor detects sudden darkness (snow burial blocks light)
2. IMU confirms recent motion (skiing/tumbling) followed by stillness (incapacitation)
3. Warning buzzer alerts user before auto-deployment
4. If no manual cancel, solenoid activates pilot valve
5. Pilot valve releases CO2 to inflate airbag

### 1.3 Block Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                         SENSOR SUBSYSTEM                            │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐          │
│  │   BH1750     │    │   LSM6DS3    │    │   Button     │          │
│  │ Light Sensor │    │     IMU      │    │   (ARM)      │          │
│  └──────┬───────┘    └──────┬───────┘    └──────┬───────┘          │
│         │ I2C               │ I2C               │ GPIO             │
└─────────┼───────────────────┼───────────────────┼──────────────────┘
          │                   │                   │
          ▼                   ▼                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                     PROCESSING SUBSYSTEM                            │
│                    ┌────────────────────┐                           │
│                    │   XIAO ESP32-C3    │                           │
│                    │                    │                           │
│                    │  - State Machine   │                           │
│                    │  - Motion Analysis │                           │
│                    │  - Light Analysis  │                           │
│                    │  - Timing Logic    │                           │
│                    └─────────┬──────────┘                           │
└──────────────────────────────┼──────────────────────────────────────┘
                               │
          ┌────────────────────┼────────────────────┐
          │                    │                    │
          ▼                    ▼                    ▼
┌──────────────┐    ┌──────────────┐    ┌──────────────────┐
│    Buzzer    │    │   LED RGB    │    │  Solenoid Driver │
│   (Warning)  │    │   (Status)   │    │    (IRLZ44N)     │
└──────────────┘    └──────────────┘    └────────┬─────────┘
                                                 │
                                                 ▼
                                        ┌──────────────────┐
                                        │  Pilot Valve     │
                                        │  (12V Solenoid)  │
                                        └────────┬─────────┘
                                                 │
                                                 ▼
                                        ┌──────────────────┐
                                        │  CO2 Cartridge   │
                                        │  Release         │
                                        └──────────────────┘
```

---

## 2. Requirements

### 2.1 Functional Requirements

| ID | Requirement | Priority | Phase 0 Target |
|----|-------------|----------|----------------|
| FR-01 | Detect burial condition (light < threshold) | Critical | 50 lux |
| FR-02 | Detect user incapacitation (stillness after motion) | Critical | 500ms stillness |
| FR-03 | Provide warning before auto-deployment | Critical | 500ms buzzer |
| FR-04 | Allow manual cancellation during warning | Critical | Button press |
| FR-05 | Activate solenoid to trigger pilot valve | Critical | 300ms pulse |
| FR-06 | Indicate system status via LED | Low | Later phase |
| FR-07 | Allow manual arming/disarming | High | Button |
| FR-08 | Log events for post-incident analysis | Low | Serial debug |

### 2.2 Non-Functional Requirements

| ID | Requirement | Phase 0 Target | Final Target |
|----|-------------|----------------|--------------|
| NFR-01 | Operating temperature | Room temp | -30°C to +10°C |
| NFR-02 | Battery life (armed, standby) | 4-6 hours | 8+ hours |
| NFR-03 | Response time (burial to trigger) | **≤2 seconds** | ≤2 seconds |
| NFR-04 | False positive rate | Not optimized | < 10% |
| NFR-05 | Reliability | Must trigger on burial | No missed triggers |
| NFR-06 | Water resistance | Not addressed | IP65 minimum |
| NFR-07 | Weight | Not addressed | < 100g |
| NFR-08 | Size | Breadboard OK | Fits in pocket |

### 2.3 Constraints

- Budget: Under $100 USD
- Must not interfere with manual trigger operation
- Must be removable for airline travel
- No external antennas (avalanche transceiver interference)

---

## 3. Hardware Design

### 3.1 Component Selection Rationale

#### 3.1.1 MCU: Seeed XIAO ESP32-C3

**Selected over alternatives because:**
- Grove ecosystem compatibility (beginner-friendly)
- Ultra-compact (21x17.5mm)
- Low power modes available
- WiFi for debugging/logging
- 3.3V operation matches sensors
- Good community support

**Specifications:**
- CPU: RISC-V 32-bit, 160MHz
- RAM: 400KB
- Flash: 4MB
- GPIO: 11 pins
- I2C: 1 bus (SDA/SCL)
- Deep sleep current: ~5μA

#### 3.1.2 Light Sensor: BH1750

**Selected over alternatives because:**
- Digital I2C output (no ADC noise issues)
- Wide range: 1-65535 lux
- Built-in 16-bit ADC
- Low power: 120μA active
- Grove module available

**Alternatives considered:**
- VEML7700: Similar, but less common
- Photoresistor + ADC: Analog, requires calibration, noisy
- TSL2591: More expensive, overkill for this application

#### 3.1.3 IMU: LSM6DS3

**Selected over alternatives because:**
- 6-axis (accelerometer + gyroscope) in one package
- Built-in motion detection interrupts
- Low power modes
- Grove module available
- Well-documented Arduino library

**Specifications:**
- Accelerometer: ±2/±4/±8/±16 g
- Gyroscope: ±125/±250/±500/±1000/±2000 dps
- ODR: Up to 6.66 kHz
- Current: 0.9mA (both active)

#### 3.1.4 Solenoid Driver: IRLZ44N

**Selected because:**
- Logic-level gate (Vgs(th) = 1-2V)
- Can be driven directly from 3.3V GPIO
- High current capability (47A continuous)
- Common, inexpensive
- Through-hole for easy prototyping

### 3.2 Schematic

```
                                    +12V (from boost converter)
                                      │
                                      │
                              ┌───────┴───────┐
                              │   Solenoid    │
                              │  Pilot Valve  │
                              │   (12V, ~1A)  │
                              └───────┬───────┘
                                      │
                              ┌───────┴───────┐
                           ┌──┤    1N4007     ├──┐  (Flyback diode)
                           │  │   (Cathode)   │  │
                           │  └───────────────┘  │
                           │                     │
                           │    DRAIN            │
                           └──────┬──────────────┘
                                  │
                            ┌─────┴─────┐
                            │  IRLZ44N  │
                            │   (NMOS)  │
                            └─────┬─────┘
                                  │ GATE
         XIAO GPIO D2 ────────────┼───────────┐
                                  │           │
                              [10kΩ]          │
                                  │           │
                                 GND         GND
                                        (SOURCE)


         I2C BUS (3.3V)
         ══════════════════════════════════════════════════
              │           │           │           │
              │    SDA    │    SCL    │    VCC    │   GND
              │           │           │           │
         ┌────┴────┐ ┌────┴────┐ ┌────┴────┐ ┌────┴────┐
         │         │ │         │ │         │ │         │
    ┌────┴────┐    │ │    ┌────┴────┐      │ │    ┌────┴────┐
    │ BH1750  ├────┴─┴────┤ LSM6DS3 ├──────┴─┴────┤  XIAO   │
    │ (0x23)  │           │ (0x6A)  │              │ ESP32C3 │
    └─────────┘           └─────────┘              └─────────┘


         STATUS INDICATORS
         ═════════════════════════════════════════
         
         XIAO GPIO D3 ────[220Ω]────┬──── LED (Green) ──── GND
                                    │
         XIAO GPIO D6 ────[220Ω]────┼──── LED (Red) ────── GND
                                    │
         XIAO GPIO D7 ────[220Ω]────┴──── LED (Blue) ───── GND


         BUZZER
         ═════════════════════════════════════════
         
         XIAO GPIO D8 ────[100Ω]──── (+) Buzzer (-) ──── GND


         ARM BUTTON
         ═════════════════════════════════════════
         
                              VCC (3.3V)
                                  │
                              [10kΩ]  (Pull-up)
                                  │
         XIAO GPIO D9 ────────────┼──────┐
                                         │
                                    [ Button ]
                                         │
                                        GND
```

### 3.3 Pin Assignment

| XIAO Pin | GPIO | Function | Connected To |
|----------|------|----------|--------------|
| D4 | GPIO6 | I2C SDA | BH1750, LSM6DS3 |
| D5 | GPIO7 | I2C SCL | BH1750, LSM6DS3 |
| D2 | GPIO4 | Solenoid | IRLZ44N Gate |
| D3 | GPIO5 | LED Green | Status LED |
| D6 | GPIO20 | LED Red | Status LED |
| D7 | GPIO21 | LED Blue | Status LED |
| D8 | GPIO8 | Buzzer | Piezo buzzer |
| D9 | GPIO9 | Button | ARM button |
| BAT | - | Power | LiPo battery |
| 3V3 | - | Power | Sensor VCC |
| GND | - | Ground | Common ground |

### 3.4 Power Budget

| Component | Active Current | Sleep Current |
|-----------|---------------|---------------|
| XIAO ESP32-C3 | 80mA | 5μA |
| BH1750 | 120μA | 1μA |
| LSM6DS3 | 900μA | 3μA |
| LEDs (avg) | 5mA | 0 |
| **Total (Active)** | **~86mA** | - |
| **Total (Sleep)** | - | **~9μA** |

With 1000mAh battery:
- Continuous active: ~11.6 hours
- With 10% duty cycle: ~4.8 days
- Deep sleep (armed, waiting): ~4.6 years (theoretical)

---

## 4. Software Architecture

### 4.1 Module Structure

```
src/
├── main.cpp              # Setup, main loop, state machine
├── sensors/
│   ├── light_sensor.h    # BH1750 interface
│   ├── light_sensor.cpp
│   ├── imu.h             # LSM6DS3 interface
│   └── imu.cpp
├── detection/
│   ├── burial_detector.h # Detection algorithm
│   └── burial_detector.cpp
├── actuators/
│   ├── solenoid.h        # Trigger control
│   ├── solenoid.cpp
│   ├── buzzer.h          # Warning sounds
│   ├── buzzer.cpp
│   ├── led_status.h      # Status indication
│   └── led_status.cpp
└── config.h              # All configuration constants
```

### 4.2 State Machine

```
                    ┌─────────────────┐
                    │    DISARMED     │◄─────────────────────────┐
                    │  (LED: Blue)    │                          │
                    └────────┬────────┘                          │
                             │                                   │
                    [ARM button pressed]                         │
                             │                                   │
                             ▼                                   │
                    ┌─────────────────┐                          │
           ┌───────▶│     ARMED       │◄──────────────┐          │
           │        │  (LED: Green)   │               │          │
           │        └────────┬────────┘               │          │
           │                 │                        │          │
           │        [Light < threshold]               │          │
           │                 │                        │          │
           │                 ▼                        │          │
           │        ┌─────────────────┐               │          │
           │        │   MONITORING    │               │          │
           │        │ (LED: Yellow)   │               │          │
           │        └────────┬────────┘               │          │
           │                 │                        │          │
           │    [Light OK]   │  [Dark + Still + Motion History]  │
           │        │        │               │        │          │
           └────────┘        │               │        │          │
                             ▼               │        │          │
                    ┌─────────────────┐      │        │          │
                    │    WARNING      │◄─────┘        │          │
                    │ (LED: Red blink)│               │          │
                    │ (Buzzer ON)     │               │          │
                    └────────┬────────┘               │          │
                             │                        │          │
              [Cancel pressed]  [Timeout 2s]          │          │
                    │               │                 │          │
                    └───────────────┤                 │          │
                                    │                 │          │
                                    ▼                 │          │
                    ┌─────────────────┐               │          │
                    │   TRIGGERED     │               │          │
                    │ (LED: Red solid)│               │          │
                    └────────┬────────┘               │          │
                             │                        │          │
                    [Solenoid pulse]                  │          │
                             │                        │          │
                             ▼                        │          │
                    ┌─────────────────┐               │          │
                    │   DEPLOYED      │───────────────┘          │
                    │  (Airbag open)  │                          │
                    └────────┬────────┘                          │
                             │                                   │
                    [Manual reset]                               │
                             │                                   │
                             └───────────────────────────────────┘
```

### 4.3 Key Algorithms

#### 4.3.1 Motion Detection

```cpp
// Detect if device is currently in motion
bool isInMotion() {
    float ax, ay, az;
    imu.getAcceleration(&ax, &ay, &az);
    
    float magnitude = sqrt(ax*ax + ay*ay + az*az);
    float delta = abs(magnitude - lastMagnitude);
    lastMagnitude = magnitude;
    
    // Update moving average
    motionBuffer[bufferIndex] = delta;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
    
    float avgMotion = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        avgMotion += motionBuffer[i];
    }
    avgMotion /= BUFFER_SIZE;
    
    return avgMotion > MOTION_THRESHOLD;
}
```

#### 4.3.2 Burial Detection

```cpp
// Main detection logic (called every 100ms)
DetectionResult checkBurial() {
    uint16_t lux = lightSensor.readLux();
    bool moving = isInMotion();
    
    // Update motion history
    if (moving) {
        lastMotionTime = millis();
    }
    
    // Check burial conditions
    bool isDark = (lux < BURIAL_LUX_THRESHOLD);
    bool wasRecentlyMoving = (millis() - lastMotionTime < MOTION_HISTORY_MS);
    bool isStill = !moving;
    
    if (isDark && wasRecentlyMoving && isStill) {
        if (darkStillStartTime == 0) {
            darkStillStartTime = millis();
        }
        
        if (millis() - darkStillStartTime > STILLNESS_THRESHOLD_MS) {
            return BURIAL_DETECTED;
        }
        return BURIAL_POSSIBLE;
    }
    
    darkStillStartTime = 0;
    return NO_BURIAL;
}
```

---

## 5. Detection Algorithm

### 5.1 Design Philosophy

The algorithm prioritizes **no missed triggers** over **no false triggers**. In avalanche rescue, a false deployment is inconvenient but a missed deployment can be fatal.

### 5.2 Detection Criteria

All conditions must be true simultaneously:

| Condition | Threshold | Rationale |
|-----------|-----------|-----------|
| Light level | < 10 lux | Snow burial blocks nearly all light |
| Recent motion | Within last 30s | Confirms user was active (skiing) |
| Current stillness | > 3 seconds | Confirms user cannot move (buried) |
| Darkness duration | > 3 seconds | Filters momentary shadows |

### 5.3 False Trigger Prevention

| Scenario | How Prevented |
|----------|---------------|
| Dark lift line | No recent motion (standing still) |
| Night skiing | User is still moving |
| Putting in bag/pocket | No recent motion pattern |
| Tree shadows | Duration too short |
| Tunnel/building | User is still moving |
| Intentional stop in shade | Can cancel during warning |

### 5.4 Threshold Calibration

Initial values (subject to field calibration):

```cpp
#define BURIAL_LUX_THRESHOLD     10      // lux
#define MOTION_THRESHOLD         0.3     // g (acceleration change)
#define STILLNESS_THRESHOLD_MS   3000    // ms
#define MOTION_HISTORY_MS        30000   // ms
#define WARNING_DURATION_MS      2000    // ms
#define SOLENOID_PULSE_MS        200     // ms
```

---

## 6. Safety Analysis

### 6.1 Failure Mode Analysis

| Failure Mode | Effect | Mitigation | Severity |
|--------------|--------|------------|----------|
| Battery dead | No auto-trigger | Low battery warning, manual trigger unaffected | Medium |
| Light sensor fail | False dark reading | Require IMU confirmation | High |
| IMU fail | No motion data | Fall back to light-only with longer delay | High |
| MOSFET fail (short) | Unexpected trigger | Use normally-open solenoid | Critical |
| MOSFET fail (open) | No trigger | Manual trigger unaffected | High |
| MCU crash | Unpredictable | Watchdog timer, fail to safe state | Critical |
| Water ingress | Short circuits | IP65 enclosure, conformal coating | High |
| Cold temperature | Battery capacity drop | Use LiPo rated for cold, insulate | Medium |

### 6.2 Safety Design Decisions

1. **Fail-Safe Solenoid:** Normally-open valve, energize to trigger
   - Power loss = no trigger (safe)
   - MCU crash = no trigger (safe)
   
2. **Manual Trigger Independence:** Electronic system is parallel to manual
   - Electronic failure doesn't affect manual operation
   
3. **Arming Required:** Must explicitly arm system
   - Prevents accidental triggers during handling
   
4. **Warning Period:** 2 seconds of buzzer before trigger
   - User can cancel false positives
   
5. **Motion Requirement:** Must have recent motion history
   - Prevents trigger from static conditions

---

## 7. Testing Protocol

### 7.1 Unit Tests

| Test ID | Component | Test | Pass Criteria |
|---------|-----------|------|---------------|
| UT-01 | Light Sensor | Read in daylight | 1000-50000 lux |
| UT-02 | Light Sensor | Read in complete darkness | < 5 lux |
| UT-03 | IMU | Read at rest | ~1g on Z axis |
| UT-04 | IMU | Detect shake | Motion flag true |
| UT-05 | Solenoid Driver | Toggle GPIO | LED indicator works |
| UT-06 | Buzzer | Play tone | Audible sound |
| UT-07 | Button | Press detection | State change |

### 7.2 Integration Tests

| Test ID | Test | Procedure | Pass Criteria |
|---------|------|-----------|---------------|
| IT-01 | State machine | Walk through all states | Correct transitions |
| IT-02 | Dark detection | Cover sensor | Enters MONITORING |
| IT-03 | Motion then still | Shake then hold still in dark | Enters WARNING |
| IT-04 | Cancel | Press button during WARNING | Returns to ARMED |
| IT-05 | Full trigger | Complete burial simulation | Solenoid fires |

### 7.3 Environmental Tests

| Test ID | Test | Conditions | Pass Criteria |
|---------|------|------------|---------------|
| ET-01 | Cold operation | -20°C freezer | Functions normally |
| ET-02 | Vibration | Shake vigorously | No false triggers |
| ET-03 | Water resistance | Spray with water | No malfunction |

### 7.4 Burial Simulation Test

**Setup:**
1. Create dark box (cardboard, covered with black cloth)
2. Attach device to pole or rope

**Procedure:**
1. Arm device in daylight
2. Shake device (simulating skiing/tumbling)
3. Lower into dark box while still shaking
4. Stop movement, hold still
5. Verify WARNING state activates after 3s
6. Verify TRIGGERED state after 2s more

**Pass Criteria:**
- Total time from insertion to trigger: 5-7 seconds
- Consistent across 10 trials

---

## 8. Bill of Materials

| Item | Part Number | Qty | Unit Price | Total | Source |
|------|-------------|-----|------------|-------|--------|
| XIAO ESP32-C3 | 113991054 | 1 | $4.99 | $4.99 | Seeed |
| Grove BH1750 | 101020132 | 1 | $5.90 | $5.90 | Seeed |
| Grove LSM6DS3 | 105020012 | 1 | $11.90 | $11.90 | Seeed |
| Grove Cable 20cm | 110990027 | 2 | $2.90 | $5.80 | Seeed |
| IRLZ44N MOSFET | IRLZ44NPBF | 2 | $1.50 | $3.00 | Amazon |
| 1N4007 Diode | 1N4007 | 5 | $0.10 | $0.50 | Amazon |
| 10kΩ Resistor | - | 5 | $0.05 | $0.25 | Amazon |
| 220Ω Resistor | - | 5 | $0.05 | $0.25 | Amazon |
| RGB LED | - | 1 | $0.50 | $0.50 | Amazon |
| Piezo Buzzer | - | 1 | $1.00 | $1.00 | Amazon |
| Tactile Button | - | 2 | $0.25 | $0.50 | Amazon |
| LiPo 1000mAh | - | 1 | $8.00 | $8.00 | Amazon |
| MT3608 Boost | - | 1 | $2.00 | $2.00 | Amazon |
| Breadboard | - | 1 | $5.00 | $5.00 | Amazon |
| Jumper Wires | - | 1 | $4.00 | $4.00 | Amazon |
| Enclosure | - | 1 | $8.00 | $8.00 | Amazon |
| **TOTAL** | | | | **$61.59** | |

---

## 9. Assembly Instructions

### 9.1 Breadboard Prototype

**Step 1: Power Rails**
- Connect XIAO 3V3 to breadboard + rail
- Connect XIAO GND to breadboard - rail

**Step 2: I2C Bus**
- Connect BH1750 SDA to XIAO D4
- Connect BH1750 SCL to XIAO D5
- Connect BH1750 VCC to + rail
- Connect BH1750 GND to - rail
- Repeat for LSM6DS3

**Step 3: Solenoid Driver**
- Place IRLZ44N on breadboard
- Connect Gate to XIAO D2
- Connect 10kΩ from Gate to GND
- Connect Drain to solenoid negative
- Connect Source to GND
- Connect solenoid positive to 12V
- Connect 1N4007 across solenoid (cathode to +)

**Step 4: Status LED**
- Connect LED common cathode to GND
- Connect R through 220Ω to D6
- Connect G through 220Ω to D3
- Connect B through 220Ω to D7

**Step 5: Buzzer**
- Connect buzzer + to D8 through 100Ω
- Connect buzzer - to GND

**Step 6: ARM Button**
- Connect 10kΩ from D9 to 3V3 (pull-up)
- Connect button from D9 to GND

---

## 10. Calibration Procedure

### 10.1 Light Threshold Calibration

1. Take device outdoors on overcast day
2. Read lux value (should be 1000-10000)
3. Place device 5cm under packed snow
4. Read lux value (should be < 50)
5. Place device 15cm under packed snow
6. Read lux value (should be < 10)
7. Set BURIAL_LUX_THRESHOLD to value that reliably indicates 10cm+ burial

### 10.2 Motion Threshold Calibration

1. Hold device completely still
2. Record acceleration magnitude variance (should be < 0.05g)
3. Simulate gentle skiing motion
4. Record acceleration magnitude variance (should be > 0.2g)
5. Set MOTION_THRESHOLD between these values

### 10.3 Timing Calibration

1. Simulate burial scenario multiple times
2. Measure time from burial to incapacitation (realistic: 2-10s)
3. Set STILLNESS_THRESHOLD_MS to balance speed vs false positives
4. Recommended starting point: 3000ms

---

## 11. Known Limitations

1. **Light sensor placement:** Must be exposed to ambient light; if covered by clothing, will fail to detect burial correctly.

2. **Deep powder vs. dense snow:** Light penetration varies significantly; calibration needed for local conditions.

3. **Night operation:** Light-based detection doesn't work at night. Future version could use pressure sensor.

4. **Battery cold sensitivity:** LiPo capacity drops significantly below -10°C. Consider LiFePO4 for extreme cold.

5. **Solenoid current spike:** 12V solenoid draws 1-2A briefly. Ensure boost converter can handle peak current.

6. **Not certified safety equipment:** This is a learning project, not certified for life-safety use.

---

## 12. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | Jan 2025 | Embedder | Initial document |

---

*End of Engineering Documentation*
