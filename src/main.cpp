/**
 * @file main.cpp
 * @brief Simulation driver for the load balancer demo.
 */

#include <iostream>
#include "loadbalancer.h"

int main() {
    int servers = 2;
    double duration = 30.0;

    std::cout << "Enter initial number of servers: ";
    if (!(std::cin >> servers)) return 1;
    std::cout << "Enter simulation duration (seconds): ";
    if (!(std::cin >> duration)) return 1;

    LoadBalancer lb(servers);
    lb.fillInitialQueue();
    lb.run(duration);

    return 0;
}
