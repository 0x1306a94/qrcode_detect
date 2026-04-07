#ifndef ordered_worker_pool_hpp
#define ordered_worker_pool_hpp

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace handler {

class OrderedWorkerPool {
  public:
    explicit OrderedWorkerPool(std::size_t threadCount)
        : m_stopped(false) {
        if (threadCount == 0) {
            threadCount = 1;
        }
        m_workers.reserve(threadCount);
        for (std::size_t index = 0; index < threadCount; ++index) {
            m_workers.emplace_back([this]() { workerLoop(); });
        }
    }

    ~OrderedWorkerPool() {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_stopped = true;
        }
        m_queueCondition.notify_all();
        for (auto &worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    template <typename Task>
    auto run(std::size_t taskCount, Task task)
        -> std::vector<std::invoke_result_t<Task, std::size_t>> {
        using ResultType = std::invoke_result_t<Task, std::size_t>;

        if (taskCount == 0) {
            return {};
        }

        std::vector<std::optional<ResultType>> orderedResults(taskCount);
        std::atomic<std::size_t> pendingCount(taskCount);
        std::mutex completionMutex;
        std::condition_variable completionCondition;
        std::exception_ptr firstException;
        std::mutex exceptionMutex;

        for (std::size_t index = 0; index < taskCount; ++index) {
            enqueue([&, index]() {
                try {
                    orderedResults[index] = task(index);
                } catch (...) {
                    std::lock_guard<std::mutex> exceptionLock(exceptionMutex);
                    if (!firstException) {
                        firstException = std::current_exception();
                    }
                }

                if (pendingCount.fetch_sub(1) == 1) {
                    std::lock_guard<std::mutex> completionLock(completionMutex);
                    completionCondition.notify_one();
                }
            });
        }

        {
            std::unique_lock<std::mutex> lock(completionMutex);
            completionCondition.wait(lock, [&pendingCount]() { return pendingCount.load() == 0; });
        }

        if (firstException) {
            std::rethrow_exception(firstException);
        }

        std::vector<ResultType> results;
        results.reserve(taskCount);
        for (auto &result : orderedResults) {
            results.push_back(std::move(result.value()));
        }
        return results;
    }

  private:
    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            if (m_stopped) {
                throw std::runtime_error("ordered worker pool stopped");
            }
            m_tasks.push(std::move(task));
        }
        m_queueCondition.notify_one();
    }

    void workerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);
                m_queueCondition.wait(lock, [this]() { return m_stopped || !m_tasks.empty(); });
                if (m_stopped && m_tasks.empty()) {
                    return;
                }
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
            task();
        }
    }

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    bool m_stopped;
};

};  // namespace handler

#endif /* ordered_worker_pool_hpp */
