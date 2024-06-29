//
//  AutoBuffer.hpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#ifndef __qrcode_detect_AutoBuffer_hpp
#define __qrcode_detect_AutoBuffer_hpp

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace qrcode {
namespace common {
class AutoBuffer {
  public:
  public:
    explicit AutoBuffer(size_t malloc_unitsize = 128);

    ~AutoBuffer();

    AutoBuffer(AutoBuffer &&other) noexcept;

    AutoBuffer &operator=(AutoBuffer &&other);

    void Append(const char *data, size_t size);

    void Reset();

    const char *data() const;

    const size_t size() const;

  private:
    void grow(size_t capacity);

  private:
    AutoBuffer(const AutoBuffer &other) = delete;
    AutoBuffer &operator=(const AutoBuffer &other) = delete;

    size_t m_malloc_unitsize;
    size_t m_capacity;
    size_t m_size;
    char *m_ptr = nullptr;
};
};  // namespace common
};  // namespace qrcode

#endif /* __qrcode_detect_AutoBuffer_hpp */
