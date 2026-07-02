#pragma once
#include "models.h"
#include <vector>

struct TfiParser {
    static constexpr int FileSize = 42;
    static constexpr int OpRemap[4] = {0, 2, 1, 3};

    static InstrumentPreset Parse(const u8* data, size_t size);
    static std::vector<u8> Serialize(const InstrumentPreset& preset);
};
