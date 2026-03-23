//
//  sliding_window_detector.hpp
//  qrcode_detect
//
//  Created by king on 2026/3/23.
//

#ifndef __qrcode_detect_sliding_window_detector_hpp
#define __qrcode_detect_sliding_window_detector_hpp

#include <qrcode_detect/core/detector.hpp>

namespace qrcode {
namespace detect {
struct Rect;

class SlidingWindowDetector : public Detector {
  public:
    explicit SlidingWindowDetector(std::shared_ptr<Detector> detector, int maxWindowSize = 1280, float overlapRatio = 0.2f);
    ~SlidingWindowDetector() override;

    /// Calculate sliding window positions (static method for external use)
    static std::vector<Rect> CalculateWindows(int imageWidth, int imageHeight, int maxWindowSize = 1280, float overlapRatio = 0.2f);

  protected:
    std::optional<Result> detectImpl(const cv::Mat &image) override;

  private:
    /// Map window coordinates to original image coordinates
    Rect mapToOriginal(const Rect &rect, const Rect &window) const;

    /// Merge detection results from all windows, removing duplicates
    Result mergeResults(std::vector<Result> &&results) const;

    /// Check if two rects represent the same QR code (based on content and position)
    bool isDuplicate(const Value &a, const Value &b) const;

    std::shared_ptr<Detector> detector_;
    int maxWindowSize_;
    float overlapRatio_;
};

}  // namespace detect
}  // namespace qrcode

#endif /* __qrcode_detect_sliding_window_detector_hpp */
