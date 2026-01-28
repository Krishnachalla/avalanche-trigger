# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- GitHub Actions CI/CD workflow for automated builds
- Issue templates for bugs, features, and hardware issues
- Pull request template
- CHANGELOG.md (this file)
- AGENTS.md for multi-AI collaboration (AGENTS.md standard)

### Changed
- README.md completely rewritten for current hardware
- Copilot instructions updated to ESP32-S3

### Removed
- `docs/CHAT_HISTORY.md` (merged into WORKLOG.md)
- `docs/EMBEDDER_INSTRUCTIONS.md` (redundant with EMBEDDER.md)

## [0.2.0] - 2025-01-28

### Changed
- **BREAKING:** Hardware platform changed from ESP32-C3 to ESP32-S3 Sense
- **BREAKING:** Light sensor changed from BH1750 to Adafruit VEML7700 (I2C addr 0x10)
- **BREAKING:** IMU changed from LSM6DS3 to Adafruit LSM6DSOX (I2C addr 0x6A)
- Pin mappings updated for ESP32-S3
- Libraries updated in platformio.ini

### Added
- I2C scanner debug tool (`src/i2c_scanner.cpp`)
- Debug session logging (`docs/DEBUG_SESSION_2025-01-28.md`)
- STEMMA QT/Qwiic connector support

## [0.1.0] - 2025-01-XX

### Added
- Initial project structure with PlatformIO
- Main firmware skeleton (`src/main.cpp`)
- Configuration file (`include/config.h`)
- State machine: DISARMED → ARMED → MONITORING → WARNING → TRIGGERED → DEPLOYED
- Detection algorithm: light threshold + motion history + stillness confirmation
- Project manifesto with phased development approach
- Engineering documentation with BOM and schematics
- AI context files (CLAUDE.md, EMBEDDER.md)

### Hardware Support (Initial)
- Seeed XIAO ESP32-C3
- Grove BH1750 light sensor
- Grove LSM6DS3 IMU
- IRLZ44N MOSFET solenoid driver
- MT3608 boost converter

---

## Version History Summary

| Version | Date | Milestone |
|---------|------|-----------|
| 0.2.0 | 2025-01-28 | Hardware change to ESP32-S3 + Adafruit sensors |
| 0.1.0 | 2025-01-XX | Initial skeleton, documentation, AI context |

## Contributors

- **Krishna** - Project owner, hardware
- **Embedder** (AI) - Firmware development, hardware debugging
- **Claude Code** (AI) - Documentation, git operations, project organization

---

*For detailed session-by-session changes, see [WORKLOG.md](WORKLOG.md)*
