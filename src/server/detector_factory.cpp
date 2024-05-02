//
//  detector_factory.cpp
//  qrcode_detect
//
//  Created by king on 2024/5/2.
//

#include "detector_factory.hpp"

#include "server_context.hpp"

#include <core/opencv_detector.hpp>
#include <core/wechat_detector.hpp>
#include <core/yolov3_detector.hpp>
#include <core/zbar_detector.hpp>
namespace qrcode {
namespace detect {
Detector *DetectorFactory::Create(reqparmas::DetectorType type, const context::Context &ctx) {
    switch (type) {
        case reqparmas::DetectorType::Wechat: {
            std::string model_dir = ctx.GetModelDir();
            static thread_local qrcode::detect::WechatDetector *_detector = new qrcode::detect::WechatDetector(model_dir + "/wechat_qrcode");
            return _detector;
        }
        case reqparmas::DetectorType::YoloV3: {
            std::string model_dir = ctx.GetModelDir();
            static thread_local qrcode::detect::Yolov3Detector *_detector = new qrcode::detect::Yolov3Detector(model_dir + "/yolov3-qrcode");
            return _detector;
        }
        case reqparmas::DetectorType::OpenCV: {
            static thread_local qrcode::detect::OpencvDetector *_detector = new qrcode::detect::OpencvDetector();
            return _detector;
        }
        case reqparmas::DetectorType::ZBar: {
            static thread_local qrcode::detect::ZbarDetector *_detector = new qrcode::detect::ZbarDetector();
            return _detector;
        }
        default: {
            static thread_local qrcode::detect::Detector *_detector = new qrcode::detect::Detector();
            return _detector;
        }
    }
}
};  // namespace detect
};  // namespace qrcode
