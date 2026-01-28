# What To Do Next

> **FOR AI ASSISTANTS**: Read `.ai/AGENTS.md` first for full project context.
> This file is the single source of truth for priorities.
> Update this file when completing tasks or discovering new requirements.
>
> **FOR KRISHNA**: Just update this file to tell any AI what you want done.

---

## ACTIVE SESSION: 2026-01-27 - Claude Code

**What's happening:** Claude Code is performing a major project reorganization based on research into GitHub ecosystem best practices and AGENTS.md standard.

**Why:** To establish proper multi-AI collaboration, fix outdated documentation, set up CI/CD, and clean up redundant files.

**Tasks in progress:**
- [ ] Fix README.md (update to current hardware)
- [ ] Create CHANGELOG.md
- [ ] Create GitHub Actions workflow (auto-build)
- [ ] Create issue templates (bug, feature, hardware)
- [ ] Merge/delete redundant docs
- [ ] Update copilot-instructions.md
- [ ] Set up GitHub Issues + Labels

**If you're Embedder:** Wait for this reorganization to complete, or read `.ai/WORKLOG.md` for latest state. Hardware info is changing from ESP32-C3 → ESP32-S3, BH1750 → VEML7700, LSM6DS3 → LSM6DSOX.

---

## Current Phase: 0 (Working Prototype)

## After Reorganization
**Waiting for hardware** - User needs to order/connect components before firmware testing.

## When Hardware Arrives

### 1. First Boot Test
- [ ] Connect XIAO ESP32-S3 Sense via USB
- [ ] Flash firmware: `pio run --target upload`
- [ ] Open serial monitor: `pio device monitor`
- [ ] Verify "System ready - DISARMED" message

### 2. I2C Sensor Test
- [ ] Wire VEML7700 and LSM6DSOX via STEMMA QT
- [ ] Run I2C scanner to verify addresses (0x10, 0x6A)
- [ ] Check sensor readings in serial output

### 3. Output Test
- [ ] Test LEDs (should show blue when disarmed)
- [ ] Test buzzer during WARNING state
- [ ] Test solenoid with LED first (not actual solenoid)

### 4. Integration Test
- [ ] Arm system with button
- [ ] Cover light sensor with hand
- [ ] Stay still for 1 second
- [ ] Verify warning buzzer sounds
- [ ] Press button to cancel before trigger

---

## Backlog (Future Phases)

### Phase 1 - Calibration
- [ ] Test with actual snow (light threshold tuning)
- [ ] Tune motion sensitivity
- [ ] Add battery monitoring

### Phase 2 - Reliability
- [ ] Add watchdog timer
- [ ] Implement brown-out detection
- [ ] Add self-test on boot

### Phase 3 - Power Optimization
- [ ] Implement deep sleep
- [ ] Reduce sensor polling rate when idle
- [ ] Target 8+ hour battery life

---

## Shopping List (BOM)
See `docs/ENGINEERING_DOC.md` Section 8 for full BOM with links.

**Minimum to start:**
1. Seeed XIAO ESP32-C3 (~$5)
2. Grove BH1750 Light Sensor (~$6)
3. Grove LSM6DS3 IMU (~$12)
4. Grove I2C Hub (~$3)
5. USB-C cable
6. Breadboard + jumper wires

**Total for basic testing: ~$30**
