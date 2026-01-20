#!/usr/bin/env bash
# Run the live server from the web/ folder while resolving project root
# Usage: ./run_live.sh --servers 3 --duration 100 --port 8000

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

python3 "$PROJECT_ROOT/tools/live_server.py" "$@"
