#pragma once

namespace nn {
namespace ui2d {
namespace detail {
template <typename T>
inline T GetBits(T bits, int pos, int len) {
    const uint32_t mask = ~(0xFFFFFFFFU << len);
    return T((bits >> pos) & mask);
}
};  // namespace detail
};  // namespace ui2d
};  // namespace nn