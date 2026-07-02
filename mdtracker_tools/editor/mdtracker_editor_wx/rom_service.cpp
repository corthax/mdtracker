#include "rom_service.h"
#include <fstream>
#include <algorithm>
#include <cstring>

RomService::RomService(SettingsService* svc) : settings(svc) {}

bool RomService::Load(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) return false;

    std::streamsize size = f.tellg();
    f.seekg(0, std::ios::beg);
    romData.resize(static_cast<size_t>(size));
    f.read(reinterpret_cast<char*>(romData.data()), size);
    romPath = path;

    // Detect ROM type from header serial (file offset 0x180)
    std::string serial = ReadString(0x180, 14);
    romType = (serial == "AI 00000000-01") ? RomType::EDMDV3 : RomType::MEDPro;
    return true;
}

void RomService::PopulateBanks(std::vector<SampleBank>& banks) {
    auto* cfg = settings->AddressForType(romType);
    int baseAddr = cfg->sampleSettingsAddr;
    for (int i = 0; i < RomConstants::SampleSettingsCount; i++) {
        int offset = baseAddr + i * RomConstants::SampleSettingsEntrySize;
        // Validate entry: bankId must be 0-3, noteId must be 0-95
        if (romData[offset] > 3 || romData[offset + 1] > 95)
            continue;
        auto& slot = banks[i / RomConstants::NotesPerBank].slots[i % RomConstants::NotesPerBank];
        int rawStart = Read32(offset + 2);
        int rawEnd = Read32(offset + 6);
        if (rawStart >= static_cast<int>(romData.size()) || rawEnd >= static_cast<int>(romData.size()))
            continue;
        // Firmware stores offsets relative to sample_bank_1; convert to absolute ROM address
        auto* cfg = settings->AddressForType(romType);
        slot.startOffset = rawStart + cfg->sampleBankAddr;
        slot.endOffset = rawEnd + cfg->sampleBankAddr;
        int rawPan = romData[offset + 10];
        slot.pan = (rawPan == 128 || rawPan == 64) ? rawPan : 192;
        slot.looped = romData[offset + 11] == 1;
        int rawRate = romData[offset + 12];
        slot.rate = (rawRate >= 0 && rawRate <= 5) ? rawRate : 0;
        slot.type = romData[offset + 13];
        slot.name = ReadString(offset + 14, RomConstants::SampleNameSize);
    }
}

void RomService::WriteBanks(const std::vector<SampleBank>& banks) {
    if (romData.empty()) return;
    auto* cfg = settings->AddressForType(romType);
    int baseAddr = cfg->sampleSettingsAddr;
    for (int i = 0; i < RomConstants::SampleSettingsCount; i++) {
        int bankId = i / RomConstants::NotesPerBank;
        int noteId = i % RomConstants::NotesPerBank;
        const auto& slot = banks[bankId].slots[noteId];
        int offset = baseAddr + i * RomConstants::SampleSettingsEntrySize;
        romData[offset] = static_cast<u8>(slot.bankId);
        romData[offset + 1] = static_cast<u8>(slot.noteId);
        // Firmware expects offsets relative to sample_bank_1
        auto* cfg = settings->AddressForType(romType);
        Write32(offset + 2, slot.startOffset - cfg->sampleBankAddr);
        Write32(offset + 6, slot.endOffset - cfg->sampleBankAddr);
        romData[offset + 10] = static_cast<u8>(slot.pan);
        romData[offset + 11] = slot.looped ? u8(1) : u8(0);
        romData[offset + 12] = static_cast<u8>(slot.rate);
        romData[offset + 13] = static_cast<u8>(slot.type);
        WriteString(offset + 14, slot.name, RomConstants::SampleNameSize);
    }
}

