using System;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace stobin
{
    class Program
    {
        static void Main()
        {
            Console.WriteLine("WAV to PCM formats converter using SGDK rescomp.");

            string currentDirectory = Directory.GetCurrentDirectory();
            string[] wavFiles = Directory.GetFiles(currentDirectory, "*.wav");
            string outFolder = $"{currentDirectory}\\out\\";
            string rescompFormat = "2ADPCM";
            string rescompSampleRate = "32000";
            string fileExtention = "2adpcm";

            if (wavFiles.Length == 0)
            {
                Console.WriteLine("No WAV files found in current directory! Exit.");
                Console.ReadKey();
                return;
            }
            Console.WriteLine();
            Console.WriteLine("Choose format:");
            Console.WriteLine("1. Convert to 2ADPCM (*.2adpcm)");
            Console.WriteLine("2. Convert to 8 bit signed PCM (*.s8pcm)");
            Console.WriteLine();

            var keyInfo = Console.ReadKey();
            Console.WriteLine();
            ConsoleKeyInfo inputHz = new ConsoleKeyInfo();

            switch (keyInfo.Key)
            {
                case ConsoleKey.D1:
                    rescompFormat = "2ADPCM";
                    fileExtention = "2adpcm";
                    rescompSampleRate = "";
                    outFolder += $"{fileExtention}\\";
                    break;

                case ConsoleKey.D2:
                    rescompFormat = "PCM";
                    fileExtention = "s8pcm";
                    outFolder += $"{fileExtention}\\";

                    Console.WriteLine();
                    Console.WriteLine("Choose sample rate:");
                    Console.WriteLine("1. 8000");
                    Console.WriteLine("2. 11025");
                    Console.WriteLine("3. 16000");
                    Console.WriteLine("4. 22050");
                    Console.WriteLine("5. 32000");
                    Console.WriteLine();

                    inputHz = Console.ReadKey();
                    Console.WriteLine();

                    switch (inputHz.Key)
                    {
                        case ConsoleKey.D1:
                            rescompSampleRate = "8000";
                            break;
                        case ConsoleKey.D2:
                            rescompSampleRate = "11025";
                            break;
                        case ConsoleKey.D3:
                            rescompSampleRate = "16000";
                            break;
                        case ConsoleKey.D4:
                            rescompSampleRate = "22050";
                            break;
                        case ConsoleKey.D5:
                            rescompSampleRate = "32000";
                            break;
                    }
                    break;
            }

            Console.WriteLine();
            Console.WriteLine("WORKING...");

            if (!Directory.Exists(outFolder)) Directory.CreateDirectory(outFolder);

            foreach (var wavFile in wavFiles)
            {
                string fileName = Path.GetFileNameWithoutExtension(wavFile);

                // Create .res file

                using (StreamWriter outputFile = File.CreateText($"{fileName}.res"))
                {
                    outputFile.WriteLine($"WAV {fileName} \"{wavFile}\" {rescompFormat} {rescompSampleRate}");
                }

                // Run rescomp

                ProcessStartInfo startInfo = new ProcessStartInfo
                {
                    FileName = @"rescomp.jar",
                    Arguments = $@"{fileName}.res -noheader"
                };
                var process = Process.Start(startInfo);

                // Convert assembly .s to binary .2adpcm

                string inputFilePath = $"{fileName}.s";
                string outputFilePath = $"{outFolder}{fileName}.{fileExtention}";

                //while (!File.Exists(inputFilePath)) { } // Wait for rescomp
                while (!process.HasExited) { }

                string inputText = File.ReadAllText(inputFilePath);

                // Find all hex numbers starting with "0x"
                var hexNumbers = Regex.Matches(inputText, @"\b0x[\da-fA-F]+\b");

                using (BinaryWriter writer = new BinaryWriter(File.Open(outputFilePath, FileMode.Create)))
                {
                    foreach (Match hexNumber in hexNumbers)
                    {
                        // Convert the hex number to byte and write to binary file
                        byte value = Convert.ToByte(hexNumber.Value, 16);
                        writer.Write(value);
                    }
                }

                Console.WriteLine($"CREATE: {outputFilePath}");

                // Cleanup

                File.Delete($"{fileName}.s");
                File.Delete($"{fileName}.res");
            }

            Console.WriteLine("DONE!");
            Console.ReadKey();
        }
    }
}
