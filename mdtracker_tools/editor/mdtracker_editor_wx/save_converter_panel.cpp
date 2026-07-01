#include "save_converter_panel.h"
#include "main_frame.h"
#include "save_converter_service.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/statline.h>
#include <fstream>

SaveConverterPanel::SaveConverterPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* openBtn = new wxButton(this, wxID_ANY, "Open SRAM File...");
    outer->Add(openBtn, 0, wxALL, 8);

    infoLabel = new wxStaticText(this, wxID_ANY, "No SRAM file loaded.");
    outer->Add(infoLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 4);

    formatLabel = new wxStaticText(this, wxID_ANY, "");
    outer->Add(formatLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* sep1 = new wxStaticLine(this, wxID_ANY);
    outer->Add(sep1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    const wxString dirs[] = {
        "16-bit (Mega Everdrive Pro/BlastEm) -> 8-bit (EDMD-V3)",
        "8-bit (EDMD-V3) -> 16-bit (Mega Everdrive Pro/BlastEm)"
    };
    directionRadio = new wxRadioBox(this, wxID_ANY, "Direction", wxDefaultPosition, wxDefaultSize,
        WXSIZEOF(dirs), dirs, 1, wxRA_SPECIFY_COLS);
    directionRadio->Enable(false);
    outer->Add(directionRadio, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    convertFormatBtn = new wxButton(this, wxID_ANY, "Convert && Save As...");
    convertFormatBtn->Enable(false);
    outer->Add(convertFormatBtn, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* sep2 = new wxStaticLine(this, wxID_ANY);
    outer->Add(sep2, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* legacyBox = new wxStaticBoxSizer(wxVERTICAL, this, "v1.05 Legacy Converter");
    auto* sizeRow = new wxBoxSizer(wxHORIZONTAL);
    sizeRow->Add(new wxStaticText(this, wxID_ANY, "Output Size:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    outputSizeChoice = new wxChoice(this, wxID_ANY);
    for (long long s : SaveConverterService::OutputSizes)
        outputSizeChoice->Append(wxString::Format("%lld bytes (%lld KB)", s, s / 1024));
    outputSizeChoice->SetSelection(3);
    sizeRow->Add(outputSizeChoice, 0);
    legacyBox->Add(sizeRow, 0, wxALL, 4);

    convertBtn = new wxButton(this, wxID_ANY, "Convert to v1.05");
    legacyBox->Add(convertBtn, 0, wxLEFT | wxRIGHT | wxBOTTOM, 4);
    outer->Add(legacyBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    outer->AddStretchSpacer();
    SetSizer(outer);

    openBtn->Bind(wxEVT_BUTTON, &SaveConverterPanel::OnOpenSrm, this);
    convertBtn->Bind(wxEVT_BUTTON, &SaveConverterPanel::OnConvert, this);
    convertFormatBtn->Bind(wxEVT_BUTTON, &SaveConverterPanel::OnConvertFormat, this);
}

void SaveConverterPanel::OnOpenSrm(wxCommandEvent&) {
    wxFileDialog dlg(this, "Open Save File",
        wxEmptyString, wxEmptyString,
        "SRAM files (*.srm;*.bin;*.sram;*.bram)|*.srm;*.bin;*.sram;*.bram|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    std::ifstream f(dlg.GetPath().ToStdString(), std::ios::binary | std::ios::ate);
    if (!f.is_open()) {
        wxMessageBox("Failed to open file.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    std::streamsize size = f.tellg();
    f.seekg(0, std::ios::beg);
    sramData.resize(static_cast<size_t>(size));
    f.read(reinterpret_cast<char*>(sramData.data()), size);

    infoLabel->SetLabel(wxString::Format("Loaded: %lld bytes", static_cast<long long>(size)));

    auto fmt = SaveConverterService::DetectFormat(sramData);
    wxString fmtStr;
    bool enableConv = false;
    switch (fmt) {
    case SaveConverterService::SramFormat::Format16Bit:
        fmtStr = "Detected: 16-bit (Everdrive Pro / BlastEm) - header at even addresses";
        directionRadio->SetSelection(0);
        enableConv = true;
        break;
    case SaveConverterService::SramFormat::Format8Bit:
        fmtStr = "Detected: 8-bit (EDMD-V3) - header at odd addresses";
        directionRadio->SetSelection(1);
        enableConv = true;
        break;
    default:
        fmtStr = "Unrecognized SRAM format (try v1.05 converter below)";
        break;
    }
    formatLabel->SetLabel(fmtStr);
    directionRadio->Enable(enableConv);
    convertFormatBtn->Enable(enableConv);
}

void SaveConverterPanel::OnConvertFormat(wxCommandEvent&) {
    if (sramData.empty()) {
        wxMessageBox("Open an SRAM file first.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    bool to8Bit = directionRadio->GetSelection() == 0;

    wxString defaultName = to8Bit ? "converted_8bit.srm" : "converted_16bit.srm";
    wxString desc = to8Bit
        ? "8-bit SRAM (*.srm;*.sram;*.bin;*.bram)|*.srm;*.sram;*.bin;*.bram|All files (*.*)|*.*"
        : "16-bit SRAM (*.srm;*.sram;*.bin;*.bram)|*.srm;*.sram;*.bin;*.bram|All files (*.*)|*.*";

    wxFileDialog dlg(this, "Save converted SRAM",
        wxEmptyString, defaultName,
        desc, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    std::vector<u8> result;
    const char* label;
    if (to8Bit) {
        result = SaveConverterService::Convert16To8(sramData);
        label = "8-bit (EDMD-V3)";
    } else {
        result = SaveConverterService::Convert8To16(sramData);
        label = "16-bit (Mega Everdrive Pro / BlastEm)";
    }

    std::ofstream f(dlg.GetPath().ToStdString(), std::ios::binary);
    f.write(reinterpret_cast<const char*>(result.data()), result.size());
    wxMessageBox(wxString::Format("Converted to %zu bytes, %s format.", result.size(), label), "Done");
}

void SaveConverterPanel::OnConvert(wxCommandEvent&) {
    if (sramData.empty()) {
        wxMessageBox("Open an SRAM file first.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    long long outputSize = SaveConverterService::OutputSizes[outputSizeChoice->GetSelection()];
    auto result = SaveConverterService::Convert(sramData, outputSize);

    wxFileDialog dlg(this, "Save converted SRAM",
        wxEmptyString, "converted.srm",
        "SRAM files (*.srm;*.sram;*.bin;*.bram)|*.srm;*.sram;*.bin;*.bram|All files (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    std::ofstream f(dlg.GetPath().ToStdString(), std::ios::binary);
    f.write(reinterpret_cast<const char*>(result.data()), result.size());
    wxMessageBox(wxString::Format("Converted to %zu bytes v1.05 format.", result.size()), "Done");
}
