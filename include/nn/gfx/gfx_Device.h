#pragma once

#include <nn/gfx/detail/gfx_Device-api.nvn.8.h>
#include <nn/gfx/detail/gfx_RequiredMemory.h>
#include <nn/util.h>

namespace nn::gfx {

class DeviceInfo;

template <class TTarget>
class TDevice : public detail::DeviceImpl<typename detail::TargetVariation<TTarget>::Type>,
                private detail::RequiredMemory<
                    detail::DeviceImpl<typename detail::TargetVariation<TTarget>::Type>> {
    NN_NO_COPY(TDevice);

public:
    typedef detail::DeviceImpl<typename detail::TargetVariation<TTarget>::Type> Impl;
    typedef typename Impl::Target Target;
    typedef DeviceInfo InfoType;

    enum RequiredMemoryInfo {
        RequiredMemoryInfo_Alignment =
            detail::RequiredMemory<Impl>::MemoryInfoImpl::RequiredMemoryInfo_Alignment
    };

    static size_t GetRequiredMemorySize(const InfoType& info) {
        return detail::RequiredMemory<Impl>::MemoryFuncImpl::GetRequiredMemorySize(info);
    }

    TDevice();
    void Initialize(const InfoType&);

    void Finalize() { return Impl::Finalize(); }

    void SetUserPtr(void*);
    void* GetUserPtr();
    const void* GetUserPtr() const;
};

}  // namespace nn::gfx