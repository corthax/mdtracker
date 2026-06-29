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

    private readonly List<SampleFile> _selectedSamples = [];
    public IReadOnlyList<SampleFile> SelectedSamples => _selectedSamples;

    [ObservableProperty]
    private SampleFile? _selectedSample;

    [ObservableProperty]
    private int _totalSize;

    public SamplePoolViewModel(RomService romService)
    {
        _romService = romService;
    }

    public void UpdateSelection(IEnumerable<SampleFile> selected)
    {
        _selectedSamples.Clear();
        _selectedSamples.AddRange(selected);
        SelectedSample = selected.FirstOrDefault();
    }

    [RelayCommand]
    private void RemoveSample()
    {
        if (SelectedSample == null) return;
        Samples.Remove(SelectedSample);
        _selectedSamples.Remove(SelectedSample);
        RecalculateOffsets();
        RecalculateSize();
    }

    [RelayCommand]
    private void ClearSamples()
    {
        Samples.Clear();
        _selectedSamples.Clear();
        SelectedSample = null;
        TotalSize = 0;
    }

    public void AddSample(SampleFile sample)
    {
        sample.Id = Samples.Count;
        Samples.Add(sample);
        RecalculateOffsets();
        RecalculateSize();
    }

    private void RecalculateOffsets()
    {
        int offset = 0;
        foreach (var s in Samples)
        {
            s.StartOffset = offset;
            s.EndOffset = offset + s.AlignedSize;
            offset += s.AlignedSize;
        }
    }

    private void RecalculateSize()
    {
        TotalSize = Samples.Sum(s => s.AlignedSize);
    }
}
