using Avalonia.Controls;
using MDTracker.Editor.ViewModels;

namespace MDTracker.Editor.Views;

public partial class SampleBankView : UserControl
{
    public SampleBankView()
    {
        InitializeComponent();
        Loaded += (_, _) => SyncRangeCombos();
    }

    private void OnBankSelectionChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (DataContext is SampleBankViewModel vm && sender is TabControl tc)
            vm.SelectedBankIndex = tc.SelectedIndex;
    }

    private void OnRangePanChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (DataContext is SampleBankViewModel vm)
            vm.RangePanIndex = RangePanCombo.SelectedIndex;
    }

    private void OnRangeRateChanged(object? sender, SelectionChangedEventArgs e)
    {
        if (DataContext is SampleBankViewModel vm)
            vm.RangeRateIndex = RangeRateCombo.SelectedIndex;
    }

    private void SyncRangeCombos()
    {
        if (DataContext is SampleBankViewModel vm)
        {
            RangePanCombo.SelectedIndex = vm.RangePanIndex;
            RangeRateCombo.SelectedIndex = vm.RangeRateIndex;
        }
    }
}
