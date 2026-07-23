#pragma once

#include <nn/gfx/gfx_TargetConfig.h>

namespace nn {
namespace gfx {
template <typename TTarget>
class TDevice;

template <typename TTarget>
class TShader;

template <typename TTarget>
class TBuffer;

template <typename TTarget>
struct BufferInfoImplData;

template <typename TTarget>
struct DeviceInfoImplData;

template <typename TTarget = TargetConfig::Variation>
class TDeviceInfo;

template <typename TTarget>
class TMemoryPool;

namespace detail {

template <typename TTarget>
class MemoryPoolImpl;

template <typename TTarget>
class MemoryPoolInfoImpl;

template <typename TTarget>
class DeviceImpl;

template <typename TTarget>
class BufferInfoImpl;

template <typename TTarget>
class ShaderImpl;

template <typename TTarget>
class BufferImpl;

template <typename TTarget>
class DeviceInfoImpl;

template <typename TTarget>
class CommandBufferImpl;

template <typename TTarget>
void Initialize();

template <typename TTarget>
void Finalize();

};  // namespace detail

};  // namespace gfx
};  // namespace nn