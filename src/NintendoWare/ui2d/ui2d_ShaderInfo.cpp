#include "nn/ui2d/ui2d_ShaderInfo.h"
#include "nn/gfx/gfx_CommandBuffer.h"
#include "nn/gfx/gfx_Shader.h"
#include "nn/gfx/gfx_StateInfo.h"
#include "nn/nn_SdkAssert.h"
#include "nn/ui2d/ui2d_Layout.h"
#include "nn/ui2d/ui2d_Types.h"
#include "nn/ui2d/ui2d_Util.h"

namespace nn {
namespace ui2d {

// mismatch, regswap
// https://decomp.me/scratch/DP7p0
void ShaderInfo::InitializeVertexStates(nn::gfx::Device* pDevice) {
    for (int i = 0, variationCount = GetVariationCount(); i < variationCount; i++) {
        const nn::gfx::Shader* pShader = GetVertexShader(i);
        nn::gfx::VertexState::InfoType info;
        info.SetDefault();

        nn::gfx::VertexAttributeStateInfo attr[3];
        int attrCount = 0;
        int stride = 0;

        if (pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Vertex,
                                      nn::gfx::ShaderInterfaceType_Input, "aVertexIndex") >= 0) {
            attr[attrCount].SetDefault();
            attr[attrCount].SetNamePtr("aVertexIndex");
            attr[attrCount].SetBufferIndex(0);
            attr[attrCount].SetFormat(nn::gfx::AttributeFormat_32_32_Float);
            attr[attrCount].SetOffset(0);
            ++attrCount;
            stride += sizeof(float) * 2;
        }

        if (pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Vertex,
                                      nn::gfx::ShaderInterfaceType_Input, "aPosition") >= 0) {
            attr[attrCount].SetDefault();
            attr[attrCount].SetNamePtr("aPosition");
            attr[attrCount].SetBufferIndex(0);
            attr[attrCount].SetFormat(nn::gfx::AttributeFormat_32_32_32_Float);
            attr[attrCount].SetOffset(stride);
            ++attrCount;
            stride += 12;
        }

        if (pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Vertex,
                                      nn::gfx::ShaderInterfaceType_Input, "aPositionVg") >= 0) {
            attr[attrCount].SetDefault();
            attr[attrCount].SetNamePtr("aPositionVg");
            attr[attrCount].SetBufferIndex(0);
            attr[attrCount].SetFormat(nn::gfx::AttributeFormat_32_32_Float);
            attr[attrCount].SetOffset(stride);
            ++attrCount;
            stride += sizeof(float) * 2;
        }

        if (pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Vertex,
                                      nn::gfx::ShaderInterfaceType_Input, "aTexCoord") >= 0) {
            attr[attrCount].SetDefault();
            attr[attrCount].SetNamePtr("aTexCoord");
            attr[attrCount].SetBufferIndex(0);
            attr[attrCount].SetFormat(nn::gfx::AttributeFormat_32_32_Float);
            attr[attrCount].SetOffset(stride);
            ++attrCount;
            stride += sizeof(float) * 2;
        }

        nn::gfx::VertexBufferStateInfo buf[1];
        buf[0].SetDefault();
        buf[0].SetStride(stride);
        buf[0].SetDivisor(0);

        info.SetVertexAttributeStateInfoArray(attr, attrCount);
        info.SetVertexBufferStateInfoArray(buf, 1);
        size_t memSize = nn::gfx::VertexState::GetRequiredMemorySize(info);
        m_pVertexStates[i].SetMemory(
            Layout::AllocateMemory(memSize, nn::gfx::VertexState::RequiredMemoryInfo_Alignment),
            memSize);
        m_pVertexStates[i].Initialize(pDevice, info, pShader);
    }
}

void ShaderInfo::Finalize(nn::gfx::Device* pDevice, bool skip) {
    for (int i = 0, variationCount = GetVariationCount(); i < variationCount; i++) {
        void* buf = m_pVertexStates[i].GetMemory();
        m_pVertexStates[i].Finalize(pDevice);
        Layout::FreeMemory(buf);

        if (!skip) {
            nn::gfx::ResShaderVariation* var =
                m_pResShaderFile->GetShaderContainer()->GetResShaderVariation(i);

            if (IsResShaderProgramInitialized(var->GetResShaderProgram(GetShaderCodeType()))) {
                var->GetResShaderProgram(GetShaderCodeType())->Finalize(pDevice);
            }
        }
    }

    if (!skip) {
        nn::gfx::ResShaderContainer* container = m_pResShaderFile->GetShaderContainer();

        if (IsResShaderContainerInitialized(container)) {
            container->Finalize(pDevice);
        }

        if (m_pResShaderFile->GetBinaryFileHeader()->IsRelocated()) {
            m_pResShaderFile->GetBinaryFileHeader()->GetRelocationTable()->Unrelocate();
        }
    }

    Layout::DeleteArray(m_pVertexStates, GetVariationCount());
    Layout::FreeMemory(m_pVertexShaderSlots);

    if (m_pGeometryShaderSlots != nullptr) {
        Layout::FreeMemory(m_pGeometryShaderSlots);
    }

    Layout::FreeMemory(m_pPixelShaderSlots);
    Layout::FreeMemory(m_pTextureSlots);
}

nn::gfx::ShaderCodeType ShaderInfo::GetShaderCodeType() const {
    return static_cast<nn::gfx::ShaderCodeType>(detail::GetBits(m_Flags, 0, 3));
}

int ShaderInfo::GetVariationCount() const {
    return m_pResShaderFile->GetShaderContainer()->GetShaderVariationCount();
}

void ShaderInfo::SetShader(nn::gfx::CommandBuffer& rBuffer, int var) const {
    nn::gfx::ResShaderVariation* pVar =
        m_pResShaderFile->GetShaderContainer()->GetResShaderVariation(var);
    rBuffer.SetShader(pVar->GetResShaderProgram(GetShaderCodeType())->GetShader(),
                      nn::gfx::ShaderStageBit_All);
    rBuffer.SetVertexState(&m_pVertexStates[var]);
}

const nn::gfx::Shader* ShaderInfo::GetVertexShader(int v) const {
    return m_pResShaderFile->GetShaderContainer()
        ->GetResShaderVariation(v)
        ->GetResShaderProgram(GetShaderCodeType())
        ->GetShader();
}

int ShaderInfo::GetTextureSlotCount() const {
    return detail::GetBits(m_Flags, 4, 4);
}

};  // namespace ui2d
};  // namespace nn