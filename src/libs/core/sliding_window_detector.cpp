//
//  sliding_window_detector.cpp
//  qrcode_detect
//
//  Created by king on 2026/3/23.
//

#include <qrcode_detect/core/detect_result.hpp>
#include <qrcode_detect/core/sliding_window_detector.hpp>

#include <opencv2/core.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>

namespace qrcode {
namespace detect {

namespace {

std::vector<int> CalculateStarts(int imageSize, int windowSize, int stepSize) {
    std::vector<int> starts;

    if (imageSize <= 0 || windowSize <= 0 || stepSize <= 0) {
        return starts;
    }

    int lastStart = imageSize - windowSize;
    for (int start = 0; start <= lastStart; start += stepSize) {
        starts.push_back(start);
    }

    if (starts.empty() || starts.back() != lastStart) {
        starts.push_back(lastStart);
    }

    return starts;
}

}  // namespace

SlidingWindowDetector::SlidingWindowDetector(std::shared_ptr<Detector> detector, int maxWindowSize, float overlapRatio)
    : detector_(std::move(detector))
    , maxWindowSize_(maxWindowSize)
    , overlapRatio_(overlapRatio) {
}

SlidingWindowDetector::~SlidingWindowDetector() = default;

std::vector<Rect> SlidingWindowDetector::CalculateWindows(int imageWidth, int imageHeight, int maxWindowSize, float overlapRatio) {
    std::vector<Rect> windows;

    if (imageWidth <= 0 || imageHeight <= 0 || maxWindowSize <= 0) {
        return windows;
    }

    int actualWindowWidth = std::min(imageWidth, maxWindowSize);
    int actualWindowHeight = std::min(imageHeight, maxWindowSize);

    // Calculate step size (with overlap)
    int stepX = static_cast<int>(actualWindowWidth * (1.0f - overlapRatio));
    int stepY = static_cast<int>(actualWindowHeight * (1.0f - overlapRatio));

    // Ensure step is at least 1 to avoid infinite loop
    stepX = std::max(stepX, 1);
    stepY = std::max(stepY, 1);

    auto xStarts = CalculateStarts(imageWidth, actualWindowWidth, stepX);
    auto yStarts = CalculateStarts(imageHeight, actualWindowHeight, stepY);

    windows.reserve(xStarts.size() * yStarts.size());
    for (int windowY : yStarts) {
        for (int windowX : xStarts) {
            windows.emplace_back(windowX, windowY, actualWindowWidth, actualWindowHeight);
        }
    }

    return windows;
}

Rect SlidingWindowDetector::mapToOriginal(const Rect &rect, const Rect &window) const {
    // Map coordinates from window to original image
    return Rect(rect.x + window.x, rect.y + window.y, rect.width, rect.height);
}

bool SlidingWindowDetector::isDuplicate(const Value &a, const Value &b) const {
    // Check if content is the same
    if (a.content != b.content) {
        return false;
    }

    // Calculate intersection over union (IoU) to determine if rects overlap significantly
    int x1 = a.rect.x;
    int y1 = a.rect.y;
    int w1 = a.rect.width;
    int h1 = a.rect.height;

    int x2 = b.rect.x;
    int y2 = b.rect.y;
    int w2 = b.rect.width;
    int h2 = b.rect.height;

    // Calculate intersection
    int xInter = std::max(x1, x2);
    int yInter = std::max(y1, y2);
    int wInter = std::max(0, std::min(x1 + w1, x2 + w2) - xInter);
    int hInter = std::max(0, std::min(y1 + h1, y2 + h2) - yInter);

    if (wInter == 0 || hInter == 0) {
        return false;  // No intersection
    }

    int area1 = w1 * h1;
    int area2 = w2 * h2;
    int areaInter = wInter * hInter;

    // If intersection is more than 50% of either rect, consider as duplicate
    float ratio1 = static_cast<float>(areaInter) / area1;
    float ratio2 = static_cast<float>(areaInter) / area2;

    return ratio1 > 0.5f || ratio2 > 0.5f;
}

Result SlidingWindowDetector::mergeResults(std::vector<Result> &&results) const {
    std::vector<Value> mergedValues;
    uint64_t totalElapsed = 0;

    for (auto &result : results) {
        totalElapsed += result.elapsed;

        for (auto &value : result.values) {
            bool isDup = false;
            for (const auto &existing : mergedValues) {
                if (isDuplicate(value, existing)) {
                    isDup = true;
                    break;
                }
            }
            if (!isDup) {
                mergedValues.push_back(std::move(value));
            }
        }
    }

    return Result(totalElapsed, std::move(mergedValues));
}

std::optional<Result> SlidingWindowDetector::detectImpl(const cv::Mat &image, const std::string &traceId) {
    int imgWidth = image.cols;
    int imgHeight = image.rows;
    int maxDim = std::max(imgWidth, imgHeight);

    // If image is small enough, use direct detection
    if (maxDim <= maxWindowSize_) {
        return detector_->detectImpl(image, traceId);
    }

    // Use sliding window for large images
    SPDLOG_INFO("[traceId={}] Using sliding window detection for large image: {}x{}", traceId, imgWidth, imgHeight);

    auto windows = CalculateWindows(imgWidth, imgHeight, maxWindowSize_, overlapRatio_);
    SPDLOG_INFO("[traceId={}] Created {} sliding windows", traceId, windows.size());

    std::vector<Result> windowResults;
    windowResults.reserve(windows.size());

    for (const auto &window : windows) {
        // Extract window region from image
        cv::Rect roi(window.x, window.y, window.width, window.height);
        cv::Mat windowImage = image(roi);

        // Detect in window
        auto windowResult = detector_->detectImpl(windowImage, traceId);

        if (windowResult) {
            // Map coordinates to original image
            std::vector<Value> mappedValues;
            for (auto &value : windowResult->values) {
                Rect mappedRect = mapToOriginal(value.rect, window);
                mappedValues.emplace_back(value.content, mappedRect);
            }
            windowResults.emplace_back(windowResult->elapsed, std::move(mappedValues));
        }
    }

    // Merge results from all windows
    return mergeResults(std::move(windowResults));
}

}  // namespace detect
}  // namespace qrcode
