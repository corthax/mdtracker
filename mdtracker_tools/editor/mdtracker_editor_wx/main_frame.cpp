#include "main_frame.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/grid.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/slider.h>
#include <wx/radiobox.h>
#include <algorithm>
#include <fstream>
#include <dwmapi.h>
#include <uxtheme.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpenRom)
    EVT_MENU(wxID_SAVE, MainFrame::OnSaveRom)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveRomAs)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

namespace {

    // Undocumented Win32 dark-mode APIs (ordinal exports from uxtheme.dll)
    int SetPreferredAppMode(int mode) {
        static auto func = []() -> int(WINAPI*)(int) {
            HMODULE h = LoadLibraryW(L"uxtheme.dll");
            return h ? reinterpret_cast<int(WINAPI*)(int)>(GetProcAddress(h, MAKEINTRESOURCEA(135))) : nullptr;
        }();
        return func ? func(mode) : 0;
    }

    bool AllowDarkForWindow(HWND hwnd, bool allow) {
        static auto func = []() -> bool(WINAPI*)(HWND, bool) {
            HMODULE h = LoadLibraryW(L"uxtheme.dll");
            return h ? reinterpret_cast<bool(WINAPI*)(HWND, bool)>(GetProcAddress(h, MAKEINTRESOURCEA(133))) : nullptr;
        }();
        return func ? func(hwnd, allow) : false;
    }

    // Palette
    const wxColour bg(0x1e, 0x1e, 0x1e);
    const wxColour ctrlBg(0x2d, 0x2d, 0x30);
    const wxColour textFg(0xcc, 0xcc, 0xcc);
    const wxColour editBg(0x3c, 0x3c, 0x3c);
    const wxColour gridBg(0x25, 0x25, 0x26);
    const wxColour gridLine(0x3c, 0x3c, 0x3c);
    const wxColour selBg(0x09, 0x47, 0x71);
    const wxColour statusBg(0x00, 0x7a, 0xcc);

    void ApplyDarkPalette(wxWindow* win, bool dark) {
        HWND hwnd = (HWND)win->GetHWND();

        if (hwnd) {
            AllowDarkForWindow(hwnd, dark);
            SetWindowTheme(hwnd, dark ? L"DarkMode_Explorer" : NULL, NULL);
            SendMessageW(hwnd, WM_THEMECHANGED, 0, 0);
        }

        if (dark) {
            if (wxDynamicCast(win, wxGrid)) {
                auto* g = static_cast<wxGrid*>(win);
                g->SetDefaultCellBackgroundColour(gridBg);
                g->SetDefaultCellTextColour(textFg);
                g->SetGridLineColour(gridLine);
                g->SetLabelBackgroundColour(ctrlBg);
                g->SetLabelTextColour(textFg);
                g->SetSelectionBackground(selBg);
                g->SetSelectionForeground(*wxWHITE);
            } else if (wxDynamicCast(win, wxListCtrl)) {
                win->SetBackgroundColour(gridBg);
                win->SetForegroundColour(textFg);
            } else if (wxDynamicCast(win, wxTextCtrl) ||
                       wxDynamicCast(win, wxSpinCtrl)) {
                win->SetBackgroundColour(editBg);
                win->SetForegroundColour(textFg);
            } else if (wxDynamicCast(win, wxButton)   ||
                       wxDynamicCast(win, wxChoice)   ||
                       wxDynamicCast(win, wxCheckBox) ||
                       wxDynamicCast(win, wxRadioBox)) {
                win->SetBackgroundColour(ctrlBg);
                win->SetForegroundColour(textFg);
            } else if (wxDynamicCast(win, wxStaticText)) {
                win->SetForegroundColour(textFg);
                win->SetBackgroundColour(bg);
            } else if (wxDynamicCast(win, wxSlider)) {
                win->SetBackgroundColour(bg);
            } else if (wxDynamicCast(win, wxStatusBar)) {
                win->SetBackgroundColour(statusBg);
                win->SetForegroundColour(*wxWHITE);
            } else if (wxDynamicCast(win, wxNotebook)) {
                win->SetBackgroundColour(bg);
                win->SetForegroundColour(textFg);
            } else {
                win->SetBackgroundColour(bg);
                win->SetForegroundColour(textFg);
            }
        } else {
            const wxColour sysWin = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
            const wxColour sysWinText = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
            const wxColour sysFace = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

            if (wxDynamicCast(win, wxGrid)) {
                auto* g = static_cast<wxGrid*>(win);
                g->SetDefaultCellBackgroundColour(sysWin);
                g->SetDefaultCellTextColour(sysWinText);
                g->SetGridLineColour(*wxLIGHT_GREY);
                g->SetLabelBackgroundColour(sysFace);
                g->SetLabelTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
                g->SetSelectionBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
                g->SetSelectionForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
            } else if (wxDynamicCast(win, wxTextCtrl) ||
                       wxDynamicCast(win, wxSpinCtrl)) {
                win->SetBackgroundColour(sysWin);
                win->SetForegroundColour(sysWinText);
            } else if (wxDynamicCast(win, wxStaticText)) {
                win->SetBackgroundColour(sysFace);
                win->SetForegroundColour(sysWinText);
            } else if (wxDynamicCast(win, wxStatusBar)) {
                win->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
                win->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
            } else {
                win->SetBackgroundColour(sysFace);
                win->SetForegroundColour(sysWinText);
            }
        }

        win->Refresh();

        wxWindowList& children = win->GetChildren();
        for (auto* child : children) {
            ApplyDarkPalette(child, dark);
        }
    }

} // anonymous namespace

