# Work Log

> **FOR AI ASSISTANTS**: Add your work to the top of this file when done.
> Format: Date, Tool Used, What Was Done, Files Changed

---

## 2026-01-27 - Claude Code (Claude Opus 4.5)
**GitHub Ecosystem Setup - COMPLETED**

**Who:** Claude Code (claude-opus-4-5-20251101)
**Requested by:** Krishna

Completed the remaining GitHub ecosystem setup that wasn't covered by Embedder's reorganization:

**Tasks Completed:**
- [x] README.md - Updated to current hardware (ESP32-S3, VEML7700, LSM6DSOX)
- [x] CHANGELOG.md - Created with version history
- [x] .github/workflows/build.yml - GitHub Actions for auto-build on push
- [x] .github/ISSUE_TEMPLATE/bug_report.md - Bug template
- [x] .github/ISSUE_TEMPLATE/feature_request.md - Feature template
- [x] .github/ISSUE_TEMPLATE/hardware_issue.md - Hardware template
- [x] .github/PULL_REQUEST_TEMPLATE.md - PR template with checklist
- [x] .github/copilot-instructions.md - Updated to current hardware
- [x] GitHub Labels created: phase:0, phase:1, firmware, hardware, blocked, ai-generated

**Note:** Embedder simultaneously reorganized AI files into `.ai/` folder - great parallel work!

**Files created/changed:**
- `README.md` - Complete rewrite
- `CHANGELOG.md` - NEW
- `.github/workflows/build.yml` - NEW
- `.github/ISSUE_TEMPLATE/*.md` - NEW (3 templates)
- `.github/PULL_REQUEST_TEMPLATE.md` - NEW
- `.github/copilot-instructions.md` - Updated

**Status:** GitHub ecosystem complete. Ready for commits and push.

---

## 2025-01-28 - Embedder
**Repository Cleanup - Moved AI files to .ai/ folder**

**What was done:**
- Created `.ai/` folder for AI assistant context files
- Moved AGENTS.md, EMBEDDER.md, NEXT.md, WORKLOG.md to `.ai/`
- Deleted redundant files: CLAUDE.md, docs/CHAT_HISTORY.md, docs/EMBEDDER_INSTRUCTIONS.md
- Moved debug sessions to `docs/debug/` folder
- Updated all internal path references

**Files changed:**
- `.ai/AGENTS.md` - Moved from root, updated paths
- `.ai/EMBEDDER.md` - Moved from root, updated paths
- `.ai/NEXT.md` - Moved from root, updated paths
- `.ai/WORKLOG.md` - Moved from root
- `docs/debug/2025-01-28.md` - Moved from docs/
- `docs/debug/2025-01-28_raw.md` - Moved from docs/
- DELETED: `CLAUDE.md`, `docs/CHAT_HISTORY.md`, `docs/EMBEDDER_INSTRUCTIONS.md`

**New structure:**
```
/
├── README.md, CHANGELOG.md    (user-facing)
├── .ai/                       (AI context - hidden)
│   ├── AGENTS.md, EMBEDDER.md, NEXT.md, WORKLOG.md
├── docs/
│   ├── ENGINEERING_DOC.md, PROJECT_MANIFESTO.md, DECISIONS.md
│   └── debug/
```

**Status:** Root is now clean. AI files are organized.

---

## 2025-01-28 - Embedder
**Created DECISIONS.md - Decision History Tracking**

**Why:** User requested a way to track the evolution of decisions - when plans changed, why they branched, what alternatives were considered. Existing docs showed WHAT changed but not the decision history.

**What was done:**
- Created `docs/DECISIONS.md` with full decision history reconstructed from logs
- Documented 8 major decisions from project inception to now
- Added timeline overview showing evolution: C3 → S3 Sense → Base S3
- Listed pending decisions needing user input
- Added "Questions for Krishna" section for gaps only user knows
- Updated `AGENTS.md` to reference DECISIONS.md in workflow

**Files changed:**
- `docs/DECISIONS.md` - NEW: Complete decision history
- `AGENTS.md` - Added DECISIONS.md to shared memory files and workflow
- `WORKLOG.md` - This entry

**Status:** Decision tracking in place. User should fill in gaps from early conversations.

---

## 2026-01-27 - Claude Code (Claude Opus 4.5)
**Major Project Reorganization - GitHub Ecosystem Setup**

**Who:** Claude Code (claude-opus-4-5-20251101)
**Requested by:** Krishna
**Why:** Embedder was confused by outdated/conflicting documentation. Need to sync all files to current hardware, set up proper GitHub ecosystem, and establish clear multi-AI workflow.

