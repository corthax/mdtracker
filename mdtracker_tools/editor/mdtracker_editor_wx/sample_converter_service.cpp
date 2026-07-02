#define _USE_MATH_DEFINES
#include "sample_converter_service.h"
#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"
#include <wavpack/wavpack.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <regex>
#include <stdexcept>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

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

static double SincP(double x) {
    if (std::abs(x) < 1e-8) return 1.0;
    return std::sin(M_PI * x) / (M_PI * x);
}

std::vector<short> SampleConverterService::Resample(const std::vector<short>& input, int inputRate, int outputRate, bool linear) const {
    if (inputRate == outputRate) return input;

    double ratio = static_cast<double>(outputRate) / inputRate;
    int outLen = static_cast<int>(input.size() * ratio + 0.5);
    if (outLen < 1) outLen = 1;

    std::vector<short> output(outLen);

    if (linear) {
        for (int i = 0; i < outLen; i++) {
            double srcPos = static_cast<double>(i) * inputRate / outputRate;
            int srcIdx = static_cast<int>(srcPos);
            double frac = srcPos - srcIdx;
            int nextIdx = std::min(srcIdx + 1, static_cast<int>(input.size()) - 1);
            output[i] = static_cast<short>(input[srcIdx] * (1.0 - frac) + input[nextIdx] * frac + 0.5);
        }
    } else {
        double fc = std::min(1.0, ratio) * 0.48;
        const double beta = 6.5;
        const int halfLen = 32;
        const double i0beta = std::cyl_bessel_i(0, beta);

        for (int i = 0; i < outLen; i++) {
            double pos = i / ratio;
            int center = static_cast<int>(pos);
            double frac = pos - center;

            double val = 0;
            double wsum = 0;

            for (int j = -halfLen; j <= halfLen; j++) {
                int idx = center + j;
                if (idx < 0 || idx >= static_cast<int>(input.size())) continue;

                double t = frac - j;
                double tx = t / halfLen;
                if (std::abs(tx) >= 1.0) continue;
                double arg = beta * std::sqrt(1.0 - tx * tx);
                double w = 2.0 * fc * SincP(2.0 * fc * t) * (std::cyl_bessel_i(0, arg) / i0beta);
                val += input[idx] * w;
                wsum += w;
            }

            if (wsum > 1e-10)
                output[i] = static_cast<short>(std::clamp(val / wsum, -32768.0, 32767.0));
            else
                output[i] = 0;
        }
    }

    return output;
}

std::vector<u8> SampleConverterService::EncodePcm8(const std::vector<short>& samples, bool clampTo254) const {
    std::vector<u8> pcm(samples.size());
    int maxVal = clampTo254 ? 254 : 255;

    for (size_t i = 0; i < samples.size(); i++) {
        int s = samples[i] >> 8;
        pcm[i] = static_cast<u8>(std::clamp(s, -128, maxVal));
    }
    return pcm;
}

std::vector<u8> SampleConverterService::dpcmPack(const std::vector<u8>& pcm8) const {
    static const int delta_tab[] = {-34, -21, -13, -8, -5, -3, -2, -1, 0, 1, 2, 3, 5, 8, 13, 21};

    auto getBestDeltaIndex = [&](int wantedLevel, int curLevel) -> int {
        int wdelta = wantedLevel - curLevel;
        int ind = 0;
        int mindiff = std::abs(wdelta - delta_tab[ind]);
        for (int i = 1; i < 16; i++) {
            int diff = std::abs(wdelta - delta_tab[i]);
            if (diff < mindiff) { mindiff = diff; ind = i; }
        }
        int newLevel = delta_tab[ind] + curLevel;
        if (newLevel > 127)  return ind - 1;
        if (newLevel < -128) return ind + 1;
        return ind;
    };

    size_t outLen = (pcm8.size() / 2) + (pcm8.size() & 1);
    std::vector<u8> result(outLen);
    int curLevel = 0;

    for (size_t off = 0; off < pcm8.size(); off += 2) {
        int wanted0 = static_cast<int>(static_cast<signed char>(pcm8[off]));
        int ind = getBestDeltaIndex(wanted0, curLevel);
        curLevel += delta_tab[ind];
        int outVal = ind;

        if (off + 1 < pcm8.size()) {
            int wanted1 = static_cast<int>(static_cast<signed char>(pcm8[off + 1]));
            ind = getBestDeltaIndex(wanted1, curLevel);
            curLevel += delta_tab[ind];
            outVal |= (ind << 4);
        }

        result[off / 2] = static_cast<u8>(outVal);
    }

    return result;
}

