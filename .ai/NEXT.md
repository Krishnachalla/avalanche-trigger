# What To Do Next

> **FOR AI ASSISTANTS**: Read `.ai/AGENTS.md` first for full project context.
> This file is the single source of truth for priorities.
> Update this file when completing tasks or discovering new requirements.
>
> **FOR KRISHNA**: Just update this file to tell any AI what you want done.

---

## Current Phase: 0 (Working Prototype)

## Last Session: 2026-01-27 - Claude Code + Embedder

**Completed:**
- [x] GitHub ecosystem setup (Actions CI, issue templates, PR template)
- [x] README.md updated to current hardware
- [x] CHANGELOG.md created
- [x] Copilot instructions updated
- [x] GitHub labels created (phase:0, phase:1, firmware, hardware, blocked, ai-generated)
- [x] AI files reorganized into `.ai/` folder
- [x] docs/DECISIONS.md created for decision tracking
- [x] Redundant files deleted

**Next:** Commit and push all changes to GitHub.

---

## Immediate Priority

**Hardware testing** - I2C scanner found devices at 0x3C (OLED) and 0x51 (unknown), but NO IMU at 0x6A.

**Questions to resolve:**
1. Is the Sense expansion board attached to your XIAO ESP32-S3?
2. What device is at address 0x51?
3. Do you have the VEML7700 and LSM6DSOX sensors to connect?

---

## When External Sensors Arrive

### 1. I2C Sensor Test
- [ ] Wire VEML7700 and LSM6DSOX via STEMMA QT
- [ ] Run I2C scanner to verify addresses (0x10, 0x6A)
- [ ] Check sensor readings in serial output

### 2. First Boot Test
- [ ] Flash main firmware: `pio run --target upload`
- [ ] Open serial monitor: `pio device monitor`
- [ ] Verify "System ready - DISARMED" message

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

## Hardware Shopping List

See `docs/ENGINEERING_DOC.md` Section 8 for full BOM with links.

**Current Hardware (you have):**
- Seeed XIAO ESP32-S3 Sense

**Still Needed:**
1. Adafruit VEML7700 Light Sensor (~$5)
2. Adafruit LSM6DSOX IMU (~$12) - OR use onboard IMU if Sense board attached
3. STEMMA QT cables (~$3)
4. Breadboard + jumper wires

**Total remaining: ~$20-25**
