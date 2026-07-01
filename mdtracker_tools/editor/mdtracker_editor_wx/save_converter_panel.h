#pragma once
#include "compat_types.h"
#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>

class MainFrame;

class SaveConverterPanel : public wxPanel {
public:
    SaveConverterPanel(wxWindow* parent, MainFrame* mainFrame);

private:
    void OnOpenSrm(wxCommandEvent& event);
    void OnConvert(wxCommandEvent& event);
    void OnConvertFormat(wxCommandEvent& event);

    MainFrame* mainFrame;
    wxChoice* outputSizeChoice;
    wxStaticText* infoLabel;
    wxStaticText* formatLabel;
    wxRadioBox* directionRadio;
    wxButton* convertFormatBtn;
    wxButton* convertBtn;
    std::vector<u8> sramData;
};
