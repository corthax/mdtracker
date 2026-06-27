using System.Linq;

namespace MDTracker.Editor.Models;

public static class DropdownHelper
{
    public static DropdownOption[] Range(int min, int max) =>
        Enumerable.Range(min, max - min + 1)
            .Select(i => new DropdownOption { Label = i.ToString(), Value = i })
            .ToArray();

    public static DropdownOption[] Range(int min, int max, Func<int, string> label) =>
        Enumerable.Range(min, max - min + 1)
            .Select(i => new DropdownOption { Label = label(i), Value = i })
            .ToArray();
}
