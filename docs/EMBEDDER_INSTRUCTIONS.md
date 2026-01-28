# Embedder Instructions - Avalanche Trigger Project

> Instructions for the AI assistant (Embedder) to maintain context across sessions.

---

## CRITICAL: Read the Manifesto First

**Before making ANY changes, read `docs/PROJECT_MANIFESTO.md`**

The manifesto defines:
- Project philosophy (Working > Optimized, Simple > Clever)
- Current phase and success criteria
- Hard specifications (2-second trigger time, safe defaults)
- Rules for contributors

**If code contradicts the manifesto, fix the code.**

---

## Project Overview

**Project Name:** Automatic Avalanche Airbag Trigger  
**Location:** `/Users/krishna/Desktop/Developmet/avalanche-trigger/`  
**User Skill Level:** Beginner in embedded systems  
**Current Phase:** Phase 0 - "Does it work at all?"  
**Primary Goal:** Detect imminent burial and deploy airbag in ≤2 seconds

---

## Key Files to Reference (Priority Order)

| File | Purpose | Read When |
|------|---------|-----------|
| `docs/PROJECT_MANIFESTO.md` | Philosophy, phases, hard rules | **ALWAYS FIRST** |
| `docs/CHAT_HISTORY.md` | Session-by-session development log | Starting session |
| `include/config.h` | Thresholds, pins, timing constants | Tuning behavior |
| `src/main.cpp` | Main application code | Changing logic |
| `docs/ENGINEERING_DOC.md` | Technical specifications, schematics | Hardware questions |
| `platformio.ini` | Build configuration | Build issues |

---

## Phase 0 Checklist (Current)

- [ ] All components powered and communicating
- [ ] Light sensor reads values
- [ ] IMU detects motion vs stillness  
- [ ] Solenoid fires on command
- [ ] State machine transitions correctly
- [ ] Can trigger by covering sensor and holding still

**Do NOT work on anything outside this scope.**

---

## Development Guidelines

### When Starting a New Session
1. **Read `docs/PROJECT_MANIFESTO.md`** - understand current phase
2. Read `docs/CHAT_HISTORY.md` to understand previous work
3. Review any TODO comments in the codebase
4. Ask user what they want to work on today
5. Confirm work aligns with current phase

### When Writing Code
1. Ask: "Does this help achieve Phase 0?" - if no, don't do it
2. Keep it simple - user is a beginner
3. Add comments explaining WHY, not just WHAT
4. Test incrementally (one sensor at a time)
5. Always print debug info to Serial
6. Bias toward triggering in ambiguous cases

### When Ending a Session
1. Update `docs/CHAT_HISTORY.md` with:
   - What was accomplished
   - Problems encountered and solutions
   - Decisions made (use decision log template from manifesto)
   - Next steps
2. Update `docs/ENGINEERING_DOC.md` if specs changed
3. Update `docs/PROJECT_MANIFESTO.md` if phase criteria met

---

## Hardware Context

### MCU: Seeed XIAO ESP32-C3
- **Voltage:** 3.3V logic
- **I2C Pins:** SDA = D4 (GPIO6), SCL = D5 (GPIO7)
- **Available GPIOs:** D0-D10
- **ADC:** 12-bit, but we're using digital I2C sensors to avoid complexity

### Sensors
| Sensor | Interface | I2C Address | Purpose |
|--------|-----------|-------------|---------|
| BH1750 | I2C | 0x23 or 0x5C | Light level (lux) |
| LSM6DS3 | I2C | 0x6A or 0x6B | Accelerometer + Gyro |

### Solenoid Driver
- IRLZ44N N-channel MOSFET (logic-level)
- Gate connected to GPIO via 10kΩ pull-down
- Flyback diode (1N4007) across solenoid
- 12V supply from boost converter (MT3608)

---

## Detection Algorithm Summary (Phase 0)

**Hard requirement: ≤2 seconds from burial to trigger**

```
STATE: ARMED
  │
  ▼
[Light < 50 lux?] ──No──▶ Stay ARMED
  │
  Yes (instant)
  ▼
STATE: MONITORING
  │
[Was there motion in last 60s?] ──No──▶ Back to ARMED (false alarm)
  │
  Yes
  ▼
[Still dark AND motionless for 500ms?] ──No──▶ Stay MONITORING
  │
  Yes
  ▼
STATE: WARNING (buzzer for 500ms)
  │
[Manual cancel pressed?] ──Yes──▶ Back to ARMED
  │
  No
  ▼
STATE: TRIGGERED (~1.0-1.5s total)
  │
  ▼
Activate solenoid pulse (300ms)
```

---

## Important Thresholds (in config.h)

| Parameter | Default | Notes |
|-----------|---------|-------|
| `BURIAL_LUX_THRESHOLD` | 50 | Conservative - false triggers OK in Phase 0 |
| `STILLNESS_THRESHOLD_MS` | 500 | Short - prioritize speed |
| `MOTION_HISTORY_MS` | 60000 | Was moving in last 60 seconds |
| `WARNING_DURATION_MS` | 500 | Brief warning, prioritize speed |
| `SOLENOID_PULSE_MS` | 300 | Enough for most pilot valves |

---

## Safety Rules (Never Violate)

1. **Default state = SAFE** - No trigger on power loss, boot, or error
2. **Manual override always works** - User can always trigger manually
3. **Never trigger while moving** - IMU must confirm stillness
4. **Require arming** - System must be explicitly armed by user
5. **Warning before auto-trigger** - Always give user chance to cancel

---

## User Preferences

- Prefers simple explanations
- Likes seeing things work incrementally
- Concerned about voltage compatibility (use I2C to avoid ADC issues)
- Comfortable with Grove connectors (plug-and-play)
- Budget conscious (under $100 total)

---

## Common Issues to Watch For

1. **I2C not working:** Check SDA/SCL wiring, pull-up resistors, correct address
2. **Light sensor reading 0:** May need time to initialize, check mode setting
3. **IMU noisy:** Use moving average or low-pass filter
4. **MOSFET not switching:** Check gate voltage, ensure logic-level MOSFET
5. **Solenoid not firing:** Check 12V supply, flyback diode orientation

---

## Links and Resources

- [XIAO ESP32-C3 Wiki](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/)
- [BH1750 Datasheet](https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf)
- [LSM6DS3 Datasheet](https://www.st.com/resource/en/datasheet/lsm6ds3.pdf)
- [IRLZ44N Datasheet](https://www.infineon.com/dgdl/irlz44npbf.pdf)

---

*This file helps Embedder maintain context. Update if project scope changes.*
