#pragma once

#include <nn/gfx/detail/gfx_Declare.h>
#include <nn/types.h>

#define NN_GFX_OPTION_EXISTS(base, name) Has##name##Impl<base>::value

#define NN_GFX_OPTION(name)                                                                        \
    template <typename TTmpImpl>                                                                   \
    struct Has##name##Impl {                                                                       \
        template <typename TCheck>                                                                 \
        static uint8_t Check(typename TCheck::name* = 0);                                          \
        template <typename TCheck>                                                                 \
        static uint16_t Check(...);                                                                \
        static const bool value = sizeof(Check<TTmpImpl>(NULL)) == sizeof(uint8_t);                \
    }

#define NN_GFX_OPTION_FUNCTION(ret, name, ...)                                                     \
    template <typename TTmpImpl, ret (*)(__VA_ARGS__)>                                             \
    struct name##ImplOption;                                                                       \
    NN_GFX_OPTION_FUNCTION_IMPL(name)

#define NN_GFX_OPTION_MEMBER_FUNCTION(ret, name, qualifier, ...)                                   \
    template <typename TTmpImpl, ret (TTmpImpl::*)(__VA_ARGS__) qualifier>                         \
    struct name##ImplOption;                                                                       \
    NN_GFX_OPTION_FUNCTION_IMPL(name)

#define NN_GFX_OPTION_FUNCTION_IMPL(name)                                                          \
    template <typename TTmpImpl>                                                                   \
    struct Has##name##Impl {                                                                       \
        template <typename TCheck>                                                                 \
        static uint8_t                                                                             \
        Check(name##ImplOption<typename TCheck::MemoryImplClass, &TCheck::name>* = 0);             \
        template <typename TCheck>                                                                 \
        static uint8_t Check(name##ImplOption<TCheck, &TCheck::name>* = 0);                        \
        template <typename TCheck>                                                                 \
        static uint16_t Check(...);                                                                \
        static const bool value = sizeof(Check<TTmpImpl>(NULL)) == sizeof(uint8_t);                \
    }

namespace nn::gfx::detail {

template <bool Cond, typename TThen, typename TElse>
struct Conditional {
    typedef TElse Type;
};
template <typename TThen, typename TElse>
struct Conditional<true, TThen, TElse> {
    typedef TThen Type;
};

template <class TType, class TValue = u64>
union Ptr {
    typedef TValue ValueType;
    typedef TType* PtrType;
    typedef Ptr<TType, TValue> SelfType;

    PtrType ptr;
    // doesn't seem to exist in SMO
    // ValueType value;

    PtrType operator->() const { return ptr; }

    PtrType* operator&() { return &ptr; }

    operator PtrType() const { return ptr; }

    template <class T>
    operator T() const {
        return static_cast<T>(ptr);
    }

    operator bool() const { return ptr != nullptr; }

    SelfType& operator=(PtrType p) {
        ptr = p;
        return *this;
    }
};

template <typename T, size_t N>
char (*ArrayLengthImpl(T (&)[N]))[N];

struct InfoHelper {
    template <typename TInfo, typename Info, void (*)(TInfo*, const Info&)>
    struct ImplOption;
    template <typename TInfo, typename Info>
    static void ConvertFromInfoImpl(TInfo* pDst, const Info& info,
                                    ImplOption<TInfo, Info, &TInfo::ConvertFromInfo>* = 0) {
        return TInfo::ConvertFromInfo(pDst, info);
    }
    template <typename TInfo, typename Info>
    static void ConvertFromInfoImpl(TInfo* pDst, const Info& info, ...) {
        *pDst = info;
    }
    template <typename TInfo, typename Info>
    static void ConvertFromInfo(TInfo* pDst, const Info& info) {
        return ConvertFromInfoImpl<TInfo, Info>(pDst, info, 0);
    }
};

struct DebugLabelHelper {
    template <typename TImpl, typename TTarget,
              void (TImpl::*)(DeviceImpl<TTarget>*, const char* label)>
    struct ImplOption;
    template <typename TImpl, typename TTarget>
    static void SetDebugLabelImpl(TImpl* pImpl, DeviceImpl<TTarget>* pDevice, const char* label,
                                  ImplOption<TImpl, TTarget, &TImpl::SetDebugLabel>* = 0) {
        return pImpl->SetDebugLabel(pDevice, label);
    }
    template <typename TImpl, typename TTarget>
    static void SetDebugLabelImpl(TImpl*, DeviceImpl<TTarget>*, const char*, ...) {}
    template <typename TImpl, typename TTarget>
    static void SetDebugLabel(TImpl* pImpl, TDevice<TTarget>* pDevice, const char* label) {
        return SetDebugLabelImpl<TImpl, TTarget>(pImpl, static_cast<DeviceImpl<TTarget>*>(pDevice),
                                                 label, 0);
    }
};

}  // namespace nn::gfx::detail