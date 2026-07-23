#pragma once

#include <nn/gfx/detail/gfx_Misc.h>
#include <nn/types.h>

namespace nn::gfx::detail {

template <class TImpl>
class RequiredMemory {
protected:
    NN_GFX_OPTION(RequiredMemoryInfo);
    NN_GFX_OPTION_MEMBER_FUNCTION(void, SetMemory, , void*, size_t);
    NN_GFX_OPTION_MEMBER_FUNCTION(void*, GetMemory, );

    enum RequiredMemoryInfo { RequiredMemoryInfo_Alignment = 1 };

    typedef typename Conditional<NN_GFX_OPTION_EXISTS(TImpl, RequiredMemoryInfo), TImpl,
                                 RequiredMemory>::Type MemoryInfoImpl;
    typedef
        typename Conditional<NN_GFX_OPTION_EXISTS(TImpl, SetMemory), TImpl, RequiredMemory>::Type
            MemoryFuncImpl;
};

}  // namespace nn::gfx::detail
