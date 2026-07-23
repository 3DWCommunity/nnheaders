#pragma once

#include <nn/gfx/gfx_Common.h>
#include <nn/gfx/gfx_TargetConfig.h>

namespace nn::gfx {

typedef TBuffer<TargetConfig::Variation> Buffer;

typedef TCommandBuffer<TargetConfig::Variation> CommandBuffer;
typedef TDescriptorPool<TargetConfig::Variation> DescriptorPool;
typedef TDevice<TargetConfig::Variation> Device;
typedef TMemoryPool<TargetConfig::Variation> MemoryPool;
typedef TQueue<TargetConfig::Variation> Queue;
typedef TSampler<TargetConfig::Variation> Sampler;
typedef TShader<TargetConfig::Variation> Shader;

typedef TRasterizerState<TargetConfig::Variation> RasterizerState;
typedef TBlendState<TargetConfig::Variation> BlendState;
typedef TDepthStencilState<TargetConfig::Variation> DepthStencilState;
typedef TVertexState<TargetConfig::Variation> VertexState;
typedef TViewportScissorState<TargetConfig::Variation> ViewportScissorState;

typedef TSwapChain<TargetConfig::Variation> SwapChain;
typedef TFence<TargetConfig::Variation> Fence;
typedef TSemaphore<TargetConfig::Variation> Semaphore;

typedef TTexture<TargetConfig::Variation> Texture;
typedef TTextureView<TargetConfig::Variation> TextureView;
typedef TColorTargetView<TargetConfig::Variation> ColorTargetView;
typedef TDepthStencilView<TargetConfig::Variation> DepthStencilView;

}  // namespace nn::gfx