void MainFrame::ApplyTheme() {
    bool dark = settingsService->settings.darkMode;

    SetPreferredAppMode(dark ? 1 : 0);

    HWND hwnd = (HWND)GetHWND();
    const BOOL darkMode = dark ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));

    ApplyDarkPalette(this, dark);
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "MD.Tracker Editor", wxDefaultPosition, wxSize(1280, 720))
    , settingsService(std::make_unique<SettingsService>())
    , romService(std::make_unique<RomService>(settingsService.get()))
    , banks(SampleBank::CreateAll())
{
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "&Open ROM...\tCtrl+O");
    fileMenu->Append(wxID_SAVE, "&Save ROM\tCtrl+S");
    fileMenu->Append(wxID_SAVEAS, "Save ROM &As...\tCtrl+Shift+S");
    fileMenu->AppendSeparator();
    int idOpenBulk = wxNewId();
    fileMenu->Append(idOpenBulk, "Open ROMs For &Bulk Save...\tCtrl+B");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4");

    Bind(wxEVT_MENU, &MainFrame::OnOpenBulkRoms, this, idOpenBulk);

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

    notebook->AddPage(sampleBankPanel, "Sample Bank Editor");
    notebook->AddPage(instrumentPanel, "Instrument Preset Editor");
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

    ApplyTheme();
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

    std::vector<std::string> failed;
    for (auto& path : bulkRomPaths) {
        std::ofstream f(path, std::ios::binary);
        if (!f.is_open()) { failed.push_back(path); continue; }
        auto& data = romService->GetRomData();
        f.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    if (!failed.empty()) {
        wxString msg = "Bulk save completed with errors.\nCould not write to:";
        for (auto& p : failed) msg += "\n" + wxString(p);
        wxMessageBox(msg, "Bulk Save Errors", wxOK | wxICON_WARNING);
    }

    wxString status = "Saved: " + wxString(romService->RomPath());
    if (!bulkRomPaths.empty())
        status += wxString::Format(" + %zu bulk ROM(s)", bulkRomPaths.size());
    statusBar->SetStatusText(status);
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

void MainFrame::OnOpenBulkRoms(wxCommandEvent&) {
    wxFileDialog dlg(this, "Select ROMs For Bulk Save",
        wxEmptyString, wxEmptyString,
        "ROM files (*.bin;*.md;*.gen)|*.bin;*.md;*.gen|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_CANCEL) return;

    wxArrayString paths;
    dlg.GetPaths(paths);
    for (auto& p : paths) {
        std::string ns = p.ToStdString();
        if (std::find(bulkRomPaths.begin(), bulkRomPaths.end(), ns) == bulkRomPaths.end())
            bulkRomPaths.push_back(ns);
    }
    sampleBankPanel->RefreshBulkRomList();
}

void MainFrame::OnQuit(wxCommandEvent&) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent&) {
    wxMessageBox("MD.Tracker Editor\n\n"
                 "A tool for editing\n"
                 "MD.Tracker ROM and SRAM save files.\n"
                 "\n"
                 "By Corthax (Scythe of Luna)\n"
                 "Version 1.5.0",
                 "About MD.Tracker Editor =^..^=", wxOK | wxICON_INFORMATION);
}
