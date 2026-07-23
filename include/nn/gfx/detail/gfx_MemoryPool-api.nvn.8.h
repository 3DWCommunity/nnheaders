#pragma once

#include <nn/gfx/detail/gfx_DataContainer.h>
#include <nn/gfx/gfx_Common.h>
#include <nn/gfx/gfx_MemoryPoolData-api.nvn.8.h>

namespace nn::gfx {

class MemoryPoolInfo;

namespace detail {

template <>
class MemoryPoolImpl<ApiVariationNvn8>
    : public DataContainer<MemoryPoolImplData<ApiVariationNvn8>> {
    NN_NO_COPY(MemoryPoolImpl);

public:
    typedef ApiVariationNvn8 Target;
    typedef MemoryPoolInfo InfoType;

    static size_t GetPoolMemoryAlignment(DeviceImpl<ApiVariationNvn8>*, const InfoType&);
    static size_t GetPoolMemorySizeGranularity(DeviceImpl<ApiVariationNvn8>*, const InfoType&);

    MemoryPoolImpl();
    ~MemoryPoolImpl();

    void Initialize(DeviceImpl<Target>*, const InfoType&);
    void Finalize(DeviceImpl<Target>*);
    void* Map() const;
    void Unmap() const;
    void FlushMappedRange(ptrdiff_t, size_t) const;
    void InvalidateMappedRange(ptrdiff_t, size_t) const;
};

}  // namespace detail

}  // namespace nn::gfx
