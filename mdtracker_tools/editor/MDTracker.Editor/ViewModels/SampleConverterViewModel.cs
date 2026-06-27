using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Models;
using MDTracker.Editor.Services;

namespace MDTracker.Editor.ViewModels;

public partial class SampleConverterViewModel : ViewModelBase
{
    private readonly SampleConverterService _converter = new();

    public ObservableCollection<SourceSample> SourceSamples { get; } = [];

    [ObservableProperty]
    private SourceSample? _selectedSource;

    [ObservableProperty]
    private int _targetRateIndex = 3;

    [ObservableProperty]
    private int _outputFormatIndex;

    [ObservableProperty]
    private long _estimatedSize;

    public int[] TargetRates => SampleConverterService.PcmRates;

    public string[] OutputFormats => ["8-bit PCM", "4-bit ADPCM (22kHz)"];

    partial void OnTargetRateIndexChanged(int value) => RecalcSize();
    partial void OnOutputFormatIndexChanged(int value) => RecalcSize();

    [RelayCommand]
    private void AddSource()
    {
    }

    [RelayCommand]
    private void RemoveSource()
    {
        if (SelectedSource != null)
            SourceSamples.Remove(SelectedSource);
    }

    public void AddWav(string name, byte[] data)
    {
        try
        {
            var wav = _converter.ParseWav(data);
            var samples = _converter.DecodeSamples(wav);
            SourceSamples.Add(new SourceSample
            {
                Name = name,
                InputRate = wav.SampleRate,
                InputChannels = wav.Channels,
                InputBits = wav.BitsPerSample,
                InputFrames = samples.Length,
                DecodedSamples = samples,
            });
        }
        catch
        {
        }
    }

    [RelayCommand]
    private void ConvertSelected()
    {
        if (SelectedSource == null) return;
        ConvertSource(SelectedSource);
    }

    [RelayCommand]
    private void ConvertAll()
    {
        foreach (var src in SourceSamples)
            ConvertSource(src);
    }

    private void ConvertSource(SourceSample src)
    {
        int targetRate = SampleConverterService.PcmRates[TargetRateIndex];
        var resampled = _converter.Resample(src.DecodedSamples, src.InputRate, targetRate);

        if (OutputFormatIndex == 0)
        {
            src.OutputData = _converter.EncodePcm8(resampled);
            src.OutputFormat = $"PCM {targetRate}Hz";
        }
        else
        {
            src.OutputData = _converter.EncodeAdpcm(resampled);
            src.OutputFormat = $"ADPCM 22050Hz";
        }
        src.OutputSize = src.OutputData.Length;
        RecalcSize();
    }

    public bool TryGetOutputData(string name, out byte[]? data, out string format)
    {
        foreach (var s in SourceSamples)
        {
            if (s.Name == name && s.OutputData != null)
            {
                data = s.OutputData;
                format = s.OutputFormat;
                return true;
            }
        }
        data = null;
        format = "";
        return false;
    }

    private void RecalcSize()
    {
        EstimatedSize = SourceSamples.Sum(s => s.OutputData?.Length ?? 0L);
    }
}

public partial class SourceSample : ObservableObject
{
    [ObservableProperty] private string _name = "";
    [ObservableProperty] private int _inputRate;
    [ObservableProperty] private int _inputChannels;
    [ObservableProperty] private int _inputBits;
    [ObservableProperty] private int _inputFrames;
    [ObservableProperty] private string _outputFormat = "";
    [ObservableProperty] private int _outputSize;
    public short[] DecodedSamples { get; set; } = [];
    public byte[]? OutputData { get; set; }

    public string InputInfo => $"{InputRate}Hz {InputChannels}ch {InputBits}bit";
    public string OutputInfo => OutputData != null ? $"{OutputFormat} ({OutputSize} bytes)" : "Not converted";
}
