using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Models;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SampleBankViewModel : ViewModelBase
{
    private readonly RomService _romService;
    private readonly SamplePoolViewModel _pool;

    public ObservableCollection<SampleSlot> Bank0Slots { get; }
    public ObservableCollection<SampleSlot> Bank1Slots { get; }
    public ObservableCollection<SampleSlot> Bank2Slots { get; }
    public ObservableCollection<SampleSlot> Bank3Slots { get; }

    public SampleBank[] Banks { get; }

    [ObservableProperty]
    private int _selectedBankIndex;

    [ObservableProperty]
    private int _rangeStart;

    [ObservableProperty]
    private int _rangeEnd = 383;

    [ObservableProperty]
    private int _rangePan = 192;

    [ObservableProperty]
    private int _rangeRate;

    [ObservableProperty]
    private int _rangePanIndex = 1;

    [ObservableProperty]
    private int _rangeRateIndex;

    partial void OnRangePanChanged(int value)
    {
        RangePanIndex = value switch { 128 => 0, 192 => 1, 64 => 2, _ => 1 };
    }

    partial void OnRangePanIndexChanged(int value)
    {
        RangePan = value switch { 0 => 128, 1 => 192, 2 => 64, _ => 192 };
    }

    partial void OnRangeRateChanged(int value) => RangeRateIndex = value;
    partial void OnRangeRateIndexChanged(int value) => RangeRate = value;

    [ObservableProperty]
    private bool _rangeLoop;

    public SampleBankViewModel(RomService romService, SamplePoolViewModel pool)
    {
        _romService = romService;
        _pool = pool;
        Banks = SampleBank.CreateAll();
        Bank0Slots = Banks[0].Slots;
        Bank1Slots = Banks[1].Slots;
        Bank2Slots = Banks[2].Slots;
        Bank3Slots = Banks[3].Slots;
    }

    public void LoadFromRom()
    {
        if (!_romService.IsLoaded) return;
        _romService.PopulateBanks(Banks);
    }

    public void SaveToRom()
    {
        if (!_romService.IsLoaded) return;
        _romService.WriteBanks(Banks);
    }

    public ObservableCollection<SampleSlot> GetBank(int index) => index switch
    {
        0 => Bank0Slots,
        1 => Bank1Slots,
        2 => Bank2Slots,
        3 => Bank3Slots,
        _ => Bank0Slots,
    };

    [RelayCommand]
    private void ApplyAutoRange()
    {
        int start = Math.Clamp(RangeStart, 0, RomConstants.NotesTotal - 1);
        int end = Math.Clamp(RangeEnd, 0, RomConstants.NotesTotal - 1);
        if (start > end) (start, end) = (end, start);

        for (int i = start; i <= end; i++)
        {
            int bank = i / RomConstants.NotesPerBank;
            int note = i % RomConstants.NotesPerBank;
            if (bank >= RomConstants.BankCount) break;
            GetBank(bank)[note].Pan = RangePan;
            GetBank(bank)[note].Rate = RangeRate;
            GetBank(bank)[note].Looped = RangeLoop;
        }
    }

    [RelayCommand]
    private void SyncFromPool(SampleSlot slot)
    {
        var sample = _pool.SelectedSample;
        if (sample == null) return;
        slot.StartOffset = (int)sample.StartOffset;
        slot.EndOffset = (int)sample.EndOffset;
        slot.Name = sample.ShortName;
        slot.SamplePoolId = sample.Id;
        slot.IsSynced = true;
    }

    [RelayCommand]
    private void AssignSamplesToRange()
    {
        var selected = _pool.SelectedSamples;
        if (selected.Count == 0) return;

        int start = Math.Clamp(RangeStart, 0, RomConstants.NotesTotal - 1);
        int end = Math.Clamp(RangeEnd, 0, RomConstants.NotesTotal - 1);
        if (start > end) (start, end) = (end, start);

        int si = 0;
        for (int i = start; i <= end && si < selected.Count; i++, si++)
        {
            int bank = i / RomConstants.NotesPerBank;
            int note = i % RomConstants.NotesPerBank;
            if (bank >= RomConstants.BankCount) break;
            var sample = selected[si];
            var slot = GetBank(bank)[note];
            slot.StartOffset = (int)sample.StartOffset;
            slot.EndOffset = (int)sample.EndOffset;
            slot.Name = sample.ShortName;
            slot.SamplePoolId = sample.Id;
            slot.IsSynced = true;
        }
    }
}
