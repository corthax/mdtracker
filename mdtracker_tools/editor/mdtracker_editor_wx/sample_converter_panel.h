#pragma once
#include "compat_types.h"
#include "sample_converter_service.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <vector>
#include <string>
#include <cstdint>

class MainFrame;

struct BatchWavEntry {
    std::string path;
    std::vector<short> decoded;
    int sampleRate{};
    int channels{};
    int bitsPerSample{};
};

class SampleConverterPanel : public wxPanel {
public:
    SampleConverterPanel(wxWindow* parent, MainFrame* mainFrame);

    struct EQBandWidgets {
        wxSlider* freqSlider;
        wxTextCtrl* freqText;
        wxSlider* qSlider;
        wxTextCtrl* qText;
        wxSlider* gainSlider;
        wxTextCtrl* gainText;
    };
    EQBandWidgets lowShelf;
    EQBandWidgets peak;
    EQBandWidgets highShelf;

private:
    void OnAddWavs(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnExportPcm8(wxCommandEvent& event);
    void OnExportAdpcm(wxCommandEvent& event);
    void OnConverterChanged(wxCommandEvent& event);
    void OnEqToggled(wxCommandEvent& event);
    void OnEqFreqSlider(wxCommandEvent& event);
    void OnEqQSlider(wxCommandEvent& event);
    void OnEqGainSlider(wxCommandEvent& event);
    void RefreshList();
    void UpdateOptionsVisibility();
    void DoExport(const wxString& outDir, const wxString& ext, bool isAdpcm);

    static double SliderToFreq(int val);
    static int FreqToSlider(double freq);
    static double SliderToQ(int val);
    static int QToSlider(double q);
    static double SliderToGain(int val);
    static int GainToSlider(double gain);

    void UpdateEqTextFromSliders();
    SampleConverterService::EQConfig ReadEQConfig() const;

    MainFrame* mainFrame;
    wxChoice* targetRateChoice;
    wxListView* fileList;
    wxChoice* converterChoice;
    wxChoice* resampleChoice;
    wxCheckBox* clampCheckbox;
    wxCheckBox* eqEnableCheckbox;
    wxButton* exportPcm8Btn;
    wxButton* exportAdpcmBtn;

    wxWindow* eqBandContainer;
    bool eqUpdating;

    std::vector<BatchWavEntry> batchWavs;

    wxDECLARE_EVENT_TABLE();
};
