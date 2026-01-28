# What To Do Next

> **FOR AI ASSISTANTS**: This is the single source of truth for priorities.
> Update this file when completing tasks or discovering new requirements.
> 
> **FOR KRISHNA**: Just update this file to tell any AI what you want done.

---

## Current Phase: 0 (Working Prototype)

## Immediate Priority
**Waiting for hardware** - User needs to order components before proceeding.

## When Hardware Arrives

### 1. First Boot Test
- [ ] Connect XIAO ESP32-C3 via USB
- [ ] Flash firmware: `pio run --target upload`
- [ ] Open serial monitor: `pio device monitor`
- [ ] Verify "System ready - DISARMED" message

### 2. I2C Sensor Test
- [ ] Wire BH1750 and LSM6DS3 to I2C bus
- [ ] Run I2C scanner to verify addresses (0x23, 0x6A)
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
