using System;
using System.Globalization;
using Avalonia.Data.Converters;

namespace MDTracker.Editor.Converters;

public class HexStringConverter : IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return value is int i ? $"0x{i:X8}" : "0x00000000";
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        if (value is string s)
        {
            s = s.Trim();
            if (s.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
                s = s[2..];
            if (int.TryParse(s, NumberStyles.HexNumber, null, out var r))
                return r;
        }
        return 0;
    }
}
