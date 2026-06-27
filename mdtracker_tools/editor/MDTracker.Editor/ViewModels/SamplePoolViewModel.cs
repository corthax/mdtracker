using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Models;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SamplePoolViewModel : ViewModelBase
{
    private readonly RomService _romService;

    public ObservableCollection<SampleFile> Samples { get; } = [];

    [ObservableProperty]
    private SampleFile? _selectedSample;

    [ObservableProperty]
    private int _totalSize;

    public SamplePoolViewModel(RomService romService)
    {
        _romService = romService;
    }

    [RelayCommand]
    private void RemoveSample()
    {
        if (SelectedSample == null) return;
        Samples.Remove(SelectedSample);
        RecalculateSize();
    }

    [RelayCommand]
    private void ClearSamples()
    {
        Samples.Clear();
        TotalSize = 0;
    }

    public void AddSample(SampleFile sample)
    {
        sample.Id = Samples.Count;
        Samples.Add(sample);
        RecalculateSize();
    }

    private void RecalculateSize()
    {
        TotalSize = Samples.Sum(s => s.AlignedSize);
    }
}
