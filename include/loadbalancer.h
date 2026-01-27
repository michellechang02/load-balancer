/**
 * @file loadbalancer.h
 * @brief LoadBalancer that manages webservers and a request queue.
 *
 * The `LoadBalancer` holds a queue of incoming `Request`s and a pool of
 * `WebServer` instances. It is responsible for seeding an initial full
 * queue, accepting new requests (randomly generated), distributing work to
 * idle servers, and scaling the number of servers up/down based on load.
 */
#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "request.h"
#include "webserver.h"

#include <queue>
#include <vector>
#include <random>
#include <string>

/**
 * @brief Controller for the load balancer simulation.
 */
class LoadBalancer {
public:
    /**
     * @brief Construct a new LoadBalancer
     *
     * @param initialServers Number of servers to start with.
     * @param rngSeed Optional RNG seed (0 = nondeterministic seed from device).
     */
    LoadBalancer(int initialServers, unsigned rngSeed = 0, bool quiet = false, bool json = false);

    /**
     * @brief Fill the initial queue to capacity (servers * 5 requests).
     *
     * This populates the internal `requestQueue_` with randomly generated
     * requests so the simulation begins with a full backlog.
     */
    void fillInitialQueue();

    /**
     * @brief Run the simulation for a given number of cycles.
     *
     * The simulation advances in discrete cycles. During each cycle the
     * load balancer may add newly arriving requests, distribute queued
     * requests to idle servers, advance server processing by one cycle,
     * and trigger scaling logic.
     *
     * @param totalCycles Number of cycles to run the simulation.
     */
    void run(double totalCycles);

    // Accessors useful for tests
    int serverCount() const;
    size_t queuedCount() const;

private:
    /**
     * @brief Execute a single simulation step (advance by dt cycles).
     * @param dt Number of cycles to advance (typically 1).
     */
    void step(double dt);

    /**
     * @brief Assign queued requests to any idle servers.
     *
     * This method pops requests from `requestQueue_` and assigns them to
     * idle `WebServer` instances until no idle servers or no queued
     * requests remain.
     */
    void distribute();

    /**
     * @brief Possibly add a newly arrived request based on RNG.
     *
     * If the RNG schedules an arrival at or before `now`, this function
     * generates a `Request` (with IPs and service cycles) and enqueues it.
     *
     * @param now Current simulation cycle.
     */
    void maybeAddNewRequest(double now);

    /**
     * @brief Scale the number of servers up or down based on queue per server.
     *
     * Adds a server when the queue-per-server exceeds a high threshold and
     * removes an idle server when the queue-per-server falls below a low
     * threshold.
     */
    void scaleServers();

    std::queue<Request> requestQueue_;
    std::vector<WebServer> servers_;

    // Random generators
    std::mt19937 rng_;
    std::exponential_distribution<double> interArrival_; // mean arrival rate (in cycles)
    std::uniform_int_distribution<int> serviceDist_; // service time in cycles (int)
    std::uniform_int_distribution<int> octetDist_; // for generating IP addresses

    // Simulation bookkeeping
    uint64_t nextRequestId_;
    double nextArrivalTime_;
    double now_;

    // Output control
    bool quiet_;
    bool json_;
};

#endif // LOADBALANCER_H
