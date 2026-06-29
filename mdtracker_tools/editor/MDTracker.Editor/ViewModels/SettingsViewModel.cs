using CommunityToolkit.Mvvm.ComponentModel;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SettingsViewModel : ViewModelBase
{
    private readonly SettingsService _settingsService;

    [ObservableProperty]
    private int _sampleSettingsAddr;

    [ObservableProperty]
    private int _sampleBankAddr;

    public SettingsViewModel(SettingsService settingsService)
    {
        _settingsService = settingsService;
        SampleSettingsAddr = _settingsService.SampleSettingsAddr;
        SampleBankAddr = _settingsService.SampleBankAddr;
    }

    partial void OnSampleSettingsAddrChanged(int value)
    {
        _settingsService.Settings.SampleSettingsAddr = value;
        _settingsService.Save();
    }

    partial void OnSampleBankAddrChanged(int value)
    {
        _settingsService.Settings.SampleBankAddr = value;
        _settingsService.Save();
    }

    public void SaveNow()
    {
        _settingsService.Settings.SampleSettingsAddr = SampleSettingsAddr;
        _settingsService.Settings.SampleBankAddr = SampleBankAddr;
        _settingsService.Save();
    }
}
