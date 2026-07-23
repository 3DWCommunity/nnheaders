#pragma once

#include <nn/gfx/detail/gfx_DataContainer.h>
#include <nn/gfx/detail/gfx_Declare.h>
#include <nn/gfx/gfx_BufferInfoData-api.nvn.8.h>
#include <nn/gfx/gfx_Common.h>

namespace nn {
namespace gfx {
class BufferInfo;

namespace detail {
template <>
class BufferInfoImpl<ApiVariationNvn8>
    : public DataContainer<BufferInfoImplData<ApiVariationNvn8>> {
public:
    typedef ApiVariationNvn8 Target;
};
};  // namespace detail
};  // namespace gfx
};  // namespace nn