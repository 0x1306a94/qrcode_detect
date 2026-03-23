//
//  detector_factory.cpp
//  qrcode_detect
//
//  Created by king on 2024/5/2.
//

#include <qrcode_detect/core/detector_factory.hpp>

#include "opencv_detector.hpp"
#include "wechat_detector.hpp"
#include "yolov3_detector.hpp"
#include "zbar_detector.hpp"

namespace qrcode {
namespace detect {
std::shared_ptr<Detector> DetectorFactory::Create(DetectorType type, const std::string &model_dir) {
    switch (type) {
        case DetectorType::Wechat: {
            return std::make_shared<WechatDetector>(model_dir + "/wechat_qrcode");
        }
        case DetectorType::YoloV3: {
            return std::make_shared<Yolov3Detector>(model_dir + "/yolov3-qrcode");
        }
        case DetectorType::OpenCV: {
            return std::make_shared<OpencvDetector>();
        }
        case DetectorType::ZBar: {
            return std::make_shared<ZbarDetector>();
        }
        default: {
            return nullptr;
        }
    }
}
};  // namespace detect
};  // namespace qrcode
