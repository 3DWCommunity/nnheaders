#pragma once

#include "nn/font/font_ResFontBase.h"

namespace nn {
namespace font {
class ResFont : public ResFontBase {
public:
    static void Unrelocate(void*);
};
};  // namespace font
};  // namespace nn