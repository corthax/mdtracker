using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Models;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SampleBankViewModel : ViewModelBase
{
    private readonly RomService _romService;

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
    private int _rangeEnd = 95;

    [ObservableProperty]
    private int _rangePan = 192;

    [ObservableProperty]
    private int _rangeRate;

    public DropdownOption? RangePanOption
    {
        get => SampleSlot.PanOptions.FirstOrDefault(o => o.Value == RangePan);
        set { if (value is not null) RangePan = value.Value; }
    }

    public DropdownOption? RangeRateOption
    {
        get => SampleSlot.RateOptions.FirstOrDefault(o => o.Value == RangeRate);
        set { if (value is not null) RangeRate = value.Value; }
    }

    partial void OnRangePanChanged(int value) => OnPropertyChanged(nameof(RangePanOption));
    partial void OnRangeRateChanged(int value) => OnPropertyChanged(nameof(RangeRateOption));

    [ObservableProperty]
    private bool _rangeLoop;

    public SampleBankViewModel(RomService romService)
    {
        _romService = romService;
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
        var active = GetBank(SelectedBankIndex);
        int start = Math.Clamp(RangeStart, 0, 95);
        int end = Math.Clamp(RangeEnd, 0, 95);
        if (start > end) (start, end) = (end, start);

        for (int i = start; i <= end; i++)
        {
            active[i].Pan = RangePan;
            active[i].Rate = RangeRate;
            active[i].Looped = RangeLoop;
        }
    }
}
