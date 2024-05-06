//
//  opencv_detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef opencv_detector_hpp
#define opencv_detector_hpp

#include "detector.hpp"

#include <memory>

namespace qrcode {
namespace detect {
class OpencvDetector : public Detector {
  public:
    explicit OpencvDetector();
    virtual ~OpencvDetector();
    std::optional<Result> DetectFromBase64(const std::string &source) override;
    std::optional<Result> DetectFromBuffer(const common::AutoBuffer &buffer) override;
    std::optional<Result> DetectFromBytes(const unsigned char *bytes, std::size_t len) override;
    std::optional<Result> DetectFromPath(const std::string &path) override;

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
};  // namespace detect
};  // namespace qrcode

#endif /* opencv_detector_hpp */
