/**
 * @file request.h
 * @brief Request definition for the load balancer simulation.
 *
 * This file defines the `Request` struct which represents an incoming
 * web request for the simulation. Each request carries source/destination
 * IPs, an arrival cycle, and the number of cycles required to service it.
 */
#ifndef REQUEST_H
#define REQUEST_H

#include <cstdint>
#include <string>

/**
 * @brief Represents a single web request.
 *
 * @var Request::id
 * Unique request identifier assigned by the `LoadBalancer`.
 *
 * @var Request::arrival
 * Cycle at which the request arrived (integer cycle count since simulation
 * start).
 *
 * @var Request::serviceCycles
 * Number of integer cycles required to service the request. The simulation
 * enforces a minimum service time (typically >= 3 cycles).
 *
 * @var Request::ip_in
 * Source IP address string (dotted-quad).
 *
 * @var Request::ip_out
 * Destination IP address string (dotted-quad).
 */
struct Request {
    uint64_t id;           /**< Unique request id */
    int arrival;           /**< Arrival time (integer cycles since simulation start) */
    int serviceCycles;     /**< Service time required (integer cycles) */
    std::string ip_in;     /**< Source IP address */
    std::string ip_out;    /**< Destination IP address */
};

#endif // REQUEST_H
