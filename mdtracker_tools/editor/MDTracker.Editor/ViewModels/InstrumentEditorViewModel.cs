using System.Collections.ObjectModel;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MDTracker.Editor.Models;

namespace MDTracker.Editor.ViewModels;

public partial class InstrumentEditorViewModel : ViewModelBase
{
    public static DropdownOption[] AlgorithmOptions { get; } = DropdownHelper.Range(0, 7);
    public static DropdownOption[] FeedbackOptions { get; } = DropdownHelper.Range(0, 7);
    public static DropdownOption[] StereoOptions { get; } = DropdownHelper.Range(0, 3, v => v switch { 0 => "Muted", 1 => "Right", 2 => "Left", 3 => "Center", _ => v.ToString() });
    public static DropdownOption[] AmsOptions { get; } = DropdownHelper.Range(0, 3);
    public static DropdownOption[] FmsOptions { get; } = DropdownHelper.Range(0, 7);
    [ObservableProperty]
    private int _presetIndex;

    [ObservableProperty]
    private int _algorithm;

    [ObservableProperty]
    private int _feedback;

    [ObservableProperty]
    private int _stereo = 3;

    [ObservableProperty]
    private int _ams;

    [ObservableProperty]
    private int _fms;

    public ObservableCollection<OperatorEditorViewModel> Operators { get; } = [];

    public InstrumentEditorViewModel()
    {
        for (int i = 0; i < 4; i++)
            Operators.Add(new OperatorEditorViewModel());
    }

    public void LoadFromPreset(InstrumentPreset preset)
    {
        Algorithm = preset.Algorithm;
        Feedback = preset.Feedback;
        Stereo = preset.Stereo;
        Ams = preset.Ams;
        Fms = preset.Fms;
        for (int i = 0; i < 4; i++)
            Operators[i].LoadFrom(preset.Operators[i]);
    }

    public void SaveToPreset(InstrumentPreset preset)
    {
        preset.Algorithm = Algorithm;
        preset.Feedback = Feedback;
        preset.Stereo = Stereo;
        preset.Ams = Ams;
        preset.Fms = Fms;
        for (int i = 0; i < 4; i++)
            Operators[i].SaveTo(preset.Operators[i]);
    }

    [RelayCommand]
    private void ResetOp(int index)
    {
        if (index >= 0 && index < Operators.Count)
            Operators[index].ResetToDefault();
    }
}

public partial class OperatorEditorViewModel : ViewModelBase
{
    public static DropdownOption[] MultipleOptions { get; } = DropdownHelper.Range(0, 15, v => v == 0 ? "0.5" : v.ToString());
    public static DropdownOption[] DetuneOptions { get; } = DropdownHelper.Range(0, 7);
    public static DropdownOption[] RateScalingOptions { get; } = DropdownHelper.Range(0, 3);
    public static DropdownOption[] AttackRateOptions { get; } = DropdownHelper.Range(0, 31);
    public static DropdownOption[] FirstDecayRateOptions { get; } = DropdownHelper.Range(0, 31);
    public static DropdownOption[] SecondaryDecayRateOptions { get; } = DropdownHelper.Range(0, 31);
    public static DropdownOption[] ReleaseRateOptions { get; } = DropdownHelper.Range(0, 15);
    public static DropdownOption[] SecondaryAmplitudeOptions { get; } = DropdownHelper.Range(0, 15);
    public static DropdownOption[] SsgEgOptions { get; } = DropdownHelper.Range(0, 8, v => v == 0 ? "disabled" : $"Type {v}");
    [ObservableProperty] private int _multiple;
    [ObservableProperty] private int _detune;
    [ObservableProperty] private int _totalLevel;
    [ObservableProperty] private int _rateScaling;
    [ObservableProperty] private int _attackRate;
    [ObservableProperty] private int _firstDecayRate;
    [ObservableProperty] private int _secondaryDecayRate;
    [ObservableProperty] private int _releaseRate;
    [ObservableProperty] private int _secondaryAmplitude;
    [ObservableProperty] private int _amplitudeModulation;

    public bool AmEnabled
    {
        get => AmplitudeModulation != 0;
        set => AmplitudeModulation = value ? 1 : 0;
    }

    partial void OnAmplitudeModulationChanged(int value) => OnPropertyChanged(nameof(AmEnabled));
    [ObservableProperty] private int _ssgEg;

    public void LoadFrom(OperatorParams op)
    {
        Multiple = op.Multiple;
        Detune = op.Detune;
        TotalLevel = op.TotalLevel;
        RateScaling = op.RateScaling;
        AttackRate = op.AttackRate;
        FirstDecayRate = op.FirstDecayRate;
        SecondaryDecayRate = op.SecondaryDecayRate;
        ReleaseRate = op.ReleaseRate;
        SecondaryAmplitude = op.SecondaryAmplitude;
        AmplitudeModulation = op.AmplitudeModulation;
        SsgEg = op.SsgEg;
    }

    public void SaveTo(OperatorParams op)
    {
        op.Multiple = Multiple;
        op.Detune = Detune;
        op.TotalLevel = TotalLevel;
        op.RateScaling = RateScaling;
        op.AttackRate = AttackRate;
        op.FirstDecayRate = FirstDecayRate;
        op.SecondaryDecayRate = SecondaryDecayRate;
        op.ReleaseRate = ReleaseRate;
        op.SecondaryAmplitude = SecondaryAmplitude;
        op.AmplitudeModulation = AmplitudeModulation;
        op.SsgEg = SsgEg;
    }

    public void ResetToDefault()
    {
        Multiple = 1;
        Detune = 3;
        TotalLevel = 127;
        RateScaling = 0;
        AttackRate = 31;
        FirstDecayRate = 0;
        SecondaryDecayRate = 0;
        ReleaseRate = 0;
        SecondaryAmplitude = 15;
        AmplitudeModulation = 0;
        SsgEg = 0;
    }
}
