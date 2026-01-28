# Automatic Avalanche Airbag Trigger

[![PlatformIO Build](https://github.com/Krishnachalla/avalanche-trigger/actions/workflows/build.yml/badge.svg)](https://github.com/Krishnachalla/avalanche-trigger/actions/workflows/build.yml)

An embedded safety device that automatically triggers an avalanche airbag when it detects burial and user incapacitation.

## Why This Exists

When buried in an avalanche, a skier has seconds to deploy their airbag. If they're knocked unconscious, they can't. This device detects burial (darkness) + incapacitation (stillness after motion) and triggers the airbag automatically.

**Philosophy:** A false deployment is annoying. A missed deployment is fatal. We bias toward triggering.

## Project Status

| Phase | Status | Description |
|-------|--------|-------------|
| **Phase 0** | **CURRENT** | Working prototype - "does it work at all?" |
| Phase 1 | Planned | Reliable operation in controlled environment |
| Phase 2 | Planned | Real snow testing |
| Phase 3 | Planned | Safety hardening |

## Hardware

| Component | Model | Purpose |
|-----------|-------|---------|
| MCU | Seeed XIAO ESP32-S3 Sense | Main controller |
| Light Sensor | Adafruit VEML7700 | Burial detection (darkness) |
| IMU | Adafruit LSM6DSOX | Motion/stillness detection |
| Solenoid Driver | IRLZ44N MOSFET | Trigger output |
| Power | 3.7V LiPo + MT3608 boost | 12V solenoid power |

### Pin Mapping (XIAO ESP32-S3)

| Function | GPIO | Notes |
|----------|------|-------|
| I2C SDA | 5 | Sensors |
| I2C SCL | 6 | Sensors |
| Solenoid | 2 | MOSFET gate |
| Button | 44 | Arm/cancel |
| Buzzer | 7 | Warning |
| LED Blue | 1 | Status |
| LED Green | 3 | Status |
| LED Red | 4 | Status |

## Quick Start

### Prerequisites

- [PlatformIO](https://platformio.org/install) (VS Code extension recommended)
- USB-C cable
- Hardware components (see BOM in `docs/ENGINEERING_DOC.md`)

### Build & Flash

```bash
# Build firmware
pio run

# Flash to board
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor
```

## How It Works

```
[DISARMED] --button--> [ARMED] --dark--> [MONITORING]
                                              |
                              still for 500ms + was moving recently
                                              |
                                              v
                       [DEPLOYED] <--300ms-- [TRIGGERED] <--500ms-- [WARNING]
                                                                   (buzzer)
                                                              button cancels
```

**Detection Logic:**
1. Light drops below 50 lux (burial)
2. Was moving in last 60 seconds (was skiing)
3. Still for 500ms (incapacitated)
4. 500ms warning buzzer (user can cancel)
5. Solenoid fires (deploys airbag)

**Total time from burial to trigger: ~1.0-1.5 seconds**

## Configuration

All tunable parameters in `include/config.h`:

```cpp
#define BURIAL_LUX_THRESHOLD    50      // Lux threshold for burial
#define STILLNESS_THRESHOLD_MS  500     // Stillness time before trigger
#define WARNING_DURATION_MS     500     // Warning buzzer duration
#define MOTION_THRESHOLD        0.3f    // Acceleration change for motion (g)
#define MOTION_HISTORY_MS       60000   // Recent motion window (60s)
#define SOLENOID_PULSE_MS       300     // Trigger pulse duration
```

## Documentation

| Document | Purpose |
|----------|---------|
| [AGENTS.md](AGENTS.md) | AI assistant context (for Embedder, Claude, etc.) |
| [docs/PROJECT_MANIFESTO.md](docs/PROJECT_MANIFESTO.md) | Philosophy, phases, hard rules |
| [docs/ENGINEERING_DOC.md](docs/ENGINEERING_DOC.md) | Full technical specs, BOM, schematics |
| [CHANGELOG.md](CHANGELOG.md) | Release history |
| [WORKLOG.md](WORKLOG.md) | Development session history |

## Safety Warning

**This is a learning/prototype project, NOT certified safety equipment.**

- Always test with solenoid disconnected first
- Never test on a person until thoroughly bench-tested
- Manual trigger must always work independently
- Do not rely on this for actual avalanche safety without extensive validation

## Contributing

This project uses multiple AI assistants (Embedder, Claude Code) for development. See [AGENTS.md](AGENTS.md) for the collaboration protocol.

**Humans:** PRs welcome! Please read `docs/PROJECT_MANIFESTO.md` first.

**AI Assistants:** Read `AGENTS.md` before making changes. Update `WORKLOG.md` when done.

## Bill of Materials

Estimated cost: **~$50-70**

See `docs/ENGINEERING_DOC.md` Section 8 for full BOM with purchase links.

## License

MIT License - For educational purposes.

---

*Built with [PlatformIO](https://platformio.org/) | Developed with [Embedder](https://embedder.com/) & [Claude Code](https://claude.ai/)*
