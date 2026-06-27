using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public class TfiParser : IInstrumentFormat
{
    public string Name => "TFM Maker Instrument";
    public string Extension => "tfi";

    public bool CanParse(byte[] data) => data.Length == 42;

    public InstrumentPreset Parse(byte[] data)
    {
        var preset = new InstrumentPreset();
        preset.Algorithm = data[0] & 0x07;
        preset.Feedback = data[1] & 0x07;
        preset.Stereo = 3;
        preset.Ams = 0;
        preset.Fms = 0;

        int[] opRemap = [0, 2, 1, 3];
        for (int op = 0; op < 4; op++)
        {
            int src = opRemap[op];
            int tfiOff = 2 + src * 10;
            var target = preset.Operators[op];
            target.Multiple = data[tfiOff] & 0x0F;
            target.Detune = data[tfiOff + 1] & 0x07;
            target.TotalLevel = data[tfiOff + 2] & 0x7F;
            target.RateScaling = data[tfiOff + 3] & 0x03;
            target.AttackRate = data[tfiOff + 4] & 0x1F;
            target.FirstDecayRate = data[tfiOff + 5] & 0x1F;
            target.SecondaryDecayRate = data[tfiOff + 6] & 0x1F;
            target.ReleaseRate = data[tfiOff + 7] & 0x0F;
            target.SecondaryAmplitude = data[tfiOff + 8] & 0x0F;
            target.SsgEg = data[tfiOff + 9] & 0x0F;
        }
        return preset;
    }
}
