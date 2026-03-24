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

#include <qrcode_detect/core/detector_type.hpp>

namespace reqparmas {

struct DetectRequest {
    qrcode::detect::DetectorType type;
    std::vector<std::string> base64;
    std::vector<std::string> url;
    // Sliding window parameters
    int maxWindowSize = 0;      // 0 means disabled, > 0 enables sliding window
    float overlapRatio = 0.2f;  // overlap ratio between windows
    // Cache control
    bool cache = false;  // require explicit cache=true to enable caching
};

};  // namespace reqparmas

#endif /* reqparmas_hpp */
