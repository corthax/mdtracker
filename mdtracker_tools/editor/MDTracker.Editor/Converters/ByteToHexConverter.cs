using System;
using System.Globalization;
using Avalonia.Data.Converters;

namespace MDTracker.Editor.Converters;

public class ByteToHexConverter : IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        if (value is byte b) return b.ToString("X2");
        if (value is int i) return i.ToString("X");
        if (value is uint u) return u.ToString("X");
        return value?.ToString();
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        throw new NotSupportedException();
    }
}
