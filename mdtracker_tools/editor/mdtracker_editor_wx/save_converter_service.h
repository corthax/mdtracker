#pragma once
#include "models.h"
#include <vector>
#include <unordered_set>

class SaveConverterService {
public:
    static constexpr long long OutputSizes[] = {65536, 131072, 262144, 524288};

    static std::unordered_set<int> ScanUsedInstruments(const std::vector<u8>& sram);
    static std::vector<u8> Convert(const std::vector<u8>& sram, long long outputSize);

private:
    static int WriteHeader(std::vector<u8>& output, int startPos);
    static int LogicalToPhysical(int logicalPos);
    static void WriteByteAtPos(std::vector<u8>& output, int logicalPos, u8 value);
    static void WriteByteAt(std::vector<u8>& output, int& logicalPos, u8 value);
    static void WriteWordAt(std::vector<u8>& output, int& logicalPos, u16 value);
    static void WriteWordAtPos(std::vector<u8>& output, int logicalPos, u16 value);
    static int WriteStaticArea(std::vector<u8>& output, int startPos, const std::vector<u8>& sram);
    static int WriteInstrumentBlock(std::vector<u8>& output, int startPos, const std::unordered_set<int>& used, const std::vector<u8>& sram);
    static int WriteSequencerBlock(std::vector<u8>& output, int startPos, const std::vector<u8>& sram);
    static int WritePatterns(std::vector<u8>& output, int startPos, const std::vector<u8>& sram, int maxPattern);
    static int ScanMatrixMaxPattern(const std::vector<u8>& sram);

    static u8 ReadSramByte(const std::vector<u8>& sram, int addr);
    static int ReadSramWord(const std::vector<u8>& sram, int addr);
};
