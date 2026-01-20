/**
 * @file webserver.h
 * @brief WebServer class representing a server that processes requests.
 *
 * `WebServer` models a single processing unit that receives `Request`
 * objects from the `LoadBalancer`, consumes integer cycles to complete
 * them, and then becomes available for another request.
 */
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "request.h"

/**
 * @brief A simple simulated web server.
 */
class WebServer {
public:
    /**
     * @brief Construct a new WebServer
     * @param id Server identifier (unique within a `LoadBalancer`).
     */
    explicit WebServer(int id);

    /**
     * @brief Assign a request to this server.
     *
     * The server records the request and its required `serviceCycles` and
     * becomes busy until those cycles have elapsed.
     *
     * @param r Request object to process (copied into server state).
     */
    void assignRequest(const Request &r);

    /**
     * @brief Advance the server by one simulation cycle.
     *
     * This decrements the server's remaining cycles for the current
     * request. If the request completes on this tick the server becomes
     * idle and the function returns `true`.
     *
     * @return `true` if the currently assigned request completed this cycle,
     *         otherwise `false`.
     */
    bool tick();

    /**
     * @brief Check if the server is currently processing a request.
     * @return `true` if busy, `false` if idle.
     */
    bool isBusy() const;

    /**
     * @brief Get the server identifier.
     * @return integer id assigned at construction.
     */
    int getId() const;

private:
    int id_;
    int remaining_; // cycles remaining for current request
    Request current_;
    bool busy_;
};

#endif // WEBSERVER_H
