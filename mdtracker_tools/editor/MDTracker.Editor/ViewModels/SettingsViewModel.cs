using System.Globalization;
using CommunityToolkit.Mvvm.ComponentModel;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SettingsViewModel : ViewModelBase
{
    private readonly SettingsService _settingsService;

    [ObservableProperty]
    private string _sampleSettingsAddrHex = "0x00000000";

    [ObservableProperty]
    private string _sampleBankAddrHex = "0x00000000";

    public SettingsViewModel(SettingsService settingsService)
    {
        _settingsService = settingsService;
        _sampleSettingsAddrHex = $"0x{_settingsService.SampleSettingsAddr:X8}";
        _sampleBankAddrHex = $"0x{_settingsService.SampleBankAddr:X8}";
    }

    public void SaveSettings(string settingsHex, string bankHex)
    {
        bool changed = false;
        if (TryParseHex(settingsHex, out var addr))
        {
            _settingsService.Settings.SampleSettingsAddr = addr;
            SampleSettingsAddrHex = $"0x{addr:X8}";
            changed = true;
        }
        if (TryParseHex(bankHex, out var bank))
        {
            _settingsService.Settings.SampleBankAddr = bank;
            SampleBankAddrHex = $"0x{bank:X8}";
            changed = true;
        }
        if (changed)
            _settingsService.Save();
    }

    private static bool TryParseHex(string? s, out int value)
    {
        value = 0;
        if (string.IsNullOrEmpty(s)) return false;
        s = s.Trim();
        if (s.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            s = s[2..];
        return int.TryParse(s, NumberStyles.HexNumber, null, out value);
    }

    public void SaveNow()
    {
        _settingsService.Save();
    }
}
