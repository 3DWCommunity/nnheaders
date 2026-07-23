#pragma once

namespace nn {
namespace ui2d {
namespace detail {

template <typename T>
inline T SetBits(T bits, int pos, int len, T val) {
    const uint32_t MaxValue = 0xFFFFFFFFU >> (32 - len);
    const T mask = T(~(MaxValue << pos));
    bits &= mask;
    bits |= val << pos;

    return bits;
}

template <typename T>
inline void SetBits(T* pBits, int pos, int len, T val) {
    const uint32_t MaxValue = 0xFFFFFFFFU >> (32 - len);
    const T mask = T(~(MaxValue << pos));
    *pBits &= mask;
    *pBits |= val << pos;
}

template <typename T>
inline T GetBits(T bits, int pos, int len) {
    const uint32_t mask = ~(0xFFFFFFFFU << len);
    return T((bits >> pos) & mask);
}
};  // namespace detail
};  // namespace ui2d
};  // namespace nn