# Load Balancer Simulation

This project simulates a simple load balancer distributing `Request`s to
`WebServer` instances. The simulation runs in discrete integer cycles and
supports dynamic scaling of servers based on queue depth.

Directory layout
- `include/` — public headers (`request.h`, `webserver.h`, `loadbalancer.h`)
- `src/` — implementation sources and `main.cpp`
- `Makefile` — build helper
- `Doxyfile` — minimal configuration for generating HTML docs

Build
-----
Requires a C++17 compiler. From project root:

```bash
make
```

Run
---
```bash
./lb_sim
# then enter: <initial_servers> <duration_in_cycles>
```

Examples
--------
Run with 3 servers for 100 cycles:

```bash
printf "3
100
" | ./lb_sim
```

Generate documentation
----------------------
Install Doxygen (macOS Homebrew):

```bash
brew install doxygen
doxygen Doxyfile
```

The HTML docs will be in `docs/html/index.html`.

Notes
-----
- The simulation logs assignments, completions, scaling events, and per-cycle
  statuses to stdout. Redirect to a file to capture long runs.
- Adjust service/arrival parameters in `src/loadbalancer.cpp` for different
  workloads and to experiment with scaling thresholds.
