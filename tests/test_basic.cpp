// Simple unit tests without external frameworks.
#include <cassert>
#include "webserver.h"
#include "loadbalancer.h"

int main() {
    // Test WebServer tick behavior
    WebServer s(1);
    Request r;
    r.id = 1;
    r.arrival = 0;
    r.serviceCycles = 3;
    r.ip_in = "1.1.1.1";
    r.ip_out = "2.2.2.2";
    assert(!s.isBusy());
    s.assignRequest(r);
    assert(s.isBusy());
    assert(!s.tick());
    assert(!s.tick());
    assert(s.tick()); // completes on third tick
    assert(!s.isBusy());

    // Test LoadBalancer initial queue fill
    LoadBalancer lb(2, 42u, true, false);
    lb.fillInitialQueue();
    int servers = lb.serverCount();
    size_t queued = lb.queuedCount();
    assert(servers == 2);
    assert(queued == static_cast<size_t>(servers * 5));

    return 0;
}
