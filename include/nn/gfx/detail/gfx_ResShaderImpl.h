#pragma once

#include <nn/gfx/detail/gfx_Declare.h>
#include <nn/gfx/gfx_Common.h>
#include <nn/gfx/gfx_VariationBase.h>

namespace nn::gfx {

class ResShaderProgram;
class ResShaderContainer;
class ResShaderFile;

namespace detail {
class ResShaderProgramImpl {
public:
    template <typename TTarget>
    static size_t NvnGetRecommendedScrachMemorySize(const ResShaderProgram*, DeviceImpl<TTarget>*);
};

class ResShaderContainerImpl {
public:
    template <typename TTarget>
    static void Initialize(ResShaderContainer* pThis, DeviceImpl<TTarget>* pDevice) {
        Initialize<TTarget>(pThis, pDevice, nullptr, 0, 0);
    }

    template <typename TTarget>
    static void Initialize(ResShaderContainer*, DeviceImpl<TTarget>*, MemoryPoolImpl<TTarget>*,
                           ptrdiff_t, size_t);

    template <typename TTarget>
    static void Finalize(ResShaderContainer*, DeviceImpl<TTarget>*);
};

}  // namespace detail

// todo: figure out where to put this
template <typename TTarget>
size_t NvnGetMaxRecommendedScratchMemorySize(TDevice<TTarget>* pDevice,
                                             const ResShaderFile* const* ppResShaderFileArray,
                                             int shaderFileCount);

}  // namespace nn::gfx