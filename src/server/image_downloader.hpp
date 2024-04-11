//
//  image_downloader.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef image_downloader_hpp
#define image_downloader_hpp

#include <string_view>

namespace qrcode {
namespace common {
class AutoBuffer;
};
};  // namespace qrcode

namespace qrcode {
namespace download {
std::size_t image_from_url(std::string_view url, common::AutoBuffer &buffer);
};
}  // namespace qrcode

#endif /* image_downloader_hpp */
