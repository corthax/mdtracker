#pragma once
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include "models.h"

class MainFrame;

class SettingsPanel : public wxPanel {
public:
    SettingsPanel(wxWindow* parent, MainFrame* mainFrame);

    void RefreshSettings(const AppSettings& settings);

private:
    void OnRomTypeChanged(wxCommandEvent& event);
    void OnSaveSettings(wxCommandEvent& event);

    void SaveCurrentType();
    void LoadType(int index);

    MainFrame* mainFrame;
    wxChoice* romTypeChoice;
    wxTextCtrl* settingsAddrCtrl;
    wxTextCtrl* bankAddrCtrl;
    wxTextCtrl* presetNameAddrCtrl;
    int activeTypeIndex = 0;

    wxDECLARE_EVENT_TABLE();
};
