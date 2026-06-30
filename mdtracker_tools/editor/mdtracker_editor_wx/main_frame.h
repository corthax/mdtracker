#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include "settings_service.h"
#include "rom_service.h"
#include "models.h"
#include "sample_bank_panel.h"
#include "instrument_editor_panel.h"
#include "sample_converter_panel.h"
#include "save_converter_panel.h"
#include "settings_panel.h"

class MainFrame : public wxFrame {
public:
    MainFrame();
    ~MainFrame() override;

    bool LoadRom(const std::string& path);
    RomService* GetRomService() { return romService.get(); }
    SettingsService* GetSettingsService() { return settingsService.get(); }
    std::vector<SampleFile>& GetSamplePool() { return samplePool; }

private:
    void OnOpenRom(wxCommandEvent& event);
    void OnSaveRom(wxCommandEvent& event);
    void OnSaveRomAs(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    std::unique_ptr<SettingsService> settingsService;
    std::unique_ptr<RomService> romService;
    std::vector<SampleBank> banks;
    std::vector<SampleFile> samplePool;

    wxNotebook* notebook;
    SampleBankPanel* sampleBankPanel;
    InstrumentEditorPanel* instrumentPanel;
    SampleConverterPanel* sampleConverterPanel;
    SaveConverterPanel* saveConverterPanel;
    SettingsPanel* settingsPanel;
    wxStatusBar* statusBar;

    wxDECLARE_EVENT_TABLE();
};
