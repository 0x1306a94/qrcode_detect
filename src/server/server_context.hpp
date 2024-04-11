//
//  server_context.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef server_context_hpp
#define server_context_hpp

#include <string>
namespace context {
class Context {
  public:
    explicit Context(const std::string model_dir);
    static Context *Current();
    static void Init(const std::string model_dir);

    std::string GetModelDir();
  private:
    std::string m_model_dir;
};
};  // namespace context

#endif /* server_context_hpp */
