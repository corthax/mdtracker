#ifndef MDT_MAIN_H_INCLUDED
#define MDT_MAIN_H_INCLUDED

#define MD_TRACKER_VERSION 5

//! HEADERS ARE NOT REPARSED IN CODEBLOCKS WHEN BUILD. DO REBUILD

// bit help
/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1U<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1U<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1U<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1U<<(b))))            // "!!" to make sure this returns 0 or 1

/* x=target variable, y=mask */
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

#define MAX_PATTERN             0x380   // 896
#define PATTERN_ROW_LAST        31      // index of max line; starts from 0

#define MAX_MATRIX_ROWS         250
#define MAX_MATRIX_SCREEN_ROW   24      // index of max line on SCREEN; starts from 0; 25 lines per page; 10 pages
#define MAX_MATRIX_PAGE         9       // MAX_MATRIX_ROWS / (MAX_MATRIX_SCREEN_ROW + 1)) - 1

#define INST_SIZE               89
#define INST_MUTE               1
#define INST_PLAY               0
#define INST_SOLO               2

#define MAX_INSTRUMENT          0xFF

#define NOTE_EMPTY              254
#define NOTE_OFF                0xFF
#define MAX_NOTE                95
#define MAX_NOTES               96
#define MAX_OCTAVE              7

#define CH3_SPECIAL             0b01000000
#define CH3_SPECIAL_CSM         0b10000000
#define CH3_NORMAL              0b00000000
#define CH3_SPECIAL_CSM_OFF     0b11000000

#define EVALUATE_0xFF           0xFF
#define EVALUATE_0xFFFF         0xFFFF
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
#define CHANNELS_TOTAL          13

#define SCREEN_MATRIX           0
#define SCREEN_PATTERN          1
#define SCREEN_INSTRUMENT       2

#define H_INT_SKIP              1   // (0, 1, 3, 6, 7, 9, 15) 224 / H_INT_SKIP+1 ticks per frame (1/60 sec)
#define TICK_SKIP_MIN           6   // fast tempo limit
#define TICK_SKIP_MAX           128 // slow tempo limit

#if (MD_TRACKER_VERSION == 5)
    #define EFFECTS_TOTAL          6
#elif (MD_TRACKER_VERSION == 2)
    #define EFFECTS_TOTAL          3
#else
    #define EFFECTS_TOTAL          3
#endif

//const char *channelNames[13] = { "y1", "y2", "y3", "..", "..", "..", "y4", "y5", "y6", "p1", "p2", "p3", "n4" };
//const char *noteNames[12] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
//const char *alphabet[38] = {"-", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W",
//"X", "Y", "Z", " ", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static void InitTracker();
static void DoEngine();

static void SetPitchFM(u8 matrixChannel, u8 note);
static void SetPitchPSG(u8 matrixChannel, u8 note);

static void PlayNote(u8 note, u8 matrixChannel);
static void StopChannelSound(u8 matrixChannel);
static void StopAllSound();
static void StopEffects(u8 matrixChannel);
static void InitGlobals(); // lfo, dac, ch3
static void SetGlobalLFO(u8 freq);
static void SetChannelVolume(u8 channel);
static void SetBPM(u16 counter);

static void WriteYM2612(u8 matrixChannel, u8 id);
static void ApplyCommand(u8 matrixChannel, u8 id, u8 fxParam, u8 fxValue);
static void CacheIstrumentToRAM(u8 id);

static void ChangeInstrumentParameter(s8 modifier);
static void ChangePatternParameter(s8 note, s8 par);
static void ChangeMatrixValue(s8 mod);

static void RequestZ80();
static void ReleaseZ80();

void DrawText(u8 plane, u8 pal, const char *str, u8 x, u8 y);
void DrawNum(u8 plane, u8 pal, const char *str, u8 x, u8 y);
void DrawHex2(u8 pal, u16 number, u8 x, u8 y);
void FillRowRight(u8 plane, u8 pal, u8 flipV, u8 flipH, u8 guiSymbol, u8 fillCount, u8 startX, u8 y);

static void JoyEvent(u16 joy, u16 changed, u16 state);

void hIntCallback();
void vIntCallback();

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
Instrument tmpInst[MAX_INSTRUMENT]; // cache instruments to RAM for faster access

#endif // MDT_MAIN_H_INCLUDED
