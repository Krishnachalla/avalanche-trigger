# Avalanche Airbag Auto-Trigger

## Project Summary
Safety-critical embedded device that automatically deploys an avalanche airbag when a skier is buried and incapacitated. Detects burial via light sensor, confirms incapacitation via IMU stillness, then triggers solenoid to deploy airbag.

## Hardware
- **MCU:** Seeed XIAO ESP32-C3 (RISC-V, WiFi/BLE, USB-C)
- **Light Sensor:** Grove BH1750 (I2C, addr 0x23)
- **IMU:** Grove LSM6DS3 (I2C, addr 0x6A)
- **Output:** IRLZ44N MOSFET driving 12V solenoid
- **Power:** 3.7V LiPo + MT3608 boost converter

## Detection Algorithm
```
ARMED → Light < 50 lux (buried)
      → Had motion in last 60s (was skiing)
      → Still for 500ms (incapacitated)
      → 500ms warning buzzer (can cancel)
      → TRIGGER solenoid
```

## Build Commands
```bash
pio run                    # Build
pio run --target upload    # Flash
pio device monitor         # Serial monitor (115200 baud)
```

## Project Structure
```
├── src/main.cpp           # Main firmware, state machine
├── include/config.h       # All tunable parameters
├── platformio.ini         # Build config, dependencies
├── docs/
│   ├── PROJECT_MANIFESTO.md   # Philosophy, phased approach
│   └── ENGINEERING_DOC.md     # Full specs, BOM, circuits
```

## Key Configuration (include/config.h)
| Parameter | Value | Purpose |
|-----------|-------|---------|
| BURIAL_LUX_THRESHOLD | 50 | Light level for burial detection |
| STILLNESS_THRESHOLD_MS | 500 | Time still before trigger |
| WARNING_DURATION_MS | 500 | Buzzer warning before trigger |
| MOTION_THRESHOLD | 0.3g | Acceleration change for motion |

## State Machine
`DISARMED → ARMED → MONITORING → WARNING → TRIGGERED → DEPLOYED`

## Pin Mapping
| Function | GPIO | Notes |
|----------|------|-------|
| I2C SDA | 6 | Shared by sensors |
| I2C SCL | 7 | Shared by sensors |
| Solenoid | 4 | MOSFET gate |
| Button | 9 | Arm/cancel (INPUT_PULLUP) |
| Buzzer | 8 | Warning tone |
| LEDs | 5,20,21 | Green, Red, Blue |

## Development Phase
**Phase 0** - Working prototype. Priorities:
1. Working > Optimized
2. Simple > Clever  
3. Bias toward triggering (false deploy beats missed deploy)

## Code Style
- Arduino framework with ESP-IDF underneath
- camelCase functions, UPPER_SNAKE_CASE constants
- All config in `config.h`, not hardcoded
- State machine with switch-case
- Safety: solenoid defaults LOW

## When Helping With This Project
- This is safety-critical - bias toward reliability over elegance
- Phase 0 means "does it work at all" - don't over-engineer
- Test incrementally: I2C scan → sensor readings → solenoid click → full sequence
- User is a beginner - prefer Grove connectors and simple solutions
- Budget constraint: keep total BOM under $100
