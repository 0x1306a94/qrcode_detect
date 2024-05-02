//
//  align.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/5/2.
//

#include "align.hpp"

namespace qrcode {
namespace detect {
Align::Align() {
    rotate90_ = false;
}

cv::Mat Align::crop(const cv::Mat &inputImg, const cv::Rect &srcRect, const float paddingW, const float paddingH,
                    const int minPadding, cv::Rect &crop_roi) {

    auto point = cv::Mat(4, 2, CV_32FC1);
    float x0 = srcRect.x;
    float y0 = srcRect.y;
    float x1 = srcRect.x + srcRect.width;
    float y1 = srcRect.y + srcRect.height;

    point.at<float>(0, 0) = x0;
    point.at<float>(0, 1) = y0;
    point.at<float>(1, 0) = x1;
    point.at<float>(1, 1) = y0;
    point.at<float>(2, 0) = x1;
    point.at<float>(2, 1) = y1;
    point.at<float>(3, 0) = x0;
    point.at<float>(3, 1) = y1;
    return crop(inputImg, point, paddingW, paddingH, minPadding, crop_roi);
}

cv::Mat Align::crop(const cv::Mat &inputImg, const cv::Mat &srcPts, const float paddingW, const float paddingH,
                    const int minPadding, cv::Rect &crop_roi) {
    int x0 = srcPts.at<float>(0, 0);
    int y0 = srcPts.at<float>(0, 1);
    int x2 = srcPts.at<float>(2, 0);
    int y2 = srcPts.at<float>(2, 1);

    int width = x2 - x0 + 1;
    int height = y2 - y0 + 1;

    int padx = std::max(paddingW * width, static_cast<float>(minPadding));
    int pady = std::max(paddingH * height, static_cast<float>(minPadding));

    crop_x_ = std::max(x0 - padx, 0);
    crop_y_ = std::max(y0 - pady, 0);
    int end_x = std::min(x2 + padx, inputImg.cols - 1);
    int end_y = std::min(y2 + pady, inputImg.rows - 1);

    crop_roi = cv::Rect(crop_x_, crop_y_, end_x - crop_x_ + 1, end_y - crop_y_ + 1);

    cv::Mat dst = inputImg(crop_roi).clone();
    if (rotate90_) {
        dst = dst.t();  // transpose
    }
    return dst;
}

cv::Mat Align::crop(const cv::Mat &inputImg, const int width, const int height) {
    cv::Mat warp_dst = cv::Mat::zeros(height, width, inputImg.type());

    cv::warpPerspective(inputImg, warp_dst, M_, warp_dst.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, 255);

    return warp_dst;
}

};  // namespace detect
};  // namespace qrcode
