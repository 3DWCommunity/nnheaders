#pragma once

#include <nn/gfx/detail/gfx_Shader-api.nvn.8.h>
#include <nn/gfx/gfx_Device.h>

namespace nn::gfx {

template <class TTarget>
class TShader : public detail::ShaderImpl<typename detail::TargetVariation<TTarget>::Type> {
    NN_NO_COPY(TShader);

    typedef detail::ShaderImpl<typename detail::TargetVariation<TTarget>::Type> Impl;

public:
    typedef typename Impl::Target Target;
    typedef ShaderInfo InfoType;

    static size_t GetBinaryCodeAlignment(TDevice<TTarget>*);

    TShader();

    ShaderInitializeResult Initialize(TDevice<Target>* pDevice, const InfoType& info) {
        return Impl::Initialize(pDevice, info);
    }

    void Finalize(TDevice<Target>* pDevice) { return Impl::Finalize(pDevice); }

    int GetInterfaceSlot(ShaderStage stage, ShaderInterfaceType shaderInterfaceType,
                         const char* pName) const {
        return Impl::GetInterfaceSlot(stage, shaderInterfaceType, pName);
    }

    void GetWorkGroupSize(int*, int*, int*) const;
    void SetUserPtr(void*);
    void* GetUserPtr();
    const void* GetUserPtr() const;
};

}  // namespace nn::gfx