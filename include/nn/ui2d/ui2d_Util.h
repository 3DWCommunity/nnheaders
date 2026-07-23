#pragma once

namespace nn {
namespace gfx {
class ResShaderProgram;
class ResShaderContainer;
};  // namespace gfx
namespace ui2d {
bool IsResShaderProgramInitialized(nn::gfx::ResShaderProgram*);

bool IsResShaderContainerInitialized(nn::gfx::ResShaderContainer*);

nn::gfx::ShaderCodeType TryInitializeAndGetShaderCodeType(nn::gfx::Device*,
                                                          nn::gfx::ResShaderVariation*);
};  // namespace ui2d
};  // namespace nn