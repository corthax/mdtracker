#pragma once
#include "models.h"
#include <string>

class SettingsService {
public:
    AppSettings settings;

    SettingsService();
    void Load();
    void Save();

    const RomAddressConfig* AddressForType(RomType type) const {
        return type == RomType::MEDPro ? &settings.medPro : &settings.edmdv3;
    }
    RomAddressConfig* AddressForType(RomType type) {
        return type == RomType::MEDPro ? &settings.medPro : &settings.edmdv3;
    }

private:
    std::string GetFilePath() const;
};
