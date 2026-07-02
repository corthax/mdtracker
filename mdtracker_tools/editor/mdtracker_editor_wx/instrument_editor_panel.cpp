#include "instrument_editor_panel.h"
#include "main_frame.h"
#include "rom_service.h"
#include "tfi_parser.h"
#include "vgi_parser.h"
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <fstream>

wxBEGIN_EVENT_TABLE(InstrumentEditorPanel, wxPanel)
    EVT_SPINCTRL(wxID_ANY, InstrumentEditorPanel::OnParamChanged)
wxEND_EVENT_TABLE()

static wxSpinCtrl* MakeSpin(wxWindow* parent, wxSizer* sizer, const wxString& label, int min, int max, int val) {
    auto* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(parent, wxID_ANY, label + ":"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    auto* spin = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(70, -1));
    spin->SetRange(min, max);
    spin->SetValue(val);
    row->Add(spin, 0);
    sizer->Add(row, 0, wxALL, 2);
    return spin;
}

static wxSpinCtrl* MakeSpinGrid(wxWindow* parent, wxFlexGridSizer* grid, const wxString& label, int min, int max, int val) {
    auto* text = new wxStaticText(parent, wxID_ANY, label + ":");
    auto* spin = new wxSpinCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(70, -1));
    spin->SetRange(min, max);
    spin->SetValue(val);
    grid->Add(text, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxLEFT, 4);
    grid->Add(spin, 0, wxALIGN_CENTER_VERTICAL);
    return spin;
}

InstrumentEditorPanel::InstrumentEditorPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);
    auto* topRow = new wxBoxSizer(wxHORIZONTAL);

    topRow->Add(new wxStaticText(this, wxID_ANY, "Preset Index:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    presetIndexCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(80, -1));
    presetIndexCtrl->SetRange(0, 255);
    topRow->Add(presetIndexCtrl, 0, wxRIGHT, 8);

    topRow->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    nameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(130, -1));
    nameCtrl->SetMaxLength(13);
    topRow->Add(nameCtrl, 0, wxRIGHT, 8);

    auto* readBtn = new wxButton(this, wxID_ANY, "Read from ROM");
    auto* writeBtn = new wxButton(this, wxID_ANY, "Write to ROM");
    topRow->Add(readBtn, 0, wxRIGHT, 4);
    topRow->Add(writeBtn, 0, wxRIGHT, 8);

    auto* importBtn = new wxButton(this, wxID_ANY, "Import");
    auto* exportBtn = new wxButton(this, wxID_ANY, "Export");
    topRow->Add(importBtn, 0, wxRIGHT, 4);
    topRow->Add(exportBtn, 0);
    outer->Add(topRow, 0, wxALL, 8);

    readBtn->Bind(wxEVT_BUTTON, &InstrumentEditorPanel::OnReadFromRom, this);
    writeBtn->Bind(wxEVT_BUTTON, &InstrumentEditorPanel::OnWriteToRom, this);
    importBtn->Bind(wxEVT_BUTTON, &InstrumentEditorPanel::OnImport, this);
    exportBtn->Bind(wxEVT_BUTTON, &InstrumentEditorPanel::OnExport, this);
    presetIndexCtrl->Bind(wxEVT_SPINCTRL, &InstrumentEditorPanel::OnPresetIndexChanged, this);

    auto* paramsSizer = new wxBoxSizer(wxHORIZONTAL);

    // Global params
    auto* globalBox = new wxStaticBoxSizer(wxVERTICAL, this, "Global");
    auto* globalGrid = new wxFlexGridSizer(2, 4, 2);
    algorithmCtrl = MakeSpinGrid(this, globalGrid, "Algorithm", 0, 7, 0);
    feedbackCtrl = MakeSpinGrid(this, globalGrid, "Feedback", 0, 7, 0);
    stereoCtrl = MakeSpinGrid(this, globalGrid, "Stereo", 0, 3, 3);
    amsCtrl = MakeSpinGrid(this, globalGrid, "AMS", 0, 3, 0);
    fmsCtrl = MakeSpinGrid(this, globalGrid, "FMS", 0, 7, 0);
    globalBox->Add(globalGrid, 0, wxEXPAND, 0);
    paramsSizer->Add(globalBox, 0, wxALL | wxEXPAND, 4);

    // Operator panels
    const char* opLabels[] = {"Operator 1", "Operator 2", "Operator 3", "Operator 4"};
    for (int op = 0; op < 4; op++) {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, this, opLabels[op]);
        auto* grid = new wxFlexGridSizer(2, 4, 2);
        OpControls c;
        c.multiple = MakeSpinGrid(this, grid, "Multiple", 0, 15, 1);
        c.detune = MakeSpinGrid(this, grid, "Detune", 0, 7, 0);
        c.totalLevel = MakeSpinGrid(this, grid, "Total Level", 0, 127, 0);
        c.rateScaling = MakeSpinGrid(this, grid, "Rate Scaling", 0, 3, 0);
        c.attackRate = MakeSpinGrid(this, grid, "Attack Rate", 0, 31, 0);
        c.firstDecayRate = MakeSpinGrid(this, grid, "1st Decay Rate", 0, 31, 0);
        c.secondaryDecayRate = MakeSpinGrid(this, grid, "2nd Decay Rate", 0, 31, 0);
        c.releaseRate = MakeSpinGrid(this, grid, "Release Rate", 0, 15, 0);
        c.secondaryAmplitude = MakeSpinGrid(this, grid, "2nd Amplitude", 0, 15, 0);
        c.amplitudeModulation = MakeSpinGrid(this, grid, "AM", 0, 1, 0);
        c.ssgEg = MakeSpinGrid(this, grid, "SSG-EG", 0, 15, 0);
        box->Add(grid, 0, wxEXPAND, 0);
        paramsSizer->Add(box, 0, wxALL | wxEXPAND, 4);
        opControls.push_back(c);
    }

    outer->Add(paramsSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 4);
    SetSizer(outer);
}

