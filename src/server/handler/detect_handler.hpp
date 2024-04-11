//
//  detect_handler.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef detect_handler_hpp
#define detect_handler_hpp

#include <memory>

#ifndef HV_HTTP_CONTEXT_H_
namespace hv {
struct HttpContext;
};
typedef std::shared_ptr<hv::HttpContext> HttpContextPtr;
#endif

namespace handler {
struct Detect {
    static int detect(const HttpContextPtr &ctx);
    static int detectFile(const HttpContextPtr &ctx);
};
};  // namespace handler

#endif /* detect_handler_hpp */
