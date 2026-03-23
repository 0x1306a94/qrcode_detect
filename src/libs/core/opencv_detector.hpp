//
//  opencv_detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef opencv_detector_hpp
#define opencv_detector_hpp

#include <qrcode_detect/core/detector.hpp>

#include <memory>

namespace qrcode {
namespace detect {
class OpencvDetector : public Detector {
  public:
    explicit OpencvDetector();
    virtual ~OpencvDetector();

  protected:
    std::optional<Result> detectImpl(const cv::Mat &image) override;

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
};  // namespace detect
};  // namespace qrcode

#endif /* opencv_detector_hpp */
