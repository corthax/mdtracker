#include <genesis.h>

#ifndef MDT_GUI_H_INCLUDED
#define MDT_GUI_H_INCLUDED

// ---------------------------------------------------------------------------
// GUI constants
// ---------------------------------------------------------------------------

#define GUI_INST_PARAM_ALG          0
#define GUI_INST_PARAM_FMS          1
#define GUI_INST_PARAM_AMS          2
#define GUI_INST_PARAM_PAN          3
#define GUI_INST_PARAM_FB           4
#define GUI_INST_PARAM_TL           5
#define GUI_INST_PARAM_RS           6
#define GUI_INST_PARAM_MUL          7
#define GUI_INST_PARAM_DT           8
#define GUI_INST_PARAM_AR           9
#define GUI_INST_PARAM_D1R          10
#define GUI_INST_PARAM_D1L          11
#define GUI_INST_PARAM_D2R          12
#define GUI_INST_PARAM_RR           13
#define GUI_INST_PARAM_AM           14
#define GUI_INST_PARAM_SSGEG        15
#define GUI_INST_PARAM_LFO          16
#define GUI_INST_PARAM_PARSEQ       17
#define GUI_INST_PARAM_ARPSEQ       18
#define GUI_INST_PARAM_PCM_BANK     19
#define GUI_INST_PARAM_PCM_NOTE     20
//#define GUI_INST_PARAM_PCM_START    21
//#define GUI_INST_PARAM_PCM_END      22
//#define GUI_INST_PARAM_PCM_TYPE     21
//#define GUI_INST_PARAM_PCM_LOOP     22
//#define GUI_INST_PARAM_PCM_RATE     23
//#define GUI_INST_PARAM_PCM_PAN      24
#define GUI_INST_PARAM_PRESET       21
#define GUI_INST_PARAM_NAME         22
#define GUI_INST_PARAM_COPY         23
#define GUI_INST_PARAM_STATE        24
#define GUI_INST_MAX_PARAMETER      23

#define GUI_INST_POSY_SAMPLE_TYPE   3
#define GUI_INST_POSY_SAMPLE_LOOP   4
#define GUI_INST_POSY_SAMPLE_RATE   5
#define GUI_INST_POSY_SAMPLE_PAN    6
#define GUI_INST_POSY_SAMPLE_NAME   8

// -----------------------------------------------------------
#define GUI_PATTERN_L_NOTE          0
#define GUI_PATTERN_L_INST          1
#define GUI_PATTERN_L_FX1_TYPE      2
#define GUI_PATTERN_L_FX1_VALUE     3
#define GUI_PATTERN_L_FX2_TYPE      4
#define GUI_PATTERN_L_FX2_VALUE     5
#define GUI_PATTERN_L_FX3_TYPE      6
#define GUI_PATTERN_L_FX3_VALUE     7

#define GUI_PATTERN_L_FX4_TYPE      8
#define GUI_PATTERN_L_FX4_VALUE     9
#define GUI_PATTERN_L_FX5_TYPE      10
#define GUI_PATTERN_L_FX5_VALUE     11
#define GUI_PATTERN_L_FX6_TYPE      12
#define GUI_PATTERN_L_FX6_VALUE     13

#define GUI_PATTERN_R_NOTE          14
#define GUI_PATTERN_R_INST          15
#define GUI_PATTERN_R_FX1_TYPE      16
#define GUI_PATTERN_R_FX1_VALUE     17
#define GUI_PATTERN_R_FX2_TYPE      18
#define GUI_PATTERN_R_FX2_VALUE     19
#define GUI_PATTERN_R_FX3_TYPE      20
#define GUI_PATTERN_R_FX3_VALUE     21

#define GUI_PATTERN_R_FX4_TYPE      22
#define GUI_PATTERN_R_FX4_VALUE     23
#define GUI_PATTERN_R_FX5_TYPE      24
#define GUI_PATTERN_R_FX5_VALUE     25
#define GUI_PATTERN_R_FX6_TYPE      26
#define GUI_PATTERN_R_FX6_VALUE     27

