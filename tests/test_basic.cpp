// Simple unit tests without external frameworks.
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>
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

    // Test that server ticks run concurrently: assign a request to each of
    // multiple WebServer instances and tick them from separate threads.
    // All servers should finish independently without data races.
    const int N = 4;
    std::vector<WebServer> ws;
    ws.reserve(N);
    for (int i = 0; i < N; ++i) ws.emplace_back(i + 1);

    Request base;
    base.arrival = 0;
    base.ip_in = "10.0.0.1";
    base.ip_out = "10.0.0.2";
    for (int i = 0; i < N; ++i) {
        base.id = static_cast<uint64_t>(i + 1);
        base.serviceCycles = 2;
        ws[i].assignRequest(base);
    }

    std::atomic<int> doneCount{0};
    std::vector<std::thread> threads;
    threads.reserve(N);
    for (int i = 0; i < N; ++i) {
        threads.emplace_back([&ws, &doneCount, i]() {
            ws[i].tick(); // cycle 1
            if (ws[i].tick()) { // cycle 2 – should complete
                ++doneCount;
            }
        });
    }
    for (auto &t : threads) t.join();
    assert(doneCount == N);
    for (int i = 0; i < N; ++i) assert(!ws[i].isBusy());

    // Test LoadBalancer multithreaded run (quiet mode, short duration)
    LoadBalancer lb2(4, 7u, true, false);
    lb2.fillInitialQueue();
    lb2.run(10.0);
    assert(lb2.serverCount() >= 1);

    return 0;
}
