# Avalanche Airbag Auto-Trigger

> A README for AI agents. Any AI tool can contribute by following this protocol.

---

## Commands

```bash
pio run                    # Build firmware
pio run --target upload    # Flash to device
pio device monitor         # Serial monitor (115200 baud)
pio run --target clean     # Clean build artifacts
```

---

## Project Overview

**What:** Safety-critical embedded device that automatically deploys an avalanche airbag when a skier is buried and incapacitated.

**Why:** Survival window after burial is short. If a skier is knocked unconscious, they can't manually deploy. This device detects burial and triggers automatically.

**How:** Light sensor detects darkness (burial) + IMU confirms stillness (incapacitation) = solenoid fires to deploy airbag.

**Phase:** 0 (Working Prototype) - Focus on "does it work at all?" not optimization.

---

## Hardware

| Component | Model | Interface | Address |
|-----------|-------|-----------|---------|
| MCU | Seeed XIAO ESP32-S3 Sense | - | - |
| Light Sensor | Adafruit VEML7700 | I2C | 0x10 |
| IMU | Adafruit LSM6DSOX | I2C | 0x6A |
| Solenoid Driver | IRLZ44N MOSFET | GPIO | - |
| Power | 3.7V LiPo + MT3608 boost | - | 12V out |

### Pin Mapping (XIAO ESP32-S3)

| Function | GPIO | Notes |
|----------|------|-------|
| I2C SDA | 5 | Shared by sensors |
| I2C SCL | 6 | Shared by sensors |
| Solenoid | 2 | MOSFET gate (active HIGH) |
| Button | 44 | Arm/cancel (INPUT_PULLUP) |
| Buzzer | 7 | Warning tone |
| LED Blue | 1 | Status |
| LED Green | 3 | Status |
| LED Red | 4 | Status |

---

## Detection Algorithm

```
ARMED → Light < 50 lux (buried)
      → Had motion in last 60s (was skiing)
      → Still for 500ms (incapacitated)
      → 500ms warning buzzer (user can cancel)
      → TRIGGER solenoid (300ms pulse)
```

**Total time from burial to trigger: ~1.0-1.5 seconds**

### State Machine

```
DISARMED → ARMED → MONITORING → WARNING → TRIGGERED → DEPLOYED
    ↑__________|        ↑__________|
    (button)            (button cancels)
```

---

## Key Configuration

All tunable parameters in `include/config.h`:

| Parameter | Value | Purpose |
|-----------|-------|---------|
| BURIAL_LUX_THRESHOLD | 50 | Light level for burial detection |
| STILLNESS_THRESHOLD_MS | 500 | Time still before trigger |
| WARNING_DURATION_MS | 500 | Buzzer warning before trigger |
| MOTION_THRESHOLD | 0.3g | Acceleration change for motion |
| MOTION_HISTORY_MS | 60000 | Recent motion window |
| SOLENOID_PULSE_MS | 300 | Trigger pulse duration |

---

## Code Style

- **Language:** C++ with Arduino framework (ESP-IDF underneath)
- **Naming:** `camelCase` functions, `UPPER_SNAKE_CASE` constants
- **Config:** All tunable parameters in `config.h`, never hardcoded
- **State machine:** Enum-based states with switch-case
- **Safety:** Solenoid defaults LOW, explicit arming required
- **Debug:** Conditional compilation with `#if DEBUG_ENABLED`

---

## Project Structure

```
├── src/main.cpp              # Main firmware, state machine
├── include/config.h          # All tunable parameters
├── platformio.ini            # Build config, dependencies
├── README.md                 # Project overview
├── CHANGELOG.md              # Release history
├── .ai/                      # AI assistant context
│   ├── AGENTS.md             # This file (AI context)
│   ├── EMBEDDER.md           # Embedder-specific config
│   ├── NEXT.md               # Current priorities
│   └── WORKLOG.md            # History of changes
├── docs/
│   ├── PROJECT_MANIFESTO.md  # Philosophy, phases, hard rules
│   ├── ENGINEERING_DOC.md    # Full specs, BOM, circuits
│   ├── DECISIONS.md          # Decision history
│   └── debug/                # Debug session logs
```

---

## Testing Sequence

When hardware is available, test incrementally:

1. **I2C scan** - Verify sensor addresses (0x10, 0x6A)
2. **Sensor readings** - Light values, accelerometer data
3. **Output test** - LEDs, buzzer, solenoid (use LED first, not actual solenoid)
4. **State transitions** - Button arm/disarm, state changes
5. **Full sequence** - Cover sensor, stay still, verify trigger

---

## Safety Constraints

### ALWAYS
- Bias toward triggering (false deploy beats missed deploy)
- Keep manual override functional
- Default to safe state (solenoid LOW)
- Test on real hardware when possible

### NEVER
- Trigger on power-up, boot, or error
- Remove manual trigger capability
- Add features not in current phase
- Optimize before it works

---

## AI Collaboration Protocol

### Shared Memory Files

| File | Purpose | Who Updates |
|------|---------|-------------|
| `.ai/AGENTS.md` | Project context (this file) | Update when architecture changes |
| `docs/DECISIONS.md` | Decision history & evolution | Append when decisions change |
| `.ai/NEXT.md` | Current priorities & tasks | Update when completing/adding tasks |
| `.ai/WORKLOG.md` | History of changes | Append to TOP after each session |

### Before Working

1. Read `.ai/AGENTS.md` (this file)
2. Read `docs/DECISIONS.md` for decision history and evolution
3. Read `.ai/NEXT.md` for current priority
4. Read relevant source files

### After Working

1. Update `.ai/WORKLOG.md` (append to TOP):
   ```markdown
   ## YYYY-MM-DD - [Tool Name]
   **Summary**
   - What was done

   **Files changed:**
   - `path/file` - Description

   **Status:** Current state / Next steps
   ```
2. Update `.ai/NEXT.md` if priorities changed
3. Update `.ai/AGENTS.md` if architecture changed

### When Stuck

- Don't guess - ask the user
- Document blockers in `.ai/NEXT.md`

---

## Tool-Specific Files

Some tools have additional context files:

| File | Tool | Purpose |
|------|------|---------|
| `.ai/EMBEDDER.md` | Embedder | Build commands, pin mapping |
| `.github/copilot-instructions.md` | GitHub Copilot | Copilot-specific rules |

These supplement, not replace, this file.

---

## Links

- **GitHub:** https://github.com/Krishnachalla/avalanche-trigger
- **Full specs:** `docs/ENGINEERING_DOC.md`
- **Philosophy:** `docs/PROJECT_MANIFESTO.md`

---

*This file follows the [AGENTS.md standard](https://agents.md/) for AI coding assistant interoperability.*
