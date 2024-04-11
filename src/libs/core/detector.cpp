//
//  detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include "detector.hpp"

#include "AutoBuffer.hpp"
#include "detect_result.hpp"

namespace qrcode {
namespace detect {

Detector::Detector() {
}

Detector::~Detector() {
}

std::optional<Result> Detector::DetectFromBase64(const std::string &source) {
    return std::nullopt;
}

std::optional<Result> Detector::DetectFromBuffer(const common::AutoBuffer &buffer) {
    return std::nullopt;
}

std::optional<Result> Detector::DetectFromBytes(const unsigned char *bytes, std::size_t len) {
    return std::nullopt;
}
}  // namespace detect
};  // namespace qrcode
