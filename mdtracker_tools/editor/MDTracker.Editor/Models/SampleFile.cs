using System.IO;

namespace MDTracker.Editor.Models;

public class SampleFile
{
    public int Id { get; set; }
    public string FileName { get; set; } = string.Empty;
    public string ShortName { get; set; } = string.Empty;
    public byte[] Data { get; set; } = [];
    public int OriginalSize { get; set; }
    public int AlignedSize => ((OriginalSize + 255) / 256) * 256;
    public long StartOffset { get; set; }
    public long EndOffset { get; set; }
    public string Extension => Path.GetExtension(FileName)?.ToLowerInvariant() ?? string.Empty;
    public bool IsAdpcm => Extension == ".2adpcm";

    public static string ExtractShortName(string filePath)
    {
        string name = Path.GetFileNameWithoutExtension(filePath);
        int open = name.IndexOf('(');
        int close = name.IndexOf(')');
        if (open != -1 && close != -1 && close > open)
            name = name.Substring(open + 1, close - open - 1);
        name = name.Replace("(", "").Replace(")", "");
        return name.Length > RomConstants.SampleNameSize
            ? name[..RomConstants.SampleNameSize]
            : name;
    }
}
