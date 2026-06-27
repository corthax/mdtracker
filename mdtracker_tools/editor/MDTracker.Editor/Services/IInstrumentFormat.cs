using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public interface IInstrumentFormat
{
    string Name { get; }
    string Extension { get; }
    bool CanParse(byte[] data);
    InstrumentPreset Parse(byte[] data);
}
