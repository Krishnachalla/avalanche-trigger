# Work Log

> **FOR AI ASSISTANTS**: Add your work to the top of this file when done.
> Format: Date, Tool Used, What Was Done, Files Changed

---

## 2025-01-XX - Embedder (Hardware Update Session)
**Major Hardware Change: ESP32-C3 → ESP32-S3 Sense + Adafruit Sensors**

User has XIAO ESP32-S3 Sense (not C3), so updated entire project to use:
- **MCU:** XIAO ESP32-S3 Sense (user already owns)
- **Light Sensor:** Adafruit VEML7700 (0-140k lux, ultra-low power)
- **IMU:** Adafruit LSM6DSOX (6-axis with ML core)

Both sensors use STEMMA QT/Qwiic connectors for easy wiring.

**Files changed:**
- `include/config.h` - Updated pin definitions for S3, new I2C addresses (VEML7700=0x10)
- `src/main.cpp` - Updated includes, sensor objects, init code for new libraries
- `platformio.ini` - Changed to seeed_xiao_esp32s3, updated lib_deps
- `docs/ENGINEERING_DOC.md` - Updated all hardware specs, BOM, pin tables, schematics
- `EMBEDDER.md` - Updated overview, hardware, pin mapping, libraries
- `CLAUDE.md` - Updated hardware section and pin mapping

**Datasheets referenced:**
- VEML7700: vishay.com/docs/84286/veml7700.pdf
- LSM6DSOX: st.com/resource/en/datasheet/lsm6dsox.pdf
- TSL2591: cdn-shop.adafruit.com/datasheets/TSL25911_Datasheet_EN_v1.pdf

**Status:** Firmware updated for new hardware, needs build test when sensors arrive

---

## 2025-01-XX - Embedder
**Setup & Documentation**
- Created project structure with PlatformIO
- Wrote main firmware skeleton (`src/main.cpp`)
- Created config file with all tunable parameters (`include/config.h`)
- Created comprehensive engineering docs (`docs/ENGINEERING_DOC.md`)
- Created project manifesto with phased approach (`docs/PROJECT_MANIFESTO.md`)
- Set up AI context files (CLAUDE.md, EMBEDDER.md)
- Initialized git repository

**Files created/modified:**
- `src/main.cpp` - Full state machine, sensor integration
- `include/config.h` - Pin definitions, thresholds, timing
- `platformio.ini` - Build config, libraries
- `docs/*` - All documentation
- `CLAUDE.md`, `EMBEDDER.md` - AI context

**Status:** Phase 0 firmware complete, awaiting hardware for testing

---

<!-- 
TEMPLATE FOR NEW ENTRIES:

## YYYY-MM-DD - [Tool Name: Claude/Cursor/Copilot/Gemini/Embedder]
**Summary of work**
- Bullet points of what was done

**Files changed:**
- `path/to/file` - Description of changes

**Status:** Current state / Next steps needed

---
-->
