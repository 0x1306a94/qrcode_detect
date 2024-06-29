//
//  detect_result.cpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#include <qrcode_detect/core/detect_result.hpp>

namespace qrcode {
namespace detect {
Rect::Rect(int x, int y, int width, int height)
    : x(x)
    , y(y)
    , width(width)
    , height(height) {
}

Value::Value(const std::string &content)
    : content(content)
    , rect(Rect(0, 0, 0, 0)) {
}

Value::Value(const std::string &content, Rect rect)
    : content(content)
    , rect(std::move(rect)) {
}

Result::Result(uint64_t elapsed)
    : elapsed(elapsed)
    , values() {
}

Result::Result(uint64_t elapsed, std::vector<Value> values)
    : elapsed(elapsed)
    , values(std::move(values)) {
}

}  // namespace detect
};  // namespace qrcode
