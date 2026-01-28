# Scripts

Automation scripts for the Avalanche Trigger project.

## Available Scripts

### `auto-sync.sh` - Continuous GitHub Sync

Automatically commits and pushes changes to GitHub.

```bash
# One-time sync (run manually)
./scripts/auto-sync.sh

# Watch mode - syncs whenever files change
./scripts/auto-sync.sh --watch

# Daemon mode - runs in background
./scripts/auto-sync.sh --daemon
```

**Best practice:** Run `--watch` in a separate terminal during AI sessions.

### `save-session.sh` - Save Chat Session

Saves AI chat session with description and pushes to GitHub.

```bash
./scripts/save-session.sh "I2C debugging and serial verification"
```

Creates:
- `docs/sessions/YYYY-MM-DD_HH-MM_session.md`
- Updates `WORKLOG.md`
- Commits and pushes

---

## Recommended Workflow

### Before Starting AI Session

```bash
# Start auto-sync in background
./scripts/auto-sync.sh --daemon
```

### After AI Session

```bash
# Save session with description
./scripts/save-session.sh "What you worked on"
```

### Or: Manual One-Time Sync

```bash
./scripts/auto-sync.sh
```

---

## Setup

Make scripts executable:

```bash
chmod +x scripts/*.sh
```

Optional - install fswatch for efficient file watching:

```bash
brew install fswatch
```

---

## How It Works

1. **auto-sync.sh** watches for file changes (using fswatch or polling)
2. When changes detected, it:
   - Stages all files (`git add -A`)
   - Creates auto-generated commit message
   - Pushes to `origin/main`

3. **save-session.sh** creates a structured session log and syncs

---

## Files Ignored

These are excluded from sync:
- `.git/`
- `.pio/` (PlatformIO build artifacts)
- `__pycache__/`
- `.DS_Store`

---

## Troubleshooting

**Push fails:**
```bash
git pull --rebase origin main
git push origin main
```

**Daemon already running:**
```bash
kill $(cat /tmp/avalanche-sync.pid)
```

**Check daemon log:**
```bash
tail -f .sync.log
```
