//
//  detector_type.hpp
//  qrcode_detect
//
//  Created by king on 2024/5/6.
//

#ifndef detector_type_hpp
#define detector_type_hpp

namespace qrcode {
namespace detect {
enum class DetectorType : int {
    None = 0,
    Wechat = 1,
    YoloV3 = 2,
    OpenCV = 3,
    ZBar = 4,
};
};
};  // namespace qrcode

#endif /* detector_type_hpp */
