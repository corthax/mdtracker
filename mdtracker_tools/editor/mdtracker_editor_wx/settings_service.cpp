#include "settings_service.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

SettingsService::SettingsService() {
    Load();
}

std::string SettingsService::GetFilePath() const {
    wxStandardPaths& stdPaths = wxStandardPaths::Get();
    wxString exeDir = stdPaths.GetExecutablePath();
    wxFileName fn(exeDir);
    return (fn.GetPath() + wxFileName::GetPathSeparator() + "settings.json").ToStdString();
}

void SettingsService::Load() {
    std::string path = GetFilePath();
    std::ifstream f(path);
    if (!f.is_open()) return;

    try {
        json j;
        f >> j;
        settings.sampleSettingsAddr = j.value("SampleSettingsAddr", 0x0003C600);
        settings.sampleBankAddr = j.value("SampleBankAddr", 0x0003F000);
    } catch (...) {}
}

void SettingsService::Save() {
    json j;
    j["SampleSettingsAddr"] = settings.sampleSettingsAddr;
    j["SampleBankAddr"] = settings.sampleBankAddr;

    std::string path = GetFilePath();
    std::ofstream f(path);
    if (f.is_open()) f << j.dump(4);
}
