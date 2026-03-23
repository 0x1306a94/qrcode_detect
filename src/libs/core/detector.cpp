//
//  detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include <qrcode_detect/core/detector.hpp>

#include <qrcode_detect/core/AutoBuffer.hpp>
#include <qrcode_detect/core/detect_result.hpp>
#include <qrcode_detect/core/image_loader.hpp>

#include <opencv2/core.hpp>

namespace qrcode {
namespace detect {

std::optional<Result> Detector::detect(std::shared_ptr<cv::Mat> image) {
    if (!image || image->empty()) {
        return std::nullopt;
    }
    return detectImpl(*image);
}

}  // namespace detect
};  // namespace qrcode
