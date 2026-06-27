using CommunityToolkit.Mvvm.ComponentModel;

namespace MDTracker.Editor.Models;

public partial class InstrumentPreset : ObservableObject
{
    public const int StructSize = 49;
    public const int NameSize = 8;
    public const int OperatorCount = 4;

    [ObservableProperty] private int _id;
    [ObservableProperty] private int _algorithm;
    [ObservableProperty] private int _feedback;
    [ObservableProperty] private int _stereo;
    [ObservableProperty] private int _ams;
    [ObservableProperty] private int _fms;
    [ObservableProperty] private string _name = string.Empty;

    public OperatorParams[] Operators { get; } = new OperatorParams[OperatorCount];

    public InstrumentPreset()
    {
        for (int i = 0; i < OperatorCount; i++)
            Operators[i] = new OperatorParams();
    }

    public byte[] Serialize()
    {
        var data = new byte[StructSize];
        data[0] = (byte)Algorithm;
        data[1] = (byte)Feedback;
        data[2] = (byte)Stereo;
        data[3] = (byte)Ams;
        data[4] = (byte)Fms;

        for (int op = 0; op < OperatorCount; op++)
        {
            int baseOff = 5 + op * 11;
            data[baseOff + 0] = (byte)Operators[op].Multiple;
            data[baseOff + 1] = (byte)Operators[op].Detune;
            data[baseOff + 2] = (byte)Operators[op].AttackRate;
            data[baseOff + 3] = (byte)Operators[op].RateScaling;
            data[baseOff + 4] = (byte)Operators[op].FirstDecayRate;
            data[baseOff + 5] = (byte)Operators[op].AmplitudeModulation;
            data[baseOff + 6] = (byte)Operators[op].SecondaryAmplitude;
            data[baseOff + 7] = (byte)Operators[op].SecondaryDecayRate;
            data[baseOff + 8] = (byte)Operators[op].ReleaseRate;
            data[baseOff + 9] = (byte)Operators[op].TotalLevel;
            data[baseOff + 10] = (byte)Operators[op].SsgEg;
        }
        return data;
    }

    public static InstrumentPreset Deserialize(byte[] data, int offset = 0)
    {
        var preset = new InstrumentPreset();
        preset.Algorithm = data[offset + 0];
        preset.Feedback = data[offset + 1];
        preset.Stereo = data[offset + 2];
        preset.Ams = data[offset + 3];
        preset.Fms = data[offset + 4];

        for (int op = 0; op < OperatorCount; op++)
        {
            int baseOff = offset + 5 + op * 11;
            preset.Operators[op].Multiple = data[baseOff + 0];
            preset.Operators[op].Detune = data[baseOff + 1];
            preset.Operators[op].AttackRate = data[baseOff + 2];
            preset.Operators[op].RateScaling = data[baseOff + 3];
            preset.Operators[op].FirstDecayRate = data[baseOff + 4];
            preset.Operators[op].AmplitudeModulation = data[baseOff + 5];
            preset.Operators[op].SecondaryAmplitude = data[baseOff + 6];
            preset.Operators[op].SecondaryDecayRate = data[baseOff + 7];
            preset.Operators[op].ReleaseRate = data[baseOff + 8];
            preset.Operators[op].TotalLevel = data[baseOff + 9];
            preset.Operators[op].SsgEg = data[baseOff + 10];
        }
        return preset;
    }

    public bool Matches(InstrumentPreset other)
    {
        if (Algorithm != other.Algorithm) return false;
        if (Feedback != other.Feedback) return false;
        if (Stereo != other.Stereo) return false;
        if (Ams != other.Ams) return false;
        if (Fms != other.Fms) return false;
        for (int op = 0; op < OperatorCount; op++)
        {
            if (Operators[op].Multiple != other.Operators[op].Multiple) return false;
            if (Operators[op].Detune != other.Operators[op].Detune) return false;
            if (Operators[op].AttackRate != other.Operators[op].AttackRate) return false;
            if (Operators[op].RateScaling != other.Operators[op].RateScaling) return false;
            if (Operators[op].FirstDecayRate != other.Operators[op].FirstDecayRate) return false;
            if (Operators[op].AmplitudeModulation != other.Operators[op].AmplitudeModulation) return false;
            if (Operators[op].SecondaryAmplitude != other.Operators[op].SecondaryAmplitude) return false;
            if (Operators[op].SecondaryDecayRate != other.Operators[op].SecondaryDecayRate) return false;
            if (Operators[op].ReleaseRate != other.Operators[op].ReleaseRate) return false;
            if (Operators[op].TotalLevel != other.Operators[op].TotalLevel) return false;
            if (Operators[op].SsgEg != other.Operators[op].SsgEg) return false;
        }
        return true;
    }
}

public partial class OperatorParams : ObservableObject
{
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
    [ObservableProperty] private int _ssgEg;
}
