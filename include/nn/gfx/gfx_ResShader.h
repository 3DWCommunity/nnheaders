#pragma once

#include <nn/gfx/detail/gfx_DataContainer.h>
#include <nn/gfx/detail/gfx_Declare.h>
#include <nn/gfx/detail/gfx_ResShaderImpl.h>
#include <nn/gfx/gfx_Common.h>
#include <nn/gfx/gfx_MemoryPool.h>
#include <nn/gfx/gfx_ResShaderData.h>
#include <nn/gfx/gfx_Shader.h>
#include <nn/util/AccessorBase.h>
#include <nn/util/util_BinaryFormat.h>

namespace nn::gfx {

class ShaderInfo;

class ResShaderProgram : public nn::util::AccessorBase<ResShaderProgramData> {
    NN_NO_COPY(ResShaderProgram);

    typedef detail::ResShaderProgramImpl Impl;

public:
    static ResShaderProgram* ToAccessor(value_type*);
    static ResShaderProgram& ToAccessor(value_type&);

    static const ResShaderProgram* ToAccessor(const value_type* pData) {
        return static_cast<const ResShaderProgram*>(pData);
    }

    template <typename TTarget>
    void Finalize(TDevice<TTarget>* pDevice) {
        static_cast<TShader<TTarget>*>(this->pObj.Get())->Finalize(pDevice);
    }

    static const ResShaderProgram& ToAccessor(const value_type&);

    ShaderInfo* GetShaderInfo() { return DataToAccessor(info); }
    const ShaderInfo* GetShaderInfo() const { return DataToAccessor(info); }

    detail::Caster<void> GetShader() { return detail::Caster<void>(this->pObj.Get()); }
    detail::Caster<const void> GetShader() const {
        return detail::Caster<const void>(this->pObj.Get());
    }
    const nngfxToolShaderCompilerShaderReflection* GetShaderCompilerReflection() const;

    template <typename TTarget>
    ShaderInitializeResult Initialize(TDevice<TTarget>* pDevice) {
        return static_cast<TShader<TTarget>*>(this->pObj.Get())
            ->Initialize(pDevice, DataToAccessor(this->info));
    }
};

class ResShaderVariation : public nn::util::AccessorBase<ResShaderVariationData> {
    NN_NO_COPY(ResShaderVariation);

public:
    static ResShaderVariation* ToAccessor(value_type* pData) {
        return static_cast<ResShaderVariation*>(pData);
    }

    static ResShaderVariation& ToAccessor(value_type&);

    static const ResShaderVariation* ToAccessor(const value_type* pData) {
        return static_cast<const ResShaderVariation*>(pData);
    }

    static const ResShaderVariation& ToAccessor(const value_type&);

    ResShaderProgram* GetResShaderProgram(ShaderCodeType codeType) {
        return const_cast<ResShaderProgram*>(
            static_cast<const ResShaderVariation*>(this)->GetResShaderProgram(codeType));
    }

    const ResShaderProgram* GetResShaderProgram(ShaderCodeType codeType) const {
        static const nn::util::BinTPtr<ResShaderProgramData> ResShaderVariationData::*
            s_pResShaderProgramDataTable[] = {
                &ResShaderVariationData::pBinaryProgram,
                &ResShaderVariationData::pIntermediateLanguageProgram,
                &ResShaderVariationData::pSourceProgram,
                nullptr,
            };

        return ResShaderProgram::ToAccessor((this->*s_pResShaderProgramDataTable[codeType]).Get());
    }
};

class ResShaderContainer : public nn::util::AccessorBase<ResShaderContainerData> {
    NN_NO_COPY(ResShaderContainer);

    typedef detail::ResShaderContainerImpl Impl;

public:
    static const int Signature = 0x63737267;  // ??

    static ResShaderContainer* ToAccessor(ResShaderContainerData* pData) {
        return static_cast<ResShaderContainer*>(pData);
    }

    static ResShaderContainer& ToAccessor(ResShaderContainerData& data) {
        return static_cast<ResShaderContainer&>(data);
    }

    static const ResShaderContainer* ToAccessor(const value_type* pData) {
        return static_cast<const ResShaderContainer*>(pData);
    }

    static const ResShaderContainer& ToAccessor(const value_type&);

    ResShaderProgram* GetResShaderProgram(int);
    const ResShaderProgram* GetResShaderProgram(int) const;

    ResShaderVariation* GetResShaderVariation(int index) {
        return ResShaderVariation::ToAccessor(pShaderVariationArray.Get() + index);
    }

    const ResShaderVariation* GetResShaderVariation(int index) const {
        return ResShaderVariation::ToAccessor(pShaderVariationArray.Get() + index);
    }

    template <typename TTarget>
    void Initialize(TDevice<TTarget>* pDevice) {
        return Impl::Initialize(this, pDevice);
    }

    template <typename TTarget>
    void Initialize(TDevice<TTarget>* pDevice, TMemoryPool<TTarget>* pMemoryPool,
                    ptrdiff_t memoryPoolOffset, size_t memoryPoolSize) {
        return Impl::Initialize(this, pDevice, pMemoryPool, memoryPoolOffset, memoryPoolSize);
    }

    template <typename TTarget>
    void Finalize(TDevice<TTarget>* pDevice) {
        return Impl::Finalize(this, pDevice);
    }

    int GetShaderVariationCount() const { return shaderVariationCount; }
};

class ResShaderFile : public nn::util::AccessorBase<ResShaderFileData> {
    NN_NO_COPY(ResShaderFile);

public:
    static const int64_t Signature = 0x48534E42;  // ??
    static const int MajorVersion = 2;
    static const int MinorVersion = 1;
    static const int MicroVersion = 5;

    static size_t GetMaxFileAlignment();

    static ResShaderFile* ToAccessor(ResShaderFile::value_type* pData) {
        return static_cast<ResShaderFile*>(pData);
    }

    static ResShaderFile& ToAccessor(ResShaderFile::value_type& data) {
        return static_cast<ResShaderFile&>(data);
    }

    static const ResShaderFile* ToAccessor(const ResShaderFile::value_type* pData) {
        return static_cast<const ResShaderFile*>(pData);
    }

    static const ResShaderFile& ToAccessor(const ResShaderFile::value_type& data) {
        return static_cast<const ResShaderFile&>(data);
    }

    static bool IsValid(const void*);
    static ResShaderFile* ResCast(void*);

    ResShaderContainer* GetShaderContainer() {
        return ResShaderContainer::ToAccessor(
            reinterpret_cast<ResShaderContainerData*>(this->fileHeader.GetFirstBlock()));
    }

    const ResShaderContainer* GetShaderContainer() const {
        return ResShaderContainer::ToAccessor(
            reinterpret_cast<const ResShaderContainerData*>(fileHeader.GetFirstBlock()));
    }

    nn::util::BinaryFileHeader* GetBinaryFileHeader() { return &fileHeader; }
    const nn::util::BinaryFileHeader* GetBinaryFileHeader() const { return &fileHeader; }
};
}  // namespace nn::gfx