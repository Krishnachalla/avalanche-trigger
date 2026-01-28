# EMBEDDER PROJECT CONTEXT

<OVERVIEW>
Name = Avalanche Airbag Auto-Trigger
Target MCU = ESP32-S3
Board = Seeed XIAO ESP32-S3 Sense
Toolchain = PlatformIO + Arduino Framework
Debug Interface = serial
RTOS / SDK = Arduino (ESP-IDF underneath)
Project Summary = Automatic avalanche burial detection and airbag deployment trigger
</OVERVIEW>

<COMMANDS>
# --- Build / Compile --------------------------------------------------------
build_command = pio run

# --- Flash ------------------------------------------------------------------
flash_command = pio run --target upload

# --- Clean ------------------------------------------------------------------
clean_command = pio run --target clean

# --- Debug ------------------------------------------------------------------
# No hardware debugger - using Serial.print() debugging only
# gdb_server_command = (not configured)
# gdb_server_host = localhost
# gdb_server_port = (not configured)
# gdb_client_command = (not configured)
# target_connection = (not configured)

# --- Serial Monitor ----------------------------------------------------------
serial_port = auto
serial_baudrate = 115200
serial_monitor_command = pio device monitor
serial_monitor_interactive = false
serial_encoding = ascii
serial_startup_commands = []
</COMMANDS>

## Project Overview

Safety-critical avalanche survival device that automatically deploys an airbag when a skier is buried and incapacitated.

**Detection Algorithm:**
1. Light drops below 50 lux (burial detected)
2. Recent motion detected within 60s (was skiing)
3. Stillness for 500ms (user incapacitated)
4. 500ms warning buzzer (user can cancel)
5. Solenoid triggers CO2 valve to deploy airbag

**Hardware:**
- MCU: Seeed XIAO ESP32-S3 Sense
- Light Sensor: Adafruit VEML7700 (I2C, 0x10)
- IMU: Adafruit LSM6DSOX (I2C, 0x6A)
- Output: MOSFET-driven solenoid (12V via MT3608 boost)
- Power: 3.7V LiPo battery

**State Machine:** DISARMED → ARMED → MONITORING → WARNING → TRIGGERED → DEPLOYED

**Phase:** Phase 0 (working prototype, not production-ready)

## Bash Commands

```bash
# Build firmware
pio run

# Flash to device
pio run --target upload

# Serial monitor (115200 baud)
pio device monitor

# Build + Flash + Monitor
pio run --target upload && pio device monitor

# Clean build artifacts
pio run --target clean

# List connected devices
pio device list

# Install dependencies
pio lib install
```

## Code Style

- **Language:** C++ with Arduino framework
- **Naming:** camelCase for variables/functions, UPPER_SNAKE_CASE for constants/macros
- **Config:** All tunable parameters in `include/config.h`
- **Comments:** Block comments for sections, inline for non-obvious logic
- **State machine:** Enum-based states with switch-case handling
- **Safety:** Solenoid defaults LOW (safe), explicit arming required
- **Debug:** Conditional compilation with `#if DEBUG_ENABLED`

## Pin Mapping (XIAO ESP32-S3 Sense)

| Function    | GPIO | XIAO Pin | Notes                    |
|-------------|------|----------|--------------------------|
| SDA         | 5    | D4       | I2C data (VEML7700, LSM6DSOX) |
| SCL         | 6    | D5       | I2C clock                |
| Solenoid    | 2    | D1       | MOSFET gate (active HIGH)|
| LED Green   | 3    | D2       | Status LED               |
| LED Red     | 4    | D3       | Status LED               |
| LED Blue    | 1    | D0       | Status LED               |
| Buzzer      | 7    | D6       | Warning buzzer           |
| Button      | 44   | D7       | Arm/disarm (active LOW)  |

## Key Files

| File | Purpose |
|------|---------|
| `src/main.cpp` | Main firmware, state machine, sensor logic |
| `include/config.h` | All tunable parameters (thresholds, timing, pins) |
| `platformio.ini` | Build config, library dependencies |
| `docs/PROJECT_MANIFESTO.md` | Philosophy, phased approach, hard rules |
| `docs/ENGINEERING_DOC.md` | Full technical specifications, BOM, circuits |

## Libraries

- `adafruit/Adafruit VEML7700 Library@^2.1.6` - Light sensor driver
- `adafruit/Adafruit LSM6DS@^4.7.2` - IMU driver (LSM6DSOX)
- `adafruit/Adafruit Unified Sensor@^1.1.14` - Sensor abstraction
- `adafruit/Adafruit BusIO@^1.14.5` - I2C/SPI abstraction

## Safety Notes

- **Bias toward triggering** - False deploy is annoying; missed deploy is fatal
- **Default safe state** - Solenoid always starts LOW, requires explicit arm
- **Manual override** - Button cancels trigger during WARNING state
- **Power loss** - System powers off safely (no trigger on brownout)
