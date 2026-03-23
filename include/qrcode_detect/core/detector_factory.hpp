//
//  detector_factory.hpp
//  qrcode_detect
//
//  Created by king on 2024/5/2.
//

#ifndef __qrcode_detect_detector_factory_hpp
#define __qrcode_detect_detector_factory_hpp

#include <qrcode_detect/core/detector_type.hpp>

#include <memory>
#include <string>

namespace qrcode {
namespace detect {
class Detector;
struct DetectorFactory {
    static std::shared_ptr<Detector> Create(DetectorType type, const std::string &model_dir);
};

};  // namespace detect
};  // namespace qrcode

#endif /* __qrcode_detect_detector_factory_hpp */
