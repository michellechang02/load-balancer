#!/usr/bin/env bash
set -euo pipefail

if [ ! -x "$(pwd)/lb_sim" ]; then
  echo "Binary lb_sim not found or not executable"
  exit 2
fi

printf "2\n5\n" | ./lb_sim | tee smoke_output.txt
grep -q "Simulation complete" smoke_output.txt
echo "Smoke test passed"
