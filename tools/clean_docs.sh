#!/usr/bin/env bash
set -euo pipefail

if [ -d "docs/html" ]; then
  echo "Removing tracked docs/html directory. Commit the removal to history if desired."
  rm -rf docs/html
else
  echo "No docs/html directory found."
fi
