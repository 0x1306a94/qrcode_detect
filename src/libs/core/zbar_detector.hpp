//
//  zbar_detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef zbar_detector_hpp
#define zbar_detector_hpp

#include <qrcode_detect/core/detector.hpp>

#include <memory>

namespace qrcode {
namespace detect {
class ZbarDetector : public Detector {
  public:
    explicit ZbarDetector();
    virtual ~ZbarDetector();

  protected:
    std::optional<Result> detectImpl(const cv::Mat &image, const std::string &traceId) override;

    friend class SlidingWindowDetector;

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
};  // namespace detect
};  // namespace qrcode

#endif /* zbar_detector_hpp */
