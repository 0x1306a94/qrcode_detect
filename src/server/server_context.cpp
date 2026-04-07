//
//  server_context.cpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#include "server_context.hpp"

#include "handler/ordered_worker_pool.hpp"

#include <algorithm>
#include <thread>

namespace context {
static Context *g_context = nullptr;
Context::Context(const std::string model_dir, const std::string cache_dir, std::size_t detect_worker_count)
    : m_model_dir(model_dir)
    , m_cache_dir(cache_dir)
    , m_detect_worker_pool(std::make_unique<handler::OrderedWorkerPool>(detect_worker_count)) {
}

Context *Context::Current() {
    return g_context;
}

std::size_t Context::DefaultDetectWorkerCount() {
    int cpu_count = std::thread::hardware_concurrency();
    int base_count = cpu_count == 0 ? 4 : cpu_count;
    auto final_count = std::max(4, std::min(base_count * 2, 8));
    return final_count;
}

std::string Context::GetModelDir() const {
    return g_context->m_model_dir;
}

std::string Context::GetCacheDir() const {
    return g_context->m_cache_dir;
}

handler::OrderedWorkerPool &Context::GetDetectWorkerPool() const {
    return *g_context->m_detect_worker_pool;
}

void Context::Init(const std::string model_dir, const std::string cache_dir, std::size_t detect_worker_count) {
    g_context = new Context(model_dir, cache_dir, detect_worker_count);
}

};  // namespace context
