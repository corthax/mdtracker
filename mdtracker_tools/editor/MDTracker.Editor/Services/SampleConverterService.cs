namespace MDTracker.Editor.Services;

public class SampleConverterService
{
    public static readonly int[] PcmRates = [8000, 11025, 13400, 16000, 22050, 32000];

    public WavInfo ParseWav(byte[] data)
    {
        int pos = 12;
        int sampleRate = 22050;
        int channels = 1;
        int bitsPerSample = 8;
        int dataSize = 0;
        int dataOffset = 0;

        while (pos + 8 <= data.Length)
        {
            var chunkId = System.Text.Encoding.ASCII.GetString(data, pos, 4);
            int chunkSize = data[pos + 4] | (data[pos + 5] << 8) | (data[pos + 6] << 16) | (data[pos + 7] << 24);
            if (chunkId == "fmt ")
            {
                channels = data[pos + 10] | (data[pos + 11] << 8);
                sampleRate = data[pos + 12] | (data[pos + 13] << 8) | (data[pos + 14] << 16) | (data[pos + 15] << 24);
                bitsPerSample = data[pos + 22] | (data[pos + 23] << 8);
            }
            else if (chunkId == "data")
            {
                dataOffset = pos + 8;
                dataSize = chunkSize;
            }
            pos += 8 + chunkSize;
        }

        if (dataSize == 0 || dataOffset == 0)
            throw new InvalidOperationException("No data chunk found in WAV file");

        return new WavInfo
        {
            SampleRate = sampleRate,
            Channels = channels,
            BitsPerSample = bitsPerSample,
            DataOffset = dataOffset,
            DataSize = dataSize,
            RawData = data,
        };
    }

    public short[] DecodeSamples(WavInfo wav)
    {
        int totalSamples = wav.DataSize / (wav.BitsPerSample / 8);
        int frames = totalSamples / wav.Channels;
        var mono = new short[frames];

        for (int f = 0; f < frames; f++)
        {
            long sum = 0;
            for (int c = 0; c < wav.Channels; c++)
            {
                int sample = wav.BitsPerSample switch
                {
                    8 => (wav.RawData[wav.DataOffset + (f * wav.Channels + c)] - 128) << 8,
                    16 => (short)(wav.RawData[wav.DataOffset + (f * wav.Channels + c) * 2] |
                                  (wav.RawData[wav.DataOffset + (f * wav.Channels + c) * 2 + 1] << 8)),
                    _ => 0,
                };
                sum += sample;
            }
            mono[f] = (short)(sum / wav.Channels);
        }
        return mono;
    }

    public short[] Resample(short[] input, int inputRate, int outputRate)
    {
        if (inputRate == outputRate) return input;
        int outLen = (int)((long)input.Length * outputRate / inputRate);
        var output = new short[outLen];
        for (int i = 0; i < outLen; i++)
        {
            double srcPos = (double)i * inputRate / outputRate;
            int srcIdx = (int)srcPos;
            double frac = srcPos - srcIdx;
            int nextIdx = Math.Min(srcIdx + 1, input.Length - 1);
            output[i] = (short)(input[srcIdx] * (1 - frac) + input[nextIdx] * frac + 0.5);
        }
        return output;
    }

    public byte[] EncodePcm8(short[] samples)
    {
        var pcm = new byte[samples.Length];
        for (int i = 0; i < samples.Length; i++)
        {
            int s = (samples[i] >> 8) + 128;
            pcm[i] = (byte)Math.Clamp(s, 0, 255);
        }
        return pcm;
    }

    public byte[] EncodeAdpcm(short[] samples)
    {
        int outLen = (samples.Length + 1) / 2;
        var output = new byte[outLen];

        int predictor = 0;
        int stepIndex = 0;

        int[] stepTable =
        [
            7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31,
            34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143,
            157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658,
            724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
            3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
            15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767,
        ];

        int[] stepIndexTable = [-1, -1, -1, -1, 2, 4, 6, 8];

        for (int i = 0; i < samples.Length; i++)
        {
            int diff = samples[i] - predictor;
            int sign = diff < 0 ? 1 : 0;
            int absDiff = diff < 0 ? -diff : diff;
            int step = stepTable[stepIndex];
            int quantized = 0;
            int stepSize = step >> 3;

            if (absDiff >= step) { quantized |= 4; absDiff -= step; }
            if (absDiff >= step >> 1) { quantized |= 2; absDiff -= step >> 1; }
            if (absDiff >= step >> 2) { quantized |= 1; }

            int nibble = (sign << 3) | quantized;

            int delta = (step >> 3);
            if ((quantized & 4) != 0) delta += step;
            if ((quantized & 2) != 0) delta += step >> 1;
            if ((quantized & 1) != 0) delta += step >> 2;
            delta >>= 3;

            if (sign == 1) predictor -= delta;
            else predictor += delta;
            predictor = Math.Clamp(predictor, short.MinValue, short.MaxValue);

            stepIndex += stepIndexTable[quantized];
            stepIndex = Math.Clamp(stepIndex, 0, stepTable.Length - 1);

            if ((i & 1) == 0)
                output[i >> 1] = (byte)(nibble << 4);
            else
                output[i >> 1] |= (byte)nibble;
        }
        return output;
    }

    public byte[] ConvertPcmToAdpcm(byte[] pcmData)
    {
        var samples = new short[pcmData.Length];
        for (int i = 0; i < pcmData.Length; i++)
            samples[i] = (short)((pcmData[i] - 128) << 8);
        return EncodeAdpcm(samples);
    }
}

public class WavInfo
{
    public int SampleRate { get; set; }
    public int Channels { get; set; }
    public int BitsPerSample { get; set; }
    public int DataOffset { get; set; }
    public int DataSize { get; set; }
    public byte[] RawData { get; set; } = [];
}