### Research Conducted
Researched best practices for:
- GitHub Actions CI/CD for PlatformIO ([docs](https://docs.platformio.org/en/stable/integration/ci/github-actions.html))
- GitHub Issues/Labels for hardware projects ([AllSpice guide](https://www.allspice.io/post/issues-and-milestones-in-git-for-hardware))
- CHANGELOG format ([Keep a Changelog](https://keepachangelog.com/))
- README best practices ([GitHub docs](https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/about-readmes))
- Multi-AI collaboration ([AGENTS.md standard](https://agents.md/))

### Problems Found
1. Hardware info inconsistent across files (some say ESP32-C3, others ESP32-S3)
2. Redundant docs: `CHAT_HISTORY.md` duplicates `WORKLOG.md`, `EMBEDDER_INSTRUCTIONS.md` duplicates `EMBEDDER.md`
3. No CI/CD automation
4. No GitHub Issues/Projects setup
5. No CHANGELOG.md

### Tasks Completed
- [ ] README.md - Updated to current hardware (ESP32-S3, VEML7700, LSM6DSOX)
- [ ] CHANGELOG.md - Created with initial release
- [ ] .github/workflows/build.yml - GitHub Actions for auto-build
- [ ] .github/ISSUE_TEMPLATE/ - Bug, feature, hardware templates
- [ ] .github/PULL_REQUEST_TEMPLATE.md - PR template
- [ ] .github/copilot-instructions.md - Updated to current hardware
- [ ] Deleted redundant: docs/CHAT_HISTORY.md, docs/EMBEDDER_INSTRUCTIONS.md
- [ ] Moved debug log: docs/DEBUG_SESSION_2025-01-28.md → docs/debug/
- [ ] GitHub Labels created via `gh` CLI
- [ ] AGENTS.md updated with reorganization info

**Files changed:** (updating as I go)
- `README.md` - Complete rewrite for current hardware
- `CHANGELOG.md` - NEW
- `.github/workflows/build.yml` - NEW
- `.github/ISSUE_TEMPLATE/*.md` - NEW
- `.github/PULL_REQUEST_TEMPLATE.md` - NEW
- `.github/copilot-instructions.md` - Updated
- `docs/CHAT_HISTORY.md` - DELETED (merged into WORKLOG.md)
- `docs/EMBEDDER_INSTRUCTIONS.md` - DELETED (redundant)
- `docs/debug/` - NEW folder for debug sessions
- `AGENTS.md` - Updated
- `NEXT.md` - Updated with session status

**Status:** IN PROGRESS - Check back when complete

---

## 2025-01-28 - Embedder
**I2C Scanner Debug Session - Serial & I2C Verification**

Debugged serial communication and I2C bus on XIAO ESP32-S3.

**Problem:** Serial monitor receiving no data, I2C devices unknown.

**What was done:**
1. Attempted serial monitor - timed out, no output
2. Tried DTR/RTS reset cycling - no effect
3. Simplified firmware to "Hello World" test - confirmed serial works
4. Restored full I2C scanner firmware
5. Ran I2C bus scan across multiple pin configurations

**Results:**
- Serial works at 115200 baud on `/dev/cu.usbmodem2101`
- I2C devices found:
  - `0x3C` - SSD1306 OLED display
  - `0x51` - Unknown device (EEPROM? RTC?)
- No IMU detected (LSM6DS3 at 0x6A/0x6B not found)

**Observer notes:** User heard clicking sounds during flash but saw no visible board response. Clicks are normal - board resetting during flash.

**Files changed:**
- `src/i2c_scanner.cpp` - Temporarily simplified to Hello World, then restored to full scanner
- `src/tests/serial_test.cpp` - NEW: Saved Hello World test for future debugging
- `docs/DEBUG_SESSION_2025-01-28.md` - NEW: Full debug session log with all details

**What was NOT saved (lost):**
- Full serial output from all 5 I2C pin configurations (only GPIO5/6 results captured)
- Exact timestamps of each operation

**Status:** Serial and I2C confirmed working. Need to identify 0x51 device and verify if Sense expansion board is attached.

---

## 2026-01-27 - Claude Code
**Adopted AGENTS.md Standard for Multi-AI Collaboration**

Researched industry best practices for AI coding assistant collaboration. Found AGENTS.md is the emerging standard (60K+ projects, Linux Foundation backed, supported by Claude, Gemini, Codex, Cursor, Copilot).

Implemented the standard for this project:
- Created `AGENTS.md` as primary context file (tool-agnostic)
- Updated `CLAUDE.md` to redirect to AGENTS.md (backwards compatibility)
- Updated `EMBEDDER.md` to reference AGENTS.md
- Established shared memory protocol: AGENTS.md + NEXT.md + WORKLOG.md

**Files changed:**
- `AGENTS.md` - NEW: Primary AI context following AGENTS.md standard
- `CLAUDE.md` - Simplified to redirect to AGENTS.md
- `EMBEDDER.md` - Added AGENTS.md reference header

**Research sources:**
- https://agents.md/
- https://developers.openai.com/codex/guides/agents-md
- https://addyosmani.com/blog/ai-coding-workflow/

**Status:** Multi-AI collaboration framework in place. Any AI tool can now contribute by reading AGENTS.md.

---

## 2026-01-27 - Claude Code
**GitHub Repository Created**

- Authenticated GitHub CLI (`gh auth login`)
- Created public repository: https://github.com/Krishnachalla/avalanche-trigger
- Pushed all commits to remote

**Status:** Code now on GitHub, ready for collaboration.

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

---

## Archive: Project Inception (from CHAT_HISTORY.md)

**Date:** January 2025
**Original request:** Build automatic trigger for manual avalanche airbag
- Detects burial (light deprivation)
- Detects user incapacitation (stillness after motion)
- Triggers pilot valve to release CO2 cartridge

**User background:**
- Beginner in embedded systems
- Budget: Under $100

**Original hardware (since changed):**
- MCU: XIAO ESP32-C3 → **Now: ESP32-S3 Sense**
- Light: BH1750 → **Now: VEML7700**
- IMU: LSM6DS3 → **Now: LSM6DSOX**

*This section archived from `docs/CHAT_HISTORY.md` which was deleted on 2026-01-27 during project reorganization.*
