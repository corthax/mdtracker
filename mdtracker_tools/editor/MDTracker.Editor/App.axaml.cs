using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using MDTracker.Editor.ViewModels;
using MDTracker.Editor.Views;

namespace MDTracker.Editor;

public partial class App : Application
{
    public override void Initialize()
    {
        AvaloniaXamlLoader.Load(this);
    }

    public override void OnFrameworkInitializationCompleted()
    {
        if (ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktop)
        {
            var vm = new MainWindowViewModel();
            desktop.MainWindow = new MainWindow
            {
                DataContext = vm,
            };
            desktop.MainWindow.Closing += (_, _) =>
            {
                var settingsVm = vm.SettingsView;
                if (settingsVm != null)
                {
                    settingsVm.SaveNow();
                }
            };
        }

        base.OnFrameworkInitializationCompleted();
    }
}
