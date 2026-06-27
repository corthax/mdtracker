namespace MDTracker.Editor.Models;

public static class RomPresetConversion
{
    public static byte[] ToRomBytes(InstrumentPreset preset)
    {
        var d = new byte[InstrumentPreset.StructSize];
        d[0] = (byte)preset.Algorithm;
        d[1] = (byte)preset.Fms;
        d[2] = (byte)preset.Ams;
        d[3] = (byte)preset.Stereo;
        d[4] = (byte)preset.Feedback;

        for (int op = 0; op < 4; op++)
        {
            var p = preset.Operators[op];
            d[5 + 0 * 4 + op] = (byte)p.TotalLevel;
            d[5 + 1 * 4 + op] = (byte)p.RateScaling;
            d[5 + 2 * 4 + op] = (byte)p.Multiple;
            d[5 + 3 * 4 + op] = (byte)p.Detune;
            d[5 + 4 * 4 + op] = (byte)p.AttackRate;
            d[5 + 5 * 4 + op] = (byte)p.FirstDecayRate;
            d[5 + 6 * 4 + op] = (byte)p.SecondaryAmplitude;
            d[5 + 7 * 4 + op] = (byte)p.SecondaryDecayRate;
            d[5 + 8 * 4 + op] = (byte)p.ReleaseRate;
            d[5 + 9 * 4 + op] = (byte)p.AmplitudeModulation;
            d[5 + 10 * 4 + op] = (byte)p.SsgEg;
        }
        return d;
    }

    public static InstrumentPreset FromRomBytes(byte[] data, int offset = 0)
    {
        var preset = new InstrumentPreset();
        preset.Algorithm = data[offset + 0];
        preset.Fms = data[offset + 1];
        preset.Ams = data[offset + 2];
        preset.Stereo = data[offset + 3];
        preset.Feedback = data[offset + 4];

        for (int op = 0; op < 4; op++)
        {
            var p = preset.Operators[op];
            p.TotalLevel = data[offset + 5 + 0 * 4 + op];
            p.RateScaling = data[offset + 5 + 1 * 4 + op];
            p.Multiple = data[offset + 5 + 2 * 4 + op];
            p.Detune = data[offset + 5 + 3 * 4 + op];
            p.AttackRate = data[offset + 5 + 4 * 4 + op];
            p.FirstDecayRate = data[offset + 5 + 5 * 4 + op];
            p.SecondaryAmplitude = data[offset + 5 + 6 * 4 + op];
            p.SecondaryDecayRate = data[offset + 5 + 7 * 4 + op];
            p.ReleaseRate = data[offset + 5 + 8 * 4 + op];
            p.AmplitudeModulation = data[offset + 5 + 9 * 4 + op];
            p.SsgEg = data[offset + 5 + 10 * 4 + op];
        }
        return preset;
    }
}