void RomService::WriteSampleBank(const std::vector<SampleFile>& pool, std::vector<SampleBank>& banks) {
    if (romData.empty()) return;
    auto* cfg = settings->AddressForType(romType);
    int bankAddr = cfg->sampleBankAddr;
    int pos = 0;

    for (auto& file : const_cast<std::vector<SampleFile>&>(pool)) {
        int aligned = file.AlignedSize();
        file.startOffset = pos;
        file.endOffset = pos + aligned;
        WriteBlock(bankAddr + pos, file.data);
        int pad = aligned - file.originalSize;
        if (pad > 0)
            std::fill(romData.begin() + bankAddr + pos + file.originalSize,
                      romData.begin() + bankAddr + pos + aligned, 0);
        pos += aligned;
    }

    for (auto& bank : banks)
        for (auto& slot : bank.slots)
            if (slot.samplePoolId >= 0 && slot.samplePoolId < static_cast<int>(pool.size())) {
                const auto& f = pool[slot.samplePoolId];
                slot.startOffset = bankAddr + static_cast<int>(f.startOffset);
                slot.endOffset = bankAddr + static_cast<int>(f.endOffset);
            }

    WriteBanks(banks);
}

void RomService::Save(const std::string& path) {
    const std::string& outPath = path.empty() ? romPath : path;
    if (outPath.empty() || romData.empty()) return;
    std::ofstream f(outPath, std::ios::binary);
    if (f.is_open()) f.write(reinterpret_cast<const char*>(romData.data()), romData.size());
}

std::vector<u8> RomService::ReadBlock(int address, int size) const {
    if (romData.empty()) return {};
    if (address + size > static_cast<int>(romData.size())) size = romData.size() - address;
    if (size <= 0) return {};
    return std::vector<u8>(romData.begin() + address, romData.begin() + address + size);
}

void RomService::WriteBlock(int address, const std::vector<u8>& data) {
    if (romData.empty()) return;
    int copySize = std::min(static_cast<int>(data.size()), static_cast<int>(romData.size()) - address);
    if (copySize > 0)
        std::copy(data.begin(), data.begin() + copySize, romData.begin() + address);
}

InstrumentPreset RomService::ReadPreset(int index) const {
    int addr; bool isMelodic;
    RomConstants::GetPresetAddress(index, addr, isMelodic);
    auto data = ReadBlock(addr, InstrumentPreset::StructSize);
    auto preset = InstrumentPreset::Deserialize(data.data());
    preset.name = ReadPresetName(index);
    return preset;
}

void RomService::WritePreset(int index, const InstrumentPreset& preset) {
    int addr; bool isMelodic;
    RomConstants::GetPresetAddress(index, addr, isMelodic);
    auto data = preset.Serialize();
    WriteBlock(addr, data);
    WritePresetName(index, preset.name);
}

std::string RomService::ReadPresetName(int index) const {
    auto* cfg = settings->AddressForType(romType);
    int addr = cfg->presetNameAddr + index * cfg->presetNameEntrySize;
    return ReadString(addr, cfg->presetNameMaxLen);
}

void RomService::WritePresetName(int index, const std::string& name) {
    auto* cfg = settings->AddressForType(romType);
    int addr = cfg->presetNameAddr + index * cfg->presetNameEntrySize;
    WriteString(addr, name, cfg->presetNameMaxLen);
}

int RomService::Read32(int offset) const {
    return (romData[offset] << 24) | (romData[offset + 1] << 16) |
           (romData[offset + 2] << 8) | romData[offset + 3];
}

void RomService::Write32(int offset, int value) {
    romData[offset] = static_cast<u8>((value >> 24) & 0xFF);
    romData[offset + 1] = static_cast<u8>((value >> 16) & 0xFF);
    romData[offset + 2] = static_cast<u8>((value >> 8) & 0xFF);
    romData[offset + 3] = static_cast<u8>(value & 0xFF);
}

std::string RomService::ReadString(int offset, int maxLen) const {
    std::string s;
    s.reserve(maxLen);
    for (int i = 0; i < maxLen; i++) {
        char c = static_cast<char>(romData[offset + i]);
        s += (c >= 32) ? c : ' ';
    }
    while (!s.empty() && s.back() == ' ') s.pop_back();
    return s;
}

void RomService::WriteString(int offset, const std::string& str, int maxLen) {
    for (int i = 0; i < maxLen; i++)
        romData[offset + i] = static_cast<u8>(i < static_cast<int>(str.size()) ? str[i] : 0x20);
}
