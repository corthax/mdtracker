#ifndef MDT_MAIN_H_INCLUDED
#define MDT_MAIN_H_INCLUDED

// bit help
// a=target variable, b=bit number to act upon 0-n
#define BIT_SET(a,b) ((a) |= (1U<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1U<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1U<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1U<<(b))))            // "!!" to make sure this returns 0 or 1

// x=target variable, y=mask
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK_ALL(x,y) (((x) & (y)) == (y))     // warning: evaluates y twice
#define BITMASK_CHECK_ANY(x,y) ((x) & (y))

// various
#define PSG_TONAL_CH3_MUTED     0
#define PSG_FIXED               1
#define PSG_TONAL_CH3_NOT_MUTED 2
#define PSG_LOWEST_NOTE         9

#define ARP_BASE                100
#define MICROTONE_STEPS         32
#define SEQ_VOL_SKIP            0xFF
#define SEQ_VOL_MIN_ATT         0x00
#define SEQ_VOL_MAX_ATT         0x7F
#define PPL_DEFAULT             4

#define PATTERN_ROWS            32
#define PATTERN_ROW_LAST        0x1F    // index of max line; starts from 0
#define PATTERN_LAST            0x380   // 896
#define PATTERN_JUMPSIDETRIGGER 0xFF

#define MATRIX_ROWS             250
#define MATRIX_ROWS_ONPAGE      25
#define MATRIX_ROW_LAST         0xF9
#define MATRIX_ROWS_ONPAGE_LAST 0x18    // index of max line on SCREEN; starts from 0; 25 lines per page; 10 pages
#define MATRIX_PAGES            9        // MAX_MATRIX_ROWS / (MAX_MATRIX_SCREEN_ROW + 1)) - 1

#define INST_SIZE               89
#define INST_MUTE               1
#define INST_PLAY               0
#define INST_SOLO               2

#define INSTRUMENTS_TOTAL       256
#define INSTRUMENTS_LAST        0xFF

#define NOTE_EMPTY              0xFE
#define NOTE_OFF                0xFF
#define NOTE_MAX                95
#define NOTES              96
#define OCTAVE_MAX              7

#define CH3_SPECIAL             0b01000000
#define CH3_SPECIAL_CSM         0b10000000
#define CH3_NORMAL              0b00000000
#define CH3_SPECIAL_CSM_OFF     0b11000000

#define OXFF                    0xFF
#define OXFFFF                  0xFFFF
#define NOTHING                 -1

#define CHANNEL_FM1             0
#define CHANNEL_FM2             1
#define CHANNEL_FM3_OP4         2
#define CHANNEL_FM3_OP3         3
#define CHANNEL_FM3_OP2         4
#define CHANNEL_FM3_OP1         5
#define CHANNEL_FM4             6
#define CHANNEL_FM5             7
#define CHANNEL_FM6_DAC         8
#define CHANNEL_PSG1            9
#define CHANNEL_PSG2            10
#define CHANNEL_PSG3            11
#define CHANNEL_PSG4_NOISE      12
#define CHANNEL_LAST            12
#define CHANNELS_TOTAL          13

#define SCREEN_MATRIX           0
#define SCREEN_PATTERN          1
#define SCREEN_INSTRUMENT       2

#define EFFECTS_TOTAL           6

#define COMB_FB_ALG 0
#define COMB_PAN_AMS_FMS 1
#define COMB_DT_MUL_1 2
#define COMB_DT_MUL_2 3
#define COMB_DT_MUL_3 4
#define COMB_DT_MUL_4 5
#define COMB_RS_AR_1 6
#define COMB_RS_AR_2 7
#define COMB_RS_AR_3 8
#define COMB_RS_AR_4 9
#define COMB_AM_D1R_1 10
#define COMB_AM_D1R_2 11
#define COMB_AM_D1R_3 12
#define COMB_AM_D1R_4 13
#define COMB_D1L_RR_1 14
#define COMB_D1L_RR_2 15
#define COMB_D1L_RR_3 16
#define COMB_D1L_RR_4 17

#define MSU_PLAY        0x1100 // decimal no. of track (1-99); playback will be stopped in the end of track
#define MSU_PLAY_LOOP 	0x1200 // decimal no. of track (1-99); playback will restart the track when end is reached
#define MSU_PAUSE 	    0x1300 // vol fading time. 1/75 of sec (75 equal to 1 sec) instant stop if 0; pause playback
#define MSU_RESUME 	    0x1400 // none; resume playback
#define MSU_VOL 	    0x1500 // volume 0-255; set cdda volume
#define MSU_SEEK_ON	    0x1600 // 0-on(default state), 1-off(no seek delays);seek time emulation switch
#define MSU_SEEK_OFF	0x1601
#define MSU_PLAYOF 	    0x1A00 // #1 = decimal no. of track (1-99) #2 = offset in sectors from the start of the track to apply when looping; play cdda track and loop from specified sector offset

