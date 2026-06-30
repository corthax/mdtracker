#pragma once
#include "compat_types.h"
#include <vector>
#include <string>
#include <cstdint>

struct WavInfo {
    int sampleRate{};
    int channels{};
    int bitsPerSample{};
    int dataOffset{};
    int dataSize{};
    std::vector<u8> rawData;
};

class SampleConverterService {
public:
    static constexpr int PcmRates[] = {8000, 11025, 13400, 16000, 22050, 32000};

    WavInfo ParseWav(const std::vector<u8>& data) const;
    std::vector<short> DecodeSamples(const WavInfo& wav) const;
    std::vector<short> DecodeFlac(const std::vector<u8>& data) const;
    std::vector<short> DecodeWavpack(const std::vector<u8>& data, int& sampleRateOut) const;
    std::vector<short> Resample(const std::vector<short>& input, int inputRate, int outputRate) const;
    std::vector<u8> EncodePcm8(const std::vector<short>& samples) const;
    std::vector<u8> EncodeAdpcm(const std::vector<short>& samples) const;
    std::vector<u8> ConvertPcmToAdpcm(const std::vector<u8>& pcmData) const;
};
