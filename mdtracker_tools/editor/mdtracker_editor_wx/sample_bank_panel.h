#pragma once
#include "compat_types.h"
#include "models.h"
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/button.h>

class MainFrame;

class SampleBankPanel : public wxPanel {
public:
    SampleBankPanel(wxWindow* parent, MainFrame* mainFrame);
    void RefreshBanks(const std::vector<SampleBank>& banks);
    void RefreshPool();

private:
    void OnBankChoice(wxCommandEvent& event);
    void OnGridCellChanged(wxGridEvent& event);
    void OnGridCellLeftClick(wxGridEvent& event);
    void OnApplyRange(wxCommandEvent& event);
    void OnAssignSamples(wxCommandEvent& event);
    void OnAddSamples(wxCommandEvent& event);
    void OnRemoveSample(wxCommandEvent& event);
    void OnClearSamples(wxCommandEvent& event);
    void SyncSlotFromPool(int slotIndex);
    void WriteSlotToGrid(int row, const SampleSlot& slot);
    void PopulateGrid();

    MainFrame* mainFrame;
    std::vector<SampleBank> currentBanks;

    wxChoice* bankChoice;
    wxGrid* grid;

    wxSpinCtrl* rangeStart;
    wxSpinCtrl* rangeEnd;
    wxChoice* rangePanChoice;
    wxChoice* rangeRateChoice;
    wxCheckBox* rangeLoopCheck;
    wxButton* applyRangeBtn;
    wxButton* assignSamplesBtn;

    wxListView* poolList;
    wxButton* addSamplesBtn;
    wxButton* removeBtn;
    wxButton* clearBtn;
    wxStaticText* poolTotalLabel;

    enum Cols {
        COL_SYNC = 0,
        COL_NOTE,
        COL_START,
        COL_END,
        COL_PAN,
        COL_RATE,
        COL_LOOP,
        COL_NAME,
        COL_COUNT
    };

    bool updating = false;

    wxDECLARE_EVENT_TABLE();
};