#define DEFAULT_TEMPO   0xB3   // 140 BPM NTSC if 16/16

void InitTracker();
void DrawText(u8 plane, u8 pal, const char *str, u8 x, u8 y);
void DrawNum(u8 plane, u8 pal, const char *str, u8 x, u8 y);
void DrawHex(u8 pal, u8 number, u8 x, u8 y);
void DrawHex2(u8 pal, u16 number, u8 x, u8 y);
void FillRowRight(u8 plane, u8 pal, u8 flipV, u8 flipH, u8 guiSymbol, u8 fillCount, u8 startX, u8 y);

static void DoEngine();
static void SetPitchFM(u8 mtxCh, u8 note);
static void SetPitchPSG(u8 mtxCh, u8 note);
static void PlayNote(u8 note, u8 mtxCh);
static void PlayNoteOff(u8 mtxCh);
static void StopChannelSound(u8 mtxCh);
static void StopAllSound();
static void StopEffects(u8 mtxCh);
static void SetGlobalLFO(u8 freq);
static void SetChannelVolume(u8 mtxCh);
static void SetChannelBaseVolume_FM(u8 mtxCh, u8 fmCh);
static void SetBPM(u16 tempo);
static void WriteYM2612(u8 mtxCh, u8 fmCh);
static void CacheIstrumentToRAM(u8 id);
static void ApplyCommand_FM(u8 mtxCh, u8 id, u8 fxParam, u8 fxValue);
static void ApplyCommand_FM3_SP(u8 mtxCh, u8 fxParam, u8 fxValue);
static void ApplyCommand_DAC(u8 fxParam, u8 fxValue);
static void ApplyCommand_Common(u8 mtxCh, u8 fxParam, u8 fxValue);
static void ApplyCommand_PSG(u8 fxParam, u8 fxValue);
static void ChangeInstrumentParameter(s8 modifier);
static void ChangePatternParameter(s8 note, s8 par);
static void ChangeMatrixValue(s16 mod);
static void RequestZ80();
static void ReleaseZ80();
static void JoyEvent(u16 joy, u16 changed, u16 state);
static void hIntCallback();
static void vIntCallback();
static void YM2612_writeRegZ80(u16 part, u8 reg, u8 data);
static s16 FindUnusedPattern();
void CalculateCombined(u8 fmCh, u8 reg);
void ForceResetVariables();

// temporal instrument storage
struct Instrument
{
    u8 FB;
    u8 ALG;
    u8 FB_ALG;

    u8 PAN;
    u8 AMS;
    u8 FMS;
    u8 PAN_AMS_FMS;

    u8 TL1;
    u8 TL2;
    u8 TL3;
    u8 TL4;

    u8 RS1;
    u8 RS2;
    u8 RS3;
    u8 RS4;
    u8 AR1;
    u8 AR2;
    u8 AR3;
    u8 AR4;
    u8 RS1_AR1;
    u8 RS2_AR2;
    u8 RS3_AR3;
    u8 RS4_AR4;

    u8 DT1;
    u8 DT2;
    u8 DT3;
    u8 DT4;
    u8 MUL1;
    u8 MUL2;
    u8 MUL3;
    u8 MUL4;
    u8 DT1_MUL1;
    u8 DT2_MUL2;
    u8 DT3_MUL3;
    u8 DT4_MUL4;

    u8 AM1;
    u8 AM2;
    u8 AM3;
    u8 AM4;
    u8 D1R1;
    u8 D1R2;
    u8 D1R3;
    u8 D1R4;
    u8 AM1_D1R1;
    u8 AM2_D1R2;
    u8 AM3_D1R3;
    u8 AM4_D1R4;

    u8 D1L1;
    u8 D1L2;
    u8 D1L3;
    u8 D1L4;
    u8 RR1;
    u8 RR2;
    u8 RR3;
    u8 RR4;
    u8 D1L1_RR1;
    u8 D1L2_RR2;
    u8 D1L3_RR3;
    u8 D1L4_RR4;

    u8 D2R1;
    u8 D2R2;
    u8 D2R3;
    u8 D2R4;

    u8 SSGEG1;
    u8 SSGEG2;
    u8 SSGEG3;
    u8 SSGEG4;
};
typedef struct Instrument Instrument;

#endif // MDT_MAIN_H_INCLUDED
