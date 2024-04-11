//
//  detect_result.hpp
//  qrcode_detect_core
//
//  Created by king on 2024/3/30.
//

#ifndef detect_result_hpp
#define detect_result_hpp

#include <cstdint>
#include <string>
#include <vector>

namespace qrcode {
namespace detect {
struct Rect {
    int x;
    int y;
    int width;
    int height;
    explicit Rect(int x, int y, int width, int height);
};

struct Value {
    std::string content;
    Rect rect;
    explicit Value(const std::string &content);
    explicit Value(const std::string &content, Rect rect);
};

struct Result {
    // 耗时，毫秒
    uint64_t elapsed;
    std::vector<Value> values;
    explicit Result(uint64_t elapsed);
    explicit Result(uint64_t elapsed, std::vector<Value> values);
};

};  // namespace detect
};  // namespace qrcode

#endif /* detect_result_hpp */
