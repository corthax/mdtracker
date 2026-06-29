using Avalonia.Controls;
using MDTracker.Editor.Models;
using MDTracker.Editor.ViewModels;

namespace MDTracker.Editor.Views;

public partial class SamplePoolView : UserControl
{
    public SamplePoolView()
    {
        InitializeComponent();
    }

    private void OnPoolSelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (DataContext is SamplePoolViewModel vm && sender is ListBox lb)
            vm.UpdateSelection(lb.SelectedItems?.OfType<SampleFile>() ?? []);
    }
}
