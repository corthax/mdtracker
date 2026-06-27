namespace MDTracker.Editor.Models;

public static class RomConstants
{
    public const int SampleSettingsAddr = 0x0003CC00;
    public const int SampleSettingsEntrySize = 28;
    public const int SampleSettingsCount = 384;
    public const int SampleSettingsTotalSize = SampleSettingsEntrySize * SampleSettingsCount;

    public const int SampleBankAddr = 0x0003F600;
    public const int SampleNameSize = 14;
    public const int SampleNamesTotalSize = SampleNameSize * SampleSettingsCount;

    public const int NotesPerBank = 96;
    public const int BankCount = 4;
    public const int NotesTotal = NotesPerBank * BankCount;

    public const int PresetStructSize = 49;
    public const int PresetCount = 256;

    public const int PBank0Addr = 0x00030C56;
    public const int MBank0Addr = 0x00033D25;

    public const int RomSize = 0x400000;

    public static int PBankAddress(int index)
    {
        return PBank0Addr - index * PresetStructSize;
    }

    public static int MBankAddress(int index)
    {
        return MBank0Addr - index * PresetStructSize;
    }

    public static (int addr, bool isMelodic) GetPresetAddress(int index)
    {
        return index < 128
            ? (MBankAddress(index), true)
            : (PBankAddress(index - 128), false);
    }
}
