//
//  image_loader.cpp
//  qrcode_detect_core
//
//  Created by king on 2026/3/23.
//

#include <qrcode_detect/core/AutoBuffer.hpp>
#include <qrcode_detect/core/image_loader.hpp>

#include "base64.hpp"

#include <opencv2/imgcodecs.hpp>

namespace qrcode {
namespace detect {

std::shared_ptr<cv::Mat> ImageLoader::fromBase64(const std::string &base64_str) {
    if (base64_str.empty()) {
        return nullptr;
    }

    std::string decode_str = common::base64::from_base64(base64_str);
    if (decode_str.empty()) {
        return nullptr;
    }

    std::vector<uchar> data(decode_str.begin(), decode_str.end());
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
    if (img.empty()) {
        return nullptr;
    }

    return std::make_shared<cv::Mat>(img);
}

std::shared_ptr<cv::Mat> ImageLoader::fromBuffer(const common::AutoBuffer &buffer) {
    if (buffer.size() == 0) {
        return nullptr;
    }

    std::vector<uchar> data(buffer.data(), buffer.data() + buffer.size());
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
    if (img.empty()) {
        return nullptr;
    }

    return std::make_shared<cv::Mat>(img);
}

std::shared_ptr<cv::Mat> ImageLoader::fromBytes(const unsigned char *bytes, std::size_t len) {
    if (bytes == nullptr || len == 0) {
        return nullptr;
    }

    std::vector<uchar> data(bytes, bytes + len);
    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
    if (img.empty()) {
        return nullptr;
    }

    return std::make_shared<cv::Mat>(img);
}

std::shared_ptr<cv::Mat> ImageLoader::fromPath(const std::string &path) {
    cv::Mat img = cv::imread(path);
    if (img.empty()) {
        return nullptr;
    }

    return std::make_shared<cv::Mat>(img);
}

}  // namespace detect
}  // namespace qrcode
