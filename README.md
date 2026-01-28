# Automatic Avalanche Airbag Trigger

An embedded system that automatically triggers an avalanche airbag when it detects that the user has been buried and is incapacitated.

## Project Status

**Phase:** Implementation - Skeleton code complete, awaiting hardware

## Quick Start

### Prerequisites
- [PlatformIO](https://platformio.org/install) (VS Code extension recommended)
- XIAO ESP32-C3 board
- BH1750 light sensor
- LSM6DS3 IMU

### Build & Upload
```bash
# Build
pio run

# Upload to board
pio run --target upload

# Monitor serial output
pio device monitor
```

## Hardware Setup

### Pin Connections

| Component | XIAO Pin | GPIO |
|-----------|----------|------|
| BH1750 SDA | D4 | GPIO6 |
| BH1750 SCL | D5 | GPIO7 |
| LSM6DS3 SDA | D4 | GPIO6 |
| LSM6DS3 SCL | D5 | GPIO7 |
| Solenoid MOSFET | D2 | GPIO4 |
| LED Green | D3 | GPIO5 |
| LED Red | D6 | GPIO20 |
| LED Blue | D7 | GPIO21 |
| Buzzer | D8 | GPIO8 |
| ARM Button | D9 | GPIO9 |

### Wiring Diagram

See `docs/ENGINEERING_DOC.md` for detailed schematics.

## Operation

### States

1. **DISARMED** (Blue LED) - System inactive
2. **ARMED** (Green LED) - Monitoring sensors
3. **MONITORING** (Yellow LED) - Dark detected, checking conditions
4. **WARNING** (Red blink + buzzer) - About to trigger, can cancel
5. **TRIGGERED** - Solenoid fired
6. **DEPLOYED** (Red solid) - Waiting for reset

### Button Functions

- **Disarmed**: Press to ARM
- **Armed/Monitoring**: Press to DISARM
- **Warning**: Press to CANCEL trigger
- **Deployed**: Press to RESET

## Configuration

Edit `include/config.h` to adjust:

```cpp
#define BURIAL_LUX_THRESHOLD    10      // Light threshold (lux)
#define MOTION_THRESHOLD        0.3f    // Motion threshold (g)
#define STILLNESS_THRESHOLD_MS  3000    // Stillness time (ms)
#define MOTION_HISTORY_MS       30000   // Motion history window (ms)
#define WARNING_DURATION_MS     2000    // Warning time (ms)
```

## Documentation

- `docs/CHAT_HISTORY.md` - Development session logs
- `docs/ENGINEERING_DOC.md` - Full technical documentation
- `docs/EMBEDDER_INSTRUCTIONS.md` - AI assistant context

## Safety Warning

**This is a learning project, not certified safety equipment.**

- Always test with solenoid disconnected first
- Never test on a person until thoroughly bench-tested
- Manual trigger must always work independently
- Do not rely on this for actual avalanche safety

## Bill of Materials

Total estimated cost: **~$62**

See `docs/ENGINEERING_DOC.md` Section 8 for full BOM.

## License

For educational purposes only.
