WAV to SGDK PCM formats converter using rescomp.

1. Put 44100Hz 16bit .wav files in this folder (or copy converter files into your samples folder)
2. Run "wav2pcm.exe", choose format
3. Converted files goes to \out

Then you can add samples into ROM and assign them to notes using MD.Tracker Editor.
Uncheck "Replace FF with 00" when adding 2ADPCM samples, but leave checked for 8 bit signed PCM - it will make them less noisy.