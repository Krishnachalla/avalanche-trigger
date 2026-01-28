# Avalanche Trigger - Project Manifesto

> **This document defines the project philosophy, phases, and hard rules.**
> **Read this FIRST before making ANY changes.**

---

## Mission Statement

Build a **working automatic avalanche airbag trigger** that detects imminent burial and deploys the airbag **fast and reliably**.

This is a **survival system**, not an academic exercise.

---

## Core Philosophy

### 1. Working > Optimized

A system that works imperfectly is infinitely better than a system that never ships.

- We do NOT optimize early
- Everything starts conservative and simple
- Refinement comes AFTER working prototype

### 2. Simple > Clever

This device must be understandable, testable, and debuggable under stress.

- No fancy algorithms until basic ones fail
- No sensor fusion until single sensors are understood
- No abstractions until concrete implementations work

### 3. Safe Failure Bias

**Missed deployment is catastrophic. False deployment is annoying.**

For early versions:
- Bias toward triggering rather than hesitating
- Accept false positives during development
- Use warnings and manual cancel as safety valves
- Default state = SAFE (no trigger on power loss, boot, or error)

Commercial tuning comes later. Survival comes first.

### 4. Elon's First Principles (Adapted)

1. **Make your requirements less dumb** - Question every spec
2. **Delete the part** - Simplest solution that works
3. **Simplify/Optimize** - Only AFTER it works
4. **Accelerate cycle time** - Only AFTER simplified
5. **Automate** - Only AFTER accelerated

We are currently at Step 1-2. Do not skip ahead.

---

## Hard Specifications (Non-Negotiable)

| Parameter | Value | Rationale |
|-----------|-------|-----------|
| **Trigger Time** | ≤2 seconds from burial detection | Survival window is short |
| **Trigger Output** | 12V solenoid pulse (200-500ms) | Works with pilot valves, electronic triggers |
| **Manual Override** | Physical button, always functional | User can always trigger/cancel |
| **Safe Default** | No trigger on power loss or error | Prevent accidental deployments |
| **Power Runtime** | 4-6 hours minimum | Full ski day coverage |

---

## Detection Logic (Phase 0)

### Simple and Fast

```
IF (light < threshold) AND (was_moving_recently) AND (now_still)
THEN → WARNING (buzzer) → TRIGGER
```

### Timing Breakdown

```
[Burial Event]
     │
     ├─ Light drops below threshold (instant detection)
     │
     ├─ Stillness confirmed (~500ms of no motion)
     │
     ├─ WARNING buzzer starts (~500ms)
     │
     └─ TRIGGER fires at ≤2 seconds total
```

### Phase 0 Thresholds

| Parameter | Value | Notes |
|-----------|-------|-------|
| `BURIAL_LUX_THRESHOLD` | 50 lux | Conservative - will tune with real snow |
| `STILLNESS_DURATION_MS` | 500 | Half second of no movement |
| `WARNING_DURATION_MS` | 500 | Brief warning, prioritize speed |
| `MOTION_HISTORY_MS` | 60000 | Was moving in last 60 seconds |
| `SOLENOID_PULSE_MS` | 300 | Enough to actuate most valves |

**Total time from burial to trigger: ~1.0-1.5 seconds**

---

## Light Threshold Rationale

### What We Know
- Bright daylight: 10,000-100,000 lux
- Overcast day: 1,000-10,000 lux
- Indoor lighting: 100-500 lux
- Twilight: 10-50 lux
- Under snow (30cm): <10 lux
- Deep burial: <1 lux

### Phase 0 Approach
- Start at **50 lux** - will trigger in heavy shadow too (false positives expected)
- This is intentional: better to trigger early than miss burial
- Real-world calibration will refine this

### What NOT to do
- Don't try to detect "rate of change" in light
- Don't try to differentiate tunnel vs burial
- Don't add complexity until simple approach fails

---

## Project Phases

### Phase 0: "Does it work at all?" (CURRENT)

**Goal:** Hardware assembled, firmware runs, can trigger solenoid manually and automatically

**Success Criteria:**
- [ ] All components powered and communicating
- [ ] Light sensor reads values
- [ ] IMU detects motion vs stillness
- [ ] Solenoid fires on command
- [ ] State machine transitions correctly
- [ ] Can trigger by covering sensor and holding still

**NOT in scope:**
- Power optimization
- Enclosure design
- False trigger refinement
- Waterproofing
- Extended testing

