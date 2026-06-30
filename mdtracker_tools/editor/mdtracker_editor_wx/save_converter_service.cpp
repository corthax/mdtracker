#include "save_converter_service.h"
#include <algorithm>
#include <cstring>

std::unordered_set<int> SaveConverterService::ScanUsedInstruments(const std::vector<u8>& sram) {
    std::unordered_set<int> used;
    for (int p = 0; p < LegacySramLayout::PatternCount; p++) {
        int patAddr = static_cast<int>(LegacySramLayout::SrmPatternData) + p * LegacySramLayout::PatternBlockSize;
        for (int r = 0; r < LegacySramLayout::PatternRows; r++) {
            int rowAddr = patAddr + r * LegacySramLayout::PatternRowSize;
            if (static_cast<unsigned>(rowAddr + 1) >= sram.size()) break;
            u8 note = ReadSramByte(sram, rowAddr);
            u8 inst = ReadSramByte(sram, rowAddr + 1);
            if ((note < LegacySramLayout::NoteEmpty || inst != 0) && inst < 0xFE)
                used.insert(inst);
        }
    }
    return used;
}

std::vector<u8> SaveConverterService::Convert(const std::vector<u8>& sram, long long outputSize) {
    auto used = ScanUsedInstruments(sram);
    int bufferSize = std::max(static_cast<int>(std::min<long long>(sram.size(), INT_MAX)),
                              static_cast<int>(outputSize));
    std::vector<u8> output(bufferSize, 0);
    int pos = 0;

    pos = WriteHeader(output, pos);
    pos = WriteStaticArea(output, pos, sram);
    pos = WriteInstrumentBlock(output, pos, used, sram);
    pos = WriteSequencerBlock(output, pos, sram);
    int maxPattern = ScanMatrixMaxPattern(sram);
    pos = WritePatterns(output, pos, sram, maxPattern);

    int finalSize = std::max(pos, static_cast<int>(outputSize));
    output.resize(finalSize);
    return output;
}

int SaveConverterService::WriteHeader(std::vector<u8>& output, int startPos) {
    const char* header = "MDT105";
    int pos = startPos;
    for (int i = 0; i < 6; i++) {
        output[pos++] = header[i];
        output[pos++] = header[i];
    }
    return pos;
}

int SaveConverterService::LogicalToPhysical(int logicalPos) {
    if (logicalPos >= 2 && (logicalPos & 1) != 0)
        return logicalPos - 2;
    return logicalPos;
}

void SaveConverterService::WriteByteAtPos(std::vector<u8>& output, int logicalPos, u8 value) {
    output[LogicalToPhysical(logicalPos)] = value;
}

void SaveConverterService::WriteByteAt(std::vector<u8>& output, int& logicalPos, u8 value) {
    WriteByteAtPos(output, logicalPos, value);
    logicalPos++;
}

void SaveConverterService::WriteWordAt(std::vector<u8>& output, int& logicalPos, u16 value) {
    WriteByteAtPos(output, logicalPos, static_cast<u8>(value >> 8));
    WriteByteAtPos(output, logicalPos + 1, static_cast<u8>(value));
    logicalPos += 2;
}

void SaveConverterService::WriteWordAtPos(std::vector<u8>& output, int logicalPos, u16 value) {
    WriteByteAtPos(output, logicalPos, static_cast<u8>(value >> 8));
    WriteByteAtPos(output, logicalPos + 1, static_cast<u8>(value));
}

int SaveConverterService::WriteStaticArea(std::vector<u8>& output, int startPos, const std::vector<u8>& sram) {
    int pos = startPos;
    WriteByteAt(output, pos, ReadSramByte(sram, static_cast<int>(LegacySramLayout::SrmLfo)));
    WriteByteAt(output, pos, 0);
    WriteByteAt(output, pos, 0);
    int tempo = ReadSramWord(sram, static_cast<int>(LegacySramLayout::SrmTempo));
    WriteWordAt(output, pos, static_cast<u16>(tempo));

    int matrixSrcBase = static_cast<int>(LegacySramLayout::SrmPatternMatrix);
    int transposeSrcBase = static_cast<int>(LegacySramLayout::SrmMatrixTranspose);

    for (int row = 0; row < SramV05Layout::MatrixRows; row++) {
        u16 bitmap = 0;
        u16 tmp[13];
        int count = 0;

        for (int ch = 0; ch < SramV05Layout::MatrixChannels; ch++) {
            int i = ch * SramV05Layout::MatrixRows + row;
            int patternID = ReadSramWord(sram, matrixSrcBase + i * 2) & 0x3FF;
            if (patternID != 0) {
                bitmap |= static_cast<u16>(1u << ch);
                int transpose = static_cast<s8>(ReadSramByte(sram, transposeSrcBase + i));
                tmp[count++] = static_cast<u16>(patternID | ((transpose & 0x3F) << 10));
            }
        }

        WriteWordAt(output, pos, bitmap);
        for (int j = 0; j < count; j++)
            WriteWordAt(output, pos, tmp[j]);
    }
    return pos;
}

int SaveConverterService::WriteInstrumentBlock(std::vector<u8>& output, int startPos,
                                                const std::unordered_set<int>& used,
                                                const std::vector<u8>& sram) {
    int pos = startPos;
    u8 lookup[256] = {};
    int compactCount = 0;
    for (int i = 0; i < 256; i++) {
        if (used.count(i)) {
            lookup[i] = SramV05Layout::InstSentinelModified;
            compactCount++;
        }
    }

    WriteWordAt(output, pos, static_cast<u16>(compactCount));
    for (int i = 0; i < 256; i++)
        WriteByteAt(output, pos, lookup[i]);

    for (int i = 0; i < 256; i++) {
        if (used.count(i)) {
            WriteByteAt(output, pos, static_cast<u8>(i));
            int instAddr = static_cast<int>(LegacySramLayout::SrmInstrumentData) + i * LegacySramLayout::InstrumentRecordSize;
            for (int b = 0; b < 49; b++)
                WriteByteAt(output, pos, ReadSramByte(sram, instAddr + b));
            for (int b = 81; b < 89; b++)
                WriteByteAt(output, pos, ReadSramByte(sram, instAddr + b));
        }
    }
    return pos;
}

