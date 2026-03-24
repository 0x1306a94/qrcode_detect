//
//  server_context.cpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#include "server_context.hpp"

namespace context {
static Context *g_context = nullptr;
Context::Context(const std::string model_dir, const std::string cache_dir)
    : m_model_dir(model_dir)
    , m_cache_dir(cache_dir) {
}

Context *Context::Current() {
    return g_context;
}

std::string Context::GetModelDir() const {
    return g_context->m_model_dir;
}

std::string Context::GetCacheDir() const {
    return g_context->m_cache_dir;
}

void Context::Init(const std::string model_dir, const std::string cache_dir) {
    g_context = new Context(model_dir, cache_dir);
}

};  // namespace context
