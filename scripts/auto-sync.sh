#!/bin/bash
#
# auto-sync.sh - Automatically sync project changes to GitHub
#
# Usage:
#   ./scripts/auto-sync.sh              # One-time sync
#   ./scripts/auto-sync.sh --watch      # Continuous watch mode
#   ./scripts/auto-sync.sh --daemon     # Background daemon
#
# What it does:
#   1. Stages all changes
#   2. Commits with auto-generated message
#   3. Pushes to origin/main
#
# Best for: Keeping GitHub in sync during AI-assisted sessions
#

set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_DIR"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log() {
    echo -e "${GREEN}[sync]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[sync]${NC} $1"
}

error() {
    echo -e "${RED}[sync]${NC} $1"
}

# Generate commit message from changed files
generate_commit_message() {
    local changes=$(git diff --cached --name-only 2>/dev/null | head -5)
    local count=$(git diff --cached --name-only 2>/dev/null | wc -l | tr -d ' ')
    local timestamp=$(date +"%Y-%m-%d %H:%M")
    
    if [ -z "$changes" ]; then
        echo "Auto-sync: no changes"
        return
    fi
    
    # Get first changed file for context
    local first_file=$(echo "$changes" | head -1)
    local dir=$(dirname "$first_file")
    
    if [ "$count" -eq 1 ]; then
        echo "Auto-sync: $first_file [$timestamp]"
    elif [ "$count" -le 3 ]; then
        echo "Auto-sync: $count files in $dir [$timestamp]"
    else
        echo "Auto-sync: $count files [$timestamp]"
    fi
}

# Single sync operation
do_sync() {
    # Check for changes
    if git diff --quiet && git diff --cached --quiet && [ -z "$(git ls-files --others --exclude-standard)" ]; then
        log "No changes to sync"
        return 0
    fi
    
    # Stage all changes
    git add -A
    
    # Check if there's anything to commit
    if git diff --cached --quiet; then
        log "Nothing staged to commit"
        return 0
    fi
    
    # Generate message and commit
    local msg=$(generate_commit_message)
    log "Committing: $msg"
    git commit -m "$msg"
    
    # Push
    log "Pushing to origin/main..."
    if git push origin main 2>&1; then
        log "Sync complete!"
    else
        error "Push failed - may need manual intervention"
        return 1
    fi
}

# Watch mode - sync on file changes
watch_mode() {
    log "Starting watch mode (Ctrl+C to stop)..."
    log "Watching: $PROJECT_DIR"
    
    # Check if fswatch is available
    if ! command -v fswatch &> /dev/null; then
        warn "fswatch not found. Install with: brew install fswatch"
        warn "Falling back to polling mode (every 30 seconds)..."
        
        while true; do
            do_sync
            sleep 30
        done
    else
        # Use fswatch for efficient file watching
        fswatch -o \
            --exclude '\.git' \
            --exclude '\.pio' \
            --exclude '__pycache__' \
            --exclude '\.DS_Store' \
            "$PROJECT_DIR" | while read -r; do
            sleep 2  # Debounce
            do_sync
        done
    fi
}

# Daemon mode - run in background
daemon_mode() {
    local pid_file="/tmp/avalanche-sync.pid"
    local log_file="$PROJECT_DIR/.sync.log"
    
    if [ -f "$pid_file" ]; then
        local old_pid=$(cat "$pid_file")
        if kill -0 "$old_pid" 2>/dev/null; then
            error "Daemon already running (PID: $old_pid)"
            error "Stop with: kill $old_pid"
            exit 1
        fi
    fi
    
    log "Starting daemon mode..."
    log "Log file: $log_file"
    log "PID file: $pid_file"
    
    nohup "$0" --watch > "$log_file" 2>&1 &
    echo $! > "$pid_file"
    
    log "Daemon started (PID: $!)"
    log "Stop with: kill $(cat $pid_file)"
}

# Main
case "${1:-}" in
    --watch|-w)
        watch_mode
        ;;
    --daemon|-d)
        daemon_mode
        ;;
    --help|-h)
        echo "Usage: $0 [--watch|--daemon|--help]"
        echo ""
        echo "Options:"
        echo "  (none)    One-time sync"
        echo "  --watch   Continuous watch mode"
        echo "  --daemon  Run in background"
        echo "  --help    Show this help"
        ;;
    *)
        do_sync
        ;;
esac