#define GUI_PATTERN_COLUMN_LAST     27
#define GUI_PATTERN_SIDE_ROWS       14

#define GUI_PATTERN_R_NOTE_OFFSET   6
#define GUI_PATTERN_R_INST_OFFSET   20
#define GUI_PATTERN_R_FX_OFFSET     52

// -----------------------------------------------------------
#define GUI_PATTERN_ROW_LAST        15
#define GUI_FX_TYPE_START           48
#define GUI_FX_VALUE_START          49
#define GUI_INFO_PRINT_X            41
#define GUI_INFO_PRINT_Y            24
#define GUI_INFO_PRINT_INST_X       47
#define GUI_INFO_PRINT_INST_Y       23
#define GUI_INST_NAME_START         97
// -----------------------------------------------------------
#define PATTEN_ROWS_PER_SIDE        16
// -----------------------------------------------------------
#define GUI_VERSION                 0
#define GUI_PSG_SQUARE              4
#define GUI_FM                      5
#define GUI_FM_OP                   6
#define GUI_PSG_NOISE               7
#define GUI_LOGO                    8
#define GUI_FX_SYM                  15
#define GUI_ALG_CORNER_UR           16
#define GUI_ALG_CORNER_UDR          17
#define GUI_ALG_CORNER_LD           18
#define GUI_ALG_CORNER_LU           19
#define GUI_ALG_CORNER_DR           20
#define GUI_ALG_CORNER_PLUS         21
#define GUI_ALG_LINE_H              22
#define GUI_SEQ_VOL_MAX_ATT         23
#define GUI_SLASH                   24
#define GUI_LOWLINE                 25
#define GUI_MUTE                    26
#define GUI_SEQ_VOL_MIN_ATT         27
#define GUI_PLAYCURSOR              28
#define GUI_ARROW                   29
#define GUI_CURSOR                  30
#define GUI_SLASH_FAT               31
#define GUI_UP1                     32
//#define GUI_SONG_TRANSPOSE          33
#define GUI_DOWN1                   34
//#define GUI_DOWN2                   35
#define GUI_MINUS                   36
#define GUI_COLON                   37
#define GUI_HASH                    38
#define GUI_BIGDOT                  39
#define GUI_PLUS                    40
#define GUI_SEPARATOR               41
#define GUI_FM_CH3                  48
// -----------------------------------------------------------
#define GUI_DIGIT_0                 0
#define GUI_DIGIT_1                 1
#define GUI_DIGIT_2                 2
#define GUI_DIGIT_3                 3
#define GUI_DIGIT_4                 4
#define GUI_DIGIT_5                 5
#define GUI_DIGIT_6                 6
#define GUI_DIGIT_7                 7
#define GUI_DIGIT_8                 8
#define GUI_DIGIT_9                 9
// -----------------------------------------------------------
#define GUI_LETTER_A                10
#define GUI_LETTER_B                11
#define GUI_LETTER_C                12
#define GUI_LETTER_D                13
#define GUI_LETTER_E                14
#define GUI_LETTER_F                15
#define GUI_LETTER_G                16
#define GUI_LETTER_H                17
#define GUI_LETTER_I                18
#define GUI_LETTER_J                19
#define GUI_LETTER_K                20
#define GUI_LETTER_L                21
#define GUI_LETTER_M                22
#define GUI_LETTER_N                23
#define GUI_LETTER_O                24
#define GUI_LETTER_P                25
#define GUI_LETTER_Q                26
#define GUI_LETTER_R                27
#define GUI_LETTER_S                28
#define GUI_LETTER_T                29
#define GUI_LETTER_U                30
#define GUI_LETTER_V                31
#define GUI_LETTER_W                32
#define GUI_LETTER_X                33
#define GUI_LETTER_Y                34
#define GUI_LETTER_Z                35
// -----------------------------------------------------------
#define GUI_MATRIX_1XX              0
#define GUI_MATRIX_2XX              1
#define GUI_MATRIX_3XX              2
#define GUI_SSG                     6
#define GUI_LOOP_START              14
#define GUI_LOOP_END                15
#define GUI_TRANSPOSE               28
#define GUI_00                      41
#define GUI_MUL                     42
#define GUI_BPM                     56
#define GUI_FPS                     58
#define GUI_PPS                     60
#define GUI_SRAM                    58
#define GUI_PPL                     62
#define GUI_UNSAVED                 2
// -----------------------------------------------------------
#define GUI_NAVIGATION_SPEED_COUNTER    3
#define GUI_NAVIGATION_DELAY            15
#define GUI_PATTERN_COLORS_MAX          63

