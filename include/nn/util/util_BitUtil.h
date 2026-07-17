#pragma once

#include <climits>
#include <cstring>
#include <nn/detail/nn_Macro.h>
#include <nn/util/detail/util_TypeTraits.h>

namespace nn::util {

template <typename T>
T reverse_bits(T x, int subword_bits = 1, int num_swar_words = 1);

template <typename T>
T align_up(T x, size_t align) {
    typedef typename detail::MakeUnsigned<T>::Type UIntType;
    UIntType mask = align - 1;
    return (x + mask) & ~mask;
}

template <typename T>
inline T reverse_bytes(T x, int subword_bytes = 1, int num_swar_words = 1) {
    return reverse_bits(x, subword_bytes * CHAR_BIT, num_swar_words);
}

template <typename T>
inline T reverse_bits(T x, int subword_bits, int num_swar_words) {
    typedef typename nn::util::detail::MakeUnsigned<T>::Type UIntType;
    int words_size = NN_BITSIZEOF(T) / num_swar_words;
    int k = words_size - subword_bits;

    UIntType u;
    std::memcpy(&u, &x, sizeof(x));

    for (int i = 1; i < NN_BITSIZEOF(T); i = i << 1) {
        const UIntType mi = static_cast<UIntType>(UIntType(-1) / ((UIntType(1) << i) + 1));
        const UIntType nmi = static_cast<UIntType>(~mi);

        if (k & i) {
            u = static_cast<UIntType>(((u & mi) << i) | ((u & nmi) >> i));
        }
    }

    std::memcpy(&x, &u, sizeof(x));
    return x;
}

}  // namespace nn::util