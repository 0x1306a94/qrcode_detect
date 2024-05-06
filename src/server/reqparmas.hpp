//
//  reqparmas.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef reqparmas_hpp
#define reqparmas_hpp

#include <string>
#include <vector>

#include <core/detector_type.hpp>

namespace reqparmas {

struct DetectRequest {
    qrcode::detect::DetectorType type;
    std::vector<std::string> base64;
    std::vector<std::string> url;
};

};  // namespace reqparmas

#endif /* reqparmas_hpp */
