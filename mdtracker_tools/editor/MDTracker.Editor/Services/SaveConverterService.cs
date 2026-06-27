using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public class SaveConverterService
{
    public static readonly long[] OutputSizes = [65536, 131072, 262144, 524288];

    public static HashSet<int> ScanUsedInstruments(byte[] sram)
    {
        var used = new HashSet<int>();
        for (int p = 0; p < LegacySramLayout.PatternCount; p++)
        {
            int patAddr = (int)LegacySramLayout.SrmPatternData + p * LegacySramLayout.PatternBlockSize;
            for (int r = 0; r < LegacySramLayout.PatternRows; r++)
            {
                int rowAddr = patAddr + r * LegacySramLayout.PatternRowSize;
                if ((uint)(rowAddr + 1) >= (uint)sram.Length) break;
                byte inst = ReadSramByte(sram, rowAddr + 1);
                if (inst < 0xFE) used.Add(inst);
            }
        }
        return used;
    }

    public static byte[] Convert(byte[] sram, long outputSize)
    {
        var used = ScanUsedInstruments(sram);
        var outStream = new MemoryStream();

        WriteHeader(outStream);
        WriteStaticArea(outStream, sram);
        var instEnd = WriteInstrumentBlock(outStream, used, sram);
        var seqEnd = WriteSequencerBlock(outStream, instEnd, sram);
        WritePatterns(outStream, seqEnd, sram);
        PadToSize(outStream, outputSize);

        return outStream.ToArray();
    }

    private static void WriteHeader(MemoryStream ms)
    {
        ms.WriteByte((byte)'M');
        ms.WriteByte((byte)'D');
        ms.WriteByte((byte)'T');
        ms.WriteByte((byte)'1');
        ms.WriteByte((byte)'0');
        ms.WriteByte((byte)'5');
        for (int i = 0; i < 6; i++) ms.WriteByte(0);
    }

    private static void WriteStaticArea(MemoryStream ms, byte[] sram)
    {
        ms.WriteByte(0);

        var matrix = new byte[LegacySramLayout.MatrixRows * LegacySramLayout.MatrixChannels];
        for (int i = 0; i < matrix.Length; i++)
            matrix[i] = ReadSramByte(sram, (int)(SramV05Layout.SramPatternMatrix + i));
        ms.Write(matrix);

        int tempo = ReadSramWord(sram, (int)LegacySramLayout.SrmTempo);
        ms.WriteByte((byte)(tempo & 0xFF));
        ms.WriteByte((byte)((tempo >> 8) & 0xFF));

        int tempoEnd = (int)(SramV05Layout.SramTempo + 2);
        int colorLen = (int)(SramV05Layout.StaticEnd - tempoEnd);
        for (int i = 0; i < colorLen; i++)
        {
            int srcAddr = (int)LegacySramLayout.SrmPatternColor + i;
            ms.WriteByte(srcAddr < sram.Length ? ReadSramByte(sram, srcAddr) : (byte)0);
        }
    }

    private static int WriteInstrumentBlock(MemoryStream ms, HashSet<int> used, byte[] sram)
    {
        int start = (int)SramV05Layout.InstBlockBase;
        ms.Position = start;

        var lookup = new byte[256];
        int compactCount = 0;

        for (int i = 0; i < 256; i++)
        {
            if (used.Contains(i))
            {
                lookup[i] = (byte)(compactCount + 1);
                compactCount++;
            }
        }

        ms.WriteByte((byte)(compactCount & 0xFF));
        ms.WriteByte((byte)((compactCount >> 8) & 0xFF));
        ms.Write(lookup);

        int compactStart = start + 2 + 256;
        ms.Position = compactStart;

        for (int i = 0; i < 256; i++)
        {
            if (used.Contains(i))
            {
                ms.WriteByte((byte)i);
                int instAddr = (int)LegacySramLayout.SrmInstrumentData + i * LegacySramLayout.InstrumentRecordSize;
                for (int b = 0; b < 49; b++)
                    ms.WriteByte(ReadSramByte(sram, instAddr + b));
                for (int b = 81; b < 89; b++)
                    ms.WriteByte(ReadSramByte(sram, instAddr + b));
            }
        }

        return (int)ms.Position;
    }

    private static int WriteSequencerBlock(MemoryStream ms, int instEnd, byte[] sram)
    {
        ms.Position = instEnd;

        for (int i = 0; i < 256; i++)
        {
            int volAddr = (int)LegacySramLayout.SrmSeqVolStart + i * 32;
            int arpAddr = (int)LegacySramLayout.SrmSeqArpStart + i * 32;

            bool hasVol = false, hasArp = false;
            for (int j = 0; j < 32; j++)
            {
                if (ReadSramByte(sram, volAddr + j) != 0xFF) hasVol = true;
                if (ReadSramByte(sram, arpAddr + j) != 0) hasArp = true;
            }

            if (hasVol || hasArp)
            {
                ms.WriteByte((byte)i);
                for (int j = 0; j < 32; j++)
                    ms.WriteByte(ReadSramByte(sram, volAddr + j));
                for (int j = 0; j < 32; j++)
                    ms.WriteByte(ReadSramByte(sram, arpAddr + j));
            }
        }

        return (int)ms.Position;
    }

    private static void WritePatterns(MemoryStream ms, int seqEnd, byte[] sram)
    {
        ms.Position = seqEnd;

        for (int p = 0; p < LegacySramLayout.PatternCount; p++)
        {
            int patAddr = (int)LegacySramLayout.SrmPatternData + p * LegacySramLayout.PatternBlockSize;
            long startPos = ms.Position;

            ms.WriteByte((byte)(SramV05Layout.PatternMagic & 0xFF));
            ms.WriteByte((byte)((SramV05Layout.PatternMagic >> 8) & 0xFF));
            ms.WriteByte((byte)(SramV05Layout.PatternFormatVersion & 0xFF));
            ms.WriteByte((byte)((SramV05Layout.PatternFormatVersion >> 8) & 0xFF));
            ms.WriteByte(32);
            ms.WriteByte(0);
            ms.WriteByte(0);
            ms.WriteByte(0);
            ms.WriteByte(0);
            ms.WriteByte(0);

            long dataStart = ms.Position;

            for (int r = 0; r < 32; r++)
            {
                int rowAddr = patAddr + r * LegacySramLayout.PatternRowSize;
                byte note = ReadSramByte(sram, rowAddr);

                if (note == LegacySramLayout.NoteEmpty)
                {
                    ms.WriteByte(6);
                    continue;
                }

                if (note == LegacySramLayout.NoteOff)
                {
                    ms.WriteByte(2);
                    ms.WriteByte(0xFF);
                }
                else
                {
                    ms.WriteByte(1);
                    ms.WriteByte(note);
                    ms.WriteByte(ReadSramByte(sram, rowAddr + 1));
                }

                byte vol = ReadSramByte(sram, rowAddr + 2);
                if (vol != 0xFF)
                {
                    ms.WriteByte(3);
                    ms.WriteByte(vol);
                }

                for (int fx = 0; fx < 4; fx++)
                {
                    int fxOff = 3 + fx * 3;
                    int fxLen = fx < 3 ? 3 : 2;
                    byte fxType = ReadSramByte(sram, rowAddr + fxOff);
                    if (fxType != 0)
                    {
                        ms.WriteByte(4);
                        ms.WriteByte(fxType);
                        ms.WriteByte(ReadSramByte(sram, rowAddr + fxOff + 1));
                        ms.WriteByte(fxLen >= 3 ? ReadSramByte(sram, rowAddr + fxOff + 2) : (byte)0);
                    }
                }

                ms.WriteByte(6);
            }

            long dataEnd = ms.Position;
            int compressedSize = (int)(dataEnd - dataStart);

            ms.Position = startPos + 4;
            ms.WriteByte(32);
            ms.WriteByte(0);
            ms.WriteByte((byte)(compressedSize & 0xFF));
            ms.WriteByte((byte)((compressedSize >> 8) & 0xFF));
            ms.WriteByte((byte)(compressedSize & 0xFF));
            ms.WriteByte((byte)((compressedSize >> 8) & 0xFF));
            ms.Position = dataEnd;
        }
    }

    private static void PadToSize(MemoryStream ms, long size)
    {
        if (size > ms.Length)
        {
            ms.SetLength(size);
            ms.Position = size;
        }
    }

    private static byte ReadSramByte(byte[] sram, int addr)
    {
        int fixedAddr = (int)LegacySramLayout.WordSwapOffset((uint)addr);
        return (uint)fixedAddr < (uint)sram.Length ? sram[fixedAddr] : (byte)0;
    }

    private static int ReadSramWord(byte[] sram, int addr)
    {
        return ReadSramByte(sram, addr) | (ReadSramByte(sram, addr + 1) << 8);
    }
}
