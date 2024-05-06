//
//  detector_factory.cpp
//  qrcode_detect
//
//  Created by king on 2024/5/2.
//

#include "detector_factory.hpp"

#include "opencv_detector.hpp"
#include "wechat_detector.hpp"
#include "yolov3_detector.hpp"
#include "zbar_detector.hpp"

namespace qrcode {
namespace detect {
Detector *DetectorFactory::Create(DetectorType type, const std::string &model_dir) {
    switch (type) {
        case DetectorType::Wechat: {
            WechatDetector *_detector = new WechatDetector(model_dir + "/wechat_qrcode");
            return _detector;
        }
        case DetectorType::YoloV3: {
            Yolov3Detector *_detector = new Yolov3Detector(model_dir + "/yolov3-qrcode");
            return _detector;
        }
        case DetectorType::OpenCV: {
            OpencvDetector *_detector = new OpencvDetector();
            return _detector;
        }
        case DetectorType::ZBar: {
            ZbarDetector *_detector = new ZbarDetector();
            return _detector;
        }
        default: {
            Detector *_detector = new Detector();
            return _detector;
        }
    }
}
};  // namespace detect
};  // namespace qrcode
