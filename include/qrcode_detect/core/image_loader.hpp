//
//  image_loader.hpp
//  qrcode_detect_core
//
//  Created by king on 2026/3/23.
//

#ifndef __qrcode_detect_image_loader_hpp
#define __qrcode_detect_image_loader_hpp

#include <cstddef>
#include <memory>
#include <string>

namespace cv {
class Mat;
};  // namespace cv

namespace qrcode {
namespace common {
class AutoBuffer;
};  // namespace common

namespace detect {

class ImageLoader {
  public:
    ImageLoader() = delete;
    ~ImageLoader() = delete;

    /// Load image from base64 string
    /// @param base64_str base64 encoded image data
    /// @return shared_ptr to cv::Mat, nullptr on failure
    static std::shared_ptr<cv::Mat> fromBase64(const std::string &base64_str);

    /// Load image from buffer
    /// @param buffer image data buffer
    /// @return shared_ptr to cv::Mat, nullptr on failure
    static std::shared_ptr<cv::Mat> fromBuffer(const common::AutoBuffer &buffer);

    /// Load image from byte array
    /// @param bytes pointer to image data
    /// @param len data length
    /// @return shared_ptr to cv::Mat, nullptr on failure
    static std::shared_ptr<cv::Mat> fromBytes(const unsigned char *bytes, std::size_t len);

    /// Load image from file path
    /// @param path image file path
    /// @return shared_ptr to cv::Mat, nullptr on failure
    static std::shared_ptr<cv::Mat> fromPath(const std::string &path);
};

}  // namespace detect
}  // namespace qrcode

#endif /* __qrcode_detect_image_loader_hpp */
