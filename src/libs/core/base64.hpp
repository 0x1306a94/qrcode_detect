//
//  base64.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef base64_hpp
#define base64_hpp

#include <string>
namespace qrcode {
namespace common {
namespace base64 {
std::string to_base64(std::string const &data);
std::string from_base64(std::string const &data);
};  // namespace base64
};  // namespace common
};  // namespace qrcode

#endif /* base64_hpp */
