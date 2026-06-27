namespace MDTracker.Editor.Models;

public static class LegacySramLayout
{
    public const int FileSize = 524288;

    public const uint SrmSampleData = 0x0726B;
    public const uint SrmSamplePan = 0x6B0AB;
    public const uint SrmSampleRate = 0x6F230;
    public const uint SrmInstrumentData = 0x00002;
    public const uint SrmPatternMatrix = 0x05905;
    public const uint SrmTempo = 0x07269;
    public const uint SrmPatternData = 0x07CEB;
    public const uint SrmPatternColor = 0x6A06B;
    public const uint SrmMatrixTranspose = 0x6A3EC;
    public const uint SrmMuteChannel = 0x6B09E;
    public const uint SrmSongTranspose = 0x6B22B;
    public const uint SrmSeqVolStart = 0x6B230;
    public const uint SrmSeqArpStart = 0x6D230;

    public const int SampleDataEntrySize = 7;
    public const int PatternRowSize = 14;
    public const int PatternRows = 32;
    public const int PatternBlockSize = PatternRowSize * PatternRows;
    public const int PatternCount = 896;
    public const int MatrixRows = 250;
    public const int MatrixChannels = 13;
    public const int InstrumentRecordSize = 89;
    public const int InstrumentCount = 256;

    public const byte NoteEmpty = 0xFE;
    public const byte NoteOff = 0xFF;

    public static uint WordSwapOffset(uint offset)
    {
        if (offset < 2) return 0;
        if ((offset & 1) == 1) offset -= 2;
        return offset;
    }
}
