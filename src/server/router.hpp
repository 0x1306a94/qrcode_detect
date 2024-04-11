//
//  router.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef router_hpp
#define router_hpp

#include <stdio.h>

namespace hv {
struct HttpService;
};

namespace qrcode {
class Router {
  public:
    static void Register(hv::HttpService &service);
};
};  // namespace qrcode

#endif /* router_hpp */
