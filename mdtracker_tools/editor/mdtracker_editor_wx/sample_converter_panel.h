#pragma once
#include "compat_types.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/button.h>
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

private:
    void OnAddWavs(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnExportPcm8(wxCommandEvent& event);
    void OnExportAdpcm(wxCommandEvent& event);
    void RefreshList();

    MainFrame* mainFrame;
    wxChoice* targetRateChoice;
    wxListView* fileList;
    wxButton* exportPcm8Btn;
    wxButton* exportAdpcmBtn;

    std::vector<BatchWavEntry> batchWavs;

    wxDECLARE_EVENT_TABLE();
};
