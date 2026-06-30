#pragma once
#include <wx/wx.h>
#include <wx/textctrl.h>
#include "models.h"

class MainFrame;

class SettingsPanel : public wxPanel {
public:
    SettingsPanel(wxWindow* parent, MainFrame* mainFrame);

    void RefreshSettings(const AppSettings& settings);

private:
    void OnSaveSettings(wxCommandEvent& event);

    MainFrame* mainFrame;
    wxTextCtrl* settingsAddrCtrl;
    wxTextCtrl* bankAddrCtrl;

    wxDECLARE_EVENT_TABLE();
};