#define GUI_SAMPLE_NAME_SIZE            14
#define GUI_PATTERN_SAMPLE_NAME_XPOS    56
#define GUI_PATTERN_SAMPLE_NAME_YPOS    23

// ---------------------------------------------------------------------------
// Shared globals (defined in main.c)
// ---------------------------------------------------------------------------
extern u8 playingMatrixRow;
extern u8 selectedMatrixScreenRow;
extern u8 selectedMatrixRow;
extern u8 selectedMatrixChannel;
extern s8 currentPage;
extern u8 selectedPatternRow;
extern u8 selectedPatternColumn;
extern u16 selectedPatternID;
extern u8 playingPatternRow;
extern u8 patternSize;
extern u8 selectedInstrumentID;
extern u8 selectedInstrumentParameter;
extern u8 selectedInstrumentOperator;
extern u8 selectedSampleBank;
extern u8 selectedSampleNote;
extern u8 currentScreen;
extern bool bRefreshScreen;
extern u8 patternRowToRefresh;
extern u16 matrixRowToRefresh;
extern u8 instrumentParameterToRefresh;
extern char str[10];
extern u8 loopStart;
extern u8 loopEnd;
extern bool bPlayback;
extern u32 BPM;
extern f32 fBPM;
extern u8 ppl_1;
extern u8 ppl_2;
extern u8 useExternalSync;
extern u32 sramUsed;
extern u8 channelFlags[];
extern u8 instrumentIsMuted[];
extern u8 midiPreset;

// ---------------------------------------------------------------------------
// GUI-owned globals (defined in MDT_GUI.c)
// ---------------------------------------------------------------------------
extern u8 line;
extern u8 chan;
extern bool bInitScreen;
extern u16 bgBaseTileIndex[5];
extern u16 asciiBaseLetters;
extern u16 asciiBaseNumbers;
extern u8 instCopyTo;
extern s8 buttonCounter;
extern bool bDoCount;
extern u8 navigationDirection;
extern u16 patternCopyFrom;
extern s8 patternCopyRangeStart;
extern s8 patternCopyRangeEnd;
extern char sampleName[14];
extern bool bReColorsAndTranspose;
extern u8 rcat_ch;
extern u8 rcat_row;

// ---------------------------------------------------------------------------
// GUI function prototypes
// ---------------------------------------------------------------------------
extern const s16 GUI_ALPHABET[38];
void DrawMute(u8 mtxCh);
void DrawStaticGUI();
void DrawInfo();
void DrawBPM();
void DisplayPatternMatrix();
void DisplayPatternEditor();
void DisplayInstrumentEditor();
void DrawSelectionCursor(u8 x, u8 y, u8 bClear);
void DrawMatrixPlaybackCursor(u8 bClear, u8 palette, s8 offset, u8 row);
void ClearPatternPlaybackCursor();
void DrawPatternPlaybackCursor();
void NavigateMatrix(u8 direction);
void NavigatePattern(u8 direction);
void NavigateInstrument(u8 direction);
void RedrawMarks();
void ReColorsAndTranspose();
void DisplaySampleName(u8 xPos, u8 yPos, u8 note, u8 bank);
void ForceResetGUI();
//void DrawUnsavedMark(u8 dirty, u8 screen);

#endif // MDT_GUI_H_INCLUDED
