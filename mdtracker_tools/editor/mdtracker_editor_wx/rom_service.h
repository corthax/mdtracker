#pragma once
#include "models.h"
#include "settings_service.h"
#include <string>
#include <vector>

class RomService {
public:
    explicit RomService(SettingsService* settings);

    bool Load(const std::string& path);
    void PopulateBanks(std::vector<SampleBank>& banks);
    void WriteBanks(const std::vector<SampleBank>& banks);
    void WriteSampleBank(const std::vector<SampleFile>& pool, std::vector<SampleBank>& banks);
    void Save(const std::string& path = "");
    bool IsLoaded() const { return !romData.empty(); }
    const std::string& RomPath() const { return romPath; }
    RomType GetRomType() const { return romType; }
    const std::vector<u8>& GetRomData() const { return romData; }

    std::vector<u8> ReadBlock(int address, int size) const;
    void WriteBlock(int address, const std::vector<u8>& data);
    InstrumentPreset ReadPreset(int index) const;
    void WritePreset(int index, const InstrumentPreset& preset);
    std::string ReadPresetName(int index) const;
    void WritePresetName(int index, const std::string& name);

private:
    SettingsService* settings;
    std::vector<u8> romData;
    std::string romPath;
    RomType romType = RomType::MEDPro;

    int Read32(int offset) const;
    void Write32(int offset, int value);
    std::string ReadString(int offset, int maxLen) const;
    void WriteString(int offset, const std::string& str, int maxLen);
};