int SaveConverterService::WriteSequencerBlock(std::vector<u8>& output, int startPos, const std::vector<u8>& sram) {
    int pos = startPos;
    std::unordered_set<int> used;
    for (int i = 0; i < 256; i++) {
        int volAddr = static_cast<int>(LegacySramLayout::SrmSeqVolStart) + i * 32;
        int arpAddr = static_cast<int>(LegacySramLayout::SrmSeqArpStart) + i * 32;
        bool hasVol = false, hasArp = false;
        for (int j = 0; j < 32; j++) {
            if (ReadSramByte(sram, volAddr + j) != 0xFF) hasVol = true;
            if (ReadSramByte(sram, arpAddr + j) != LegacySramLayout::NoteEmpty) hasArp = true;
        }
        if (hasVol || hasArp) used.insert(i);
    }

    WriteWordAt(output, pos, static_cast<u16>(used.size()));
    for (int i = 0; i < 256; i++)
        WriteByteAt(output, pos, used.count(i) ? SramV05Layout::InstSentinelModified : u8(0));

    for (int i = 0; i < 256; i++) {
        if (used.count(i)) {
            int volAddr = static_cast<int>(LegacySramLayout::SrmSeqVolStart) + i * 32;
            int arpAddr = static_cast<int>(LegacySramLayout::SrmSeqArpStart) + i * 32;
            WriteByteAt(output, pos, static_cast<u8>(i));
            for (int j = 0; j < 32; j++)
                WriteByteAt(output, pos, ReadSramByte(sram, volAddr + j));
            for (int j = 0; j < 32; j++)
                WriteByteAt(output, pos, ReadSramByte(sram, arpAddr + j));
        }
    }
    return pos;
}

int SaveConverterService::WritePatterns(std::vector<u8>& output, int startPos,
                                         const std::vector<u8>& sram, int maxPattern) {
    int pos = startPos;
    WriteWordAt(output, pos, SramV05Layout::PatternMagic);
    WriteWordAt(output, pos, SramV05Layout::PatternFormatVersion);
    int regionSizePos = pos;
    WriteWordAt(output, pos, 0);

    for (int p = 0; p <= maxPattern; p++) {
        std::vector<u8> events;
        int patAddr = static_cast<int>(LegacySramLayout::SrmPatternData) + p * LegacySramLayout::PatternBlockSize;

        for (int row = 0; row < LegacySramLayout::PatternRows; row++) {
            int rowAddr = patAddr + row * LegacySramLayout::PatternRowSize;
            u8 note = ReadSramByte(sram, rowAddr);
            u8 inst = ReadSramByte(sram, rowAddr + 1);

            if (note <= 95 || note == LegacySramLayout::NoteOff || inst != 0) {
                events.push_back(static_cast<u8>(row * SramV05Layout::EvtCount + 0));
                events.push_back(note);
                events.push_back(inst);
            }

            for (int fx = 0; fx < SramV05Layout::EffectsTotal; fx++) {
                int fxOff = 2 + fx * 2;
                u8 fxType = ReadSramByte(sram, rowAddr + fxOff);
                u8 fxVal = ReadSramByte(sram, rowAddr + fxOff + 1);
                if (fxType != 0 || fxVal != 0) {
                    events.push_back(static_cast<u8>(row * SramV05Layout::EvtCount + 1 + fx));
                    events.push_back(fxType);
                    events.push_back(fxVal);
                }
            }
        }

        if (!events.empty()) {
            u8 color = (p < LegacySramLayout::PatternCount)
                ? ReadSramByte(sram, static_cast<int>(LegacySramLayout::SrmPatternColor) + p) & 0x3F
                : u8(0);
            WriteWordAt(output, pos, static_cast<u16>(p | (color << 10)));
            WriteWordAt(output, pos, static_cast<u16>(events.size() / 3));
            for (u8 b : events) WriteByteAt(output, pos, b);
            if ((events.size() & 1) != 0) WriteByteAt(output, pos, 0);
        }
    }

    WriteWordAtPos(output, regionSizePos, static_cast<u16>(pos - startPos));
    return pos;
}

int SaveConverterService::ScanMatrixMaxPattern(const std::vector<u8>& sram) {
    int matrixBase = static_cast<int>(LegacySramLayout::SrmPatternMatrix);
    int maxPat = 0;
    int cellCount = SramV05Layout::MatrixRows * SramV05Layout::MatrixChannels;
    for (int i = 0; i < cellCount; i++) {
        int pat = ReadSramWord(sram, matrixBase + i * 2) & 0x3FF;
        if (pat > maxPat) maxPat = pat;
    }
    return maxPat;
}

u8 SaveConverterService::ReadSramByte(const std::vector<u8>& sram, int addr) {
    int fixedAddr = static_cast<int>(LegacySramLayout::WordSwapOffset(static_cast<uint32_t>(addr)));
    return static_cast<unsigned>(fixedAddr) < sram.size() ? sram[fixedAddr] : u8(0);
}

int SaveConverterService::ReadSramWord(const std::vector<u8>& sram, int addr) {
    return (ReadSramByte(sram, addr) << 8) | ReadSramByte(sram, addr + 1);
}
