# Decision Log

> **Purpose:** Track the evolution of technical decisions, why they changed, and what alternatives were considered.
> 
> **For AI Assistants:** Read this to understand how we got here, not just where we are.
> **For Krishna:** Add notes when you make decisions or remember context from earlier conversations.

---

## How to Read This Document

Decisions are listed **chronologically** (oldest first). Each entry shows:
- What changed
- Why it changed
- What we considered
- Impact on project

If something seems wrong in the codebase, check here first - the "current" state may have evolved from an earlier assumption.

---

## Timeline Overview

```
[Project Start] ──► ESP32-C3 + Grove sensors (original plan)
       │
       ▼
[Hardware Update] ──► ESP32-S3 Sense + Adafruit sensors (user had S3)
       │
       ▼
[2025-01-28 Discovery] ──► Base ESP32-S3, NOT Sense variant (no onboard IMU)
       │
       ▼
[Current State] ──► Need external IMU, docs partially outdated
```

---

## Decision 001: Original Hardware Selection

**Date:** January 2025 (project inception)  
**Context:** User wanted to build avalanche airbag auto-trigger on budget (<$100)  
**Decision:** Selected Grove ecosystem for beginner-friendliness

### Original BOM
| Component | Selection | Reason |
|-----------|-----------|--------|
| MCU | XIAO ESP32-C3 | Cheap ($5), Grove compatible |
| Light Sensor | Grove BH1750 | I2C, plug-and-play |
| IMU | Grove LSM6DS3 | 6-axis, Grove connector |
| Connectors | Grove cables | No soldering |

### Status: SUPERSEDED by Decision 002

---

## Decision 002: Hardware Change - ESP32-S3 + Adafruit Sensors

**Date:** January 2025 (exact date unknown)  
**Context:** User revealed they already own XIAO ESP32-S3 Sense, not C3  
**Triggered by:** User input during planning discussion

### What Changed
| Component | Old | New | Why |
|-----------|-----|-----|-----|
| MCU | ESP32-C3 | ESP32-S3 Sense | User already owns S3 |
| Light Sensor | BH1750 | VEML7700 | Better range (140k lux), STEMMA QT |
| IMU | LSM6DS3 | LSM6DSOX | Better features, STEMMA QT |
| Connectors | Grove | STEMMA QT | Adafruit ecosystem consistency |

### Alternatives Considered
1. **Keep C3 plan, user buys C3** - Rejected (user already has S3)
2. **S3 + Grove sensors** - Possible but STEMMA QT is cleaner
3. **S3 Sense + Adafruit sensors** - Selected (best compatibility)

### Impact
- Pin mapping changed (C3 vs S3 have different GPIOs)
- I2C addresses changed (VEML7700 = 0x10, not BH1750 = 0x23)
- Libraries changed in platformio.ini
- All docs needed updating

### Status: PARTIALLY SUPERSEDED by Decision 003

---

## Decision 003: Board Variant Discovery - NOT Sense Version

**Date:** 2025-01-28  
**Context:** Running I2C scanner, expected to find onboard IMU  
**Triggered by:** I2C scan found NO device at 0x6A/0x6B (LSM6DS3 addresses)

### Discovery
User's board is **Seeed XIAO ESP32-S3** (base version), NOT **XIAO ESP32-S3 Sense**.

The "Sense" variant includes:
- Onboard OV2640 camera
- Onboard PDM microphone  
- Onboard LSM6DS3 IMU at 0x6A

User's board has NONE of these.

### I2C Scan Results (2025-01-28)
| Address | Device | Expected? |
|---------|--------|-----------|
| 0x3C | SSD1306 OLED | No - external display connected |
| 0x51 | Unknown (EEPROM? RTC?) | No - unknown external device |
| 0x6A | LSM6DS3 (onboard IMU) | YES - **NOT FOUND** |
| 0x10 | VEML7700 (external) | No - not connected yet |

### Impact
- **Need external IMU** - must purchase Adafruit LSM6DSOX
- **Need external light sensor** - must purchase Adafruit VEML7700
- **Docs say "Sense"** - multiple files reference wrong board variant
- **Shopping list outdated** - NEXT.md still lists wrong parts

### Files That Need Updating
| File | Issue |
|------|-------|
| `AGENTS.md` | Says "ESP32-S3 Sense" |
| `docs/ENGINEERING_DOC.md` | Says "Sense", assumes onboard IMU |
| `NEXT.md` | Shopping list has C3 and Grove parts |
| `README.md` | May reference wrong board |

### Status: CURRENT - Docs not yet fully updated

---

## Decision 004: Detection Algorithm - Simple First

**Date:** January 2025 (project inception)  
**Context:** Designing burial detection logic  
**Philosophy:** Working > Optimized, Simple > Clever

### Algorithm Selected
```
IF (dark) AND (was_moving_recently) AND (now_still) THEN trigger
```

### Alternatives Considered

1. **Light rate-of-change detection**
   - Detect "sudden" darkness vs gradual
   - Rejected: Adds complexity, snow burial IS sudden anyway

2. **Pressure sensor (depth)**
   - Detect snow pressure on device
   - Rejected: Different sensor, Phase 0 focus on light+motion

3. **ML-based classification**
   - Train model on burial vs normal scenarios
   - Rejected: LSM6DSOX has ML core, but too complex for Phase 0

