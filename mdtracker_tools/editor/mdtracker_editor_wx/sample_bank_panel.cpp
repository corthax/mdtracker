#include "sample_bank_panel.h"
#include "main_frame.h"
#include "sample_converter_service.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <fstream>

wxBEGIN_EVENT_TABLE(SampleBankPanel, wxPanel)
    EVT_CHOICE(wxID_ANY, SampleBankPanel::OnBankChoice)
    EVT_GRID_CELL_CHANGED(SampleBankPanel::OnGridCellChanged)
wxEND_EVENT_TABLE()

SampleBankPanel::SampleBankPanel(wxWindow* parent, MainFrame* frame)
    : wxPanel(parent), mainFrame(frame)
{
    auto* outer = new wxBoxSizer(wxVERTICAL);

    auto* topRow = new wxBoxSizer(wxHORIZONTAL);
    topRow->Add(new wxStaticText(this, wxID_ANY, "Bank:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    bankChoice = new wxChoice(this, wxID_ANY);
    for (int i = 0; i < 4; i++)
        bankChoice->Append(wxString::Format("Bank %d", i));
    bankChoice->SetSelection(0);
    topRow->Add(bankChoice, 0, wxRIGHT, 8);
    outer->Add(topRow, 0, wxALL, 8);

    auto* rangeBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Range Edit");
    rangeBox->Add(new wxStaticText(this, wxID_ANY, "Start:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    rangeStart = new wxSpinCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxSize(55, -1));
    rangeStart->SetRange(0, 95);
    rangeBox->Add(rangeStart, 0, wxRIGHT, 4);

    rangeBox->Add(new wxStaticText(this, wxID_ANY, "End:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    rangeEnd = new wxSpinCtrl(this, wxID_ANY, "95", wxDefaultPosition, wxSize(55, -1));
    rangeEnd->SetRange(0, 95);
    rangeEnd->SetValue(95);
    rangeBox->Add(rangeEnd, 0, wxRIGHT, 8);

    rangeBox->Add(new wxStaticText(this, wxID_ANY, "Pan:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    rangePanChoice = new wxChoice(this, wxID_ANY);
    rangePanChoice->Append("L");
    rangePanChoice->Append("C");
    rangePanChoice->Append("R");
    rangePanChoice->SetSelection(1);
    rangeBox->Add(rangePanChoice, 0, wxRIGHT, 4);

    rangeBox->Add(new wxStaticText(this, wxID_ANY, "Rate:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    rangeRateChoice = new wxChoice(this, wxID_ANY);
    for (int r : SampleSlot::RateOptions)
        rangeRateChoice->Append(wxString::Format("%d", r));
    rangeRateChoice->SetSelection(1);
    rangeBox->Add(rangeRateChoice, 0, wxRIGHT, 4);

    rangeLoopCheck = new wxCheckBox(this, wxID_ANY, "Loop");
    rangeBox->Add(rangeLoopCheck, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    applyRangeBtn = new wxButton(this, wxID_ANY, "Apply");
    rangeBox->Add(applyRangeBtn, 0, wxRIGHT, 4);

    assignSamplesBtn = new wxButton(this, wxID_ANY, "Assign Samples");
    rangeBox->Add(assignSamplesBtn, 0);

    outer->Add(rangeBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* splitBox = new wxBoxSizer(wxHORIZONTAL);

    auto* poolPanel = new wxPanel(this);
    auto* poolSizer = new wxBoxSizer(wxVERTICAL);

    auto* poolHeader = new wxBoxSizer(wxHORIZONTAL);
    poolHeader->Add(new wxStaticText(poolPanel, wxID_ANY, "Sample Pool"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    addSamplesBtn = new wxButton(poolPanel, wxID_ANY, "+Add Samples");
    poolHeader->Add(addSamplesBtn, 0);
    poolSizer->Add(poolHeader, 0, wxEXPAND | wxALL, 4);

    poolList = new wxListView(poolPanel, wxID_ANY, wxDefaultPosition, wxSize(220, -1),
        wxLC_REPORT | wxLC_NO_HEADER);
    poolList->AppendColumn("ID", wxLIST_FORMAT_LEFT, 30);
    poolList->AppendColumn("Name", wxLIST_FORMAT_LEFT, 130);
    poolList->AppendColumn("Size", wxLIST_FORMAT_RIGHT, 60);
    poolSizer->Add(poolList, 1, wxEXPAND | wxALL, 4);

    auto* poolButtons = new wxBoxSizer(wxHORIZONTAL);
    removeBtn = new wxButton(poolPanel, wxID_ANY, "Remove");
    clearBtn = new wxButton(poolPanel, wxID_ANY, "Clear");
    poolButtons->Add(removeBtn, 0, wxRIGHT, 4);
    poolButtons->Add(clearBtn, 0);
    poolSizer->Add(poolButtons, 0, wxLEFT | wxRIGHT | wxBOTTOM, 4);

    poolTotalLabel = new wxStaticText(poolPanel, wxID_ANY, "Total: 0 bytes");
    poolSizer->Add(poolTotalLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 4);

    poolPanel->SetSizer(poolSizer);
    splitBox->Add(poolPanel, 0, wxEXPAND | wxRIGHT, 4);

    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(96, COL_COUNT);

    grid->SetColLabelValue(COL_SYNC, wxEmptyString);
    grid->SetColLabelValue(COL_NOTE, "Note");
    grid->SetColLabelValue(COL_START, "Start");
    grid->SetColLabelValue(COL_END, "End");
    grid->SetColLabelValue(COL_PAN, "Pan");
    grid->SetColLabelValue(COL_RATE, "Rate");
    grid->SetColLabelValue(COL_LOOP, "Loop");
    grid->SetColLabelValue(COL_NAME, "Name");

    grid->SetColSize(COL_SYNC, 30);
    grid->SetColSize(COL_NOTE, 85);
    grid->SetColSize(COL_START, 80);
    grid->SetColSize(COL_END, 80);
    grid->SetColSize(COL_PAN, 45);
    grid->SetColSize(COL_RATE, 60);
    grid->SetColSize(COL_LOOP, 35);
    grid->SetColSize(COL_NAME, 160);

    wxArrayString panChoices;
    panChoices.Add("L");
    panChoices.Add("C");
    panChoices.Add("R");

    wxArrayString rateChoices;
    for (int r : SampleSlot::RateOptions)
        rateChoices.Add(wxString::Format("%d", r));

    for (int row = 0; row < 96; row++) {
        grid->SetCellEditor(row, COL_PAN, new wxGridCellChoiceEditor(panChoices));
        grid->SetCellAlignment(row, COL_PAN, wxALIGN_CENTER, wxALIGN_CENTRE);
        grid->SetCellEditor(row, COL_RATE, new wxGridCellChoiceEditor(rateChoices));
        grid->SetCellAlignment(row, COL_RATE, wxALIGN_CENTER, wxALIGN_CENTRE);
        grid->SetCellEditor(row, COL_LOOP, new wxGridCellBoolEditor);
        grid->SetCellRenderer(row, COL_LOOP, new wxGridCellBoolRenderer);
        grid->SetCellAlignment(row, COL_LOOP, wxALIGN_CENTER, wxALIGN_CENTRE);
        grid->SetCellAlignment(row, COL_START, wxALIGN_RIGHT, wxALIGN_CENTRE);
        grid->SetCellAlignment(row, COL_END, wxALIGN_RIGHT, wxALIGN_CENTRE);
        grid->SetReadOnly(row, COL_SYNC, true);
        grid->SetReadOnly(row, COL_NOTE, true);
    }

    grid->SetRowLabelSize(0);
    grid->SetGridLineColour(*wxLIGHT_GREY);
    grid->EnableEditing(true);
    grid->DisableDragRowSize();

    splitBox->Add(grid, 1, wxEXPAND);

    outer->Add(splitBox, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(outer);

    grid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &SampleBankPanel::OnGridCellLeftClick, this);
    applyRangeBtn->Bind(wxEVT_BUTTON, &SampleBankPanel::OnApplyRange, this);
    assignSamplesBtn->Bind(wxEVT_BUTTON, &SampleBankPanel::OnAssignSamples, this);
    addSamplesBtn->Bind(wxEVT_BUTTON, &SampleBankPanel::OnAddSamples, this);
    removeBtn->Bind(wxEVT_BUTTON, &SampleBankPanel::OnRemoveSample, this);
    clearBtn->Bind(wxEVT_BUTTON, &SampleBankPanel::OnClearSamples, this);
}

void SampleBankPanel::WriteSlotToGrid(int row, const SampleSlot& slot) {
    grid->SetCellValue(row, COL_SYNC, slot.isSynced ? "(S)" : "S");
    grid->SetCellValue(row, COL_NOTE, slot.NoteName());
    int bankBase = mainFrame->GetSettingsService()->SampleBankAddr();
    int startRel = (slot.startOffset >= bankBase) ? slot.startOffset - bankBase : 0;
    int endRel = (slot.endOffset >= bankBase) ? slot.endOffset - bankBase : 0;
    grid->SetCellValue(row, COL_START, wxString::Format("%d", startRel));
    grid->SetCellValue(row, COL_END, wxString::Format("%d", endRel));

    int panIdx = (slot.pan == 128) ? 0 : (slot.pan == 192) ? 1 : 2;
    grid->SetCellValue(row, COL_PAN, SampleSlot::PanLabels[panIdx]);

    int rateIdx = (slot.rate >= 0 && slot.rate < 6) ? slot.rate : 0;
    grid->SetCellValue(row, COL_RATE, SampleSlot::RateLabels[rateIdx]);

    grid->SetCellValue(row, COL_LOOP, slot.looped ? "1" : "");
    grid->SetCellValue(row, COL_NAME, wxString(slot.name));
}

void SampleBankPanel::PopulateGrid() {
    updating = true;
    int selBank = bankChoice->GetSelection();
    if (selBank < 0 || selBank >= static_cast<int>(currentBanks.size())) {
        updating = false;
        return;
    }
    const auto& bank = currentBanks[selBank];
    for (int row = 0; row < 96 && row < static_cast<int>(bank.slots.size()); row++)
        WriteSlotToGrid(row, bank.slots[row]);
    updating = false;
}

void SampleBankPanel::RefreshBanks(const std::vector<SampleBank>& banks) {
    currentBanks = banks;
    PopulateGrid();
}

void SampleBankPanel::RefreshPool() {
    auto& pool = mainFrame->GetSamplePool();
    poolList->DeleteAllItems();

    int totalSize = 0;
    for (auto& sf : pool) {
        long idx = poolList->InsertItem(poolList->GetItemCount(), wxString::Format("%d", sf.id));
        poolList->SetItem(idx, 1, wxString(sf.shortName));
        int sz = sf.originalSize;
        poolList->SetItem(idx, 2, wxString::Format("%d", sz));
        totalSize += sz;
    }

    poolTotalLabel->SetLabel(wxString::Format("Total: %d bytes", totalSize));
}

void SampleBankPanel::OnBankChoice(wxCommandEvent&) {
    PopulateGrid();
}

void SampleBankPanel::OnGridCellChanged(wxGridEvent& event) {
    if (updating) return;
    int row = event.GetRow();
    int col = event.GetCol();
    int selBank = bankChoice->GetSelection();
    if (selBank < 0 || selBank >= static_cast<int>(currentBanks.size())) return;
    if (row < 0 || row >= 96) return;
    auto& slot = currentBanks[selBank].slots[row];

    int bankBase = mainFrame->GetSettingsService()->SampleBankAddr();
    switch (col) {
        case COL_START:
            slot.startOffset = bankBase + wxAtoi(grid->GetCellValue(row, col));
            break;
        case COL_END:
            slot.endOffset = bankBase + wxAtoi(grid->GetCellValue(row, col));
            break;
        case COL_PAN: {
            wxString val = grid->GetCellValue(row, col);
            slot.pan = (val == "L") ? 128 : (val == "R") ? 64 : 192;
            break;
        }
        case COL_RATE: {
            wxString val = grid->GetCellValue(row, col);
            for (int i = 0; i < 6; i++) {
                if (val == SampleSlot::RateLabels[i]) {
                    slot.rate = i;
                    break;
                }
            }
            break;
        }
        case COL_LOOP:
            slot.looped = (grid->GetCellValue(row, col) == "1");
            break;
        case COL_NAME:
            slot.name = grid->GetCellValue(row, col).ToStdString();
            break;
    }
}

void SampleBankPanel::OnGridCellLeftClick(wxGridEvent& event) {
    int col = event.GetCol();
    if (col == COL_SYNC) {
        SyncSlotFromPool(event.GetRow());
        event.Skip();
    } else if (col == COL_PAN || col == COL_RATE) {
        event.Skip();
        CallAfter([this]() { grid->EnableCellEditControl(true); });
    } else if (col == COL_LOOP) {
        int selBank = bankChoice->GetSelection();
        if (selBank >= 0) {
            auto& slot = currentBanks[selBank].slots[event.GetRow()];
            slot.looped = !slot.looped;
            WriteSlotToGrid(event.GetRow(), slot);
        }
    } else {
        event.Skip();
    }
}

void SampleBankPanel::SyncSlotFromPool(int slotIndex) {
    int selBank = bankChoice->GetSelection();
    if (selBank < 0 || selBank >= static_cast<int>(currentBanks.size())) return;
    auto& slot = currentBanks[selBank].slots[slotIndex];

    auto& pool = mainFrame->GetSamplePool();

    long poolSel = poolList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (poolSel < 0) {
        wxMessageBox("Select a sample in the pool first.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    if (poolSel >= static_cast<long>(pool.size())) return;
    const auto& sf = pool[poolSel];
    slot.samplePoolId = sf.id;
    slot.isSynced = true;
    slot.name = sf.shortName;
    slot.startOffset = mainFrame->GetSettingsService()->SampleBankAddr() + static_cast<int>(sf.startOffset);
    slot.endOffset = mainFrame->GetSettingsService()->SampleBankAddr() + static_cast<int>(sf.endOffset);

    WriteSlotToGrid(slotIndex, slot);
}

void SampleBankPanel::OnApplyRange(wxCommandEvent&) {
    int selBank = bankChoice->GetSelection();
    if (selBank < 0 || selBank >= static_cast<int>(currentBanks.size())) return;

    int start = rangeStart->GetValue();
    int end = rangeEnd->GetValue();
    if (start > end) std::swap(start, end);

    int panVal = (rangePanChoice->GetSelection() == 0) ? 128 :
                 (rangePanChoice->GetSelection() == 1) ? 192 : 64;
    int rateIdx = rangeRateChoice->GetSelection();
    bool looped = rangeLoopCheck->GetValue();

    for (int i = start; i <= end; i++) {
        auto& slot = currentBanks[selBank].slots[i];
        slot.pan = panVal;
        slot.rate = rateIdx;
        slot.looped = looped;
        WriteSlotToGrid(i, slot);
    }
}

void SampleBankPanel::OnAssignSamples(wxCommandEvent&) {
    int selBank = bankChoice->GetSelection();
    if (selBank < 0 || selBank >= static_cast<int>(currentBanks.size())) return;

    auto& pool = mainFrame->GetSamplePool();
    if (pool.empty()) {
        wxMessageBox("No samples in pool.\nAdd samples first, then try again.",
                     "Pool Empty", wxOK | wxICON_INFORMATION);
        return;
    }

    std::vector<int> sel;
    long item = poolList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    while (item >= 0) {
        sel.push_back(static_cast<int>(item));
        item = poolList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }
    if (sel.empty()) {
        wxMessageBox("Select samples in the pool to assign.", "No Selection", wxOK | wxICON_INFORMATION);
        return;
    }

    int start = rangeStart->GetValue();
    int end = rangeEnd->GetValue();
    if (start > end) std::swap(start, end);

    int si = 0;
    for (int i = start; i <= end && si < static_cast<int>(sel.size()); i++, si++) {
        auto& slot = currentBanks[selBank].slots[i];
        const auto& sf = pool[sel[si]];
        slot.samplePoolId = sf.id;
        slot.isSynced = true;
        slot.name = sf.shortName;
        slot.startOffset = mainFrame->GetSettingsService()->SampleBankAddr() + static_cast<int>(sf.startOffset);
        slot.endOffset = mainFrame->GetSettingsService()->SampleBankAddr() + static_cast<int>(sf.endOffset);
        WriteSlotToGrid(i, slot);
    }
}

void SampleBankPanel::OnAddSamples(wxCommandEvent&) {
    wxFileDialog dlg(this, "Add sample files",
        wxEmptyString, wxEmptyString,
        "Sample files (*.wav;*.flac;*.wv;*.s8pcm;*.2adpcm)|*.wav;*.flac;*.wv;*.s8pcm;*.2adpcm|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);
    if (dlg.ShowModal() == wxID_CANCEL) return;

    SampleConverterService svc;
    wxArrayString paths;
    dlg.GetPaths(paths);

    auto& pool = mainFrame->GetSamplePool();

    for (size_t f = 0; f < paths.size(); f++) {
        const wxString& path = paths[f];
        wxString ext = path.AfterLast('.').Lower();

        std::ifstream file(path.ToStdString(), std::ios::binary | std::ios::ate);
        if (!file.is_open()) continue;
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<u8> fileData(static_cast<size_t>(fileSize));
        file.read(reinterpret_cast<char*>(fileData.data()), fileSize);

        std::vector<u8> pcm8;

        if (ext == "wav") {
            try {
                auto wav = svc.ParseWav(fileData);
                auto samples = svc.DecodeSamples(wav);
                auto resampled = svc.Resample(samples, wav.sampleRate, 22050);
                pcm8 = svc.EncodePcm8(resampled);
            } catch (const std::exception& e) {
                wxMessageBox(wxString::Format("Failed to parse %s:\n%s", path, e.what()),
                             "Error", wxOK | wxICON_ERROR);
                continue;
            }
        } else if (ext == "flac") {
            try {
                auto samples = svc.DecodeFlac(fileData);
                auto resampled = svc.Resample(samples, 44100, 22050);
                pcm8 = svc.EncodePcm8(resampled);
            } catch (const std::exception& e) {
                wxMessageBox(wxString::Format("Failed to decode %s:\n%s", path, e.what()),
                             "Error", wxOK | wxICON_ERROR);
                continue;
            }
        } else if (ext == "wv") {
            try {
                int sr = 0;
                auto samples = svc.DecodeWavpack(fileData, sr);
                auto resampled = svc.Resample(samples, sr, 22050);
                pcm8 = svc.EncodePcm8(resampled);
            } catch (const std::exception& e) {
                wxMessageBox(wxString::Format("Failed to decode %s:\n%s", path, e.what()),
                             "Error", wxOK | wxICON_ERROR);
                continue;
            }
        } else if (ext == "s8pcm" || ext == "2adpcm") {
            pcm8 = std::move(fileData);
        } else {
            continue;
        }

        SampleFile sf;
        sf.id = static_cast<int>(pool.size());
        sf.fileName = path.ToStdString();
        sf.shortName = SampleFile::ExtractShortName(path.ToStdString());
        sf.data = std::move(pcm8);
        sf.originalSize = static_cast<int>(sf.data.size());
        pool.push_back(std::move(sf));
    }

    int offset = 0;
    for (auto& sf : pool) {
        sf.startOffset = offset;
        sf.endOffset = offset + sf.AlignedSize();
        offset += sf.AlignedSize();
    }

    RefreshPool();
}

void SampleBankPanel::OnRemoveSample(wxCommandEvent&) {
    auto& pool = mainFrame->GetSamplePool();

    std::vector<int> sel;
    long item = poolList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    while (item >= 0) {
        sel.push_back(static_cast<int>(item));
        item = poolList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }
    if (sel.empty()) return;

    std::sort(sel.begin(), sel.end(), std::greater<int>());
    for (int idx : sel)
        pool.erase(pool.begin() + idx);

    for (size_t i = 0; i < pool.size(); i++)
        pool[i].id = static_cast<int>(i);

    int offset = 0;
    for (auto& sf : pool) {
        sf.startOffset = offset;
        sf.endOffset = offset + sf.AlignedSize();
        offset += sf.AlignedSize();
    }

    RefreshPool();
}

void SampleBankPanel::OnClearSamples(wxCommandEvent&) {
    auto& pool = mainFrame->GetSamplePool();
    if (pool.empty()) return;
    pool.clear();
    RefreshPool();
}
