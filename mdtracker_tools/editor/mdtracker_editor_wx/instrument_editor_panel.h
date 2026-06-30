#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include "models.h"

class MainFrame;

class InstrumentEditorPanel : public wxPanel {
public:
    InstrumentEditorPanel(wxWindow* parent, MainFrame* mainFrame);

    void LoadPreset(int index);
    void RefreshFromModel(const InstrumentPreset& preset);

private:
    void OnPresetIndexChanged(wxCommandEvent& event);
    void OnParamChanged(wxSpinEvent& event);
    void OnReadFromRom(wxCommandEvent& event);
    void OnWriteToRom(wxCommandEvent& event);

    MainFrame* mainFrame;
    int currentPreset = 0;

    wxSpinCtrl* presetIndexCtrl;
    wxSpinCtrl* algorithmCtrl;
    wxSpinCtrl* feedbackCtrl;
    wxSpinCtrl* stereoCtrl;
    wxSpinCtrl* amsCtrl;
    wxSpinCtrl* fmsCtrl;

    struct OpControls {
        wxSpinCtrl* multiple;
        wxSpinCtrl* detune;
        wxSpinCtrl* totalLevel;
        wxSpinCtrl* rateScaling;
        wxSpinCtrl* attackRate;
        wxSpinCtrl* firstDecayRate;
        wxSpinCtrl* secondaryDecayRate;
        wxSpinCtrl* releaseRate;
        wxSpinCtrl* secondaryAmplitude;
        wxSpinCtrl* amplitudeModulation;
        wxSpinCtrl* ssgEg;
    };
    std::vector<OpControls> opControls;
    bool updating = false;

    wxDECLARE_EVENT_TABLE();
};
