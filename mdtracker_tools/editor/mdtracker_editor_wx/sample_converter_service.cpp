#include "sample_converter_service.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include <wavpack/wavpack.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <string>

WavInfo SampleConverterService::ParseWav(const std::vector<u8>& data) const {
    int pos = 12;
    int sampleRate = 22050;
    int channels = 1;
    int bitsPerSample = 8;
    int dataSize = 0;
    int dataOffset = 0;

    while (pos + 8 <= static_cast<int>(data.size())) {
        char chunkId[5] = {};
        std::memcpy(chunkId, &data[pos], 4);
        int chunkSize = data[pos + 4] | (data[pos + 5] << 8) | (data[pos + 6] << 16) | (data[pos + 7] << 24);

        if (std::strcmp(chunkId, "fmt ") == 0) {
            channels = data[pos + 10] | (data[pos + 11] << 8);
            sampleRate = data[pos + 12] | (data[pos + 13] << 8) | (data[pos + 14] << 16) | (data[pos + 15] << 24);
            bitsPerSample = data[pos + 22] | (data[pos + 23] << 8);
        } else if (std::strcmp(chunkId, "data") == 0) {
            dataOffset = pos + 8;
            dataSize = chunkSize;
        }
        pos += 8 + chunkSize;
    }

    if (dataSize == 0 || dataOffset == 0)
        throw std::runtime_error("No data chunk found in WAV file");

    WavInfo wav;
    wav.sampleRate = sampleRate;
    wav.channels = channels;
    wav.bitsPerSample = bitsPerSample;
    wav.dataOffset = dataOffset;
    wav.dataSize = dataSize;
    wav.rawData = data;
    return wav;
}

std::vector<short> SampleConverterService::DecodeSamples(const WavInfo& wav) const {
    int totalSamples = wav.dataSize / (wav.bitsPerSample / 8);
    int frames = totalSamples / wav.channels;
    std::vector<short> mono(frames);

    for (int f = 0; f < frames; f++) {
        long long sum = 0;
        for (int c = 0; c < wav.channels; c++) {
            int sample = 0;
            if (wav.bitsPerSample == 8) {
                sample = (wav.rawData[wav.dataOffset + (f * wav.channels + c)] - 128) << 8;
            } else if (wav.bitsPerSample == 16) {
                int idx = wav.dataOffset + (f * wav.channels + c) * 2;
                sample = static_cast<short>(wav.rawData[idx] | (wav.rawData[idx + 1] << 8));
            }
            sum += sample;
        }
        mono[f] = static_cast<short>(sum / wav.channels);
    }
    return mono;
}

std::vector<short> SampleConverterService::DecodeFlac(const std::vector<u8>& data) const {
    drflac* flac = drflac_open_memory(data.data(), data.size(), nullptr);
    if (!flac)
        throw std::runtime_error("Failed to open FLAC file");

    drflac_uint64 totalFrames = flac->totalPCMFrameCount;
    int channels = flac->channels;
    int frames = static_cast<int>(totalFrames);

    std::vector<short> temp(static_cast<size_t>(frames) * channels);
    drflac_read_pcm_frames_s16(flac, totalFrames, temp.data());
    drflac_close(flac);

    if (frames == 0)
        throw std::runtime_error("FLAC file contains no samples");

    if (channels == 1)
        return temp;

    std::vector<short> mono(frames);
    for (int f = 0; f < frames; f++) {
        long long sum = 0;
        for (int c = 0; c < channels; c++)
            sum += temp[static_cast<size_t>(f) * channels + c];
        mono[f] = static_cast<short>(sum / channels);
    }
    return mono;
}

