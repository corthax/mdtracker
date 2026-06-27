using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Models;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class PresetPoolViewModel : ViewModelBase
{
    private readonly InstrumentFormatService _formatService;

    public ObservableCollection<InstrumentPreset> Presets { get; } = [];

    [ObservableProperty]
    private InstrumentPreset? _selectedPreset;

    public string FilterString => _formatService.FilterString;

    public PresetPoolViewModel()
    {
        _formatService = new InstrumentFormatService();
        _formatService.Register(new VgiParser());
        _formatService.Register(new TfiParser());
    }

    [RelayCommand]
    private void RemovePreset()
    {
        if (SelectedPreset != null)
            Presets.Remove(SelectedPreset);
    }

    public InstrumentPreset? TryParse(byte[] data)
    {
        return _formatService.Parse(data);
    }

    public void AddPreset(InstrumentPreset preset)
    {
        preset.Id = Presets.Count;
        Presets.Add(preset);
    }
}
