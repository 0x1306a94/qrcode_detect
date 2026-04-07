//
//  zbar_detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include "zbar_detector.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <zbar.h>

#include <qrcode_detect/core/detect_result.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace qrcode {
namespace detect {
class ZbarDetector::Implement {
  private:
    zbar::ImageScanner scanner;

  public:
    explicit Implement() {
        scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    }

    ~Implement() {
    }

    std::optional<Result> Detect(const cv::Mat &image, const std::string &traceId) {

        spdlog::stopwatch sw;

        cv::Mat grayImage;
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

        int width = image.cols;
        int height = image.rows;
        uchar *raw = (uchar *)grayImage.data;
        zbar::Image zimage(width, height, "Y800", raw, width * height);
        int n = scanner.scan(zimage);
        auto elapsed = duration_cast<milliseconds>(sw.elapsed());
        SPDLOG_INFO("[traceId={}] detect elapsed: {} result_count: {}", traceId, elapsed, n);
        sw.reset();

        std::vector<Value> values;
        for (zbar::Image::SymbolIterator symbol = zimage.symbol_begin(); symbol != zimage.symbol_end(); ++symbol) {

            if (symbol->get_location_size() == 4) {
                // 左上、右上、右下、左下
                std::vector<cv::Point> points;
                for (int i = 0; i < 4; i++) {
                    points.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
                }
                cv::Rect rect = cv::boundingRect(points);
                values.emplace_back(symbol->get_data(), detect::Rect(rect.x, rect.y, rect.width, rect.height));
            }
        }

        return Result(static_cast<uint64_t>(elapsed.count()), std::move(values));
    }
};

ZbarDetector::ZbarDetector()
    : Detector()
    , m_impl(std::make_unique<Implement>()) {
    SPDLOG_TRACE("Create {}", fmt::ptr(this));
}

ZbarDetector::~ZbarDetector() {
    SPDLOG_TRACE("Destory {}", fmt::ptr(this));
}

std::optional<Result> ZbarDetector::detectImpl(const cv::Mat &image, const std::string &traceId) {
    return m_impl->Detect(image, traceId);
}
};  // namespace detect
};  // namespace qrcode
