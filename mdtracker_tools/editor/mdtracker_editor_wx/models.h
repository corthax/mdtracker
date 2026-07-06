#pragma once
#include "compat_types.h"
#include <string>
#include <vector>
#include <array>
#include <cstring>

struct OperatorParams {
    int multiple{};
    int detune{};
    int totalLevel{};
    int rateScaling{};
    int attackRate{};
    int firstDecayRate{};
    int secondaryDecayRate{};
    int releaseRate{};
    int secondaryAmplitude{};
    int amplitudeModulation{};
    int ssgEg{};
};

struct InstrumentPreset {
    static constexpr int StructSize = 49;
    static constexpr int NameSize = 8;
    static constexpr int OperatorCount = 4;

    int id{};
    int algorithm{};
    int feedback{};
    int stereo{};
    int ams{};
    int fms{};
    std::string name;
    std::array<OperatorParams, OperatorCount> operators;

    InstrumentPreset() { name.resize(NameSize, ' '); }

    static InstrumentPreset Deserialize(const u8* data, int offset = 0) {
        InstrumentPreset p;
        p.algorithm = data[offset + 0];
        p.fms = data[offset + 1];
        p.ams = data[offset + 2];
        p.stereo = data[offset + 3];
        p.feedback = data[offset + 4];

        for (int op = 0; op < OperatorCount; op++) {
            auto& opd = p.operators[op];
            opd.totalLevel = data[offset + 5 + 0 * 4 + op];
            opd.rateScaling = data[offset + 5 + 1 * 4 + op];
            opd.multiple = data[offset + 5 + 2 * 4 + op];
            opd.detune = data[offset + 5 + 3 * 4 + op];
            opd.attackRate = data[offset + 5 + 4 * 4 + op];
            opd.firstDecayRate = data[offset + 5 + 5 * 4 + op];
            opd.secondaryAmplitude = data[offset + 5 + 6 * 4 + op];
            opd.secondaryDecayRate = data[offset + 5 + 7 * 4 + op];
            opd.releaseRate = data[offset + 5 + 8 * 4 + op];
            opd.amplitudeModulation = data[offset + 5 + 9 * 4 + op];
            opd.ssgEg = data[offset + 5 + 10 * 4 + op];
        }
        return p;
    }

    std::vector<u8> Serialize() const {
        std::vector<u8> d(StructSize);
        d[0] = static_cast<u8>(algorithm);
        d[1] = static_cast<u8>(fms);
        d[2] = static_cast<u8>(ams);
        d[3] = static_cast<u8>(stereo);
        d[4] = static_cast<u8>(feedback);

        for (int op = 0; op < OperatorCount; op++) {
            const auto& opd = operators[op];
            d[5 + 0 * 4 + op] = static_cast<u8>(opd.totalLevel);
            d[5 + 1 * 4 + op] = static_cast<u8>(opd.rateScaling);
            d[5 + 2 * 4 + op] = static_cast<u8>(opd.multiple);
            d[5 + 3 * 4 + op] = static_cast<u8>(opd.detune);
            d[5 + 4 * 4 + op] = static_cast<u8>(opd.attackRate);
            d[5 + 5 * 4 + op] = static_cast<u8>(opd.firstDecayRate);
            d[5 + 6 * 4 + op] = static_cast<u8>(opd.secondaryAmplitude);
            d[5 + 7 * 4 + op] = static_cast<u8>(opd.secondaryDecayRate);
            d[5 + 8 * 4 + op] = static_cast<u8>(opd.releaseRate);
            d[5 + 9 * 4 + op] = static_cast<u8>(opd.amplitudeModulation);
            d[5 + 10 * 4 + op] = static_cast<u8>(opd.ssgEg);
        }
        return d;
    }

