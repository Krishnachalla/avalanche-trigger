---
name: Hardware Issue
about: Report a hardware problem, wiring issue, or component failure
title: '[HARDWARE] '
labels: hardware
assignees: ''
---

## Hardware Issue Description
Describe the hardware problem.

## Components Involved
- [ ] MCU (XIAO ESP32-S3)
- [ ] Light sensor (VEML7700)
- [ ] IMU (LSM6DSOX)
- [ ] Solenoid/MOSFET circuit
- [ ] Power supply (LiPo/boost converter)
- [ ] Buzzer
- [ ] LEDs
- [ ] Button
- [ ] Wiring/connections
- [ ] Other: ___

## Symptoms
- What's happening?
- Any error messages in serial output?
- Any physical signs (LEDs, sounds, heat)?

## Diagnostic Steps Taken
What have you already tried?

1.
2.

## I2C Scan Results (if applicable)
```
Paste I2C scanner output here
```

## Wiring Photo
If possible, attach a photo of your wiring setup.

## Multimeter Readings (if applicable)
- Voltage at ___: ___ V
- Continuity check: ___

## Additional Context
Any other information that might help diagnose the issue?

---
*Tip: Run the I2C scanner (`src/i2c_scanner.cpp`) to verify sensor connections.*
