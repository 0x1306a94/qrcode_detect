//
//  opencv_detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include "opencv_detector.hpp"

#include "AutoBuffer.hpp"
#include "base64.hpp"
#include "detect_result.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

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

    std::optional<Result> Detect(const cv::Mat &image) {

        spdlog::stopwatch sw;

        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        // 左上、右上、右下、左下
        cv::Mat points;
        std::vector<std::string> contents;
        detector->detectAndDecodeMulti(grayImage, contents, points);
        auto elapsed = duration_cast<milliseconds>(sw.elapsed());
        SPDLOG_TRACE("detect elapsed: {}", elapsed);
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

std::optional<Result> OpencvDetector::DetectFromBase64(const std::string &source) {
    if (source.empty()) {
        return std::nullopt;
    }
    std::string decode_str = common::base64::from_base64(source);
    if (decode_str.empty()) {
        return std::nullopt;
    }

    std::vector<uchar> data(decode_str.begin(), decode_str.end());
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
    if (img.empty()) {
        return std::nullopt;
    }
    return m_impl->Detect(img);
}

std::optional<Result> OpencvDetector::DetectFromBuffer(const common::AutoBuffer &buffer) {
    if (buffer.size() == 0) {
        return std::nullopt;
    }
    std::vector<uchar> data(buffer.data(), buffer.data() + buffer.size());
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
    if (img.empty()) {
        return std::nullopt;
    }
    return m_impl->Detect(img);
}

std::optional<Result> OpencvDetector::DetectFromBytes(const unsigned char *bytes, std::size_t len) {
    if (bytes == nullptr || len == 0) {
        return std::nullopt;
    }
    std::vector<uchar> data(bytes, bytes + len);
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
    if (img.empty()) {
        return std::nullopt;
    }
    return m_impl->Detect(img);
}
};  // namespace detect
};  // namespace qrcode
