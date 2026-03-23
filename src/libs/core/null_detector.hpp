//
//  null_detector.hpp
//  qrcode_detect
//
//  Created by KK on 2026/3/23.
//

#ifndef null_detector_hpp
#define null_detector_hpp

#include <qrcode_detect/core/detector.hpp>

#include <memory>

namespace qrcode {
namespace detect {
class NullDetector : public Detector {
  public:
    explicit NullDetector();
    virtual ~NullDetector();

  protected:
    std::optional<Result> detectImpl(const cv::Mat &image) override;
};
};  // namespace detect
};  // namespace qrcode

#endif /* null_detector_hpp */
