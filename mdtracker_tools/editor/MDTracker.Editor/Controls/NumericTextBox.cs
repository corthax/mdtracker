using Avalonia.Controls;
using Avalonia.Input;
using System.Globalization;

namespace MDTracker.Editor.Controls;

public class NumericTextBox : TextBox
{
    protected override Type StyleKeyOverride => typeof(TextBox);

    public NumericTextBox()
    {
        TextChanged += (_, _) =>
        {
            if (string.IsNullOrEmpty(Text))
                Text = "0";
        };
    }

    protected override void OnTextInput(TextInputEventArgs e)
    {
        if (e.Text is not null && !e.Text.All(char.IsDigit))
            e.Handled = true;
        base.OnTextInput(e);
    }
}

public class StringToIntConverter : Avalonia.Data.Converters.IValueConverter
{
    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return value is int i ? i.ToString() : "0";
    }

    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return value is string s && int.TryParse(s, out var r) ? r : 0;
    }
}
