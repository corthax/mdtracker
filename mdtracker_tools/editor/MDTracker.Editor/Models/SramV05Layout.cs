namespace MDTracker.Editor.Models;

public static class SramV05Layout
{
    // Block 1: Header (12 bytes at 0x00-0x0B)
    public const int HeaderSize = 12;

    // Block 1: Static data (fixed addresses)
    public const int StaticBase = 12;
    public const int SramGlobalLfo = 12;       // 3 bytes (logical 12-14)
    public const int SramTempo = 15;            // 2 bytes (logical 15-16)
    public const int StaticEnd = 17;

    // Block 2: Matrix (compact bitmap, at StaticEnd)
    public const int MatrixBlockBase = 17;

    // Block 3-5 are at dynamic positions (matrixBlockEnd, instBlockEnd, seqBlockEnd)

    // Compact record sizes
    public const int InstRecordSize = 58;       // 1 id + 49 data + 8 name
    public const int SeqRecordSize = 65;        // 1 id + 32 vol + 32 arp
    public const int LookupTableSize = 256;

    public const byte InstSentinelModified = 0xFF;

    // Pattern format
    public const int PatternMagic = 0x4D44;
    public const int PatternFormatVersion = 2;
    public const int PatternRegionHeaderSize = 6;
    public const int EvtCount = 7;
    public const int EffectsTotal = 6;

    // Matrix dimensions
    public const int MatrixRows = 250;
    public const int MatrixChannels = 13;
}
