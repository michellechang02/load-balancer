/**
 * @file loadbalancer.cpp
 * @brief Implementation of the LoadBalancer simulation.
 */

#include "loadbalancer.h"

#include <iostream>
#include <iomanip>

LoadBalancer::LoadBalancer(int initialServers, unsigned rngSeed, bool quiet, bool json)
        : rng_(rngSeed == 0 ? (unsigned)std::random_device{}() : rngSeed),
            interArrival_(1.0 / 1.0), // average 1 arrival per cycle by default
            serviceDist_(3, 8),       // service times between 3 and 8 cycles (min 3)
            octetDist_(1, 254),
            nextRequestId_(1),
            nextArrivalTime_(0.0),
            now_(0.0),
            quiet_(quiet),
            json_(json) {
    for (int i = 0; i < initialServers; ++i) servers_.emplace_back(i + 1);
    nextArrivalTime_ = 1; // first arrival in cycle 1
}

void LoadBalancer::fillInitialQueue() {
    size_t target = servers_.size() * 5;
    for (size_t i = 0; i < target; ++i) {
        Request r;
        r.id = nextRequestId_++;
        r.arrival = static_cast<int>(now_);
        r.serviceCycles = serviceDist_(rng_);
        r.ip_in = std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_));
        r.ip_out = std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_));
        requestQueue_.push(r);
    }
}

void LoadBalancer::maybeAddNewRequest(double now) {
    if (static_cast<int>(now) >= static_cast<int>(nextArrivalTime_)) {
        Request r;
        r.id = nextRequestId_++;
        r.arrival = static_cast<int>(now);
        r.serviceCycles = serviceDist_(rng_);
        r.ip_in = std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_));
        r.ip_out = std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_)) + "." + std::to_string(octetDist_(rng_));
        requestQueue_.push(r);
        int nextDelta = std::max(1, static_cast<int>(std::ceil(interArrival_(rng_))));
        nextArrivalTime_ = now + nextDelta;
    }
}

void LoadBalancer::distribute() {
    for (auto &s : servers_) {
        if (!s.isBusy() && !requestQueue_.empty()) {
            Request r = requestQueue_.front();
            requestQueue_.pop();
            if (!quiet_) {
                if (json_) {
                    std::cout << "{\"event\":\"assign\",\"server\":" << s.getId()
                              << ",\"req\":" << r.id << ",\"ip_in\":\"" << r.ip_in << "\",\"ip_out\":\"" << r.ip_out << "\",\"cycles\":" << r.serviceCycles << "}\n";
                } else {
                    std::cout << "[assign] server=" << s.getId() << " req=" << r.id << " ip_in=" << r.ip_in << " ip_out=" << r.ip_out << " cycles=" << r.serviceCycles << "\n";
                }
            }
            s.assignRequest(r);
        }
    }
}

void LoadBalancer::scaleServers() {
    double queuePerServer = servers_.empty() ? (double)requestQueue_.size() : (double)requestQueue_.size() / servers_.size();
    if (queuePerServer > 4.0) {
        int newid = (int)servers_.size() + 1;
        servers_.emplace_back(newid);
        if (!quiet_) {
            if (json_) std::cout << "{\"event\":\"scale_add\",\"server\":" << newid << ",\"servers\":" << servers_.size() << "}\n";
            else std::cout << "[scale] Added server " << newid << " (servers=" << servers_.size() << ")\n";
        }
    } else if (queuePerServer < 1.0 && servers_.size() > 1) {
        if (!servers_.back().isBusy()) {
            int id = servers_.back().getId();
            servers_.pop_back();
            if (!quiet_) {
                if (json_) std::cout << "{\"event\":\"scale_remove\",\"server\":" << id << ",\"servers\":" << servers_.size() << "}\n";
                else std::cout << "[scale] Removed server " << id << " (servers=" << servers_.size() << ")\n";
            }
        }
    }
}

void LoadBalancer::step(double dt) {
    now_ += dt;
    maybeAddNewRequest(now_);
    distribute();
    for (auto &s : servers_) {
        bool finished = s.tick();
        if (finished) {
            if (!quiet_) {
                if (json_) std::cout << "{\"event\":\"done\",\"server\":" << s.getId() << ",\"t\":" << now_ << "}\n";
                else std::cout << "[done] server=" << s.getId() << " at t=" << now_ << "\n";
            }
        }
    }
    if (static_cast<int>(now_ * 10) % 10 == 0) {
        scaleServers();
    }
}

void LoadBalancer::run(double totalSeconds) {
    double dt = 1.0; // one cycle per tick
    std::cout << std::fixed << std::setprecision(0);
    if (json_) std::cout << "{\"event\":\"start\",\"servers\":" << servers_.size() << ",\"duration\":" << totalSeconds << "}\n";
    else if (!quiet_) std::cout << "Starting simulation: servers=" << servers_.size() << ", duration=" << totalSeconds << "s\n";
    while (now_ < totalSeconds) {
        step(dt);
        if (json_) std::cout << "{\"event\":\"tick\",\"t\":" << now_ << ",\"servers\":" << servers_.size() << ",\"queued\":" << requestQueue_.size() << "}\n";
        else if (!quiet_) std::cout << "t=" << now_ << " | servers=" << servers_.size() << " | queued=" << requestQueue_.size() << "\n";
    }
    if (json_) std::cout << "{\"event\":\"complete\",\"queued\":" << requestQueue_.size() << ",\"servers\":" << servers_.size() << "}\n";
    else if (!quiet_) std::cout << "Simulation complete. Final queued=" << requestQueue_.size() << " servers=" << servers_.size() << "\n";
}

int LoadBalancer::serverCount() const { return static_cast<int>(servers_.size()); }

size_t LoadBalancer::queuedCount() const { return requestQueue_.size(); }