std::vector<short> SampleConverterService::DecodeWavpack(const std::vector<u8>& data, int& sampleRateOut) const {
    struct MemStream {
        const u8* ptr;
        size_t size;
        size_t offset;
    };
    MemStream ms{data.data(), data.size(), 0};

    WavpackStreamReader64 reader{};
    reader.read_bytes = [](void* id, void* buf, int32_t b) -> int32_t {
        auto* s = static_cast<MemStream*>(id);
        int32_t n = std::min(b, static_cast<int32_t>(s->size - s->offset));
        std::memcpy(buf, s->ptr + s->offset, n);
        s->offset += n;
        return n;
    };
    reader.write_bytes = [](void*, void*, int32_t) -> int32_t { return 0; };
    reader.get_pos = [](void* id) -> int64_t {
        return static_cast<MemStream*>(id)->offset;
    };
    reader.set_pos_abs = [](void* id, int64_t pos) -> int {
        auto* s = static_cast<MemStream*>(id);
        if (pos < 0 || static_cast<size_t>(pos) > s->size) return 1;
        s->offset = static_cast<size_t>(pos);
        return 0;
    };
    reader.set_pos_rel = [](void* id, int64_t delta, int) -> int {
        auto* s = static_cast<MemStream*>(id);
        int64_t newPos = static_cast<int64_t>(s->offset) + delta;
        if (newPos < 0 || static_cast<uint64_t>(newPos) > s->size) return 1;
        s->offset = static_cast<size_t>(newPos);
        return 0;
    };
    reader.push_back_byte = [](void*, int) -> int { return 1; };
    reader.get_length = [](void* id) -> int64_t {
        return static_cast<MemStream*>(id)->size;
    };
    reader.can_seek = [](void*) -> int { return 1; };
    reader.truncate_here = [](void*) -> int { return 0; };
    reader.close = [](void*) -> int { return 0; };

    char errorBuf[80] = {};
    auto* wpc = WavpackOpenFileInputEx64(&reader, &ms, nullptr, errorBuf, OPEN_WVC | OPEN_NORMALIZE, 0);
    if (!wpc)
        throw std::runtime_error(std::string("Failed to open WavPack file: ") + errorBuf);

    int numSamples = static_cast<int>(WavpackGetNumSamples(wpc));
    int channels = WavpackGetNumChannels(wpc);
    int bps = WavpackGetBytesPerSample(wpc);
    sampleRateOut = WavpackGetSampleRate(wpc);

    if (numSamples == 0) {
        WavpackCloseFile(wpc);
        throw std::runtime_error("WavPack file contains no samples");
    }

    std::vector<int32_t> temp(static_cast<size_t>(numSamples) * channels);
    int decoded = WavpackUnpackSamples(wpc, temp.data(), numSamples);
    WavpackCloseFile(wpc);

    if (decoded == 0)
        throw std::runtime_error("Failed to decode WavPack samples");

    std::vector<short> mono(numSamples);
    for (int f = 0; f < numSamples; f++) {
        long long sum = 0;
        for (int c = 0; c < channels; c++) {
            int32_t sample = temp[static_cast<size_t>(f) * channels + c];
            if (bps > 2) sample >>= 16;
            else if (bps == 1) sample <<= 8;
            sum += sample;
        }
        mono[f] = static_cast<short>(std::clamp(sum / channels, -32768LL, 32767LL));
    }
    return mono;
}

std::vector<short> SampleConverterService::Resample(const std::vector<short>& input, int inputRate, int outputRate) const {
    if (inputRate == outputRate) return input;
    int outLen = static_cast<int>((static_cast<long long>(input.size()) * outputRate) / inputRate);
    std::vector<short> output(outLen);

    for (int i = 0; i < outLen; i++) {
        double srcPos = static_cast<double>(i) * inputRate / outputRate;
        int srcIdx = static_cast<int>(srcPos);
        double frac = srcPos - srcIdx;
        int nextIdx = std::min(srcIdx + 1, static_cast<int>(input.size()) - 1);
        output[i] = static_cast<short>(input[srcIdx] * (1.0 - frac) + input[nextIdx] * frac + 0.5);
    }
    return output;
}

std::vector<u8> SampleConverterService::EncodePcm8(const std::vector<short>& samples) const {
    std::vector<u8> pcm(samples.size());
    for (size_t i = 0; i < samples.size(); i++) {
        int s = (samples[i] >> 8) + 128;
        pcm[i] = static_cast<u8>(std::clamp(s, 0, 255));
    }
    return pcm;
}

std::vector<u8> SampleConverterService::EncodeAdpcm(const std::vector<short>& samples) const {
    int outLen = (static_cast<int>(samples.size()) + 1) / 2;
    std::vector<u8> output(outLen);

    const int stepTable[] = {
        7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31,
        34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
        157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
        724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
        3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
        15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
    };
    const int stepIndexTable[] = {-1, -1, -1, -1, 2, 4, 6, 8};

    int predictor = 0;
    int stepIndex = 0;

    for (int i = 0; i < static_cast<int>(samples.size()); i++) {
        int diff = samples[i] - predictor;
        int sign = diff < 0 ? 1 : 0;
        int absDiff = diff < 0 ? -diff : diff;
        int step = stepTable[stepIndex];
        int quantized = 0;
        int stepSize = step >> 3;

        if (absDiff >= step) { quantized |= 4; absDiff -= step; }
        if (absDiff >= step >> 1) { quantized |= 2; absDiff -= step >> 1; }
        if (absDiff >= step >> 2) { quantized |= 1; }

        int nibble = (sign << 3) | quantized;

        int delta = (step >> 3);
        if ((quantized & 4) != 0) delta += step;
        if ((quantized & 2) != 0) delta += step >> 1;
        if ((quantized & 1) != 0) delta += step >> 2;
        delta >>= 3;

        if (sign == 1) predictor -= delta;
        else predictor += delta;
        predictor = std::clamp(predictor, static_cast<int>(SHRT_MIN), static_cast<int>(SHRT_MAX));

        stepIndex += stepIndexTable[quantized];
        stepIndex = std::clamp(stepIndex, 0, static_cast<int>(sizeof(stepTable) / sizeof(stepTable[0])) - 1);

        if ((i & 1) == 0)
            output[i >> 1] = static_cast<u8>(nibble << 4);
        else
            output[i >> 1] |= static_cast<u8>(nibble);
    }
    return output;
}

std::vector<u8> SampleConverterService::ConvertPcmToAdpcm(const std::vector<u8>& pcmData) const {
    std::vector<short> samples(pcmData.size());
    for (size_t i = 0; i < pcmData.size(); i++)
        samples[i] = static_cast<short>((pcmData[i] - 128) << 8);
    return EncodeAdpcm(samples);
}
