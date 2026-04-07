//
//  server_context.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef server_context_hpp
#define server_context_hpp

#include <cstddef>
#include <memory>
#include <string>

namespace handler {
class OrderedWorkerPool;
}

namespace context {
class Context {
  public:
    explicit Context(const std::string model_dir, const std::string cache_dir = "", std::size_t detect_worker_count = DefaultDetectWorkerCount());
    static Context *Current();
    static void Init(const std::string model_dir, const std::string cache_dir = "", std::size_t detect_worker_count = DefaultDetectWorkerCount());
    static std::size_t DefaultDetectWorkerCount();

    std::string GetModelDir() const;
    std::string GetCacheDir() const;
    handler::OrderedWorkerPool &GetDetectWorkerPool() const;

  private:
    std::string m_model_dir;
    std::string m_cache_dir;
    std::unique_ptr<handler::OrderedWorkerPool> m_detect_worker_pool;
};
};  // namespace context

#endif /* server_context_hpp */
