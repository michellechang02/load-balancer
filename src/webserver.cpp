/**
 * @file webserver.cpp
 * @brief WebServer implementation.
 */

#include "webserver.h"

WebServer::WebServer(int id) : id_(id), remaining_(0), busy_(false) {}

void WebServer::assignRequest(const Request &r) {
    current_ = r;
    remaining_ = r.serviceCycles;
    busy_ = true;
}

bool WebServer::tick() {
    if (!busy_) return false;
    --remaining_;
    if (remaining_ <= 0) {
        busy_ = false;
        remaining_ = 0;
        return true;
    }
    return false;
}

bool WebServer::isBusy() const { return busy_; }

int WebServer::getId() const { return id_; }
