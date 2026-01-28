# Avalanche Trigger Project - Chat History

> This document is updated after each development session to maintain continuity.

---

## Session 1 - Project Inception
**Date:** January 2025

### Initial Request
User wants to build an **automatic trigger for a manual avalanche airbag** that:
- Detects burial (light deprivation)
- Detects user incapacitation (stillness after motion)
- Automatically triggers the pilot valve to release CO2 cartridge

### User Background
- Beginner in embedded systems
- Has access to Seeed Studio and Adafruit boards
- Concerned about sensor selection, voltage compatibility, and ADC pinout complexity
- Budget: Under $100

### Key Decisions Made

#### Hardware Selection
- **MCU:** Seeed XIAO ESP32-C3
  - Tiny, low-power, Grove compatible
  - WiFi for debugging
  - 3.3V logic
  
- **Light Sensor:** Grove BH1750
  - Digital I2C (no ADC complexity)
  - 1-65535 lux range
  
- **IMU:** Grove LSM6DS3
  - 6-axis accelerometer + gyro
  - Detects motion and stillness
  
- **Solenoid Driver:** IRLZ44N MOSFET
  - Logic-level (triggers at 3.3V)
  - Can drive 12V solenoid

#### Airbag Type
- CO2 cartridge with pilot valve (solenoid-activated)

#### Safety Tolerance
- Medium accuracy for working prototype
- Not production-grade, but should be reliable

### Detection Algorithm Approach
1. **Phase 1:** Light < 10 lux for 3-5 seconds → trigger
2. **Phase 2:** Add IMU confirmation (recent motion + current stillness)
3. **Phase 3:** Add manual override and warning buzzer

### Estimated Budget
~$60-70 for all components

### Files Created This Session
- `docs/CHAT_HISTORY.md` - This file
- `docs/EMBEDDER_INSTRUCTIONS.md` - AI assistant context
- `docs/ENGINEERING_DOC.md` - Full technical documentation
- `platformio.ini` - Build configuration
- `include/config.h` - Configuration constants
- `src/main.cpp` - Main application (skeleton)

### Next Session Goals
- Order components from Seeed Studio
- Set up development environment (PlatformIO)
- Write and test BH1750 light sensor code

---

## Session Notes Template

### Session N - [Title]
**Date:** [Date]

#### What We Worked On
- [Item 1]
- [Item 2]

#### Problems Encountered
- [Problem and solution]

#### Decisions Made
- [Decision and rationale]

#### Code Changes
- [File]: [Description of change]

#### Next Session Goals
- [Goal 1]
- [Goal 2]

---

*Last Updated: Session 1*
