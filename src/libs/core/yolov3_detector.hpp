//
//  yolov3_detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef yolov3_detector_hpp
#define yolov3_detector_hpp

#include "detector.hpp"

#include <memory>

// https://github.com/hpc203/yolo-qrcode-opencv/blob/main/main.cpp
namespace qrcode {
namespace detect {
class Yolov3Detector : public Detector {
  public:
    explicit Yolov3Detector(const std::string &model_dir);
    virtual ~Yolov3Detector();
    std::optional<Result> DetectFromBase64(const std::string &source) override;
    std::optional<Result> DetectFromBuffer(const common::AutoBuffer &buffer) override;
    std::optional<Result> DetectFromBytes(const unsigned char *bytes, std::size_t len) override;

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
};  // namespace detect
};  // namespace qrcode

#endif /* yolov3_detector_hpp */