    bool Matches(const InstrumentPreset& other) const {
        if (algorithm != other.algorithm) return false;
        if (feedback != other.feedback) return false;
        if (stereo != other.stereo) return false;
        if (ams != other.ams) return false;
        if (fms != other.fms) return false;
        for (int op = 0; op < OperatorCount; op++) {
            const auto& a = operators[op];
            const auto& b = other.operators[op];
            if (a.multiple != b.multiple) return false;
            if (a.detune != b.detune) return false;
            if (a.attackRate != b.attackRate) return false;
            if (a.rateScaling != b.rateScaling) return false;
            if (a.firstDecayRate != b.firstDecayRate) return false;
            if (a.amplitudeModulation != b.amplitudeModulation) return false;
            if (a.secondaryAmplitude != b.secondaryAmplitude) return false;
            if (a.secondaryDecayRate != b.secondaryDecayRate) return false;
            if (a.releaseRate != b.releaseRate) return false;
            if (a.totalLevel != b.totalLevel) return false;
            if (a.ssgEg != b.ssgEg) return false;
        }
        return true;
    }
};

struct SampleSlot {
    int bankId{};
    int noteId{};
    int startOffset{};
    int endOffset{};
    int pan{192};
    bool looped{};
    int rate{};
    int type{};
    std::string name;
    int samplePoolId{-1};
    bool isSynced{};

    SampleSlot() { name.resize(14, ' '); }

    static constexpr int TypePcm8 = 0;
    static constexpr int TypeAdpcm = 1;
    static constexpr const char* TypeLabels[] = {"S8PCM", "2ADPCM"};

    static constexpr int PanOptions[] = {128, 192, 64};
    static constexpr const char* PanLabels[] = {"L", "C", "R"};
    static constexpr int RateOptions[] = {32000, 22050, 16000, 13400, 11025, 8000};
    static constexpr const char* RateLabels[] = {"32000", "22050", "16000", "13400", "11025", "8000"};

    int BankNoteIndex() const { return bankId * 96 + noteId; }

    std::string NoteName() const {
        const char* noteNames[] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};
        int octave = noteId / 12;
        int note = noteId % 12;
        char buf[32];
        snprintf(buf, sizeof(buf), "%d: %s%d", BankNoteIndex(), noteNames[note], octave);
        return buf;
    }
};

struct SampleFile {
    int id{};
    std::string fileName;
    std::string shortName;
    std::vector<u8> data;
    int originalSize{};
    int type{}; // 0 = S8PCM, 1 = 2ADPCM
    long long startOffset{};
    long long endOffset{};

    int AlignedSize() const { return ((originalSize + 255) / 256) * 256; }
    bool IsAdpcm() const { return fileName.size() >= 7 && fileName.substr(fileName.size() - 7) == ".2adpcm"; }

    static std::string ExtractShortName(const std::string& filePath) {
        auto pos = filePath.find_last_of("/\\");
        std::string name = (pos != std::string::npos) ? filePath.substr(pos + 1) : filePath;
        auto dot = name.find_last_of('.');
        if (dot != std::string::npos) name = name.substr(0, dot);

        auto open = name.find('(');
        auto close = name.find(')');
        if (open != std::string::npos && close != std::string::npos && close > open)
            name = name.substr(open + 1, close - open - 1);

        if (name.size() > 14) name = name.substr(0, 14);
        return name;
    }
};

struct SampleBank {
    int bankId;
    std::vector<SampleSlot> slots;

    explicit SampleBank(int id) : bankId(id) {
        slots.resize(96);
        for (int i = 0; i < 96; i++) {
            slots[i].bankId = id;
            slots[i].noteId = i;
        }
    }

    static std::vector<SampleBank> CreateAll() {
        std::vector<SampleBank> banks;
        banks.reserve(4);
        for (int i = 0; i < 4; i++)
            banks.emplace_back(i);
        return banks;
    }
};

struct RomConstants {
    static constexpr int SampleSettingsAddr = 0x0003e500;
    static constexpr int SampleSettingsEntrySize = 28;
    static constexpr int SampleSettingsCount = 384;
    static constexpr int SampleSettingsTotalSize = SampleSettingsEntrySize * SampleSettingsCount;
    static constexpr int SampleBankAddr = 0x00040f00;
    static constexpr int SampleNameSize = 14;
    static constexpr int NotesPerBank = 96;
    static constexpr int BankCount = 4;
    static constexpr int PresetStructSize = 49;
    static constexpr int PresetCount = 256;
    static constexpr int PBank0Addr = 0x00031a18;
    static constexpr int MBank0Addr = 0x00033298;
    static constexpr int RomSize = 0x0037FFFF;

