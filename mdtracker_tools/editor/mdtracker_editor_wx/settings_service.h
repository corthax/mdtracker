#pragma once
#include "models.h"
#include <string>

class SettingsService {
public:
    AppSettings settings;

    SettingsService();
    void Load();
    void Save();

    int SampleSettingsAddr() const { return settings.sampleSettingsAddr; }
    int SampleBankAddr() const { return settings.sampleBankAddr; }

private:
    std::string GetFilePath() const;
};
