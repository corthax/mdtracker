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
                byte note = ReadSramByte(sram, rowAddr);
                byte inst = ReadSramByte(sram, rowAddr + 1);
                if ((note < LegacySramLayout.NoteEmpty || inst != 0) && inst < 0xFE)
                    used.Add(inst);
            }
        }
        return used;
    }

    public static byte[] Convert(byte[] sram, long outputSize)
    {
        var used = ScanUsedInstruments(sram);
        // Allocate enough to hold the converted data (v1.05 ≈ v1.04 in logical volume)
        int bufferSize = Math.Max((int)Math.Min(sram.Length, int.MaxValue), (int)outputSize);
        byte[] output = new byte[bufferSize];
        int pos = 0;

        pos = WriteHeader(output, pos);
        pos = WriteStaticArea(output, pos, sram);
        pos = WriteInstrumentBlock(output, pos, used, sram);
        pos = WriteSequencerBlock(output, pos, sram);
        int maxPattern = ScanMatrixMaxPattern(sram);
        pos = WritePatterns(output, pos, sram, maxPattern);

        // Return at least outputSize bytes (zero-padded), never truncate below pos
        int finalSize = Math.Max(pos, (int)outputSize);
        if (finalSize < output.Length)
            Array.Resize(ref output, finalSize);
        return output;
    }

    private static int WriteHeader(byte[] output, int startPos)
    {
        byte[] header = [(byte)'M', (byte)'D', (byte)'T', (byte)'1', (byte)'0', (byte)'5'];
        int pos = startPos;
        foreach (byte c in header)
        {
            output[pos++] = c;
            output[pos++] = c;
        }
        return pos;
    }

    private static int LogicalToPhysical(int logicalPos)
    {
        if (logicalPos >= 2 && (logicalPos & 1) != 0)
            return logicalPos - 2;
        return logicalPos;
    }

    private static void WriteByteAtPos(byte[] output, int logicalPos, byte value)
    {
        output[LogicalToPhysical(logicalPos)] = value;
    }

    private static void WriteByteAt(byte[] output, ref int logicalPos, byte value)
    {
        WriteByteAtPos(output, logicalPos, value);
        logicalPos++;
    }

    private static void WriteWordAt(byte[] output, ref int logicalPos, ushort value)
    {
        WriteByteAtPos(output, logicalPos, (byte)(value >> 8));
        WriteByteAtPos(output, logicalPos + 1, (byte)value);
        logicalPos += 2;
    }

    private static int WriteStaticArea(byte[] output, int startPos, byte[] sram)
    {
        int pos = startPos;

        // LFO: 3 bytes at logical 12-14
        WriteByteAt(output, ref pos, ReadSramByte(sram, (int)LegacySramLayout.SrmLfo));
        WriteByteAt(output, ref pos, 0);
        WriteByteAt(output, ref pos, 0);

        // Tempo: 2 bytes at logical 15-16 (before matrix in v1.05)
        int tempo = ReadSramWord(sram, (int)LegacySramLayout.SrmTempo);
        WriteWordAt(output, ref pos, (ushort)tempo);

        // Matrix: compact bitmap format starting at logical 17
        // Each row: u16 bitmap + u16 per set channel (channel order)
        int matrixSrcBase = (int)LegacySramLayout.SrmPatternMatrix;
        int transposeSrcBase = (int)LegacySramLayout.SrmMatrixTranspose;
        for (int row = 0; row < SramV05Layout.MatrixRows; row++)
        {
            ushort bitmap = 0;
            ushort[] tmp = new ushort[SramV05Layout.MatrixChannels];
            int count = 0;

            for (int ch = 0; ch < SramV05Layout.MatrixChannels; ch++)
            {
                int i = ch * SramV05Layout.MatrixRows + row;
                int patternID = ReadSramWord(sram, matrixSrcBase + i * 2) & 0x3FF;
                if (patternID != 0)
                {
                    bitmap |= (ushort)(1u << ch);
                    int transpose = (sbyte)ReadSramByte(sram, transposeSrcBase + i);
                    tmp[count++] = (ushort)(patternID | ((transpose & 0x3F) << 10));
                }
            }

            WriteWordAt(output, ref pos, bitmap);
            for (int j = 0; j < count; j++)
                WriteWordAt(output, ref pos, tmp[j]);
        }

        return pos;
    }

    private static int WriteInstrumentBlock(byte[] output, int startPos, HashSet<int> used, byte[] sram)
    {
        int pos = startPos;

        var lookup = new byte[256];
        int compactCount = 0;
        for (int i = 0; i < 256; i++)
        {
            if (used.Contains(i))
            {
                lookup[i] = SramV05Layout.InstSentinelModified;
                compactCount++;
            }
        }

        WriteWordAt(output, ref pos, (ushort)compactCount);

        for (int i = 0; i < 256; i++)
            WriteByteAt(output, ref pos, lookup[i]);

        for (int i = 0; i < 256; i++)
        {
            if (used.Contains(i))
            {
                WriteByteAt(output, ref pos, (byte)i);
                int instAddr = (int)LegacySramLayout.SrmInstrumentData + i * LegacySramLayout.InstrumentRecordSize;
                for (int b = 0; b < 49; b++)
                    WriteByteAt(output, ref pos, ReadSramByte(sram, instAddr + b));
                for (int b = 81; b < 89; b++)
                    WriteByteAt(output, ref pos, ReadSramByte(sram, instAddr + b));
            }
        }

        return pos;
    }

    private static int WriteSequencerBlock(byte[] output, int startPos, byte[] sram)
    {
        int pos = startPos;

        var used = new HashSet<int>();
        for (int i = 0; i < 256; i++)
        {
            int volAddr = (int)LegacySramLayout.SrmSeqVolStart + i * 32;
            int arpAddr = (int)LegacySramLayout.SrmSeqArpStart + i * 32;

            bool hasVol = false, hasArp = false;
            for (int j = 0; j < 32; j++)
            {
                if (ReadSramByte(sram, volAddr + j) != 0xFF) hasVol = true;
                if (ReadSramByte(sram, arpAddr + j) != LegacySramLayout.NoteEmpty) hasArp = true;
            }

            if (hasVol || hasArp)
                used.Add(i);
        }

        WriteWordAt(output, ref pos, (ushort)used.Count);
        for (int i = 0; i < 256; i++)
            WriteByteAt(output, ref pos, used.Contains(i) ? SramV05Layout.InstSentinelModified : (byte)0);

        for (int i = 0; i < 256; i++)
        {
            if (used.Contains(i))
            {
                int volAddr = (int)LegacySramLayout.SrmSeqVolStart + i * 32;
                int arpAddr = (int)LegacySramLayout.SrmSeqArpStart + i * 32;

                WriteByteAt(output, ref pos, (byte)i);
                for (int j = 0; j < 32; j++)
                    WriteByteAt(output, ref pos, ReadSramByte(sram, volAddr + j));
                for (int j = 0; j < 32; j++)
                    WriteByteAt(output, ref pos, ReadSramByte(sram, arpAddr + j));
            }
        }

        return pos;
    }

    private static int WritePatterns(byte[] output, int startPos, byte[] sram, int maxPattern)
    {
        int pos = startPos;

        WriteWordAt(output, ref pos, SramV05Layout.PatternMagic);
        WriteWordAt(output, ref pos, SramV05Layout.PatternFormatVersion);
        int regionSizePos = pos;
        WriteWordAt(output, ref pos, 0);

        for (int p = 0; p <= maxPattern; p++)
        {
            var events = new List<byte>();
            int patAddr = (int)LegacySramLayout.SrmPatternData + p * LegacySramLayout.PatternBlockSize;

            for (int row = 0; row < LegacySramLayout.PatternRows; row++)
            {
                int rowAddr = patAddr + row * LegacySramLayout.PatternRowSize;
                byte note = ReadSramByte(sram, rowAddr);
                byte inst = ReadSramByte(sram, rowAddr + 1);

                if (note <= 95 || note == LegacySramLayout.NoteOff || inst != 0)
                {
                    events.Add((byte)(row * SramV05Layout.EvtCount + 0));
                    events.Add(note);
                    events.Add(inst);
                }

                for (int fx = 0; fx < SramV05Layout.EffectsTotal; fx++)
                {
                    int fxOff = 2 + fx * 2;
                    byte fxType = ReadSramByte(sram, rowAddr + fxOff);
                    byte fxVal = ReadSramByte(sram, rowAddr + fxOff + 1);
                    if (fxType != 0 || fxVal != 0)
                    {
                        events.Add((byte)(row * SramV05Layout.EvtCount + 1 + fx));
                        events.Add(fxType);
                        events.Add(fxVal);
                    }
                }
            }

            if (events.Count > 0)
            {
                byte color = p < LegacySramLayout.PatternCount
                    ? (byte)(ReadSramByte(sram, (int)LegacySramLayout.SrmPatternColor + p) & 0x3F)
                    : (byte)0;
                WriteWordAt(output, ref pos, (ushort)(p | (color << 10)));
                WriteWordAt(output, ref pos, (ushort)(events.Count / 3));
                foreach (byte b in events)
                    WriteByteAt(output, ref pos, b);
                if ((events.Count & 1) != 0)
                    WriteByteAt(output, ref pos, 0);
            }
        }

        WriteWordAtPos(output, regionSizePos, (ushort)(pos - startPos));
        return pos;
    }

    private static int ScanMatrixMaxPattern(byte[] sram)
    {
        int matrixBase = (int)LegacySramLayout.SrmPatternMatrix;
        int maxPat = 0;
        int cellCount = SramV05Layout.MatrixRows * SramV05Layout.MatrixChannels;
        for (int i = 0; i < cellCount; i++)
        {
            int pat = ReadSramWord(sram, matrixBase + i * 2) & 0x3FF;
            if (pat > maxPat) maxPat = pat;
        }
        return maxPat;
    }

    private static void WriteWordAtPos(byte[] output, int logicalPos, ushort value)
    {
        WriteByteAtPos(output, logicalPos, (byte)(value >> 8));
        WriteByteAtPos(output, logicalPos + 1, (byte)value);
    }

    private static byte ReadSramByte(byte[] sram, int addr)
    {
        int fixedAddr = (int)LegacySramLayout.WordSwapOffset((uint)addr);
        return (uint)fixedAddr < (uint)sram.Length ? sram[fixedAddr] : (byte)0;
    }

    private static int ReadSramWord(byte[] sram, int addr)
    {
        return (ReadSramByte(sram, addr) << 8) | ReadSramByte(sram, addr + 1);
    }
}