void InstrumentEditorPanel::LoadPreset(int index) {
    currentPreset = index;
    if (!mainFrame->GetRomService()->IsLoaded()) return;
    auto preset = mainFrame->GetRomService()->ReadPreset(index);
    RefreshFromModel(preset);
}

void InstrumentEditorPanel::RefreshFromModel(const InstrumentPreset& preset) {
    updating = true;
    nameCtrl->SetValue(preset.name);
    algorithmCtrl->SetValue(preset.algorithm);
    feedbackCtrl->SetValue(preset.feedback);
    stereoCtrl->SetValue(preset.stereo);
    amsCtrl->SetValue(preset.ams);
    fmsCtrl->SetValue(preset.fms);

    for (int op = 0; op < 4; op++) {
        const auto& p = preset.operators[op];
        opControls[op].multiple->SetValue(p.multiple);
        opControls[op].detune->SetValue(p.detune);
        opControls[op].totalLevel->SetValue(p.totalLevel);
        opControls[op].rateScaling->SetValue(p.rateScaling);
        opControls[op].attackRate->SetValue(p.attackRate);
        opControls[op].firstDecayRate->SetValue(p.firstDecayRate);
        opControls[op].secondaryDecayRate->SetValue(p.secondaryDecayRate);
        opControls[op].releaseRate->SetValue(p.releaseRate);
        opControls[op].secondaryAmplitude->SetValue(p.secondaryAmplitude);
        opControls[op].amplitudeModulation->SetValue(p.amplitudeModulation);
        opControls[op].ssgEg->SetValue(p.ssgEg);
    }
    updating = false;
}

void InstrumentEditorPanel::OnPresetIndexChanged(wxCommandEvent&) {
    LoadPreset(presetIndexCtrl->GetValue());
}

void InstrumentEditorPanel::OnParamChanged(wxSpinEvent&) {
    if (updating) return;
}

