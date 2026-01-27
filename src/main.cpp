/**
 * @file main.cpp
 * @brief Simulation driver for the load balancer demo.
 */

#include <iostream>
#include <string>
#include <sstream>
#include "loadbalancer.h"

static void printUsage(const char *prog) {
    std::cout << "Usage: " << prog << " [--seed N] [--quiet] [--json] [--servers N] [--duration N]\n";
}

int main(int argc, char **argv) {
    int servers = 2;
    double duration = 30.0;
    unsigned seed = 0;
    bool quiet = false;
    bool json = false;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--seed" && i + 1 < argc) { seed = static_cast<unsigned>(std::stoul(argv[++i])); }
        else if (a == "--quiet") { quiet = true; }
        else if (a == "--json") { json = true; }
        else if (a == "--servers" && i + 1 < argc) { servers = std::stoi(argv[++i]); }
        else if (a == "--duration" && i + 1 < argc) { duration = std::stod(argv[++i]); }
        else if (a == "-h" || a == "--help") { printUsage(argv[0]); return 0; }
        else {
            std::cerr << "Unknown arg: " << a << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    LoadBalancer lb(servers, seed, quiet, json);
    lb.fillInitialQueue();
    lb.run(duration);

    return 0;
}