std::vector<u8> SampleConverterService::EncodeAdpcm(const std::vector<short>& samples) const {
    std::vector<u8> pcm8(samples.size());
    for (size_t i = 0; i < samples.size(); i++)
        pcm8[i] = static_cast<u8>(std::clamp(samples[i] >> 8, -128, 255));
    return dpcmPack(pcm8);
}

std::vector<u8> SampleConverterService::ConvertPcmToAdpcm(const std::vector<u8>& pcmData) const {
    return dpcmPack(pcmData);
}

void SampleConverterService::CalcLowShelf(double freq, int sampleRate, double q, double gainDB,
    double& b0, double& b1, double& b2, double& a1, double& a2) const
{
    double A = std::pow(10.0, gainDB / 40.0);
    double w0 = 2.0 * M_PI * freq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * q);
    double twoSqrtA = 2.0 * std::sqrt(A);

    b0 = A * ((A + 1.0) - (A - 1.0) * std::cos(w0) + twoSqrtA * alpha);
    b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * std::cos(w0));
    b2 = A * ((A + 1.0) - (A - 1.0) * std::cos(w0) - twoSqrtA * alpha);
    double a0 = ((A + 1.0) + (A - 1.0) * std::cos(w0) + twoSqrtA * alpha);
    a1 = -2.0 * ((A - 1.0) + (A + 1.0) * std::cos(w0));
    a2 = ((A + 1.0) + (A - 1.0) * std::cos(w0) - twoSqrtA * alpha);

    b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;
}

void SampleConverterService::CalcPeak(double freq, int sampleRate, double q, double gainDB,
    double& b0, double& b1, double& b2, double& a1, double& a2) const
{
    double A = std::pow(10.0, gainDB / 40.0);
    double w0 = 2.0 * M_PI * freq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * q);

    b0 = 1.0 + alpha * A;
    b1 = -2.0 * std::cos(w0);
    b2 = 1.0 - alpha * A;
    double a0 = 1.0 + alpha / A;
    a1 = -2.0 * std::cos(w0);
    a2 = 1.0 - alpha / A;

    b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;
}

void SampleConverterService::CalcHighShelf(double freq, int sampleRate, double q, double gainDB,
    double& b0, double& b1, double& b2, double& a1, double& a2) const
{
    double A = std::pow(10.0, gainDB / 40.0);
    double w0 = 2.0 * M_PI * freq / sampleRate;
    double alpha = std::sin(w0) / (2.0 * q);
    double twoSqrtA = 2.0 * std::sqrt(A);

    b0 = A * ((A + 1.0) + (A - 1.0) * std::cos(w0) + twoSqrtA * alpha);
    b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * std::cos(w0));
    b2 = A * ((A + 1.0) + (A - 1.0) * std::cos(w0) - twoSqrtA * alpha);
    double a0 = ((A + 1.0) - (A - 1.0) * std::cos(w0) + twoSqrtA * alpha);
    a1 = 2.0 * ((A - 1.0) - (A + 1.0) * std::cos(w0));
    a2 = ((A + 1.0) - (A - 1.0) * std::cos(w0) - twoSqrtA * alpha);

    b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;
}

std::vector<short> SampleConverterService::ProcessBiquad(const std::vector<short>& input,
    double b0, double b1, double b2, double a1, double a2) const
{
    std::vector<short> output(input.size());
    double x1 = 0, x2 = 0, y1 = 0, y2 = 0;

    for (size_t i = 0; i < input.size(); i++) {
        double x0 = input[i];
        double y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        output[i] = static_cast<short>(std::clamp(y0, -32768.0, 32767.0));
        x2 = x1; x1 = x0;
        y2 = y1; y1 = y0;
    }
    return output;
}

std::vector<short> SampleConverterService::ApplyEQ(const std::vector<short>& input, int sampleRate, const EQConfig& eq) const {
    double b0, b1, b2, a1, a2;

    CalcLowShelf(eq.lowShelf.freqHz, sampleRate, eq.lowShelf.q, eq.lowShelf.gainDB, b0, b1, b2, a1, a2);
    auto out = ProcessBiquad(input, b0, b1, b2, a1, a2);

    CalcPeak(eq.peak.freqHz, sampleRate, eq.peak.q, eq.peak.gainDB, b0, b1, b2, a1, a2);
    out = ProcessBiquad(out, b0, b1, b2, a1, a2);

    CalcHighShelf(eq.highShelf.freqHz, sampleRate, eq.highShelf.q, eq.highShelf.gainDB, b0, b1, b2, a1, a2);
    out = ProcessBiquad(out, b0, b1, b2, a1, a2);

    return out;
}

