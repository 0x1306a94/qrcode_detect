//
//  opencv_detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include "opencv_detector.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <qrcode_detect/core/detect_result.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace qrcode {
namespace detect {
class OpencvDetector::Implement {
  private:
    cv::Ptr<cv::QRCodeDetector> detector;

  public:
    explicit Implement()
        : detector(cv::makePtr<cv::QRCodeDetector>()) {
    }

    ~Implement() {
    }

    std::optional<Result> Detect(const cv::Mat &image, const std::string &traceId) {

        spdlog::stopwatch sw;

        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        // 左上、右上、右下、左下
        cv::Mat points;
        std::vector<std::string> contents;
        detector->detectAndDecodeMulti(grayImage, contents, points);
        auto elapsed = duration_cast<milliseconds>(sw.elapsed());
        SPDLOG_INFO("[traceId={}] detect elapsed: {}", traceId, elapsed);
        sw.reset();
        std::vector<Value> values;
        for (int idx = 0; idx < points.rows; idx++) {
            cv::Rect rect = cv::boundingRect(points.row(idx));
            values.emplace_back(contents[idx], detect::Rect(rect.x, rect.y, rect.width, rect.height));
        }
        return Result(static_cast<uint64_t>(elapsed.count()), std::move(values));
    }
};

OpencvDetector::OpencvDetector()
    : Detector()
    , m_impl(std::make_unique<Implement>()) {
    SPDLOG_TRACE("Create {}", fmt::ptr(this));
}

OpencvDetector::~OpencvDetector() {
    SPDLOG_TRACE("Destory {}", fmt::ptr(this));
}

std::optional<Result> OpencvDetector::detectImpl(const cv::Mat &image, const std::string &traceId) {
    return m_impl->Detect(image, traceId);
}
};  // namespace detect
};  // namespace qrcode
