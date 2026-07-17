#include "nn/ui2d/ui2d_ArcExtractor.h"
#include "nn/font/detail/font_ResourceFormat.h"
#include "nn/font/font_ResFont.h"
#include "nn/util/util_BinTypes.h"
#include "nn/util/util_BinaryFormat.h"
#include "nn/util/util_FormatString.h"
#include "nn/util/util_StringUtil.h"

namespace {
template <typename T>
T EndianToHost(nn::ui2d::EndianTypes e, const T& val) {
    return (e == nn::ui2d::EndianTypes_Big) ? nn::font::detail::ByteSwap(val) : val;
}

inline uint32_t calcHash32(const char* str, uint32_t key) {
    uint32_t ret = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        ret = ret * key + str[i];
    }
    return ret;
}

inline int32_t binarySearch_(uint32_t hash, const nn::ui2d::ArcExtractor::FATEntry* pEntries,
                             int32_t start, int32_t end, nn::ui2d::EndianTypes endian) {
    int32_t middle;

    for (;;) {
        middle = (start + end) / 2;
        uint32_t bufHash = EndianToHost(endian, pEntries[middle].hash);
        if (bufHash == hash) {
            return middle;
        } else if (bufHash < hash) {
            if (start == middle) {
                return -1;
            }
            start = middle;
        } else {
            if (end == middle) {
                return -1;
            }
            end = middle;
        }
    }
}
};  // namespace

namespace nn {
namespace ui2d {

#define SWAP_ENDIAN(val) (EndianToHost(m_EndianType, (val)))

ArcExtractor::ArcExtractor(const void* pArchive)
    : m_pArchiveBlockHeader(NULL), m_pFATBlockHeader(NULL), m_pFNTBlock(NULL), m_pFATEntries(NULL),
      m_FATEntryCount(0), m_pDataBlock(NULL), m_EndianType(EndianTypes_Little) {
    PrepareArchive(pArchive);
}

ArcExtractor::ArcExtractor()
    : m_pArchiveBlockHeader(nullptr), m_pFATBlockHeader(nullptr), m_pFNTBlock(nullptr),
      m_pFATEntries(nullptr), m_FATEntryCount(0), m_pDataBlock(nullptr),
      m_EndianType(EndianTypes_Little) {}

ArcExtractor::~ArcExtractor() {}

void ArcExtractor::Relocate(const void*) {}

// something seems to get duplicated here but I can't tell what without a lot of issues
void ArcExtractor::Unrelocate(const void* pArchive) {
    ArcExtractor a(pArchive);
    int count = a.GetFileCount();

    for (int i = 0; i < count; i++) {
        void* pFile = a.GetFileFast(nullptr, i);
        nn::util::BinaryFileHeader* pHeader = static_cast<nn::util::BinaryFileHeader*>(pFile);

        if (pHeader->signature.IsValid('BNTX') || pHeader->signature.IsValid('BNSH')) {
            if (pHeader->IsRelocated()) {
                pHeader->GetRelocationTable()->Unrelocate();
                pHeader->SetRelocated(false);
            }
        }

        if (pHeader->signature.IsValid('FFNT')) {
            nn::font::ResFont::Unrelocate(pFile);
        }
    }
}

int ArcExtractor::GetFileCount() const {
    if (m_FATEntryCount < 0) {
        return 0;
    }

    return m_FATEntryCount;
}

inline void ArcExtractor::SetArcFileInfo(ArcFileInfo* pInfo, int startOffs, size_t len) {
    pInfo->m_StartOffset = startOffs;
    pInfo->m_Length = len;
}

void* ArcExtractor::GetFileFast(ArcFileInfo* pInfo, int id) {
    if (id < 0 || id >= m_FATEntryCount) {
        return nullptr;
    }

    uint32_t startOffs = SWAP_ENDIAN(m_pFATEntries[id].dataStartOffset);

    if (pInfo != nullptr) {
        uint32_t endOffs = SWAP_ENDIAN(m_pFATEntries[id].dataEndOffset);

        if (startOffs > endOffs) {
            return nullptr;
        }

        uint32_t len = endOffs - startOffs;
        SetArcFileInfo(pInfo, startOffs, len);
    }

    return nn::util::BytePtr(const_cast<uint8_t*>(m_pDataBlock)).Advance(startOffs).Get();
}

int ArcExtractor::ConvertPathToEntryId(const char* pPath) const {
    uint32_t hash = calcHash32(pPath, SWAP_ENDIAN(m_pFATBlockHeader->hashKey));
    int32_t start = 0;
    int32_t end = m_FATEntryCount;

    int32_t id = binarySearch_(hash, m_pFATEntries, start, end, m_EndianType);

    if (id == -1) {
        return -1;
    }

    uint32_t offs = SWAP_ENDIAN(m_pFATEntries[id].nameOffset);

    if (offs != 0) {
        id = id - ((offs >> 24) - 1);
        while (id < end) {
            const FATEntry* e = &m_pFATEntries[id];

            if (static_cast<uint32_t>(SWAP_ENDIAN(e->hash)) != hash) {
                return -1;
            } else {
                uint32_t nameOffs = SWAP_ENDIAN(e->nameOffset);

                if (nn::util::ConstBytePtr(m_pFNTBlock).Advance(nameOffs & 0xFFFFFF).Get() >
                    m_pDataBlock) {
                    return -1;
                }

                if (std::strcmp(pPath, m_pFNTBlock + (nameOffs & 0xFFFFFF) * 4) == 0) {
                    return id;
                }
            }

            id++;
        }
    }

    return id;
}

// needs some work with endian checking
int ArcExtractor::ReadEntry(int* pHandle, ArcEntry* pEntries, int n) const {
    int32_t cnt = 0;

    while (*pHandle + cnt < SWAP_ENDIAN(m_pFATBlockHeader->fileCount) && cnt < n) {
        int32_t id = *pHandle + cnt;
        uint32_t offs = SWAP_ENDIAN(m_pFATEntries[id].nameOffset);

        if (offs == 0) {
            if (m_EndianType == EndianTypes_Big) {
                nn::util::SNPrintf(pEntries[cnt].name, 256, "%08x",
                                   SWAP_ENDIAN(m_pFATEntries[id].hash));
            } else {
                nn::util::SNPrintf(pEntries[cnt].name, 256, "%08x", (m_pFATEntries[id].hash));
            }

        } else {
            if (nn::util::ConstBytePtr(m_pFNTBlock).Advance(offs & 0xFFFFFF).Get() > m_pDataBlock) {
                pEntries[cnt].name[0] = '\0';
            } else {
                nn::util::Strlcpy(pEntries[cnt].name, m_pFNTBlock + (offs & 0xFFFFFFF) * 4, 256);
            }
        }

        cnt++;
    }

    *pHandle += cnt;
    return cnt;
}
};  // namespace ui2d
};  // namespace nn