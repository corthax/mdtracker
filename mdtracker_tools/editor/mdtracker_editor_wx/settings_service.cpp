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

static void LoadOne(json& j, const char* key, RomAddressConfig& cfg) {
    auto& obj = j[key];
    cfg.sampleSettingsAddr = obj.value("SampleSettingsAddr", 0x0003C600);
    cfg.sampleBankAddr = obj.value("SampleBankAddr", 0x0003F000);
    cfg.presetNameAddr = obj.value("PresetNameAddr", 0x0003A000);
}

static void SaveOne(json& j, const char* key, const RomAddressConfig& cfg) {
    j[key]["SampleSettingsAddr"] = cfg.sampleSettingsAddr;
    j[key]["SampleBankAddr"] = cfg.sampleBankAddr;
    j[key]["PresetNameAddr"] = cfg.presetNameAddr;
}

void SettingsService::Load() {
    std::string path = GetFilePath();
    std::ifstream f(path);
    if (!f.is_open()) return;

    try {
        json j;
        f >> j;
        LoadOne(j, "MEDPro", settings.medPro);
        LoadOne(j, "EDMDV3", settings.edmdv3);
    } catch (...) {}
}

void SettingsService::Save() {
    json j;
    SaveOne(j, "MEDPro", settings.medPro);
    SaveOne(j, "EDMDV3", settings.edmdv3);

    std::string path = GetFilePath();
    std::ofstream f(path);
    if (f.is_open()) f << j.dump(4);
}
