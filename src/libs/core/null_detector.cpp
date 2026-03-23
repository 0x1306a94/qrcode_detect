//
//  null_detector.cpp
//  qrcode_detect
//
//  Created by KK on 2026/3/23.
//

#include "null_detector.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/spdlog.h>

#include <qrcode_detect/core/detect_result.hpp>

namespace qrcode {
namespace detect {

NullDetector::NullDetector()
    : Detector() {
    SPDLOG_TRACE("Create {}", fmt::ptr(this));
}

NullDetector::~NullDetector() {
    SPDLOG_TRACE("Destory {}", fmt::ptr(this));
}

std::optional<Result> NullDetector::detectImpl(const cv::Mat &image) {
    return std::nullopt;
}
};  // namespace detect
};  // namespace qrcode
