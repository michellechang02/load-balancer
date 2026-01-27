![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

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

Smoke test
----------
Run the included smoke script after building to verify a short run:

```bash
make
bash tools/run_smoke.sh
```

Publishing docs
---------------
Generated Doxygen HTML is currently in `docs/html`. Instead of committing
generated files to the repository, use the GitHub Action `Publish Doxygen`
(`.github/workflows/publish_docs.yml`) to build and publish docs to the
`gh-pages` branch. Run it from the Actions tab or via `workflow_dispatch`.
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
