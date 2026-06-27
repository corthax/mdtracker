#ifndef MDT_SAMPLESETTINGS_H_INCLUDED
#define MDT_SAMPLESETTINGS_H_INCLUDED

#define SAMPLE_SETTINGS_ENTRY_SIZE  28

struct __attribute__((packed)) SampleSettings
{
    u8  bankId;
    u8  noteId;
    u32 startOffset;
    u32 endOffset;
    u8  pan;
    u8  looped;
    u8  rate;
    u8  type;
    char name[14];
};

static inline const struct SampleSettings* GetSampleSettings(u8 bank, u8 note)
{
    u16 index = (u16)bank * NOTES + note;
    return (const struct SampleSettings*)(sample_settings + index * SAMPLE_SETTINGS_ENTRY_SIZE);
}

#endif
