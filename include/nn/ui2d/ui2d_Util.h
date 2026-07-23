#pragma once

namespace nn {
namespace gfx {
class ResShaderProgram;
class ResShaderContainer;
};  // namespace gfx
namespace ui2d {
bool IsResShaderProgramInitialized(nn::gfx::ResShaderProgram*);

bool IsResShaderContainerInitialized(nn::gfx::ResShaderContainer*);
};  // namespace ui2d
};  // namespace nn