**Timeline:** Get it working. Period.

---

### Phase 1: "Does it work reliably?"

**Goal:** Consistent operation in controlled environment

**Success Criteria:**
- [ ] 10 consecutive successful triggers in dark/still conditions
- [ ] 10 consecutive non-triggers in light/moving conditions
- [ ] Manual override works every time
- [ ] No crashes or resets during 4-hour run

**Changes from Phase 0:**
- Tune light threshold based on testing
- Tune motion threshold based on testing
- Add basic LED status indicators
- Fix any bugs discovered

---

### Phase 2: "Does it work in snow?"

**Goal:** Real-world validation in controlled outdoor environment

**Success Criteria:**
- [ ] Triggers when buried in snow pile (not avalanche)
- [ ] Does not trigger during normal skiing motion
- [ ] Survives cold temperature operation
- [ ] Battery lasts 4+ hours in cold

**Changes from Phase 1:**
- Cold-temperature calibration
- Real snow light calibration
- Weatherproof connections
- Battery monitoring

---

### Phase 3: "Does it work safely?"

**Goal:** False positive reduction, safety hardening

**Success Criteria:**
- [ ] <10% false trigger rate during normal use
- [ ] Documented failure modes
- [ ] User manual written
- [ ] Testing with airbag system (disconnected actuator)

**Changes from Phase 2:**
- Multi-condition validation
- Timeout and lockout logic
- Error recovery improvements

---

### Phase 4+: Future (Not Planned)

- Smaller form factor
- Integrated with existing airbag systems
- GPS/beacon integration
- Multi-unit mesh networking
- Commercial certification

**Do not think about Phase 4+ until Phase 2 is complete.**

---

## What We Are NOT Building (Yet)

| Feature | Why Not Now |
|---------|-------------|
| GPS tracking | Adds complexity, not core function |
| Cellular/satellite alert | Adds complexity, not core function |
| Avalanche prediction | Requires more sensors, ML |
| Heart rate monitoring | Different sensor, different problem |
| Multi-person coordination | Mesh networking complexity |
| Companion app | Software complexity |
| Rechargeable battery management | BMS complexity |

These may come later. Not now.

---

## Hardware Bill (Phase 0)

| Component | Purpose | Qty |
|-----------|---------|-----|
| XIAO ESP32-C3 | MCU | 1 |
| Grove BH1750 | Light sensor | 1 |
| Grove LSM6DS3 | IMU (motion) | 1 |
| 12V Solenoid | Trigger actuator | 1 |
| IRLZ44N MOSFET | Solenoid driver | 1 |
| 1N4007 Diode | Flyback protection | 1 |
| Buzzer | Warning | 1 |
| Push button | Manual trigger/cancel | 1 |
| 3.7V LiPo | Power | 1 |
| MT3608 | Boost to 12V | 1 |
| Misc | Wires, resistors, breadboard | - |

**Estimated cost: $60-80**

---

## Rules for Contributors (Human or AI)

### ALWAYS
1. Read this manifesto before making changes
2. Test on real hardware when possible
3. Bias toward triggering in ambiguous cases
4. Keep manual override functional
5. Log decisions in CHAT_HISTORY.md

### NEVER
1. Add features not in current phase
2. Optimize before it works
3. Remove the manual trigger capability
4. Make changes that could trigger on power-up
5. Skip hardware testing for "obvious" changes

### WHEN IN DOUBT
- Ask: "Does this help achieve Phase 0?"
- If no → don't do it
- If yes → do the simplest version

---

## Decision Log Template

When making non-trivial decisions, document them:

```markdown
### Decision: [Title]
**Date:** YYYY-MM-DD
**Context:** What situation required this decision?
**Options Considered:**
1. Option A - pros/cons
2. Option B - pros/cons
**Decision:** What was chosen and why
**Status:** Implemented / Pending / Reversed
```

---

## Glossary

| Term | Definition |
|------|------------|
| **Burial** | User covered by snow, light blocked |
| **Stillness** | No significant acceleration change |
| **Motion** | Acceleration above threshold |
| **Trigger** | Solenoid activation to deploy airbag |
| **False Positive** | Trigger when not buried |
| **False Negative** | No trigger when buried (catastrophic) |
| **Incapacitated** | User unable to manually deploy |

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 0.1 | 2025-01-XX | Initial manifesto |

---

*This document is the source of truth. If code contradicts this document, fix the code.*
