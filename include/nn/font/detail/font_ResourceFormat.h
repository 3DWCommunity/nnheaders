#pragma once

#include <nn/types.h>

namespace nn {
namespace font {
namespace detail {

typedef union {
    uint32_t UInt32;
    int32_t SInt32;
    float Float32;
} Type32;

inline uint64_t ByteSwap(uint64_t val) {
    const uint64_t MASK = 0xFF00FF00FF00FF00ULL;
    const uint64_t MASK2 = 0xFFFF0000FFFF0000ULL;
    val = ((val & MASK) >> 8) | ((val << 8) & MASK);
    val = ((val & MASK2) >> 16) | ((val << 16) & MASK2);
    return (val >> 32) | (val << 32);
}

inline uint32_t ByteSwap(uint32_t val) {
    const uint32_t MASK = 0xFF00FF00;
    val = ((val & MASK) >> 8) | ((val << 8) & MASK);
    return (val >> 16) | (val << 16);
}

inline int32_t ByteSwap(int32_t val) {
    Type32 data;
    data.SInt32 = val;
    data.UInt32 = ByteSwap(data.UInt32);
    return data.SInt32;
}
};  // namespace detail
};  // namespace font
};  // namespace nn