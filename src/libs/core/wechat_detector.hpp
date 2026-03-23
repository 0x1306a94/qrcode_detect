//
//  wechat_detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef wechat_detector_hpp
#define wechat_detector_hpp

#include <qrcode_detect/core/detector.hpp>

#include <memory>

namespace qrcode {
namespace detect {

class WechatDetector : public Detector {
  public:
    explicit WechatDetector(const std::string &model_dir);
    virtual ~WechatDetector();

  protected:
    std::optional<Result> detectImpl(const cv::Mat &image) override;

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
};  // namespace detect
};  // namespace qrcode

#endif /* wechat_detector_hpp */
