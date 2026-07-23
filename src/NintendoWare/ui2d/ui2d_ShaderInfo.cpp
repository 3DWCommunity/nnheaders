#include "nn/ui2d/ui2d_ShaderInfo.h"
#include "nn/gfx/gfx_CommandBuffer.h"
#include "nn/gfx/gfx_ResShader.h"
#include "nn/gfx/gfx_Shader.h"
#include "nn/gfx/gfx_StateInfo.h"
#include "nn/nn_SdkAssert.h"
#include "nn/ui2d/ui2d_Layout.h"
#include "nn/ui2d/ui2d_Types.h"
#include "nn/ui2d/ui2d_Util.h"

namespace nn {
namespace ui2d {

void ShaderInfo::Initialize(nn::gfx::Device* pDevice, void* pShader) {
    InitializeWithVariationTable(pDevice, pShader, nullptr);
}

void ShaderInfo::InitializeWithVariationTable(nn::gfx::Device* pDevice, void* pShader,
                                              const void* pTable, nn::gfx::MemoryPool* pMemPool,
                                              ptrdiff_t poolOffs, size_t poolSize, int slotCount) {
    SetTextureSlotCount(slotCount);
    m_pResShaderFile = nn::gfx::ResShaderFile::ResCast(pShader);
    nn::gfx::ResShaderContainer* pContainer = m_pResShaderFile->GetShaderContainer();

    if (!IsResShaderContainerInitialized(pContainer)) {
        if (pMemPool == nullptr) {
            pContainer->Initialize(pDevice);
        } else {
            nn::util::BytePtr fileTop(pShader);
            pContainer->Initialize(pDevice, pMemPool, fileTop.Distance(pContainer) + poolOffs,
                                   poolSize);
        }
    }

    nn::gfx::ResShaderVariation* pVar = pContainer->GetResShaderVariation(0);
    const nn::gfx::ShaderCodeType type = TryInitializeAndGetShaderCodeType(pDevice, pVar);
    SetShaderCodeType(type);

    for (int i = 1, variationCount = GetVariationCount(); i < variationCount; i++) {
        nn::gfx::ResShaderVariation* curVar = pContainer->GetResShaderVariation(i);
        // this leads to nothing in the assembly
        const bool isInit = IsResShaderProgramInitialized(curVar->GetResShaderProgram(type));
        nn::gfx::ShaderInitializeResult res =
            isInit ? nn::gfx::ShaderInitializeResult_SetupFailed :
                     curVar->GetResShaderProgram(type)->Initialize(pDevice);
    }

    int varCount = GetVariationCount();
    m_pVertexStates = Layout::NewArray<nn::gfx::VertexState>(varCount);
    m_pVertexShaderSlots = static_cast<int*>(Layout::AllocateMemory(sizeof(int) * varCount));
    m_pGeometryShaderSlots = static_cast<int*>(Layout::AllocateMemory(sizeof(int) * varCount));
    m_pPixelShaderSlots = static_cast<int*>(Layout::AllocateMemory(sizeof(int) * varCount));
    m_pTextureSlots = static_cast<int*>(Layout::AllocateMemory(sizeof(int) * varCount * slotCount));

    for (int i = 0, variationCount = GetVariationCount(); i < variationCount; i++) {
        const nn::gfx::Shader* pVertexShader = GetVertexShader(i);
        const nn::gfx::Shader* pGeometryShader = GetGeometryShader(i);
        const nn::gfx::Shader* pPixelShader = GetPixelShader(i);
        m_pVertexShaderSlots[i] = pVertexShader->GetInterfaceSlot(
            nn::gfx::ShaderStage_Vertex, nn::gfx::ShaderInterfaceType_ConstantBuffer,
            "uConstantBufferForVertexShader");
        m_pGeometryShaderSlots[i] = pGeometryShader->GetInterfaceSlot(
            nn::gfx::ShaderStage_Geometry, nn::gfx::ShaderInterfaceType_ConstantBuffer,
            "uConstantBufferForGeometryShader");
        m_pPixelShaderSlots[i] = pPixelShader->GetInterfaceSlot(
            nn::gfx::ShaderStage_Pixel, nn::gfx::ShaderInterfaceType_ConstantBuffer,
            "uConstantBufferForPixelShader");

        const int bufSize = 32;
        char samplerNameBuffer[bufSize];
        for (int j = 0; j < slotCount; j++) {
            nn::util::SNPrintf(samplerNameBuffer, bufSize, "uTexture%d", j);
            *(m_pTextureSlots + i * slotCount + j) = pPixelShader->GetInterfaceSlot(
                nn::gfx::ShaderStage_Pixel, nn::gfx::ShaderInterfaceType_Sampler,
                samplerNameBuffer);
        }
    }

    InitializeVertexStates(pDevice);
    m_pVariationTable = pTable;
}

void ShaderInfo::SetTextureSlotCount(int count) {
    m_Flags = detail::SetBits(m_Flags, 4, 4, static_cast<uint32_t>(count));
}

void ShaderInfo::SetShaderCodeType(nn::gfx::ShaderCodeType code) {
    m_Flags = detail::SetBits(m_Flags, 0, 3, static_cast<uint32_t>(code));
}

int ShaderInfo::GetVariationCount() const {
    return m_pResShaderFile->GetShaderContainer()->GetShaderVariationCount();
}

const nn::gfx::Shader* ShaderInfo::GetVertexShader(int v) const {
    return m_pResShaderFile->GetShaderContainer()
        ->GetResShaderVariation(v)
        ->GetResShaderProgram(GetShaderCodeType())
        ->GetShader();
}

const nn::gfx::Shader* ShaderInfo::GetGeometryShader(int v) const {
    return m_pResShaderFile->GetShaderContainer()
        ->GetResShaderVariation(v)
        ->GetResShaderProgram(GetShaderCodeType())
        ->GetShader();
}

const nn::gfx::Shader* ShaderInfo::GetPixelShader(int v) const {
    return m_pResShaderFile->GetShaderContainer()
        ->GetResShaderVariation(v)
        ->GetResShaderProgram(GetShaderCodeType())
        ->GetShader();
}

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
            stride += 8;
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
            stride += 8;
        }

        if (pShader->GetInterfaceSlot(nn::gfx::ShaderStage_Vertex,
                                      nn::gfx::ShaderInterfaceType_Input, "aTexCoord") >= 0) {
            attr[attrCount].SetDefault();
            attr[attrCount].SetNamePtr("aTexCoord");
            attr[attrCount].SetBufferIndex(0);
            attr[attrCount].SetFormat(nn::gfx::AttributeFormat_32_32_Float);
            attr[attrCount].SetOffset(stride);
            ++attrCount;
            stride += 8;
        }

        nn::gfx::VertexBufferStateInfo buf[1];
        buf[0].SetDefault();
        buf[0].SetDivisor(0);
        buf[0].SetStride(stride);

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

void ShaderInfo::SetShader(nn::gfx::CommandBuffer& rBuffer, int var) const {
    nn::gfx::ResShaderVariation* pVar =
        m_pResShaderFile->GetShaderContainer()->GetResShaderVariation(var);
    rBuffer.SetShader(pVar->GetResShaderProgram(GetShaderCodeType())->GetShader(),
                      nn::gfx::ShaderStageBit_All);
    rBuffer.SetVertexState(&m_pVertexStates[var]);
}

int ShaderInfo::GetTextureSlotCount() const {
    return detail::GetBits(m_Flags, 4, 4);
}

};  // namespace ui2d
};  // namespace nn