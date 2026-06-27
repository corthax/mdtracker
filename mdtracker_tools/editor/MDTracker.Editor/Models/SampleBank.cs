using System.Collections.ObjectModel;

namespace MDTracker.Editor.Models;

public class SampleBank
{
    public int BankId { get; }
    public ObservableCollection<SampleSlot> Slots { get; } = new();

    public SampleBank(int bankId)
    {
        BankId = bankId;
        for (int i = 0; i < RomConstants.NotesPerBank; i++)
        {
            var slot = new SampleSlot { BankId = bankId, NoteId = i };
            Slots.Add(slot);
        }
    }

    public static SampleBank[] CreateAll()
    {
        var banks = new SampleBank[RomConstants.BankCount];
        for (int i = 0; i < RomConstants.BankCount; i++)
            banks[i] = new SampleBank(i);
        return banks;
    }
}
