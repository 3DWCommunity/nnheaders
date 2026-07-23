#pragma once

#include <nn/types.h>

#include <nn/gfx/detail/gfx_Fwd.h>
#include <nn/gfx/gfx_Variation-api.nvn.h>
#include <nn/util.h>

namespace nn::gfx {

struct OutOfMemoryEventArg {
    size_t minRequiredSize;
};

union ClearColorValue {
    float valueFloat[4];
    int32_t valueInt[4];
    uint32_t valueUint[4];
};

}  // namespace nn::gfx