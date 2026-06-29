using CommunityToolkit.Mvvm.ComponentModel;

namespace MDTracker.Editor.Models;

public partial class SampleSlot : ObservableObject
{
    [ObservableProperty] private int _bankId;
    [ObservableProperty] private int _noteId;
    [ObservableProperty] private int _startOffset;
    [ObservableProperty] private int _endOffset;
    [ObservableProperty] private int _pan = 192;
    [ObservableProperty] private int _panIndex = 1;
    [ObservableProperty] private bool _looped;
    [ObservableProperty] private int _rate;
    [ObservableProperty] private int _rateIndex;
    [ObservableProperty] private int _type;
    [ObservableProperty] private string _name = string.Empty;

    [ObservableProperty] private int _samplePoolId = -1;
    [ObservableProperty] private bool _isSynced;

    public static DropdownOption[] PanOptions { get; } =
    [
        new() { Label = "L", Value = 128 },
        new() { Label = "C", Value = 192 },
        new() { Label = "R", Value = 64 },
    ];

    public static DropdownOption[] RateOptions { get; } =
    [
        new() { Label = "32000", Value = 0 },
        new() { Label = "22050", Value = 1 },
        new() { Label = "16000", Value = 2 },
        new() { Label = "13400", Value = 3 },
        new() { Label = "11025", Value = 4 },
        new() { Label = "8000",  Value = 5 },
    ];

    partial void OnPanChanged(int value)
    {
        PanIndex = value switch { 128 => 0, 192 => 1, 64 => 2, _ => 1 };
    }

    partial void OnPanIndexChanged(int value)
    {
        Pan = value switch { 0 => 128, 1 => 192, 2 => 64, _ => 192 };
    }

    partial void OnRateChanged(int value) => RateIndex = value;
    partial void OnRateIndexChanged(int value) => Rate = value;

    public int BankNoteIndex => BankId * RomConstants.NotesPerBank + NoteId;

    public string NoteName
    {
        get
        {
            var noteNames = new[] { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
            var octave = NoteId / 12;
            var note = NoteId % 12;
            return $"{BankNoteIndex}: {noteNames[note]}{octave}";
        }
    }
}
