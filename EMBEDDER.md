# EMBEDDER PROJECT CONTEXT

<OVERVIEW>
Name = Avalanche Airbag Auto-Trigger
Target MCU = ESP32-C3
Board = Seeed XIAO ESP32-C3
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
- MCU: Seeed XIAO ESP32-C3
- Light Sensor: Grove BH1750 (I2C, 0x23)
- IMU: Grove LSM6DS3 (I2C, 0x6A)
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

## Pin Mapping (XIAO ESP32-C3)

| Function    | GPIO | XIAO Pin | Notes                    |
|-------------|------|----------|--------------------------|
| SDA         | 6    | D4       | I2C data (BH1750, LSM6DS3) |
| SCL         | 7    | D5       | I2C clock                |
| Solenoid    | 4    | D2       | MOSFET gate (active HIGH)|
| LED Green   | 5    | D3       | Status LED               |
| LED Red     | 20   | D6       | Status LED               |
| LED Blue    | 21   | D7       | Status LED               |
| Buzzer      | 8    | D8       | Warning buzzer           |
| Button      | 9    | D9       | Arm/disarm (active LOW)  |

## Key Files

| File | Purpose |
|------|---------|
| `src/main.cpp` | Main firmware, state machine, sensor logic |
| `include/config.h` | All tunable parameters (thresholds, timing, pins) |
| `platformio.ini` | Build config, library dependencies |
| `docs/PROJECT_MANIFESTO.md` | Philosophy, phased approach, hard rules |
| `docs/ENGINEERING_DOC.md` | Full technical specifications, BOM, circuits |

## Libraries

- `claws/BH1750@^1.3.0` - Light sensor driver
- `adafruit/Adafruit LSM6DS@^4.7.2` - IMU driver
- `adafruit/Adafruit Unified Sensor@^1.1.14` - Sensor abstraction
- `adafruit/Adafruit BusIO@^1.14.5` - I2C/SPI abstraction

## Safety Notes

- **Bias toward triggering** - False deploy is annoying; missed deploy is fatal
- **Default safe state** - Solenoid always starts LOW, requires explicit arm
- **Manual override** - Button cancels trigger during WARNING state
- **Power loss** - System powers off safely (no trigger on brownout)