std::string SampleConverterService::FindRescompJar() const {
#ifdef _WIN32
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return "";
    std::string exePath(path);
    size_t pos = exePath.find_last_of("\\/");
    if (pos == std::string::npos) return "";
    std::string dir = exePath.substr(0, pos);
    std::string jarPath = dir + "\\rescomp.jar";
    std::ifstream f(jarPath);
    if (f.good()) return jarPath;
#endif
    return "";
}

bool SampleConverterService::RescompAvailable() const {
    return !FindRescompJar().empty();
}

std::string SampleConverterService::WriteTempWav(const std::vector<short>& samples, int sampleRate, const std::string& dir) const {
    std::string wavPath = dir + "\\input.wav";
    std::ofstream f(wavPath, std::ios::binary);
    int dataSize = static_cast<int>(samples.size()) * 2;

    auto write32 = [&](int v) { f.write(reinterpret_cast<const char*>(&v), 4); };
    auto write16 = [&](short v) { f.write(reinterpret_cast<const char*>(&v), 2); };

    f.write("RIFF", 4);
    write32(36 + dataSize);
    f.write("WAVE", 4);
    f.write("fmt ", 4);
    write32(16);
    write16(1);
    write16(1);
    write32(sampleRate);
    write32(sampleRate * 2);
    write16(2);
    write16(16);
    f.write("data", 4);
    write32(dataSize);
    f.write(reinterpret_cast<const char*>(samples.data()), static_cast<size_t>(dataSize));

    return wavPath;
}

std::vector<u8> SampleConverterService::ConvertViaRescomp(const std::vector<short>& samples, int sampleRate,
                                                            const std::string& format, int targetRate) const
{
    std::string jar = FindRescompJar();
    if (jar.empty())
        throw std::runtime_error("rescomp.jar not found next to executable");

    namespace fs = std::filesystem;
    int pid = GetCurrentProcessId();
    thread_local std::mt19937 rng(std::random_device{}());
    std::string tmpName = "mdtracker_rescomp_" + std::to_string(pid) + "_" + std::to_string(rng());
    fs::path tmpDir = fs::temp_directory_path() / fs::path(tmpName);
    fs::create_directories(tmpDir);

    try {
        std::string dirS = tmpDir.string();
        WriteTempWav(samples, sampleRate, dirS);

        std::string resPath = dirS + "\\input.res";
        {
            std::ofstream rf(resPath);
            rf << "WAV input \"input.wav\" " << format << " " << targetRate;
        }

        std::string cmd = "java -jar \"" + jar + "\" \"" + resPath + "\" -noheader";
        int ret = std::system(cmd.c_str());
        if (ret != 0)
            throw std::runtime_error("rescomp.jar failed with exit code " + std::to_string(ret));

        std::string asmPath = dirS + "\\input.s";
        std::string asmText;
        {
            std::ifstream asmFile(asmPath);
            if (!asmFile.good())
                throw std::runtime_error("rescomp.jar did not produce expected output file");
            asmText.assign((std::istreambuf_iterator<char>(asmFile)), std::istreambuf_iterator<char>());
        }

        std::vector<u8> result;
        std::regex hexRx(R"(\b0x([0-9a-fA-F]{2})\b)");
        auto it = std::sregex_iterator(asmText.begin(), asmText.end(), hexRx);
        auto end = std::sregex_iterator();
        for (; it != end; ++it)
            result.push_back(static_cast<u8>(std::stoi((*it)[1].str(), nullptr, 16)));

        for (int attempt = 0; attempt < 3; attempt++) {
            std::error_code ec;
            fs::remove_all(tmpDir, ec);
            if (!ec) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50 * (attempt + 1)));
        }
        return result;
    } catch (...) {
        for (int attempt = 0; attempt < 3; attempt++) {
            std::error_code ec;
            fs::remove_all(tmpDir, ec);
            if (!ec) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50 * (attempt + 1)));
        }
        throw;
    }
}
