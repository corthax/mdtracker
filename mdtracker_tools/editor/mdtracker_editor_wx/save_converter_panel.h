#pragma once
#include "compat_types.h"
#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/stattext.h>

class MainFrame;

class SaveConverterPanel : public wxPanel {
public:
    SaveConverterPanel(wxWindow* parent, MainFrame* mainFrame);

private:
    void OnOpenSrm(wxCommandEvent& event);
    void OnConvert(wxCommandEvent& event);

    MainFrame* mainFrame;
    wxChoice* outputSizeChoice;
    wxStaticText* infoLabel;
    std::vector<u8> sramData;

    wxDECLARE_EVENT_TABLE();
};
