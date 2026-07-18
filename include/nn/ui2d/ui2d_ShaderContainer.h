#pragma once

#include <nn/gfx/gfx_Types.h>
#include <nn/util/util_IntrusiveList.h>

namespace nn {
namespace ui2d {
class ShaderRefLink {
public:
    ShaderRefLink(bool);

    ~ShaderRefLink();

    void Finalize(nn::gfx::Device*);

    nn::util::IntrusiveListNode m_Link;
    char m_Name[8];
};
};  // namespace ui2d
};  // namespace nn