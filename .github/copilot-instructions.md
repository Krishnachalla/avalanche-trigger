# GitHub Copilot Instructions

> **Last updated:** 2026-01-27 by Claude Code
> **Primary context:** Read `AGENTS.md` for full project details.

## Project Context

Avalanche airbag auto-trigger - embedded firmware for **XIAO ESP32-S3 Sense** that detects burial and deploys airbag.

**Current Phase:** Phase 0 (Working Prototype)

## Hardware (Current)

| Component | Model | I2C Address |
|-----------|-------|-------------|
| MCU | XIAO ESP32-S3 Sense | - |
| Light Sensor | Adafruit VEML7700 | 0x10 |
| IMU | Adafruit LSM6DSOX | 0x6A |

## Code Patterns

### Preferred

```cpp
// Constants in config.h
#define BURIAL_LUX_THRESHOLD 50

// State machine with enum
enum SystemState { STATE_DISARMED, STATE_ARMED, ... };

// camelCase functions
void checkBurialConditions() { ... }

// Safety: outputs default safe
digitalWrite(PIN_SOLENOID, LOW);

// Debug output
Serial.println("State changed to ARMED");
```

### Avoid

```cpp
// Don't hardcode values
if (lux < 50) // Bad - use BURIAL_LUX_THRESHOLD

// Don't use complex abstractions
class AbstractSensorFactory // Overkill for Phase 0

// Don't add features not in current phase
void addWiFiTelemetry() // Not in scope yet
```

## Key Files

| File | Purpose |
|------|---------|
| `src/main.cpp` | Main firmware, state machine |
| `include/config.h` | All tunable parameters |
| `AGENTS.md` | Full AI context |

## After Changes

1. Test builds: `pio run`
2. Update `WORKLOG.md` with what you changed

## Safety Rules

- Solenoid must default LOW (safe state)
- Never trigger on boot/power-up
- Always allow manual cancel
- Bias toward triggering (false positive > false negative)
