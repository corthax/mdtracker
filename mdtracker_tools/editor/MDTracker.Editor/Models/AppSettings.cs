namespace MDTracker.Editor.Models;

public class AppSettings
{
    public string RomPath { get; set; } = string.Empty;
    public string SamplesPath { get; set; } = string.Empty;
    public string PresetsPath { get; set; } = string.Empty;
    public int SampleSettingsAddr { get; set; } = 0x0003E500;
    public int SampleBankAddr { get; set; } = 0x00040F00;
}
