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

namespace reqparmas {
enum class DetectorType : int {
    None = 0,
    Wechat = 1,
    YoloV3 = 2,
    OpenCV = 3,
    ZBar = 4,
};

struct DetectRequest {
    DetectorType type;
    std::vector<std::string> base64;
    std::vector<std::string> url;
};

};  // namespace reqparmas

#endif /* reqparmas_hpp */
