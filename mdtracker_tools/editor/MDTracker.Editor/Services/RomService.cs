using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public class RomService
{
    private byte[]? _romData;
    public string? RomPath { get; private set; }
    public bool IsLoaded => _romData != null;

    public string Load(string path)
    {
        _romData = File.ReadAllBytes(path);
        RomPath = path;
        return path;
    }

    public void PopulateBanks(SampleBank[] banks)
    {
        for (int i = 0; i < RomConstants.SampleSettingsCount; i++)
        {
            int offset = RomConstants.SampleSettingsAddr + i * RomConstants.SampleSettingsEntrySize;
            var slot = banks[i / RomConstants.NotesPerBank].Slots[i % RomConstants.NotesPerBank];
            slot.BankId = _romData![offset];
            slot.NoteId = _romData[offset + 1];
            slot.StartOffset = Read24(offset + 2);
            slot.EndOffset = Read24(offset + 5);
            slot.Pan = _romData[offset + 8];
            slot.Looped = _romData[offset + 9] != 0;
            slot.Rate = _romData[offset + 10];
            slot.Type = _romData[offset + 11];
            slot.Name = ReadString(offset + 12, RomConstants.SampleNameSize);
        }
    }

    public void WriteBanks(SampleBank[] banks)
    {
        if (_romData == null) return;
        for (int i = 0; i < RomConstants.SampleSettingsCount; i++)
        {
            int bankId = i / RomConstants.NotesPerBank;
            int noteId = i % RomConstants.NotesPerBank;
            var slot = banks[bankId].Slots[noteId];
            int offset = RomConstants.SampleSettingsAddr + i * RomConstants.SampleSettingsEntrySize;
            _romData[offset] = (byte)slot.BankId;
            _romData[offset + 1] = (byte)slot.NoteId;
            Write24(offset + 2, slot.StartOffset);
            Write24(offset + 5, slot.EndOffset);
            _romData[offset + 8] = (byte)slot.Pan;
            _romData[offset + 9] = (byte)(slot.Looped ? 1 : 0);
            _romData[offset + 10] = (byte)slot.Rate;
            _romData[offset + 11] = (byte)slot.Type;
            WriteString(offset + 12, slot.Name, RomConstants.SampleNameSize);
        }
    }

    public void Save(string? path = null)
    {
        path ??= RomPath;
        if (path == null || _romData == null) return;
        File.WriteAllBytes(path, _romData);
    }

    public byte[] ReadBlock(int address, int size)
    {
        if (_romData == null) return [];
        var block = new byte[size];
        Array.Copy(_romData, address, block, 0, size);
        return block;
    }

    public void WriteBlock(int address, byte[] data)
    {
        if (_romData == null) return;
        Array.Copy(data, 0, _romData, address, data.Length);
    }

    public InstrumentPreset ReadPreset(int index)
    {
        var (addr, _) = RomConstants.GetPresetAddress(index);
        var data = ReadBlock(addr, InstrumentPreset.StructSize);
        return RomPresetConversion.FromRomBytes(data);
    }

    public void WritePreset(int index, InstrumentPreset preset)
    {
        var (addr, _) = RomConstants.GetPresetAddress(index);
        var data = RomPresetConversion.ToRomBytes(preset);
        WriteBlock(addr, data);
    }

    private int Read24(int offset)
    {
        return (_romData![offset] << 16) | (_romData[offset + 1] << 8) | _romData[offset + 2];
    }

    private void Write24(int offset, int value)
    {
        _romData![offset] = (byte)((value >> 16) & 0xFF);
        _romData[offset + 1] = (byte)((value >> 8) & 0xFF);
        _romData[offset + 2] = (byte)(value & 0xFF);
    }

    private string ReadString(int offset, int maxLen)
    {
        var chars = new char[maxLen];
        for (int i = 0; i < maxLen; i++)
        {
            var c = (char)_romData![offset + i];
            chars[i] = c >= 32 ? c : ' ';
        }
        return new string(chars).TrimEnd();
    }

    private void WriteString(int offset, string str, int maxLen)
    {
        for (int i = 0; i < maxLen; i++)
            _romData![offset + i] = (byte)(i < str.Length ? str[i] : 0x20);
    }
}
