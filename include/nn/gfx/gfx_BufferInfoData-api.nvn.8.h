#pragma once

#include <nn/gfx/detail/gfx_Declare.h>
#include <nn/gfx/gfx_Variation-api.nvn.h>
#include <nn/nn_BitTypes.h>
#include <nvn/nvn.h>

namespace nn {
namespace gfx {
template <>
struct BufferInfoImplData<ApiVariationNvn8> {
    NVNbufferBuilder nvnBufferBuilder;
    uint32_t size;
    Bit32 gpuAccessFlags;
};
};  // namespace gfx
};  // namespace nn