# Claude Code Context

> **This project uses the AGENTS.md standard.**
> **Primary context file: `AGENTS.md`**

For full project context, read `AGENTS.md` first.

This file exists for Claude Code compatibility and contains any Claude-specific overrides.

---

## Claude-Specific Notes

- Use `pio` commands for building/flashing (not arduino-cli)
- Prefer editing `config.h` over hardcoding values in `main.cpp`
- Run `pio device monitor` to see debug output

## Collaboration Files

| File | Purpose |
|------|---------|
| `AGENTS.md` | Master context (read this first) |
| `NEXT.md` | Current priorities |
| `WORKLOG.md` | History (append to top when done) |

## Quick Reference

```bash
pio run                    # Build
pio run --target upload    # Flash
pio device monitor         # Serial monitor
```

---

*See `AGENTS.md` for full project documentation.*
