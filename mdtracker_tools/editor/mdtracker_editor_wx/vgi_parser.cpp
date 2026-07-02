#include "vgi_parser.h"
#include <cstring>

InstrumentPreset VgiParser::Parse(const u8* data, size_t size) {
    InstrumentPreset preset;
    if (size < FileSize) return preset;

    preset.algorithm = data[0] & 0x07;
    preset.feedback = data[1] & 0x07;

    int b2 = data[2];
    preset.stereo = (b2 >> 6) & 0x03;
    preset.ams = (b2 >> 4) & 0x03;
    preset.fms = b2 & 0x07;

    for (int op = 0; op < 4; op++) {
        int src = OpRemap[op];
        int off = 3 + src * 10;
        auto& p = preset.operators[op];
        p.multiple = data[off] & 0x0F;
        p.detune = data[off + 1] & 0x07;
        p.totalLevel = data[off + 2] & 0x7F;
        p.rateScaling = data[off + 3] & 0x03;
        p.attackRate = data[off + 4] & 0x1F;
        p.firstDecayRate = data[off + 5] & 0x1F;
        p.secondaryDecayRate = data[off + 6] & 0x1F;
        p.releaseRate = data[off + 7] & 0x0F;
        p.secondaryAmplitude = data[off + 8] & 0x0F;
        p.ssgEg = data[off + 9] & 0x0F;
    }
    return preset;
}

std::vector<u8> VgiParser::Serialize(const InstrumentPreset& preset) {
    std::vector<u8> d(FileSize, 0);
    d[0] = static_cast<u8>(preset.algorithm & 0x07);
    d[1] = static_cast<u8>(preset.feedback & 0x07);
    d[2] = static_cast<u8>(
        ((preset.stereo & 0x03) << 6) |
        ((preset.ams & 0x03) << 4) |
        (preset.fms & 0x07)
    );

    for (int op = 0; op < 4; op++) {
        int dst = OpRemap[op];
        int off = 3 + dst * 10;
        const auto& p = preset.operators[op];
        d[off] = static_cast<u8>(p.multiple & 0x0F);
        d[off + 1] = static_cast<u8>(p.detune & 0x07);
        d[off + 2] = static_cast<u8>(p.totalLevel & 0x7F);
        d[off + 3] = static_cast<u8>(p.rateScaling & 0x03);
        d[off + 4] = static_cast<u8>(p.attackRate & 0x1F);
        d[off + 5] = static_cast<u8>(p.firstDecayRate & 0x1F);
        d[off + 6] = static_cast<u8>(p.secondaryDecayRate & 0x1F);
        d[off + 7] = static_cast<u8>(p.releaseRate & 0x0F);
        d[off + 8] = static_cast<u8>(p.secondaryAmplitude & 0x0F);
        d[off + 9] = static_cast<u8>(p.ssgEg & 0x0F);
    }
    return d;
}
