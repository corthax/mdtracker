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

SettingsPanel::SettingsPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);
    auto* box = new wxStaticBoxSizer(wxVERTICAL, this, "Addresses");

    auto* grid = new wxFlexGridSizer(2, 8, 8);
    grid->AddGrowableCol(1);

    grid->Add(new wxStaticText(this, wxID_ANY, "Sample Settings Addr:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    settingsAddrCtrl = new wxTextCtrl(this, wxID_ANY, "0003E500", wxDefaultPosition, wxSize(120, -1));
    grid->Add(settingsAddrCtrl, 0, wxEXPAND);

    grid->Add(new wxStaticText(this, wxID_ANY, "Sample Bank Addr:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    bankAddrCtrl = new wxTextCtrl(this, wxID_ANY, "00040F00", wxDefaultPosition, wxSize(120, -1));
    grid->Add(bankAddrCtrl, 0, wxEXPAND);

    box->Add(grid, 0, wxALL, 8);

    auto* saveBtn = new wxButton(this, wxID_ANY, "Save Settings");
    box->Add(saveBtn, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    outer->Add(box, 0, wxEXPAND | wxALL, 8);
    outer->AddStretchSpacer();
    SetSizer(outer);

    saveBtn->Bind(wxEVT_BUTTON, &SettingsPanel::OnSaveSettings, this);

    RefreshSettings(mainFrame->GetSettingsService()->settings);
}

void SettingsPanel::RefreshSettings(const AppSettings& settings) {
    settingsAddrCtrl->SetValue(wxString::Format("%08X", settings.sampleSettingsAddr));
    bankAddrCtrl->SetValue(wxString::Format("%08X", settings.sampleBankAddr));
}

void SettingsPanel::OnSaveSettings(wxCommandEvent&) {
    auto toInt = [](const wxString& s) -> int {
        wxString hex = s.Strip(wxString::both);
        if (hex.empty()) return 0;
        unsigned long val = 0;
        hex.ToULong(&val, 16);
        return static_cast<int>(val);
    };

    auto& s = mainFrame->GetSettingsService()->settings;
    s.sampleSettingsAddr = toInt(settingsAddrCtrl->GetValue());
    s.sampleBankAddr = toInt(bankAddrCtrl->GetValue());
    mainFrame->GetSettingsService()->Save();
    RefreshSettings(s);
}
