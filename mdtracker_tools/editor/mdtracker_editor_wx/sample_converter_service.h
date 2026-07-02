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
    std::vector<short> Resample(const std::vector<short>& input, int inputRate, int outputRate, bool linear = false) const;
    std::vector<u8> EncodePcm8(const std::vector<short>& samples, bool clampTo254 = false) const;
    std::vector<u8> EncodeAdpcm(const std::vector<short>& samples) const;
    std::vector<u8> ConvertPcmToAdpcm(const std::vector<u8>& pcmData) const;

    struct EQConfig {
        struct Band { double freqHz; double q; double gainDB; };
        Band lowShelf{200, 0.707, 0};
        Band peak{1000, 0.707, 0};
        Band highShelf{10000, 0.707, 0};
    };
    std::vector<short> ApplyEQ(const std::vector<short>& input, int sampleRate, const EQConfig& eq) const;

    // Rescomp pipeline
    bool RescompAvailable() const;
    std::vector<u8> ConvertViaRescomp(const std::vector<short>& samples, int sampleRate,
                                       const std::string& format, int targetRate) const;

private:
    std::vector<u8> dpcmPack(const std::vector<u8>& pcm8) const;
    std::vector<short> ProcessBiquad(const std::vector<short>& input, double b0, double b1, double b2, double a1, double a2) const;
    void CalcLowShelf(double freq, int sampleRate, double q, double gainDB, double& b0, double& b1, double& b2, double& a1, double& a2) const;
    void CalcPeak(double freq, int sampleRate, double q, double gainDB, double& b0, double& b1, double& b2, double& a1, double& a2) const;
    void CalcHighShelf(double freq, int sampleRate, double q, double gainDB, double& b0, double& b1, double& b2, double& a1, double& a2) const;
    std::string FindRescompJar() const;
    std::string WriteTempWav(const std::vector<short>& samples, int sampleRate, const std::string& dir) const;
};
