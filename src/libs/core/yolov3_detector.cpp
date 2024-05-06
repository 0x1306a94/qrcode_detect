//
//  yolov3_detector.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include "yolov3_detector.hpp"

#include "AutoBuffer.hpp"
#include "base64.hpp"
#include "detect_result.hpp"

#include "align.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <zbar.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace qrcode {
namespace detect {
class Yolov3Detector::Implement {
  private:
    float confThreshold;
    float nmsThreshold;
    cv::dnn::Net net;
    zbar::ImageScanner scanner;
    const int inpWidth = 416;
    const int inpHeight = 416;

  public:
    explicit Implement(float confThreshold, float nmsThreshold, const std::string &model_dir)
        : confThreshold(confThreshold)
        , nmsThreshold(nmsThreshold)
        , scanner() {
        std::string cfg_path = model_dir + "/qrcode-yolov3-tiny.cfg";
        std::string weights_path = model_dir + "/qrcode-yolov3-tiny.weights";
        this->net = cv::dnn::readNet(cfg_path, weights_path);
    }

    ~Implement() {
    }

    std::optional<Result> Detect(const cv::Mat &image) {

        spdlog::stopwatch sw;

        int img_w = image.cols;
        int img_h = image.rows;

        std::vector<int> classIds;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;
        std::vector<cv::Mat> outs;

        cv::Mat input;
        cv::resize(image, input, cv::Size(this->inpWidth, this->inpHeight), 0, 0, cv::INTER_CUBIC);

        cv::dnn::blobFromImage(input, input, 1 / 255.0, cv::Size(this->inpWidth, this->inpHeight), cv::Scalar(0, 0, 0), true, false);
        this->net.setInput(input);
        this->net.forward(outs, this->net.getUnconnectedOutLayersNames());

        for (std::size_t i = 0; i < outs.size(); ++i) {
            float *data = (float *)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) {
                cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                cv::Point classIdPoint;
                double confidence;
                // Get the value and location of the maximum score
                cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > this->confThreshold) {
                    int centerX = (int)(data[0] * img_w);
                    int centerY = (int)(data[1] * img_h);
                    int width = (int)(data[2] * img_w);
                    int height = (int)(data[3] * img_h);
                    int left = (int)(centerX - width / 2);
                    int top = (int)(centerY - height / 2);

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);

        std::vector<Value> values;

        Align aligner;
        float padding_w = 0.1f, padding_h = 0.1f;
        auto min_padding = 15;
        for (size_t i = 0; i < indices.size(); ++i) {
            int idx = indices[i];
            cv::Rect box = boxes[idx];

            cv::Rect crop_roi;
            cv::Mat cropped = aligner.crop(image, box, padding_w, padding_h, min_padding, crop_roi);
            cv::cvtColor(cropped, cropped, cv::COLOR_BGR2GRAY);

            zbar::Image zbar_image(crop_roi.width, crop_roi.height, "Y800", cropped.data, crop_roi.width * crop_roi.height);
            scanner.scan(zbar_image);

            for (zbar::Image::SymbolIterator symbol = zbar_image.symbol_begin(); symbol != zbar_image.symbol_end(); ++symbol) {
                if (symbol->get_location_size() == 4) {
                    // 左上、右上、右下、左下
                    std::vector<cv::Point> points;
                    for (int i = 0; i < 4; i++) {
                        points.push_back(cv::Point(symbol->get_location_x(i), symbol->get_location_y(i)));
                    }
                    cv::Rect rect = cv::boundingRect(points);
                    values.emplace_back(symbol->get_data(), detect::Rect(crop_roi.x + rect.x, crop_roi.y + rect.y, rect.width, rect.height));
                }
            }
        }

        auto elapsed = duration_cast<milliseconds>(sw.elapsed());
        SPDLOG_TRACE("detect elapsed: {}", elapsed);

        return Result(static_cast<uint64_t>(elapsed.count()), std::move(values));
    }
};

Yolov3Detector::Yolov3Detector(const std::string &model_dir)
    : Detector()
    , m_impl(std::make_unique<Implement>(0.6, 0.5, model_dir)) {
    SPDLOG_TRACE("Create {}", fmt::ptr(this));
}

Yolov3Detector::~Yolov3Detector() {
    SPDLOG_TRACE("Destory {}", fmt::ptr(this));
}

std::optional<Result> Yolov3Detector::DetectFromBase64(const std::string &source) {
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

std::optional<Result> Yolov3Detector::DetectFromBuffer(const common::AutoBuffer &buffer) {
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

std::optional<Result> Yolov3Detector::DetectFromBytes(const unsigned char *bytes, std::size_t len) {
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