void InstrumentEditorPanel::OnReadFromRom(wxCommandEvent&) {
    if (!mainFrame->GetRomService()->IsLoaded()) {
        wxMessageBox("No ROM loaded.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    LoadPreset(presetIndexCtrl->GetValue());
}

void InstrumentEditorPanel::OnWriteToRom(wxCommandEvent&) {
    if (!mainFrame->GetRomService()->IsLoaded()) {
        wxMessageBox("No ROM loaded.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    InstrumentPreset preset;
    preset.id = currentPreset;
    preset.name = nameCtrl->GetValue().ToStdString();
    preset.algorithm = algorithmCtrl->GetValue();
    preset.feedback = feedbackCtrl->GetValue();
    preset.stereo = stereoCtrl->GetValue();
    preset.ams = amsCtrl->GetValue();
    preset.fms = fmsCtrl->GetValue();

    for (int op = 0; op < 4; op++) {
        auto& p = preset.operators[op];
        p.multiple = opControls[op].multiple->GetValue();
        p.detune = opControls[op].detune->GetValue();
        p.totalLevel = opControls[op].totalLevel->GetValue();
        p.rateScaling = opControls[op].rateScaling->GetValue();
        p.attackRate = opControls[op].attackRate->GetValue();
        p.firstDecayRate = opControls[op].firstDecayRate->GetValue();
        p.secondaryDecayRate = opControls[op].secondaryDecayRate->GetValue();
        p.releaseRate = opControls[op].releaseRate->GetValue();
        p.secondaryAmplitude = opControls[op].secondaryAmplitude->GetValue();
        p.amplitudeModulation = opControls[op].amplitudeModulation->GetValue();
        p.ssgEg = opControls[op].ssgEg->GetValue();
    }

    mainFrame->GetRomService()->WritePreset(currentPreset, preset);
    mainFrame->GetSettingsService()->Save();
}

void InstrumentEditorPanel::OnImport(wxCommandEvent&) {
    if (!mainFrame->GetRomService()->IsLoaded()) {
        wxMessageBox("No ROM loaded.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxFileDialog dlg(this, "Import Instrument", "", "",
        "Instrument files (*.tfi;*.vgi)|*.tfi;*.vgi",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    std::ifstream f(dlg.GetPath().ToStdString(), std::ios::binary | std::ios::ate);
    if (!f.is_open()) {
        wxMessageBox("Failed to open file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::streamsize size = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<u8> buf(static_cast<size_t>(size));
    if (!f.read(reinterpret_cast<char*>(buf.data()), size)) {
        wxMessageBox("Failed to read file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    InstrumentPreset preset;
    wxString path = dlg.GetPath();
    if (path.Lower().EndsWith(".tfi") && size == TfiParser::FileSize)
        preset = TfiParser::Parse(buf.data(), buf.size());
    else if (path.Lower().EndsWith(".vgi") && size == VgiParser::FileSize)
        preset = VgiParser::Parse(buf.data(), buf.size());
    else {
        wxMessageBox("Unrecognized or invalid instrument file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxFileName fn(dlg.GetPath());
    wxString name = fn.GetName();
    if (name.Length() > 13) name = name.Left(13);
    preset.name = name.ToStdString();

    RefreshFromModel(preset);
}

void InstrumentEditorPanel::OnExport(wxCommandEvent&) {
    wxFileDialog dlg(this, "Export Instrument", "", "",
        "TFI files (*.tfi)|*.tfi|VGI files (*.vgi)|*.vgi",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    InstrumentPreset preset;
    preset.name = nameCtrl->GetValue().ToStdString();
    preset.algorithm = algorithmCtrl->GetValue();
    preset.feedback = feedbackCtrl->GetValue();
    preset.stereo = stereoCtrl->GetValue();
    preset.ams = amsCtrl->GetValue();
    preset.fms = fmsCtrl->GetValue();

    for (int op = 0; op < 4; op++) {
        auto& p = preset.operators[op];
        p.multiple = opControls[op].multiple->GetValue();
        p.detune = opControls[op].detune->GetValue();
        p.totalLevel = opControls[op].totalLevel->GetValue();
        p.rateScaling = opControls[op].rateScaling->GetValue();
        p.attackRate = opControls[op].attackRate->GetValue();
        p.firstDecayRate = opControls[op].firstDecayRate->GetValue();
        p.secondaryDecayRate = opControls[op].secondaryDecayRate->GetValue();
        p.releaseRate = opControls[op].releaseRate->GetValue();
        p.secondaryAmplitude = opControls[op].secondaryAmplitude->GetValue();
        p.amplitudeModulation = opControls[op].amplitudeModulation->GetValue();
        p.ssgEg = opControls[op].ssgEg->GetValue();
    }

    std::vector<u8> data;
    wxString path = dlg.GetPath();
    if (path.Lower().EndsWith(".tfi"))
        data = TfiParser::Serialize(preset);
    else if (path.Lower().EndsWith(".vgi"))
        data = VgiParser::Serialize(preset);
    else
        return;

    std::ofstream f(path.ToStdString(), std::ios::binary);
    if (f.is_open())
        f.write(reinterpret_cast<const char*>(data.data()), data.size());
}