    static int PBankAddress(int index) { return PBank0Addr - index * PresetStructSize; }
    static int MBankAddress(int index) { return MBank0Addr - index * PresetStructSize; }
    static void GetPresetAddress(int index, int& addr, bool& isMelodic) {
        if (index < 128) { addr = MBankAddress(index); isMelodic = true; }
        else { addr = PBankAddress(index - 128); isMelodic = false; }
    }
};

struct LegacySramLayout {
    static constexpr int FileSize = 524288;
    static constexpr uint32_t SrmLfo = 0x05902;
    static constexpr uint32_t SrmSampleData = 0x0726B;
    static constexpr uint32_t SrmSamplePan = 0x6B0AB;
    static constexpr uint32_t SrmSampleRate = 0x6F230;
    static constexpr uint32_t SrmInstrumentData = 0x00002;
    static constexpr uint32_t SrmPatternMatrix = 0x05905;
    static constexpr uint32_t SrmTempo = 0x07269;
    static constexpr uint32_t SrmPatternData = 0x07CEB;
    static constexpr uint32_t SrmPatternColor = 0x6A06B;
    static constexpr uint32_t SrmMatrixTranspose = 0x6A3EC;
    static constexpr uint32_t SrmMuteChannel = 0x6B09E;
    static constexpr uint32_t SrmSongTranspose = 0x6B22B;
    static constexpr uint32_t SrmSeqVolStart = 0x6B230;
    static constexpr uint32_t SrmSeqArpStart = 0x6D230;
    static constexpr int SampleDataEntrySize = 7;
    static constexpr int PatternRowSize = 14;
    static constexpr int PatternRows = 32;
    static constexpr int PatternBlockSize = PatternRowSize * PatternRows;
    static constexpr int PatternCount = 896;
    static constexpr int InstrumentRecordSize = 89;
    static constexpr int InstrumentCount = 256;

    static constexpr u8 NoteEmpty = 0xFE;
    static constexpr u8 NoteOff = 0xFF;

    // Legacy 1.04 SRAM had byte-swapped odd offsets via old SRAMW_writeByte redirect
    static uint32_t WordSwapOffset(uint32_t offset) {
        if (offset < 2) return 0;
        if ((offset & 1) == 1) offset -= 2;
        return offset;
    }
};

struct SramV05Layout {
    static constexpr int HeaderSize = 12;
    static constexpr int StaticBase = 12;
    static constexpr int SramGlobalLfo = 12;
    static constexpr int SramTempo = 15;
    static constexpr int StaticEnd = 17;
    static constexpr int MatrixBlockBase = 17;
    static constexpr int InstRecordSize = 58;
    static constexpr int SeqRecordSize = 65;
    static constexpr int LookupTableSize = 256;
    static constexpr u8 InstSentinelModified = 0xFF;
    static constexpr int PatternMagic = 0x4D44;
    static constexpr int PatternFormatVersion = 2;
    static constexpr int PatternRegionHeaderSize = 6;
    static constexpr int EvtCount = 7;
    static constexpr int EffectsTotal = 6;
    static constexpr int MatrixRows = 250;
    static constexpr int MatrixChannels = 13;
};

enum class RomType { MEDPro, EDMDV3 };

struct RomAddressConfig {
    int sampleSettingsAddr = 0x0003C600;
    int sampleBankAddr = 0x0003F000;
    int presetNameAddr = 0x0003A000;
    int presetNameEntrySize = 14;
    int presetNameMaxLen = 13;
};

struct AppSettings {
    RomAddressConfig medPro;
    RomAddressConfig edmdv3;
    int defaultConversionType{}; // 0 = S8PCM, 1 = 2ADPCM
};
