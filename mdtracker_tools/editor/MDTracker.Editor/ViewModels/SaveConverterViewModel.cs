using System;
using System.IO;
using System.Linq;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SaveConverterViewModel : ViewModelBase
{
    private byte[]? _sramData;

    [ObservableProperty]
    private string _sourcePath = string.Empty;

    [ObservableProperty]
    private int _usedInstrumentCount;

    [ObservableProperty]
    private int _outputSizeIndex = 3;

    [ObservableProperty]
    private string _outputPath = string.Empty;

    [ObservableProperty]
    private string _status = string.Empty;

    public string[] OutputSizeLabels => SaveConverterService.OutputSizes.Select(s => $"{s / 1024} KB").ToArray();

    public event EventHandler? OpenSourceRequested;
    public event EventHandler? SaveOutputRequested;

    [RelayCommand]
    private void OpenSource() => OpenSourceRequested?.Invoke(this, EventArgs.Empty);

    [RelayCommand]
    private void SaveOutput() => SaveOutputRequested?.Invoke(this, EventArgs.Empty);

    public void LoadSource(string path)
    {
        try
        {
            var sram = File.ReadAllBytes(path);
            SourcePath = path;
            _sramData = sram;

            var used = SaveConverterService.ScanUsedInstruments(sram);
            UsedInstrumentCount = used.Count;
            Status = $"Scanned: {used.Count} instruments used, {sram.Length:N0} bytes";
        }
        catch (Exception ex)
        {
            Status = $"Error: {ex.Message}";
        }
    }

    public bool ConvertAndSave(string outputPath)
    {
        try
        {
            if (_sramData == null) { Status = "No source loaded"; return false; }

            long size = SaveConverterService.OutputSizes[OutputSizeIndex];
            var result = SaveConverterService.Convert(_sramData, size);
            File.WriteAllBytes(outputPath, result);
            OutputPath = outputPath;
            Status = $"Saved: {result.Length:N0} bytes → {outputPath}";
            return true;
        }
        catch (Exception ex)
        {
            Status = $"Conversion failed: {ex.Message}";
            return false;
        }
    }
}
