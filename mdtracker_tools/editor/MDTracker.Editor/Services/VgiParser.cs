using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public class VgiParser : IInstrumentFormat
{
    public string Name => "VGM Maker Instrument";
    public string Extension => "vgi";

    public bool CanParse(byte[] data) => data.Length == 43;

    public InstrumentPreset Parse(byte[] data)
    {
        var preset = new InstrumentPreset();
        preset.Algorithm = data[0] & 0x07;
        preset.Feedback = data[1] & 0x07;

        int b4 = data[2];
        preset.Stereo = (b4 >> 6) & 0x03;
        preset.Ams = (b4 >> 4) & 0x03;
        preset.Fms = b4 & 0x07;

        int[] opRemap = [0, 2, 1, 3];
        for (int op = 0; op < 4; op++)
        {
            int src = opRemap[op];
            int vgiOff = 3 + src * 10;
            var target = preset.Operators[op];
            target.Multiple = data[vgiOff] & 0x0F;
            target.Detune = data[vgiOff + 1] & 0x07;
            target.TotalLevel = data[vgiOff + 2] & 0x7F;
            target.RateScaling = data[vgiOff + 3] & 0x03;
            target.AttackRate = data[vgiOff + 4] & 0x1F;
            target.FirstDecayRate = data[vgiOff + 5] & 0x1F;
            target.SecondaryDecayRate = data[vgiOff + 6] & 0x1F;
            target.ReleaseRate = data[vgiOff + 7] & 0x0F;
            target.SecondaryAmplitude = data[vgiOff + 8] & 0x0F;
            target.SsgEg = data[vgiOff + 9] & 0x0F;
        }
        return preset;
    }
}
