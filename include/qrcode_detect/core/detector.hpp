//
//  detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef __qrcode_detect_detector_hpp
#define __qrcode_detect_detector_hpp

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cv {
class Mat;
};  // namespace cv

namespace qrcode {
namespace detect {
struct Point;
struct Value;
struct Result;

class Detector {
  public:
    explicit Detector() = default;
    virtual ~Detector() = default;

    /// Detect QR codes from shared_ptr<cv::Mat>
    std::optional<Result> detect(std::shared_ptr<cv::Mat> image, const std::string &traceId = "");

  protected:
    /// Actual detection implementation, to be overridden by subclasses
    virtual std::optional<Result> detectImpl(const cv::Mat &image, const std::string &traceId) = 0;

  private:
    // Allow SlidingWindowDetector to access detectImpl
    friend class SlidingWindowDetector;
};
};  // namespace detect
};  // namespace qrcode

#endif /* __qrcode_detect_detector_hpp */
