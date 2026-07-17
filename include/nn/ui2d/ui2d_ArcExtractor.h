#pragma once

#include <nn/types.h>

namespace nn {
namespace ui2d {

enum EndianTypes { EndianTypes_Big = 0, EndianTypes_Little = 1 };

class ArcEntry {
public:
    ArcEntry() { name[0] = '\0'; }

    char name[256];
};

class ArcFileInfo {
public:
    ArcFileInfo() {
        m_StartOffset = 0;
        m_Length = 0;
    }

    int GetStartOffset() const { return m_StartOffset; }

    size_t GetLength() const { return m_Length; }

    int32_t m_StartOffset;
    int32_t m_Length;
};

class ArcExtractor {
public:
    ArcExtractor(const void*);
    ArcExtractor();
    ~ArcExtractor();

    bool PrepareArchive(const void*);
    int GetFileCount() const;
    void* GetFileFast(ArcFileInfo*, int);
    int ConvertPathToEntryId(const char*) const;
    int ReadEntry(int*, ArcEntry*, int) const;
    void SetArcFileInfo(ArcFileInfo*, int, size_t);

    static void Relocate(const void*);
    static void Unrelocate(const void*);

    struct ArchiveBlockHeader {
        char signature[4];
        int16_t headerSize;
        int16_t byteOrder;
        int32_t fileSize;
        int32_t dataBlockOffset;
        int16_t version;
        int16_t reserved;
    };

    struct FATBlockHeader {
        char signature[4];
        int16_t headerSize;
        int16_t fileCount;
        int32_t hashKey;
    };

    struct FATEntry {
        int32_t hash;
        int32_t nameOffset;
        int32_t dataStartOffset;
        int32_t dataEndOffset;
    };

    struct FNTBlockHeader {
        char signature[4];
        int16_t headerSize;
        int16_t reserved;
    };

    const ArchiveBlockHeader* m_pArchiveBlockHeader;
    const FATBlockHeader* m_pFATBlockHeader;
    const char* m_pFNTBlock;
    FATEntry* m_pFATEntries;
    int m_FATEntryCount;
    const uint8_t* m_pDataBlock;
    EndianTypes m_EndianType;
};
};  // namespace ui2d
};  // namespace nn