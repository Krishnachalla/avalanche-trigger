#!/bin/bash
#
# save-session.sh - Save AI chat session to docs and sync to GitHub
#
# Usage:
#   ./scripts/save-session.sh "Brief description of what was done"
#
# This creates:
#   - docs/sessions/YYYY-MM-DD_HH-MM_session.md
#   - Updates WORKLOG.md
#   - Commits and pushes to GitHub
#

set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_DIR"

TIMESTAMP=$(date +"%Y-%m-%d")
TIMESTAMP_FULL=$(date +"%Y-%m-%d_%H-%M")
SESSION_FILE="docs/sessions/${TIMESTAMP_FULL}_session.md"
DESCRIPTION="${1:-AI session}"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log() {
    echo -e "${GREEN}[session]${NC} $1"
}

# Create sessions directory if needed
mkdir -p docs/sessions

# Create session file
log "Creating session file: $SESSION_FILE"

cat > "$SESSION_FILE" << EOF
# Session Log - $TIMESTAMP_FULL

## Summary
$DESCRIPTION

## Session Details

**Date:** $TIMESTAMP
**Tool:** Embedder / AI Assistant
**Project:** Avalanche Trigger

---

## Changes Made

<!-- List changes below -->

---

## Files Modified

\`\`\`
$(git status --short 2>/dev/null || echo "No git changes detected")
\`\`\`

---

## Notes

<!-- Add any notes here -->

---

*Session saved at $(date +"%H:%M:%S")*
EOF

log "Session file created"

# Update WORKLOG.md
log "Updating WORKLOG.md..."

# Create temp file with new entry
TEMP_FILE=$(mktemp)
cat > "$TEMP_FILE" << EOF
## $TIMESTAMP - AI Session
**$DESCRIPTION**

Session log: \`$SESSION_FILE\`

**Files changed:**
$(git status --short 2>/dev/null | head -10 | sed 's/^/- /' || echo "- (none detected)")

**Status:** Session saved

---

EOF

# Prepend to WORKLOG.md (after the header)
if [ -f "WORKLOG.md" ]; then
    # Find line after first "---" and insert there
    head -6 WORKLOG.md > "${TEMP_FILE}.new"
    cat "$TEMP_FILE" >> "${TEMP_FILE}.new"
    tail -n +7 WORKLOG.md >> "${TEMP_FILE}.new"
    mv "${TEMP_FILE}.new" WORKLOG.md
fi

rm -f "$TEMP_FILE"

log "WORKLOG.md updated"

# Git operations
log "Committing and pushing..."

git add -A
git commit -m "Session: $DESCRIPTION [$TIMESTAMP_FULL]" || {
    log "Nothing to commit"
    exit 0
}

git push origin main && log "Pushed to GitHub!" || {
    echo -e "${YELLOW}[session]${NC} Push failed - run 'git push' manually"
}

log "Done! Session saved to $SESSION_FILE"
