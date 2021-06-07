#ifndef PRESETS_H_INCLUDED
#define PRESETS_H_INCLUDED

struct Preset_FM
{
    u8 algorithm;
    u8 feedback;
    u8 stereo;
    u8 ams;
    u8 fms;

    u8 multiple_1;
    u8 detune_1;
    u8 attackRate_1;
    u8 rateScaling_1;
    u8 firstDecayRate_1;
    u8 amplitudeModulation_1;
    u8 secondaryAmplitude_1;
    u8 secondaryDecayRate_1;
    u8 releaseRate_1;
    u8 totalLevel_1;
    u8 ssgEg_1;

    u8 multiple_2;
    u8 detune_2;
    u8 attackRate_2;
    u8 rateScaling_2;
    u8 firstDecayRate_2;
    u8 amplitudeModulation_2;
    u8 secondaryAmplitude_2;
    u8 secondaryDecayRate_2;
    u8 releaseRate_2;
    u8 totalLevel_2;
    u8 ssgEg_2;

    u8 multiple_3;
    u8 detune_3;
    u8 attackRate_3;
    u8 rateScaling_3;
    u8 firstDecayRate_3;
    u8 amplitudeModulation_3;
    u8 secondaryAmplitude_3;
    u8 secondaryDecayRate_3;
    u8 releaseRate_3;
    u8 totalLevel_3;
    u8 ssgEg_3;

    u8 multiple_4;
    u8 detune_4;
    u8 attackRate_4;
    u8 rateScaling_4;
    u8 firstDecayRate_4;
    u8 amplitudeModulation_4;
    u8 secondaryAmplitude_4;
    u8 secondaryDecayRate_4;
    u8 releaseRate_4;
    u8 totalLevel_4;
    u8 ssgEg_4;
};
typedef struct Preset_FM Preset_FM;

const Preset_FM* M_BANK_0[256];
const char* presetName[256];

void InitPresets();

#endif // PRESETS_H_INCLUDED
