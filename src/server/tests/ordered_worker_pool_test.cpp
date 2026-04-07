#include "handler/ordered_worker_pool.hpp"

#include <cassert>
#include <chrono>
#include <thread>
#include <vector>

int main() {
    handler::OrderedWorkerPool pool(3);
    const std::vector<int> delays = {120, 10, 80, 20, 60};

    auto results = pool.run(delays.size(), [&](std::size_t index) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delays[index]));
        return static_cast<int>(index);
    });

    assert(results.size() == delays.size());
    for (std::size_t index = 0; index < results.size(); ++index) {
        assert(results[index] == static_cast<int>(index));
    }

    return 0;
}
