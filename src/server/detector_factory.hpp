//
//  detector_factory.hpp
//  qrcode_detect
//
//  Created by king on 2024/5/2.
//

#ifndef detector_factory_hpp
#define detector_factory_hpp

#include "reqparmas.hpp"

namespace context {
class Context;
};
namespace qrcode {
namespace detect {
class Detector;
struct DetectorFactory {
    Detector *Create(reqparmas::DetectorType type, const context::Context &ctx);
};

};  // namespace detect
};  // namespace qrcode

#endif /* detector_factory_hpp */
