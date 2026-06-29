using System.Text.Json;
using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public class SettingsService
{
    private readonly string _filePath;

    public AppSettings Settings { get; private set; } = new();

    public SettingsService()
    {
        var appDir = AppDomain.CurrentDomain.BaseDirectory;
        _filePath = Path.Combine(appDir, "settings.json");
        Load();
    }

    public void Load()
    {
        try
        {
            if (File.Exists(_filePath))
            {
                var json = File.ReadAllText(_filePath);
                Settings = JsonSerializer.Deserialize<AppSettings>(json) ?? new AppSettings();
            }
        }
        catch
        {
            Settings = new AppSettings();
        }
    }

    public void Save()
    {
        try
        {
            var json = JsonSerializer.Serialize(Settings, new JsonSerializerOptions { WriteIndented = true });
            File.WriteAllText(_filePath, json);
        }
        catch
        {
            // Silently fail on settings save
        }
    }

    public int SampleSettingsAddr => Settings.SampleSettingsAddr;
    public int SampleBankAddr => Settings.SampleBankAddr;
}
