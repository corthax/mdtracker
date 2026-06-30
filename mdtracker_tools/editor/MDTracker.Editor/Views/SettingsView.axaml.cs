using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using MDTracker.Editor.ViewModels;

namespace MDTracker.Editor.Views;

public partial class SettingsView : UserControl
{
    public SettingsView()
    {
        InitializeComponent();
    }

    private void OnSaveSettingsClick(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        if (DataContext is SettingsViewModel vm)
            vm.SaveSettings(SampleSettingsTextBox.Text ?? string.Empty, SampleBankTextBox.Text ?? string.Empty);
    }
}
