#include "save_converter_panel.h"
#include "main_frame.h"
#include "save_converter_service.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <fstream>

wxBEGIN_EVENT_TABLE(SaveConverterPanel, wxPanel)
wxEND_EVENT_TABLE()

SaveConverterPanel::SaveConverterPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* openBtn = new wxButton(this, wxID_ANY, "Open SRAM File...");
    outer->Add(openBtn, 0, wxALL, 8);

    infoLabel = new wxStaticText(this, wxID_ANY, "No SRAM file loaded.");
    outer->Add(infoLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* sizeRow = new wxBoxSizer(wxHORIZONTAL);
    sizeRow->Add(new wxStaticText(this, wxID_ANY, "Output Size:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    outputSizeChoice = new wxChoice(this, wxID_ANY);
    for (long long s : SaveConverterService::OutputSizes)
        outputSizeChoice->Append(wxString::Format("%lld bytes (%lld KB)", s, s / 1024));
    outputSizeChoice->SetSelection(3);
    sizeRow->Add(outputSizeChoice, 0);
    outer->Add(sizeRow, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* convertBtn = new wxButton(this, wxID_ANY, "Convert to v1.05");
    outer->Add(convertBtn, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    outer->AddStretchSpacer();
    SetSizer(outer);

    openBtn->Bind(wxEVT_BUTTON, &SaveConverterPanel::OnOpenSrm, this);
    convertBtn->Bind(wxEVT_BUTTON, &SaveConverterPanel::OnConvert, this);
}

void SaveConverterPanel::OnOpenSrm(wxCommandEvent&) {
    wxFileDialog dlg(this, "Open SRM/SRAM file",
        wxEmptyString, wxEmptyString,
        "SRAM files (*.srm;*.bin)|*.srm;*.bin|All files (*.*)|*.*",
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
        "SRAM files (*.srm)|*.srm|All files (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    std::ofstream f(dlg.GetPath().ToStdString(), std::ios::binary);
    f.write(reinterpret_cast<const char*>(result.data()), result.size());
    wxMessageBox(wxString::Format("Converted to %zu bytes v1.05 format.", result.size()), "Done");
}
