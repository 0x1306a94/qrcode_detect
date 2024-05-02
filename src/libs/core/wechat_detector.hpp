//
//  wechat_detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef wechat_detector_hpp
#define wechat_detector_hpp

#include "detector.hpp"

#include <memory>

namespace qrcode {
namespace detect {

class WechatDetector : public Detector {
  public:
    explicit WechatDetector(const std::string &model_dir);
    virtual ~WechatDetector();
    std::optional<Result> DetectFromBase64(const std::string &source) override;
    std::optional<Result> DetectFromBuffer(const common::AutoBuffer &buffer) override;
    std::optional<Result> DetectFromBytes(const unsigned char *bytes, std::size_t len) override;

  private:
    class Implement;
    std::unique_ptr<Implement> m_impl;
};
};  // namespace detect
};  // namespace qrcode

#endif /* wechat_detector_hpp */