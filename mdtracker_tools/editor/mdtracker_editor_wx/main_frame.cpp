#include "main_frame.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpenRom)
    EVT_MENU(wxID_SAVE, MainFrame::OnSaveRom)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveRomAs)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "MDTracker Editor (wx)", wxDefaultPosition, wxSize(960, 720))
    , settingsService(std::make_unique<SettingsService>())
    , banks(SampleBank::CreateAll())
{
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "&Open ROM...\tCtrl+O");
    fileMenu->Append(wxID_SAVE, "&Save ROM\tCtrl+S");
    fileMenu->Append(wxID_SAVEAS, "Save ROM &As...\tCtrl+Shift+S");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4");

    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

    notebook = new wxNotebook(this, wxID_ANY);

    instrumentPanel = new InstrumentEditorPanel(notebook, this);
    sampleBankPanel = new SampleBankPanel(notebook, this);
    sampleConverterPanel = new SampleConverterPanel(notebook, this);
    saveConverterPanel = new SaveConverterPanel(notebook, this);
    settingsPanel = new SettingsPanel(notebook, this);

    notebook->AddPage(instrumentPanel, "Instrument Editor");
    notebook->AddPage(sampleBankPanel, "Sample Bank");
    notebook->AddPage(sampleConverterPanel, "Sample Converter");
    notebook->AddPage(saveConverterPanel, "Save Converter");
    notebook->AddPage(settingsPanel, "Settings");

    // Initialize sample bank grid with default (pre-allocated) banks
    sampleBankPanel->RefreshBanks(banks);

    statusBar = CreateStatusBar(1);
    statusBar->SetStatusText("No ROM loaded");

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(notebook, 1, wxEXPAND);
    SetSizer(sizer);

    romService = std::make_unique<RomService>(settingsService.get());
}

MainFrame::~MainFrame() {
    settingsService->Save();
}

bool MainFrame::LoadRom(const std::string& path) {
    if (!romService->Load(path)) {
        wxMessageBox("Failed to load ROM file.", "Error", wxOK | wxICON_ERROR);
        return false;
    }

    romService->PopulateBanks(banks);
    statusBar->SetStatusText("Loaded: " + wxString(path));

    // Refresh all panels
    sampleBankPanel->RefreshBanks(banks);
    settingsPanel->RefreshSettings(settingsService->settings);

    return true;
}

void MainFrame::OnOpenRom(wxCommandEvent&) {
    wxFileDialog dlg(this, "Open ROM File",
        wxEmptyString, wxEmptyString,
        "ROM files (*.bin;*.md;*.gen)|*.bin;*.md;*.gen|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_CANCEL) return;
    LoadRom(dlg.GetPath().ToStdString());
}

void MainFrame::OnSaveRom(wxCommandEvent&) {
    if (!romService->IsLoaded()) return;
    romService->WriteSampleBank(samplePool, sampleBankPanel->GetBanks());
    romService->Save();
    statusBar->SetStatusText("Saved: " + wxString(romService->RomPath()));
}

void MainFrame::OnSaveRomAs(wxCommandEvent&) {
    if (!romService->IsLoaded()) return;
    wxFileDialog dlg(this, "Save ROM As",
        wxEmptyString, wxEmptyString,
        "ROM files (*.bin;*.md;*.gen)|*.bin;*.md;*.gen|All files (*.*)|*.*",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() == wxID_CANCEL) return;
    romService->WriteSampleBank(samplePool, sampleBankPanel->GetBanks());
    romService->Save(dlg.GetPath().ToStdString());
    statusBar->SetStatusText("Saved: " + dlg.GetPath());
}

void MainFrame::OnQuit(wxCommandEvent&) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent&) {
    wxMessageBox("MDTracker Editor (wxWidgets port)\n\n"
                 "A tool for editing SEGA Mega Drive / Genesis\n"
                 "MDTracker ROM files.",
                 "About MDTracker Editor", wxOK | wxICON_INFORMATION);
}
