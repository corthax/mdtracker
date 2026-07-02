#include "sample_converter_panel.h"
#include "main_frame.h"
#include "sample_converter_service.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/filename.h>
#include <fstream>

wxBEGIN_EVENT_TABLE(SampleConverterPanel, wxPanel)
wxEND_EVENT_TABLE()

double SampleConverterPanel::SliderToFreq(int val) {
    return 20.0 * std::pow(1000.0, val / 1000.0);
}

int SampleConverterPanel::FreqToSlider(double freq) {
    if (freq <= 20.0) return 0;
    if (freq >= 20000.0) return 1000;
    return static_cast<int>(std::lround(1000.0 * std::log(freq / 20.0) / std::log(1000.0)));
}

double SampleConverterPanel::SliderToQ(int val) {
    return val * 0.1;
}

int SampleConverterPanel::QToSlider(double q) {
    return static_cast<int>(std::lround(q * 10.0));
}

double SampleConverterPanel::SliderToGain(int val) {
    return val * 0.1;
}

int SampleConverterPanel::GainToSlider(double gain) {
    return static_cast<int>(std::lround(gain * 10.0));
}

SampleConverterPanel::SampleConverterPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame), eqUpdating(false)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* topRow = new wxBoxSizer(wxHORIZONTAL);
    auto* addBtn = new wxButton(this, wxID_ANY, "Add Audio Files...");
    topRow->Add(addBtn, 0, wxRIGHT, 4);
    auto* clearBtn = new wxButton(this, wxID_ANY, "Clear");
    topRow->Add(clearBtn, 0, wxRIGHT, 12);
    topRow->Add(new wxStaticText(this, wxID_ANY, "Target Rate:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    targetRateChoice = new wxChoice(this, wxID_ANY);
    for (int r : SampleConverterService::PcmRates)
        targetRateChoice->Append(wxString::Format("%d Hz", r));
    targetRateChoice->SetSelection(5);
    topRow->Add(targetRateChoice, 0);
    topRow->Add(new wxStaticText(this, wxID_ANY, "  Converter:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    converterChoice = new wxChoice(this, wxID_ANY);
    converterChoice->Append("Internal (HQ)");
    converterChoice->Append("Rescomp (SGDK)");
    converterChoice->SetSelection(0);
    topRow->Add(converterChoice, 0);
    topRow->Add(new wxStaticText(this, wxID_ANY, "  Resample:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    resampleChoice = new wxChoice(this, wxID_ANY);
    resampleChoice->Append("High Quality");
    resampleChoice->Append("Linear");
    resampleChoice->SetSelection(0);
    topRow->Add(resampleChoice, 0);
    outer->Add(topRow, 0, wxALL, 8);

    fileList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_NO_HEADER);
    fileList->AppendColumn("File", wxLIST_FORMAT_LEFT, 220);
    fileList->AppendColumn("Rate", wxLIST_FORMAT_RIGHT, 60);
    fileList->AppendColumn("Ch", wxLIST_FORMAT_RIGHT, 35);
    fileList->AppendColumn("Bits", wxLIST_FORMAT_RIGHT, 40);
    fileList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
    outer->Add(fileList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* exportBox = new wxStaticBoxSizer(wxVERTICAL, this, "Export");
    auto* btnRow = new wxBoxSizer(wxHORIZONTAL);
    exportPcm8Btn = new wxButton(this, wxID_ANY, "Export All as S8PCM");
    btnRow->Add(exportPcm8Btn, 0, wxALL, 4);
    exportAdpcmBtn = new wxButton(this, wxID_ANY, "Export All as ADPCM");
    btnRow->Add(exportAdpcmBtn, 0, wxALL, 4);
    exportBox->Add(btnRow, 0, wxEXPAND);
    clampCheckbox = new wxCheckBox(this, wxID_ANY, "Clamp S8PCM range");
    clampCheckbox->SetToolTip("Check this to clamp S8PCM to [-127, 127] if quiet parts sounds noisy");
    exportBox->Add(clampCheckbox, 0, wxLEFT | wxRIGHT | wxTOP, 8);

    eqEnableCheckbox = new wxCheckBox(this, wxID_ANY, "Enable EQ");
    eqEnableCheckbox->SetToolTip("Apply 3-band EQ (low shelf, peak, high shelf) before resampling");
    exportBox->Add(eqEnableCheckbox, 0, wxLEFT | wxRIGHT | wxTOP, 8);

    eqBandContainer = new wxWindow(this, wxID_ANY);
    auto* eqGrid = new wxFlexGridSizer(3, 7, 2, 4);

    struct { const char* label; double freq, q, gain; } bandDefs[] = {
        {"Low Shelf", 200, 0.707, 0},
        {"Mid Peak", 1000, 0.707, 0},
        {"High Shelf", 10000, 0.707, 0},
    };
    EQBandWidgets* bands[] = {&lowShelf, &peak, &highShelf};

    for (int b = 0; b < 3; b++) {
        auto& w = *bands[b];
        auto& d = bandDefs[b];

        eqGrid->Add(new wxStaticText(eqBandContainer, wxID_ANY, d.label), 0, wxALIGN_CENTER_VERTICAL);

        w.freqSlider = new wxSlider(eqBandContainer, wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(80, -1));
        eqGrid->Add(w.freqSlider, 0, wxALIGN_CENTER_VERTICAL);

        w.freqText = new wxTextCtrl(eqBandContainer, wxID_ANY, "", wxDefaultPosition, wxSize(50, -1), wxTE_PROCESS_ENTER);
        eqGrid->Add(w.freqText, 0, wxALIGN_CENTER_VERTICAL);

        w.qSlider = new wxSlider(eqBandContainer, wxID_ANY, 7, 1, 100, wxDefaultPosition, wxSize(60, -1));
        eqGrid->Add(w.qSlider, 0, wxALIGN_CENTER_VERTICAL);

        w.qText = new wxTextCtrl(eqBandContainer, wxID_ANY, "", wxDefaultPosition, wxSize(42, -1), wxTE_PROCESS_ENTER);
        eqGrid->Add(w.qText, 0, wxALIGN_CENTER_VERTICAL);

        w.gainSlider = new wxSlider(eqBandContainer, wxID_ANY, 0, -240, 240, wxDefaultPosition, wxSize(80, -1));
        eqGrid->Add(w.gainSlider, 0, wxALIGN_CENTER_VERTICAL);

        w.gainText = new wxTextCtrl(eqBandContainer, wxID_ANY, "", wxDefaultPosition, wxSize(42, -1), wxTE_PROCESS_ENTER);
        eqGrid->Add(w.gainText, 0, wxALIGN_CENTER_VERTICAL);

        w.freqSlider->SetValue(FreqToSlider(d.freq));
        w.freqText->SetValue(wxString::Format("%.0f", d.freq));
        w.qSlider->SetValue(QToSlider(d.q));
        w.qText->SetValue(wxString::Format("%.2f", d.q));
        w.gainSlider->SetValue(GainToSlider(d.gain));
        w.gainText->SetValue(wxString::Format("%.1f", d.gain));
    }

    eqBandContainer->SetSizer(eqGrid);
    exportBox->Add(eqBandContainer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);

    outer->Add(exportBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(outer);

    // Bind events
    addBtn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnAddWavs, this);
    clearBtn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnClear, this);
    exportPcm8Btn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnExportPcm8, this);
    exportAdpcmBtn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnExportAdpcm, this);
    converterChoice->Bind(wxEVT_CHOICE, &SampleConverterPanel::OnConverterChanged, this);
    eqEnableCheckbox->Bind(wxEVT_CHECKBOX, &SampleConverterPanel::OnEqToggled, this);

    auto bindBand = [this](EQBandWidgets& b) {
        b.freqSlider->Bind(wxEVT_SLIDER, &SampleConverterPanel::OnEqFreqSlider, this);
        b.qSlider->Bind(wxEVT_SLIDER, &SampleConverterPanel::OnEqQSlider, this);
        b.gainSlider->Bind(wxEVT_SLIDER, &SampleConverterPanel::OnEqGainSlider, this);

        auto setupText = [this](wxTextCtrl* text, wxSlider* slider, int(*toSlider)(double), double minVal, double maxVal, const char* fmt) {
            auto commit = [this, text, slider, toSlider, minVal, maxVal, fmt]() {
                if (eqUpdating) return;
                double val;
                if (text->GetValue().ToDouble(&val)) {
                    val = std::clamp(val, minVal, maxVal);
                    slider->SetValue(toSlider(val));
                    text->SetValue(wxString::Format(fmt, val));
                }
            };
            text->Bind(wxEVT_TEXT_ENTER, [commit](wxCommandEvent&) { commit(); });
            text->Bind(wxEVT_KILL_FOCUS, [commit](wxFocusEvent& evt) { commit(); evt.Skip(); });
        };

        setupText(b.freqText, b.freqSlider, FreqToSlider, 20.0, 20000.0, "%.0f");
        setupText(b.qText, b.qSlider, QToSlider, 0.1, 10.0, "%.2f");
        setupText(b.gainText, b.gainSlider, GainToSlider, -24.0, 24.0, "%.1f");
    };
    bindBand(lowShelf);
    bindBand(peak);
    bindBand(highShelf);

    UpdateOptionsVisibility();
}

void SampleConverterPanel::UpdateOptionsVisibility() {
    bool isInternal = (converterChoice->GetSelection() == 0);
    resampleChoice->Show(isInternal);
    clampCheckbox->Show(isInternal);
    bool eqEnabled = eqEnableCheckbox->GetValue();
    eqBandContainer->Show(eqEnabled);
    GetSizer()->Layout();
}

void SampleConverterPanel::OnConverterChanged(wxCommandEvent&) {
    UpdateOptionsVisibility();
}

void SampleConverterPanel::OnEqToggled(wxCommandEvent&) {
    UpdateOptionsVisibility();
}

void SampleConverterPanel::UpdateEqTextFromSliders() {
    eqUpdating = true;
    auto upd = [](wxSlider* s, wxTextCtrl* t, double(*toVal)(int), const char* fmt) {
        t->SetValue(wxString::Format(fmt, toVal(s->GetValue())));
    };
    upd(lowShelf.freqSlider, lowShelf.freqText, SliderToFreq, "%.0f");
    upd(lowShelf.qSlider, lowShelf.qText, SliderToQ, "%.2f");
    upd(lowShelf.gainSlider, lowShelf.gainText, SliderToGain, "%.1f");
    upd(peak.freqSlider, peak.freqText, SliderToFreq, "%.0f");
    upd(peak.qSlider, peak.qText, SliderToQ, "%.2f");
    upd(peak.gainSlider, peak.gainText, SliderToGain, "%.1f");
    upd(highShelf.freqSlider, highShelf.freqText, SliderToFreq, "%.0f");
    upd(highShelf.qSlider, highShelf.qText, SliderToQ, "%.2f");
    upd(highShelf.gainSlider, highShelf.gainText, SliderToGain, "%.1f");
    eqUpdating = false;
}

void SampleConverterPanel::OnEqFreqSlider(wxCommandEvent&) {
    if (eqUpdating) return;
    UpdateEqTextFromSliders();
}

void SampleConverterPanel::OnEqQSlider(wxCommandEvent&) {
    if (eqUpdating) return;
    UpdateEqTextFromSliders();
}

void SampleConverterPanel::OnEqGainSlider(wxCommandEvent&) {
    if (eqUpdating) return;
    UpdateEqTextFromSliders();
}

SampleConverterService::EQConfig SampleConverterPanel::ReadEQConfig() const {
    auto readBand = [](const EQBandWidgets& w) -> SampleConverterService::EQConfig::Band {
        SampleConverterService::EQConfig::Band b;
        double val;
        if (w.freqText->GetValue().ToDouble(&val)) b.freqHz = std::clamp(val, 20.0, 20000.0);
        if (w.qText->GetValue().ToDouble(&val)) b.q = std::clamp(val, 0.1, 10.0);
        if (w.gainText->GetValue().ToDouble(&val)) b.gainDB = std::clamp(val, -24.0, 24.0);
        return b;
    };
    SampleConverterService::EQConfig cfg;
    cfg.lowShelf = readBand(lowShelf);
    cfg.peak = readBand(peak);
    cfg.highShelf = readBand(highShelf);
    return cfg;
}

void SampleConverterPanel::RefreshList() {
    fileList->DeleteAllItems();
    for (const auto& entry : batchWavs) {
        wxFileName fn(entry.path);
        long idx = fileList->InsertItem(fileList->GetItemCount(), fn.GetFullName());
        fileList->SetItem(idx, 1, wxString::Format("%d", entry.sampleRate));
        fileList->SetItem(idx, 2, wxString::Format("%d", entry.channels));
        fileList->SetItem(idx, 3, wxString::Format("%d", entry.bitsPerSample));
        fileList->SetItem(idx, 4, "Ready");
    }
}

void SampleConverterPanel::OnAddWavs(wxCommandEvent&) {
    wxFileDialog dlg(this, "Select audio files",
        wxEmptyString, wxEmptyString,
        "Audio files (*.wav;*.flac;*.wv)|*.wav;*.flac;*.wv|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    wxArrayString paths;
    dlg.GetPaths(paths);

    SampleConverterService svc;

    for (size_t i = 0; i < paths.size(); i++) {
        const wxString& path = paths[i];
        wxString ext = path.AfterLast('.').Lower();

        std::ifstream f(path.ToStdString(), std::ios::binary | std::ios::ate);
        if (!f.is_open()) continue;
        std::streamsize size = f.tellg();
        f.seekg(0, std::ios::beg);
        std::vector<u8> fileData(static_cast<size_t>(size));
        f.read(reinterpret_cast<char*>(fileData.data()), size);

        try {
            BatchWavEntry entry;
            entry.path = path.ToStdString();

            if (ext == "wav") {
                auto wav = svc.ParseWav(fileData);
                entry.sampleRate = wav.sampleRate;
                entry.channels = wav.channels;
                entry.bitsPerSample = wav.bitsPerSample;
                entry.decoded = svc.DecodeSamples(wav);
            } else if (ext == "flac") {
                entry.decoded = svc.DecodeFlac(fileData);
                entry.sampleRate = 44100;
                entry.channels = 1;
                entry.bitsPerSample = 16;
            } else if (ext == "wv") {
                int sr = 0;
                entry.decoded = svc.DecodeWavpack(fileData, sr);
                entry.sampleRate = sr;
                entry.channels = 1;
                entry.bitsPerSample = 16;
            } else {
                continue;
            }

            batchWavs.push_back(std::move(entry));
        } catch (const std::exception& e) {
            wxFileName fn(path);
            long idx = fileList->InsertItem(fileList->GetItemCount(), fn.GetFullName());
            fileList->SetItem(idx, 4, wxString::Format("Error: %s", e.what()));
        }
    }

    RefreshList();
}

void SampleConverterPanel::OnClear(wxCommandEvent&) {
    batchWavs.clear();
    RefreshList();
}

void SampleConverterPanel::DoExport(const wxString& outDir, const wxString& ext, bool isAdpcm) {
    SampleConverterService svc;
    int targetRate = SampleConverterService::PcmRates[targetRateChoice->GetSelection()];
    int ok = 0, fail = 0;
    bool useRescomp = (converterChoice->GetSelection() == 1);

    for (size_t i = 0; i < batchWavs.size(); i++) {
        auto& entry = batchWavs[i];
        wxFileName fn(entry.path);
        fn.SetExt(ext);
        wxString outPath = outDir + wxFileName::GetPathSeparator() + fn.GetFullName();

        try {
            auto samples = entry.decoded;

            if (eqEnableCheckbox->GetValue()) {
                auto cfg = ReadEQConfig();
                samples = svc.ApplyEQ(samples, entry.sampleRate, cfg);
            }

            std::vector<u8> out;

            if (useRescomp) {
                if (!svc.RescompAvailable())
                    throw std::runtime_error("rescomp.jar not found. Falling back to internal converter.\n"
                                              "Place rescomp.jar next to the editor executable.");
                std::string fmt = isAdpcm ? "2ADPCM" : "PCM";
                out = svc.ConvertViaRescomp(samples, entry.sampleRate, fmt, targetRate);
            } else {
                bool linearResample = (resampleChoice->GetSelection() == 1);
                auto resampled = svc.Resample(samples, entry.sampleRate, targetRate, linearResample);
                bool isPcm = !isAdpcm;
                bool clampS8 = isPcm && clampCheckbox->GetValue();
                out = isAdpcm ? (std::vector<u8>)svc.EncodeAdpcm(resampled)
                               : (std::vector<u8>)svc.EncodePcm8(resampled, clampS8);
            }

            std::ofstream f(outPath.ToStdString(), std::ios::binary);
            f.write(reinterpret_cast<const char*>(out.data()), out.size());

            fileList->SetItem(static_cast<long>(i), 4, "OK");
            ok++;
        } catch (const std::exception& e) {
            fileList->SetItem(static_cast<long>(i), 4, wxString::Format("Error: %s", e.what()));
            fail++;
        }
    }

    wxMessageBox(wxString::Format("Done. %d succeeded, %d failed.", ok, fail), "Batch Export");
}

void SampleConverterPanel::OnExportPcm8(wxCommandEvent&) {
    if (batchWavs.empty()) {
        wxMessageBox("No audio files loaded.", "Batch Empty", wxOK | wxICON_INFORMATION);
        return;
    }

    wxDirDialog dirDlg(this, "Select output directory for S8PCM files",
        wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDlg.ShowModal() == wxID_CANCEL) return;

    DoExport(dirDlg.GetPath(), "s8pcm", false);
}

void SampleConverterPanel::OnExportAdpcm(wxCommandEvent&) {
    if (batchWavs.empty()) {
        wxMessageBox("No audio files loaded.", "Batch Empty", wxOK | wxICON_INFORMATION);
        return;
    }

    wxDirDialog dirDlg(this, "Select output directory for ADPCM files",
        wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDlg.ShowModal() == wxID_CANCEL) return;

    DoExport(dirDlg.GetPath(), "2adpcm", true);
}
