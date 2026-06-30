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

SampleConverterPanel::SampleConverterPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
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
    targetRateChoice->SetSelection(4);
    topRow->Add(targetRateChoice, 0);
    outer->Add(topRow, 0, wxALL, 8);

    fileList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_NO_HEADER);
    fileList->AppendColumn("File", wxLIST_FORMAT_LEFT, 220);
    fileList->AppendColumn("Rate", wxLIST_FORMAT_RIGHT, 60);
    fileList->AppendColumn("Ch", wxLIST_FORMAT_RIGHT, 35);
    fileList->AppendColumn("Bits", wxLIST_FORMAT_RIGHT, 40);
    fileList->AppendColumn("Status", wxLIST_FORMAT_LEFT, 100);
    outer->Add(fileList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* exportBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Export");
    exportPcm8Btn = new wxButton(this, wxID_ANY, "Export All as PCM8");
    exportBox->Add(exportPcm8Btn, 0, wxALL, 4);
    exportAdpcmBtn = new wxButton(this, wxID_ANY, "Export All as ADPCM");
    exportBox->Add(exportAdpcmBtn, 0, wxALL, 4);
    outer->Add(exportBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(outer);

    addBtn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnAddWavs, this);
    clearBtn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnClear, this);
    exportPcm8Btn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnExportPcm8, this);
    exportAdpcmBtn->Bind(wxEVT_BUTTON, &SampleConverterPanel::OnExportAdpcm, this);
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

static void DoExport(SampleConverterPanel* panel, const std::vector<BatchWavEntry>& batch, wxListView* fileList,
    wxChoice* targetRateChoice, const wxString& outDir, const wxString& ext, bool isAdpcm)
{
    SampleConverterService svc;
    int targetRate = SampleConverterService::PcmRates[targetRateChoice->GetSelection()];
    int ok = 0, fail = 0;

    for (size_t i = 0; i < batch.size(); i++) {
        auto& entry = batch[i];
        wxFileName fn(entry.path);
        fn.SetExt(ext);
        wxString outPath = outDir + wxFileName::GetPathSeparator() + fn.GetFullName();

        try {
            auto resampled = svc.Resample(entry.decoded, entry.sampleRate, targetRate);
            auto out = isAdpcm ? (std::vector<u8>)svc.EncodeAdpcm(resampled)
                               : (std::vector<u8>)svc.EncodePcm8(resampled);

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

    wxDirDialog dirDlg(this, "Select output directory for PCM8 files",
        wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDlg.ShowModal() == wxID_CANCEL) return;

    DoExport(this, batchWavs, fileList, targetRateChoice, dirDlg.GetPath(), "pcm8", false);
}

void SampleConverterPanel::OnExportAdpcm(wxCommandEvent&) {
    if (batchWavs.empty()) {
        wxMessageBox("No audio files loaded.", "Batch Empty", wxOK | wxICON_INFORMATION);
        return;
    }

    wxDirDialog dirDlg(this, "Select output directory for ADPCM files",
        wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDlg.ShowModal() == wxID_CANCEL) return;

    DoExport(this, batchWavs, fileList, targetRateChoice, dirDlg.GetPath(), "2adpcm", true);
}
