using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.Platform.Storage;
using MDTracker.Editor.Models;
using MDTracker.Editor.ViewModels;

namespace MDTracker.Editor.Views;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        DataContextChanged += OnDataContextChanged;
    }

    private void OnDataContextChanged(object? sender, System.EventArgs e)
    {
        if (DataContext is MainWindowViewModel vm)
        {
            vm.OpenRomRequested += async (_, _) => await HandleOpenRom(vm);
            vm.SaveRomRequested += async (_, _) => await HandleSaveRom(vm);
            vm.AddSamplesRequested += async (_, _) => await HandleAddSamples(vm);
            vm.AddPresetsRequested += async (_, _) => await HandleAddPresets(vm);

            var sv = vm.SaveConverterView;
            sv.OpenSourceRequested += async (_, _) => await HandleSaveConverterOpen(sv);
            sv.SaveOutputRequested += async (_, _) => await HandleSaveConverterSave(sv);


        }
    }

    private async Task HandleOpenRom(MainWindowViewModel vm)
    {
        var files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open ROM File",
            FileTypeFilter =
            [
                new("ROM files") { Patterns = ["*.bin", "*.md", "*.gen"] },
                new("All files") { Patterns = ["*.*"] },
            ],
        });
        if (files.Count > 0)
            vm.LoadRom(files[0].Path.LocalPath);
    }

    private async Task HandleSaveRom(MainWindowViewModel vm)
    {
        var file = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Save ROM File",
            DefaultExtension = "bin",
            FileTypeChoices =
            [
                new("ROM files") { Patterns = ["*.bin"] },
            ],
        });
        if (file != null)
            vm.SaveRomToPath(file.Path.LocalPath);
    }

    private async Task HandleAddSamples(MainWindowViewModel vm)
    {
        var files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Add Sample Files",
            AllowMultiple = true,
            FileTypeFilter =
            [
                new("Sample files") { Patterns = ["*.wav", "*.pcm", "*.2adpcm"] },
                new("All files") { Patterns = ["*.*"] },
            ],
        });
        foreach (var file in files)
        {
            var data = await System.IO.File.ReadAllBytesAsync(file.Path.LocalPath);
            var ext = System.IO.Path.GetExtension(file.Path.LocalPath).ToLowerInvariant();
            if (ext == ".wav")
                vm.SampleConverterView.AddWav(System.IO.Path.GetFileName(file.Path.LocalPath), data);
            vm.SamplePoolView.AddSample(new SampleFile
            {
                FileName = file.Path.LocalPath,
                ShortName = System.IO.Path.GetFileName(file.Path.LocalPath),
                Data = data,
                OriginalSize = data.Length,
            });
        }
    }

    private async Task HandleAddPresets(MainWindowViewModel vm)
    {
        var files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Add Instrument Presets",
            AllowMultiple = true,
            FileTypeFilter =
            [
                new("VGM Maker Instrument") { Patterns = ["*.vgi"] },
                new("TFM Maker Instrument") { Patterns = ["*.tfi"] },
                new("All files") { Patterns = ["*.*"] },
            ],
        });
        foreach (var file in files)
        {
            var data = await System.IO.File.ReadAllBytesAsync(file.Path.LocalPath);
            var preset = vm.PresetPoolView.TryParse(data);
            if (preset != null)
            {
                preset.Name = System.IO.Path.GetFileNameWithoutExtension(file.Path.LocalPath);
                vm.PresetPoolView.AddPreset(preset);
            }
        }
    }

    private async Task HandleSaveConverterOpen(SaveConverterViewModel sv)
    {
        var files = await StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open v1.04 Save File",
            FileTypeFilter =
            [
                new("SRAM save files") { Patterns = ["*.sram", "*.srm", "*.bin"] },
                new("All files") { Patterns = ["*.*"] },
            ],
        });
        if (files.Count > 0)
            sv.LoadSource(files[0].Path.LocalPath);
    }

    private async Task HandleSaveConverterSave(SaveConverterViewModel sv)
    {
        var file = await StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Save v1.05 Save File",
            DefaultExtension = "sram",
            FileTypeChoices =
            [
                new("SRAM save files") { Patterns = ["*.sram"] },
            ],
        });
        if (file != null)
            sv.ConvertAndSave(file.Path.LocalPath);
    }
}