4. **Multi-sensor fusion**
   - Kalman filter combining light + IMU + pressure
   - Rejected: Optimization before working prototype

### Why Simple Approach
- False positive = annoying but safe
- False negative = potentially fatal
- Simple logic is debuggable under stress
- Can always add complexity later

### Status: CURRENT - Not yet tested on hardware

---

## Decision 005: Timing Parameters

**Date:** January 2025  
**Context:** How fast should system respond?

### Parameters Selected
| Parameter | Value | Rationale |
|-----------|-------|-----------|
| Stillness threshold | 500ms | Quick detection, may cause false positives |
| Warning duration | 500ms | Brief warning, prioritize speed |
| Motion history | 60s | Must have moved in last minute |
| Total burial→trigger | ~1.5s | Within 2s requirement |

### Alternatives Considered

1. **Longer stillness (3s)**
   - Fewer false positives
   - Rejected for Phase 0: Speed more important than precision

2. **Longer warning (2s)**
   - More time to cancel
   - Rejected: Delays potentially life-saving trigger

3. **Shorter motion history (30s)**
   - Stricter "was skiing" requirement
   - May revisit in Phase 1

### Status: CURRENT - Subject to real-world calibration

---

## Decision 006: Solenoid Driver Design

**Date:** January 2025  
**Context:** How to drive 12V solenoid from 3.3V MCU

### Design Selected
- IRLZ44N N-channel MOSFET (logic-level gate)
- 10kΩ pull-down on gate (default OFF)
- 1N4007 flyback diode across solenoid
- MT3608 boost converter for 12V

### Why IRLZ44N
- Logic-level: Vgs(th) = 1-2V, works with 3.3V GPIO
- High current: 47A continuous (solenoid needs ~1A)
- Common, cheap, through-hole for breadboard

### Safety Design
- **Normally OFF** - MOSFET gate pulled to GND
- **Fail-safe** - Power loss = no trigger (safe)
- **Flyback diode** - Protects MOSFET from solenoid inductance

### Status: CURRENT - Not yet tested on hardware

---

## Decision 007: Multi-AI Collaboration Framework

**Date:** 2025-01-27  
**Context:** Multiple AI tools (Claude Code, Embedder, Copilot) working on same project  
**Triggered by:** Embedder getting confused by outdated/conflicting docs

### Framework Selected
- **AGENTS.md** as primary context file (industry standard)
- **WORKLOG.md** for change history
- **NEXT.md** for current priorities
- Tool-specific files (EMBEDDER.md, CLAUDE.md) supplement but don't replace

### Why AGENTS.md Standard
- Supported by Claude, Gemini, Copilot, Cursor
- 60K+ projects using it
- Linux Foundation backed
- Single source of truth for any AI tool

### Status: CURRENT - Framework in place

---

## Decision 008: GitHub Ecosystem Setup

**Date:** 2025-01-27  
**Context:** Project needed proper version control and collaboration infrastructure  
**Done by:** Claude Code

### What Was Set Up
- GitHub repository (public)
- GitHub Actions CI/CD for PlatformIO
- Issue templates (bug, feature, hardware)
- PR template
- Labels for hardware/firmware/docs

### Status: CURRENT - CI/CD operational

---

## Pending Decisions (Need Input)

### P1: What is device at I2C 0x51?

**Context:** Found during 2025-01-28 I2C scan  
**Possibilities:**
- AT24C32 EEPROM (common at 0x50-0x57)
- DS3231 RTC (sometimes at 0x51)
- External sensor user connected
- Artifact/error

**Action needed:** User to identify what's connected, or run device-specific probe

---

### P2: Which external sensors to buy?

**Context:** Base ESP32-S3 has no onboard sensors  
**Options:**

| Option | Sensors | Cost | Pros | Cons |
|--------|---------|------|------|------|
| A | Adafruit VEML7700 + LSM6DSOX | ~$18 | Original plan, STEMMA QT | Two purchases |
| B | All-in-one BME280 + separate IMU | ~$15 | Adds temp/humidity | Still need IMU |
| C | Grove sensors (original plan) | ~$18 | Simpler connectors | Different ecosystem |

**Action needed:** User to decide and purchase

---

### P3: Update all docs to "Base ESP32-S3"?

**Context:** Multiple files say "Sense" variant  
**Options:**
1. Update now (before sensors arrive)
2. Update when hardware finalized
3. Leave as-is, note in DECISIONS.md

**Action needed:** User preference

---

## Questions for Krishna (Fill In When You Remember)

These are gaps in the decision history that only you know:

1. **When did you decide on avalanche trigger project?**
   - What inspired it? Personal experience? News story?

2. **Did you already have the OLED display (0x3C)?**
   - Was it intentionally connected for this project?

3. **What is the 0x51 device?**
   - Did you connect something you forgot about?

4. **Original conversation details?**
   - What AI tool did you first discuss this with?
   - Any key decisions from early chats not captured here?

5. **Budget constraints changed?**
   - Still targeting <$100?
   - Willing to spend more for better sensors?

---

## Document History

| Date | Change |
|------|--------|
| 2025-01-28 | Created DECISIONS.md, reconstructed history from WORKLOG.md and session logs |

---

*This document is append-only. Don't delete old decisions - mark them SUPERSEDED and explain why.*
