//
//  detector.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef detector_hpp
#define detector_hpp

#include <optional>
#include <string>
#include <vector>

namespace qrcode {
namespace common {
class AutoBuffer;
};
};  // namespace qrcode

namespace qrcode {
namespace detect {
struct Point;
struct Value;
struct Result;

class Detector {
  public:
    explicit Detector() = default;
    virtual ~Detector() = default;
    virtual std::optional<Result> DetectFromBase64(const std::string &source);
    virtual std::optional<Result> DetectFromBuffer(const common::AutoBuffer &buffer);
    virtual std::optional<Result> DetectFromBytes(const unsigned char *bytes, std::size_t len);
    virtual std::optional<Result> DetectFromPath(const std::string &path);
};
};  // namespace detect
};  // namespace qrcode

#endif /* detector_hpp */
