//
//  wechat_detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include "wechat_detector.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/wechat_qrcode.hpp>

#include <qrcode_detect/core/detect_result.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace qrcode {
namespace detect {

class WechatDetector::Implement {
  public:
    cv::Ptr<cv::wechat_qrcode::WeChatQRCode> detector;
    explicit Implement(const std::string &model_dir) {
        std::string detector_prototxt_path = model_dir + "/detect.prototxt";
        std::string detector_caffe_model_path = model_dir + "/detect.caffemodel";
        std::string super_resolution_prototxt_path = model_dir + "/sr.prototxt";
        std::string super_resolution_caffe_model_path = model_dir + "/sr.caffemodel";
        detector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(detector_prototxt_path, detector_caffe_model_path, super_resolution_prototxt_path, super_resolution_caffe_model_path);
        if (!detector) {
            SPDLOG_ERROR("WechatDetector 创建失败");
        }
    }

    ~Implement() {
    }

    std::optional<Result> Detect(const cv::Mat &image) {
        if (!detector) {
            return std::nullopt;
        }

        spdlog::stopwatch sw;

        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        std::vector<cv::Mat> points;
        std::vector<std::string> contents = detector->detectAndDecode(grayImage, points);
        auto elapsed = duration_cast<milliseconds>(sw.elapsed());
        sw.reset();
        SPDLOG_TRACE("detect elapsed: {}", elapsed);
        Result result(static_cast<uint64_t>(elapsed.count()));
        std::vector<Value> values;
        for (int i = 0; i < points.size(); i++) {
            cv::Rect rect = cv::boundingRect(points[i]);
            values.emplace_back(contents[i], detect::Rect(rect.x, rect.y, rect.width, rect.height));
        }

        return Result(static_cast<uint64_t>(elapsed.count()), std::move(values));
    }
};

WechatDetector::WechatDetector(const std::string &model_dir)
    : Detector()
    , m_impl(std::make_unique<Implement>(model_dir)) {
    SPDLOG_TRACE("Create {}", fmt::ptr(this));
}

WechatDetector::~WechatDetector() {
    SPDLOG_TRACE("Destory {}", fmt::ptr(this));
}

std::optional<Result> WechatDetector::detectImpl(const cv::Mat &image) {
    return m_impl->Detect(image);
}

};  // namespace detect
};  // namespace qrcode
