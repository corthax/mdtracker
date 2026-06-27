using System.Collections.Generic;
using System.Linq;
using MDTracker.Editor.Models;

namespace MDTracker.Editor.Services;

public class InstrumentFormatService
{
    private readonly List<IInstrumentFormat> _formats = [];

    public void Register(IInstrumentFormat format)
    {
        _formats.Add(format);
    }

    public IInstrumentFormat? FindParser(byte[] data)
    {
        return _formats.FirstOrDefault(f => f.CanParse(data));
    }

    public IInstrumentFormat? FindParserByExtension(string extension)
    {
        return _formats.FirstOrDefault(f =>
            f.Extension.Equals(extension, StringComparison.OrdinalIgnoreCase));
    }

    public InstrumentPreset? Parse(byte[] data, string? fileName = null)
    {
        var format = FindParser(data);
        return format?.Parse(data);
    }

    public string FilterString
    {
        get
        {
            var filters = _formats.Select(f => $"{f.Name} (*.{f.Extension})|*.{f.Extension}");
            return string.Join("|", filters) + "|Any file (*.*)|*.*";
        }
    }
}
