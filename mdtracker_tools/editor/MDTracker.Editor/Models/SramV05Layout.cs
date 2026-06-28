namespace MDTracker.Editor.Models;

public static class SramV05Layout
{
    public const int HeaderSize = 12;

    public const int StaticBase = 12;
    public const int SramGlobalLfo = 12;
    public const int SramPatternMatrix = 15;
    public const int SramTempo = 6515;
    public const int StaticEnd = 6517;

    public const int InstBlockBase = StaticEnd;
    public const int InstModCountAddr = StaticEnd;
    public const int InstLookupTableAddr = StaticEnd + 2;
    public const int InstLookupTableSize = 256;
    public const int InstCompactStart = StaticEnd + 2 + 256;
    public const int InstRecordSize = 58;

    public const int SeqRecordSize = 65;

    public const int PatternMagic = 0x4D44;
    public const int PatternFormatVersion = 2;
    public const int PatternRegionHeaderSize = 6;
    public const int EvtCount = 7;
    public const int EffectsTotal = 6;

    public const int MatrixRows = 250;
    public const int MatrixChannels = 13;

    public const byte InstSentinelModified = 0xFF;
}
