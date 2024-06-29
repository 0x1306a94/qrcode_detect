//
//  detector_factory.hpp
//  qrcode_detect
//
//  Created by king on 2024/5/2.
//

#ifndef __qrcode_detect_detector_factory_hpp
#define __qrcode_detect_detector_factory_hpp

#include "detector_type.hpp"

#include <string>

namespace context {
class Context;
};
namespace qrcode {
namespace detect {
class Detector;
struct DetectorFactory {
    Detector *Create(DetectorType type, const std::string &model_dir);
};

};  // namespace detect
};  // namespace qrcode

#endif /* __qrcode_detect_detector_factory_hpp */
