# GitHub Copilot Instructions

## Project Context
Avalanche airbag auto-trigger - embedded firmware for ESP32-C3 that detects burial and deploys airbag.

Read `CLAUDE.md` for full project details.

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
```

### Avoid
```cpp
// Don't hardcode values
if (lux < 50) // Bad - use BURIAL_LUX_THRESHOLD

// Don't use complex abstractions
class AbstractSensorFactory // Overkill for Phase 0

// Don't add features not requested
void addWiFiTelemetry() // Not in scope
```

## After Changes
Update `WORKLOG.md` with what you changed.
