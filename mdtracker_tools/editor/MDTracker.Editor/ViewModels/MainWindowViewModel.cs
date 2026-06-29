using System;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class MainWindowViewModel : ViewModelBase
{
    private readonly SettingsService _settingsService = new();
    private readonly RomService _romService;

    public SampleBankViewModel SampleBankView { get; }
    public SamplePoolViewModel SamplePoolView { get; }
    public InstrumentEditorViewModel InstrumentEditorView { get; }
    public PresetPoolViewModel PresetPoolView { get; }
    public SampleConverterViewModel SampleConverterView { get; }
    public SaveConverterViewModel SaveConverterView { get; }
    public SettingsViewModel SettingsView { get; }

    [ObservableProperty]
    private string _romPath = string.Empty;

    [ObservableProperty]
    private bool _isRomLoaded;

    public event EventHandler<string?>? OpenRomRequested;
    public event EventHandler? SaveRomRequested;
    public event EventHandler? AddSamplesRequested;
    public event EventHandler? AddPresetsRequested;


    public MainWindowViewModel()
    {
        _romService = new RomService(_settingsService);
        SamplePoolView = new SamplePoolViewModel(_romService);
        SampleBankView = new SampleBankViewModel(_romService, SamplePoolView);
        InstrumentEditorView = new InstrumentEditorViewModel();
        PresetPoolView = new PresetPoolViewModel();
        SampleConverterView = new SampleConverterViewModel();
        SaveConverterView = new SaveConverterViewModel();
        SettingsView = new SettingsViewModel(_settingsService);
    }

    public void LoadRom(string path)
    {
        try
        {
            _romService.Load(path);
            RomPath = path;
            IsRomLoaded = true;
            SampleBankView.LoadFromRom();
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Failed to load ROM: {ex.Message}");
        }
    }

    public void SaveRomToPath(string? path = null)
    {
        if (!_romService.IsLoaded) return;
        _romService.WriteSampleBank(SamplePoolView, SampleBankView.Banks);
        _romService.Save(path);
    }

    [RelayCommand]
    private void OpenRom() => OpenRomRequested?.Invoke(this, null);

    [RelayCommand]
    private void SaveRom() => SaveRomRequested?.Invoke(this, EventArgs.Empty);

    [RelayCommand]
    private void AddSamples() => AddSamplesRequested?.Invoke(this, EventArgs.Empty);

    [RelayCommand]
    private void AddPresets() => AddPresetsRequested?.Invoke(this, EventArgs.Empty);

}
