#include "settings_panel.h"
#include "main_frame.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/msgdlg.h>
#include <wx/valtext.h>
#include <algorithm>
#include <cctype>

wxBEGIN_EVENT_TABLE(SettingsPanel, wxPanel)
wxEND_EVENT_TABLE()

static RomAddressConfig* TargetForIndex(AppSettings& s, int idx) {
    return idx == 0 ? &s.medPro : &s.edmdv3;
}

SettingsPanel::SettingsPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);
    auto* box = new wxStaticBoxSizer(wxVERTICAL, this, "Addresses");

    auto* topRow = new wxBoxSizer(wxHORIZONTAL);
    topRow->Add(new wxStaticText(this, wxID_ANY, "ROM Type:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    romTypeChoice = new wxChoice(this, wxID_ANY);
    romTypeChoice->Append("MED Pro");
    romTypeChoice->Append("EDMD-V3");
    romTypeChoice->SetSelection(0);
    topRow->Add(romTypeChoice, 0);
    box->Add(topRow, 0, wxALL, 8);

    auto* grid = new wxFlexGridSizer(2, 8, 8);
    grid->AddGrowableCol(1);

    grid->Add(new wxStaticText(this, wxID_ANY, "Sample Settings Addr:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    settingsAddrCtrl = new wxTextCtrl(this, wxID_ANY, "0003E500", wxDefaultPosition, wxSize(120, -1));
    grid->Add(settingsAddrCtrl, 0, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "Sample Bank Addr:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    bankAddrCtrl = new wxTextCtrl(this, wxID_ANY, "00040F00", wxDefaultPosition, wxSize(120, -1));
    grid->Add(bankAddrCtrl, 0, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "Preset Name Addr:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    presetNameAddrCtrl = new wxTextCtrl(this, wxID_ANY, "0003A000", wxDefaultPosition, wxSize(120, -1));
    grid->Add(presetNameAddrCtrl, 0, wxEXPAND);

    box->Add(grid, 0, wxALL, 8);

    auto* saveBtn = new wxButton(this, wxID_ANY, "Save Settings");
    box->Add(saveBtn, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    outer->Add(box, 0, wxEXPAND | wxALL, 8);
    outer->AddStretchSpacer();
    SetSizer(outer);

    romTypeChoice->Bind(wxEVT_CHOICE, &SettingsPanel::OnRomTypeChanged, this);
    saveBtn->Bind(wxEVT_BUTTON, &SettingsPanel::OnSaveSettings, this);

    activeTypeIndex = 0;
    RefreshSettings(mainFrame->GetSettingsService()->settings);
}

void SettingsPanel::RefreshSettings(const AppSettings& settings) {
    auto* cfg = TargetForIndex(const_cast<AppSettings&>(settings), activeTypeIndex);
    settingsAddrCtrl->SetValue(wxString::Format("%08X", cfg->sampleSettingsAddr));
    bankAddrCtrl->SetValue(wxString::Format("%08X", cfg->sampleBankAddr));
    presetNameAddrCtrl->SetValue(wxString::Format("%08X", cfg->presetNameAddr));
}

void SettingsPanel::SaveCurrentType() {
    auto toInt = [](const wxString& s) -> int {
        wxString hex = s.Strip(wxString::both);
        if (hex.empty()) return 0;
        unsigned long val = 0;
        hex.ToULong(&val, 16);
        return static_cast<int>(val);
    };

    auto* cfg = TargetForIndex(mainFrame->GetSettingsService()->settings, activeTypeIndex);
    cfg->sampleSettingsAddr = toInt(settingsAddrCtrl->GetValue());
    cfg->sampleBankAddr = toInt(bankAddrCtrl->GetValue());
    cfg->presetNameAddr = toInt(presetNameAddrCtrl->GetValue());
}

void SettingsPanel::LoadType(int index) {
    activeTypeIndex = index;
    RefreshSettings(mainFrame->GetSettingsService()->settings);
}

void SettingsPanel::OnRomTypeChanged(wxCommandEvent&) {
    SaveCurrentType();
    LoadType(romTypeChoice->GetSelection());
}

void SettingsPanel::OnSaveSettings(wxCommandEvent&) {
    SaveCurrentType();
    mainFrame->GetSettingsService()->Save();
}
