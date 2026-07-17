#pragma once

namespace nn {
namespace util {
template <typename T>
inline T LoadEndianReversed(const T* pSrc) {
    typedef typename nn::util::detail::MakeUnsigned<T>::Type UIntType;
    UIntType u;
    std::memcpy(&u, pSrc, sizeof(T));
    u = reverse_bytes(u);
    T result;
    std::memcpy(&result, &u, sizeof(T));
    return result;
}

template <typename T>
inline T LoadLittleEndian(const T* p) {
    return LoadEndianReversed(p);
}
};  // namespace util
};  // namespace nn