// sgdk
#include <genesis.h>
#include <sram.h>

// krikzz
#include <ssf.h>

// res
#include "fontdata.h"
#include "samples.h"
#include "graphics.h"

// inc
#include "MDT_Main.h"
#include "MDT_SRAM.h"
#include "MDT_Chips.h"
#include "MDT_NoteTables.h"
#include "MDT_ModuleData.h"
#include "MDT_GUI.h"
#include "MDT_PCM.h"
#include "MDT_Info.h"
#include "MDT_Presets.h"
#include "MDT_Version.h"

#define MDT_HEADER              "MDT100" // 4D 00 44 00 54 00 31 00 30 00 30 00
#define STRING_EMPTY            ""
#define H_INT_DURATION_NTSC     744     // ; 744
#define H_INT_DURATION_PAL      892     // ; 892

#define TICK_SKIP_MIN           0x01       // fast tempo limit, 1 tick = H_INT_SKIP h-Blanks; 6
#define TICK_SKIP_MAX           0xFF    // slow tempo limit; 128

u8 H_INT_SKIP = 2;       // 1 == no skip

bool bWriteRegs = TRUE;

u8 loopStart = OXFF; // matrix loop region
u8 loopEnd = OXFF;

u8 playingMatrixRow = 0; // current played line
u8 selectedMatrixScreenRow = 0; // selected matrix line on SCREEN
u8 selectedMatrixRow = 0; // selected pattern matrix index according to page
u8 selectedMatrixChannel = 0; // playback channel
u8 updateCursor = 1; // playback cursor
s8 currentPage = 0; // pattern matrix page

u8 selectedPatternRow = 0; // 0 .. 15 + patternColumnShift
u8 selectedPatternColumn = 0;
u16 selectedPatternID = 0;

u8 patternRowToRefresh = OXFF; // to refresh only edited pattern line; 255 = refresh everything
u16 matrixRowToRefresh = OXFFFF; //! change
u8 instrumentParameterToRefresh = OXFF; // 255-- counter to refresh one instrument parameter per frame; avoid slowdown

u8 lastEnteredNote = 45; // A-3 by default
u8 lastEnteredEffect = 1; // TL1
u8 lastEnteredEffectValue = 1;
u8 lastEnteredInstrumentID = 1;
u16 lastEnteredPattern = 1;

u8 line = 0; // redraw screen data line counter
u8 chan = 0; // draw only one cell per VBlank to avoid slowdown?

u8 playingPatternRow = 0; // current played pattern row

s8 channelTranspose[CHANNELS_TOTAL];

u16 channelPlayingPatternID[CHANNELS_TOTAL];
s8 channelMatrixTranspose[CHANNELS_TOTAL];

u8 channelPreviousInstrument[CHANNELS_TOTAL];
u8 channelPreviousEffectType[CHANNELS_TOTAL][EFFECTS_TOTAL];
u8 channelPreviousNote[CHANNELS_TOTAL];
u8 channelArpSeqID[CHANNELS_TOTAL];
u8 channelArpSeqMODE[CHANNELS_TOTAL];
u8 channelVolSeqID[CHANNELS_TOTAL];
u8 channelVolSeqMODE[CHANNELS_TOTAL];
u8 channelCurrentRowNote[CHANNELS_TOTAL]; // affected by channelMatrixTranspose
u8 channelNoteAutoCut[CHANNELS_TOTAL];

u8 channelSEQCounter_VOL[CHANNELS_TOTAL];
u8 channelSEQCounter_ARP[CHANNELS_TOTAL];

u8 channelRowShift[CHANNELS_TOTAL][PATTERN_ROWS];

u8 selectedInstrumentID = 1; // 0 instrument is empty
u8 selectedInstrumentParameter = 0; // 0..53
u8 selectedInstrumentOperator = 0; // 0..53

u8 seqArpValue[INSTRUMENTS_TOTAL][SEQ_STEP_LAST+1];
u8 seqVolValue[INSTRUMENTS_TOTAL][SEQ_STEP_LAST+1];

// screen
u8 currentScreen = SCREEN_MATRIX;
bool bInitScreen = TRUE; // redraw selection brackets
bool bRefreshScreen = TRUE; // refresh data of current screen

char str[10]; //! symbol buffer !!!may cause crash if overflowed!!!

// engine
u8 FM_CH3_Mode = CH3_NORMAL; // global
u8 FM_CH3_OpNoteStatus = 0b00000010; // ch3 each of operators status, note on or off
u8 PSG_NoiseMode = PSG_NOISE_TYPE_PERIODIC;

bool bPlayback = FALSE;
u8 ppl_1 = PPL_DEFAULT; // pulse per line
u8 ppl_2 = PPL_DEFAULT;
u8 maxPulse = PPL_DEFAULT;
s16 pulseCounter = 0;

// channel effects
u8 channelFlags[CHANNELS_TOTAL] = {1,1,1,1,1,1,1,1,1,1,1,1,1};
s16 channelArp[CHANNELS_TOTAL];

s8 channelMicrotone[CHANNELS_TOTAL];
s8 channelPitchSlideSpeed[CHANNELS_TOTAL];
s8 channelPitchSlideValue[CHANNELS_TOTAL];
s8 channelPitchSkipStep[CHANNELS_TOTAL];
s8 channelPitchSkipStepCounter[CHANNELS_TOTAL];
s8 channelModNotePitch[CHANNELS_TOTAL];

s8 channelVibrato[CHANNELS_TOTAL];
u8 channelVibratoMode[CHANNELS_TOTAL];
u16 channelVibratoDepth[CHANNELS_TOTAL]; // (u4) * channelVibratoDepthMult
u8 channelVibratoDepthMult[CHANNELS_TOTAL];
u16 channelVibratoSpeed[CHANNELS_TOTAL]; // (u4) * channelVibratoSpeedMult
u8 channelVibratoSpeedMult[CHANNELS_TOTAL];
u16 channelVibratoPhase[CHANNELS_TOTAL];
s8 channelFinalPitch[CHANNELS_TOTAL];
s8 channelModNoteVibrato[CHANNELS_TOTAL];

u8 channelTremoloDepth[CHANNELS_TOTAL];
u8 channelTremoloSpeed[CHANNELS_TOTAL];
u8 channelTremoloSpeedMult[CHANNELS_TOTAL];
u16 channelTremoloPhase[CHANNELS_TOTAL];
u8 channelTremolo[CHANNELS_TOTAL];

u8 channelBaseVolume[CHANNELS_TOTAL];
u8 channelSeqAttenuation[CHANNELS_TOTAL];
s16 channelAttenuation[CHANNELS_TOTAL];
u8 channelSlotBaseLevel[CHANNELS_TOTAL][4]; // need less
s16 channelVolumeChangeSpeed[CHANNELS_TOTAL];
u8 channelVolumePulseSkip[CHANNELS_TOTAL];
u8 channelVolumePulseCounter[CHANNELS_TOTAL];

u8 channelNoteCut[CHANNELS_TOTAL];
u8 channelNoteRetrigger[CHANNELS_TOTAL];
u8 channelNoteRetriggerCounter[CHANNELS_TOTAL];

bool bPsgIsPlayingNote[4];

s16 matrixRowJumpTo = OXFF;
u8 patternRowJumpTo = OXFF;
u8 channelNoteDelayCounter[CHANNELS_TOTAL];

u8 FM_CH3_OpFreq[4];

u8 instrumentIsMuted[INSTRUMENTS_TOTAL]; // 1 = mute

// FM CH6 DAC
u32 samplesSize = 0;
u8 sampleToPlay = 0;
u32 sampleSeekTime = 0;

u8 selectedSampleBank = 0;
u8 selectedSampleNote = 0;
u32 sampleBankSize = 0;
u8 activeSampleBank = 0;

const u8* sampleStart[4][NOTES];
u32 sampleLength[4][NOTES];
u8 sampleRate[4][NOTES];
u8 samplePan[4][NOTES];
bool sampleLoop[4][NOTES];

bool bDAC_enable = TRUE; // global, 0xF0 to enable. 0 to disable

// copy/paste
u16 patternCopyFrom = 1;
s8 patternCopyRangeStart = NOTHING;
s8 patternCopyRangeEnd = NOTHING;

u16 hIntToSkip = 0;
u16 hIntCounter = 0;
bool bDoPulse = FALSE;

u16 bgBaseTileIndex[4];
u16 asciiBaseLetters, asciiBaseNumbers;
u8 instCopyTo = 0x01; // instrument copy

bool bBusTaken = FALSE;

s8 buttonCounter = GUI_NAVIGATION_DELAY; // signed just in case of overflow
bool bDoCount = FALSE;
u8 navigationDirection = BUTTON_RIGHT;

u8 FM_CH6_DAC_Pan = SOUND_PAN_CENTER;

u32 FPS = 0;
u32 BPM = 0;
f32 fBPM = 0; // very bad precision
u32 PPS = 0;

u8 patternSize = 0x1F;

Instrument tmpInst[INSTRUMENTS_TOTAL]; // cache instruments to RAM for faster access
Instrument chInst[CHANNELS_TOTAL]; // to apply commands; FM only

u8 midiPreset = 0;

u16 msu_drv();
vu16 *mcd_cmd = (vu16 *) 0xA12010;  // command
vu32 *mcd_arg = (vu32 *) 0xA12012;  // argument
vu8 *mcd_cmd_ck = (vu8 *) 0xA1201F; // increment for command execution
vu8 *mcd_stat = (vu8 *) 0xA12020;   // Driver ready for commands processing when 0xA12020 sets to 0
u16 msu_resp;

static const u8 GUI_PATTERNCOLORS[14] = { 42, 43, 44, 45, 46, 47, 56, 57, 58, 59, 60, 61, 62, 63 };

static const u8 GUI_FM_ALG_GRID[8][4][12] =
{
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_3, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_2, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_CORNER_LD, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, GUI_ALG_CORNER_UDR, GUI_FM_OP, GUI_DIGIT_2, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0 },
        { GUI_FM_OP, GUI_DIGIT_3, GUI_ALG_CORNER_LU, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_CORNER_LD, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, GUI_ALG_CORNER_UDR, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0 },
        { GUI_FM_OP, GUI_DIGIT_3, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_2, GUI_ALG_CORNER_LU, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_3, GUI_ALG_CORNER_LD, 0, 0, 0, 0, 0, 0, },
        { 0, 0, 0, 0, 0, GUI_ALG_CORNER_UDR, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0, },
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_2, GUI_ALG_CORNER_LU, 0, 0, 0, 0, 0, 0, },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_3, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { GUI_FM_OP, GUI_DIGIT_2, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, GUI_ALG_CORNER_DR, GUI_FM_OP, GUI_DIGIT_3, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_CORNER_PLUS, GUI_FM_OP, GUI_DIGIT_2, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, GUI_ALG_CORNER_UR, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { GUI_FM_OP, GUI_DIGIT_1, GUI_ALG_LINE_H, GUI_FM_OP, GUI_DIGIT_3, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_2, GUI_ARROW, 0, 0, 0, 0, 0, 0, },
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0, 0, 0, 0, },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_1, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_2, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_3, GUI_ARROW, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, GUI_FM_OP, GUI_DIGIT_4, GUI_ARROW, 0, 0, 0, 0, 0, 0 }
    },
};

static const u8 GUI_NOTE_NAMES[2][12] =
{
    {
        GUI_LETTER_C, GUI_LETTER_C,
        GUI_LETTER_D, GUI_LETTER_D,
        GUI_LETTER_E,
        GUI_LETTER_F, GUI_LETTER_F,
        GUI_LETTER_G, GUI_LETTER_G,
        GUI_LETTER_A, GUI_LETTER_A,
        GUI_LETTER_B
    },
    {
        GUI_MINUS, GUI_HASH,
        GUI_MINUS, GUI_HASH,
        GUI_MINUS,
        GUI_MINUS, GUI_HASH,
        GUI_MINUS, GUI_HASH,
        GUI_MINUS, GUI_HASH,
        GUI_MINUS
    }
};

static const s16 GUI_ALPHABET[38] =
{
    GUI_MINUS + 36,
    GUI_LETTER_A,
    GUI_LETTER_B,
    GUI_LETTER_C,
    GUI_LETTER_D,
    GUI_LETTER_E,
    GUI_LETTER_F,
    GUI_LETTER_G,
    GUI_LETTER_H,
    GUI_LETTER_I,
    GUI_LETTER_J,
    GUI_LETTER_K,
    GUI_LETTER_L,
    GUI_LETTER_M,
    GUI_LETTER_N,
    GUI_LETTER_O,
    GUI_LETTER_P,
    GUI_LETTER_Q,
    GUI_LETTER_R,
    GUI_LETTER_S,
    GUI_LETTER_T,
    GUI_LETTER_U,
    GUI_LETTER_V,
    GUI_LETTER_W,
    GUI_LETTER_X,
    GUI_LETTER_Y,
    GUI_LETTER_Z,
    -282,
    GUI_DIGIT_0,
    GUI_DIGIT_1,
    GUI_DIGIT_2,
    GUI_DIGIT_3,
    GUI_DIGIT_4,
    GUI_DIGIT_5,
    GUI_DIGIT_6,
    GUI_DIGIT_7,
    GUI_DIGIT_8,
    GUI_DIGIT_9
};

int main(bool hardReset)
{
    if (!hardReset) SYS_hardReset(); // clear on soft reset
    ForceResetVariables();
    InitPresets();
    InitInfo();
    InitTracker();
	while(1)
    {
        DoEngine();

        switch (currentScreen)
        {
        case SCREEN_MATRIX: DisplayPatternMatrix(); break;
        case SCREEN_PATTERN: DisplayPatternEditor(); break;
        case SCREEN_INSTRUMENT: DisplayInstrumentEditor(); break;
        default: break;
        }
        FPS = SYS_getFPS();
    }
	return(0);
}

static inline void hIntCallback()
{
    hIntCounter--;
    if (!hIntCounter)
    {
        bDoPulse = TRUE;
        hIntCounter = hIntToSkip;
    }
}

static inline void vIntCallback()
{
    static u32 _fps; // redraw only if FPS changes

    SYS_doVBlankProcessEx(ON_VBLANK);

    if (_fps != FPS) { _fps = FPS; uintToStr(FPS, str, 3); DrawNum(BG_A, PAL1, str, 15, 27); DrawNum(BG_A, PAL1, str, 55, 27); }

    // fast navigation
    if (bDoCount)
    {
        buttonCounter--;
        if (buttonCounter < 1)
        {
            buttonCounter = GUI_NAVIGATION_SPEED_COUNTER;
            switch (currentScreen)
            {
            case SCREEN_MATRIX: NavigateMatrix(navigationDirection); break;
            case SCREEN_PATTERN: NavigatePattern(navigationDirection); break;
            case SCREEN_INSTRUMENT: NavigateInstrument(navigationDirection); break;
            default: break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------------------
void NavigateMatrix(u8 direction)
{
    switch (direction)
    {
    case BUTTON_LEFT:
        if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) { // not BPM
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
            if (selectedMatrixChannel > CHANNEL_FM1) selectedMatrixChannel--; else selectedMatrixChannel = CHANNEL_LAST;
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        }
        break;
    case BUTTON_RIGHT:
        if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) { // not BPM
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
            if (selectedMatrixChannel < CHANNELS_TOTAL-1) selectedMatrixChannel++; else selectedMatrixChannel = CHANNEL_FM1;
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        }
        break;
    case BUTTON_UP:
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
        if (selectedMatrixScreenRow > 0) selectedMatrixScreenRow--; else selectedMatrixScreenRow = MATRIX_ROWS_ONPAGE_LAST;
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        selectedMatrixRow = selectedMatrixScreenRow + currentPage * MATRIX_ROWS_ONPAGE;
        break;
    case BUTTON_DOWN:
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
        if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) selectedMatrixScreenRow++; else selectedMatrixScreenRow = 0;
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        selectedMatrixRow = selectedMatrixScreenRow + currentPage * MATRIX_ROWS_ONPAGE;
        break;
    }
}
// -------------------------------------------------------------------------------------------------------------
void NavigatePattern(u8 direction)
{
    switch (direction)
    {
    case BUTTON_LEFT:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternColumn > 0) selectedPatternColumn--;
        else selectedPatternColumn = GUI_PATTERN_COLUMN_LAST; // jump right
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case BUTTON_RIGHT:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternColumn < GUI_PATTERN_COLUMN_LAST) selectedPatternColumn++;
        else selectedPatternColumn = 0; // jump left
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case BUTTON_UP:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternRow > 0) selectedPatternRow--;
        else selectedPatternRow = GUI_PATTERN_ROW_LAST; // jump down; 0-15 = pattern;
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case BUTTON_DOWN:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternRow < GUI_PATTERN_ROW_LAST) selectedPatternRow++; // MAX_PATTERN_LINES - 16
        else selectedPatternRow = 0; // jump up
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case PATTERN_JUMPSIDETRIGGER: // jump to other side
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternColumn < 14) selectedPatternColumn += 14;
        else selectedPatternColumn -= 14;
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    }
}
// -------------------------------------------------------------------------------------------------------------
void NavigateInstrument(u8 direction)
{
    switch (direction)
    {
    case BUTTON_LEFT:
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, TRUE);

        switch (selectedInstrumentParameter)
        {
        case GUI_INST_PARAM_ALG:
        case GUI_INST_PARAM_FMS:
        case GUI_INST_PARAM_AMS:
        case GUI_INST_PARAM_PAN:
        case GUI_INST_PARAM_FB:
        case GUI_INST_PARAM_LFO:
        case GUI_INST_PARAM_PCM_BANK:
        case GUI_INST_PARAM_PCM_NOTE:
        case GUI_INST_PARAM_PCM_LOOP:
        case GUI_INST_PARAM_COPY:
            selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_TL:
        case GUI_INST_PARAM_RS:
        case GUI_INST_PARAM_MUL:
        case GUI_INST_PARAM_DT:
        case GUI_INST_PARAM_AR:
        case GUI_INST_PARAM_D1R:
        case GUI_INST_PARAM_D1L:
        case GUI_INST_PARAM_D2R:
        case GUI_INST_PARAM_RR:
        case GUI_INST_PARAM_AM:
        case GUI_INST_PARAM_SSGEG:
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = 3;
            break;
        case GUI_INST_PARAM_VOLSEQ:
        case GUI_INST_PARAM_ARPSEQ:
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = SEQ_STEP_LAST;
            break;
        case GUI_INST_PARAM_NAME:
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = 7;
            break;
        case GUI_INST_PARAM_PCM_START:
        case GUI_INST_PARAM_PCM_END:
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = 2;
            break;
        }

        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, FALSE);
        break;

    case BUTTON_RIGHT:
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, TRUE);

        switch (selectedInstrumentParameter)
        {
        case GUI_INST_PARAM_ALG:
        case GUI_INST_PARAM_FMS:
        case GUI_INST_PARAM_AMS:
        case GUI_INST_PARAM_PAN:
        case GUI_INST_PARAM_FB:
        case GUI_INST_PARAM_LFO:
        case GUI_INST_PARAM_PCM_BANK:
        case GUI_INST_PARAM_PCM_NOTE:
        case GUI_INST_PARAM_PCM_LOOP:
        case GUI_INST_PARAM_COPY:
            selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_TL:
        case GUI_INST_PARAM_RS:
        case GUI_INST_PARAM_MUL:
        case GUI_INST_PARAM_DT:
        case GUI_INST_PARAM_AR:
        case GUI_INST_PARAM_D1R:
        case GUI_INST_PARAM_D1L:
        case GUI_INST_PARAM_D2R:
        case GUI_INST_PARAM_RR:
        case GUI_INST_PARAM_AM:
        case GUI_INST_PARAM_SSGEG:
            if (selectedInstrumentOperator < 3) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_VOLSEQ:
        case GUI_INST_PARAM_ARPSEQ:
            if (selectedInstrumentOperator < SEQ_STEP_LAST) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_NAME:
            if (selectedInstrumentOperator < 7) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_PCM_START:
        case GUI_INST_PARAM_PCM_END:
            if (selectedInstrumentOperator < 2) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
            break;
        }

        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, FALSE);
        break;

    case BUTTON_UP:
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, TRUE);
        switch (selectedInstrumentParameter)
        {
        case GUI_INST_PARAM_ALG:
        case GUI_INST_PARAM_NAME:
        case GUI_INST_PARAM_COPY:
        case GUI_INST_PARAM_PCM_BANK:
            selectedInstrumentOperator = 0;
            break;
        }

        if (selectedInstrumentParameter > 0) selectedInstrumentParameter--; else selectedInstrumentParameter = GUI_INST_MAX_PARAMETER; // jump down
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, FALSE);
        break;

    case BUTTON_DOWN:
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, TRUE);
        switch (selectedInstrumentParameter)
        {
        case GUI_INST_PARAM_ARPSEQ:
        case GUI_INST_PARAM_NAME:
        case GUI_INST_PARAM_COPY:
        case GUI_INST_PARAM_PCM_END:
            selectedInstrumentOperator = 0;
            break;
        }

        if (selectedInstrumentParameter < GUI_INST_MAX_PARAMETER) selectedInstrumentParameter++; else selectedInstrumentParameter = 0; // jump up
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, FALSE);
        break;
    }
}
// -------------------------------------------------------------------------------------------------------------
static inline void DoEngine()
{
    static u8 _seqValue = 0;
    static u8 _fxType = 0;
    static u8 _fxValue = 0;
    static bool _bBeginPlay = TRUE;
    static u8 _inst = 0;
    static s16 _key = 0;
    static s8 _test = 0;

    // vibrato tool
    auto s8 vibrato(u8 mtxCh) {
        static s8 vib = 0;
        switch (channelVibratoMode[mtxCh])
        {
        case 1:
            vib = abs((s8)fix16ToRoundedInt(fix16Mul(FIX16(channelVibratoDepth[mtxCh]), sinFix16(channelVibratoPhase[mtxCh]))));
            break;
        case 2:
            vib = -abs((s8)fix16ToRoundedInt(fix16Mul(FIX16(channelVibratoDepth[mtxCh]), sinFix16(channelVibratoPhase[mtxCh]))));
            break;
        default:
            vib = (s8)fix16ToRoundedInt(fix16Mul(FIX16(channelVibratoDepth[mtxCh]), sinFix16(channelVibratoPhase[mtxCh])));
            break;
        }

        channelVibratoPhase[mtxCh] += channelVibratoSpeed[mtxCh];
        if (channelVibratoPhase[mtxCh] > 1023) channelVibratoPhase[mtxCh] -= 1024;

        return vib;
    }

    auto inline void seq_vol(u8 mtxCh) {
        if (!channelVolSeqMODE[mtxCh] || (channelVolSeqMODE[mtxCh] && (channelSEQCounter_VOL[mtxCh] < SEQ_STEP_LAST)))
        {
            _seqValue = seqVolValue[channelVolSeqID[mtxCh]][channelSEQCounter_VOL[mtxCh]]; channelSEQCounter_VOL[mtxCh]++;
            if (_seqValue != SEQ_VOL_SKIP)
            {
                channelSeqAttenuation[mtxCh] = _seqValue;
                SetChannelVolume(mtxCh);
            }
        }
        if (!channelVolSeqMODE[mtxCh] && (channelSEQCounter_VOL[mtxCh] > SEQ_STEP_LAST)) channelSEQCounter_VOL[mtxCh] = 0;
    }

    auto inline void seq_arp(u8 mtxCh) {
        if (!channelArpSeqMODE[mtxCh] || (channelArpSeqMODE[mtxCh] && (channelSEQCounter_ARP[mtxCh] < SEQ_STEP_LAST)))
        {
            _seqValue = seqArpValue[channelArpSeqID[mtxCh]][channelSEQCounter_ARP[mtxCh]]; channelSEQCounter_ARP[mtxCh]++;
            if (!pulseCounter)
            {
                if (_seqValue != NOTE_EMPTY)
                {
                    if (_seqValue > ARP_BASE) _key = channelCurrentRowNote[mtxCh] + (_seqValue - ARP_BASE);
                    else if (_seqValue < ARP_BASE) _key = channelCurrentRowNote[mtxCh] - (ARP_BASE - _seqValue);
                    if (_key < 0 || _key > NOTE_MAX || _seqValue == ARP_BASE) _key = channelCurrentRowNote[mtxCh];
                }
            }
            else
            {
                if (_seqValue != NOTE_EMPTY)
                {
                    if (_seqValue > ARP_BASE) channelArp[mtxCh] = channelPreviousNote[mtxCh] + (_seqValue - ARP_BASE);
                    else if (_seqValue < ARP_BASE) channelArp[mtxCh] = channelPreviousNote[mtxCh] - (ARP_BASE - _seqValue);
                    if (channelArp[mtxCh] < 0 || channelArp[mtxCh] > NOTE_MAX || _seqValue == ARP_BASE) channelArp[mtxCh] = channelPreviousNote[mtxCh];
                    PlayNote(channelArp[mtxCh], mtxCh);
                }
            }
        }
        if (!channelArpSeqMODE[mtxCh] && (channelSEQCounter_ARP[mtxCh] > SEQ_STEP_LAST)) channelSEQCounter_ARP[mtxCh] = 0;
    }

    auto inline void do_row(u8 mtxCh) {
        if (channelFlags[mtxCh])
        {
            auto inline void command(u8 type, u8 val, u8 effect) {
                _fxType = SRAM_ReadPattern(channelPlayingPatternID[mtxCh], playingPatternRow, type);
                _fxValue = SRAM_ReadPattern(channelPlayingPatternID[mtxCh], playingPatternRow, val);

                if (_fxType)
                {
                    channelPreviousEffectType[mtxCh][effect] = _fxType;
                    switch (mtxCh)
                    {
                    case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM4: case CHANNEL_FM5:
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], _fxType, _fxValue);
                        break;
                    case CHANNEL_FM6_DAC:
                        ApplyCommand_DAC(_fxType, _fxValue);
                        if (!bDAC_enable) ApplyCommand_FM(CHANNEL_FM6_DAC, channelPreviousInstrument[CHANNEL_FM6_DAC], _fxType, _fxValue);
                        break;
                    case CHANNEL_FM3_OP1: case CHANNEL_FM3_OP2: case CHANNEL_FM3_OP3:
                        ApplyCommand_FM3_SP(mtxCh, _fxType, _fxValue);
                        break;
                    case CHANNEL_FM3_OP4:
                        ApplyCommand_FM3_SP(mtxCh, _fxType, _fxValue);
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], _fxType, _fxValue);
                        break;
                    default: ApplyCommand_PSG(_fxType, _fxValue); break;
                    }
                    ApplyCommand_Common(mtxCh, _fxType, _fxValue);
                }
                else if (_fxValue)
                {
                    switch (mtxCh)
                    {
                    case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM4: case CHANNEL_FM5:
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], channelPreviousEffectType[mtxCh][effect], _fxValue);
                        break;
                    case CHANNEL_FM6_DAC:
                        ApplyCommand_DAC(channelPreviousEffectType[CHANNEL_FM6_DAC][effect], _fxValue);
                        if (!bDAC_enable) ApplyCommand_FM(CHANNEL_FM6_DAC, channelPreviousInstrument[CHANNEL_FM6_DAC], channelPreviousEffectType[CHANNEL_FM6_DAC][effect], _fxValue);
                        break;
                    case CHANNEL_FM3_OP1: case CHANNEL_FM3_OP2: case CHANNEL_FM3_OP3:
                        ApplyCommand_FM3_SP(mtxCh, channelPreviousEffectType[mtxCh][effect], _fxValue); // currently useless
                        break;
                    case CHANNEL_FM3_OP4:
                        ApplyCommand_FM3_SP(mtxCh, channelPreviousEffectType[mtxCh][effect], _fxValue);
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], channelPreviousEffectType[mtxCh][effect], _fxValue);
                        break;
                    default: ApplyCommand_PSG(channelPreviousEffectType[mtxCh][effect], _fxValue); break;
                    }
                    ApplyCommand_Common(mtxCh, channelPreviousEffectType[mtxCh][effect], _fxValue);
                }
            }

            auto inline void apply_commands() {
                command(DATA_FX1_TYPE, DATA_FX1_VALUE, 0);
                command(DATA_FX2_TYPE, DATA_FX2_VALUE, 1);
                command(DATA_FX3_TYPE, DATA_FX3_VALUE, 2);
                command(DATA_FX4_TYPE, DATA_FX4_VALUE, 3);
                command(DATA_FX5_TYPE, DATA_FX5_VALUE, 4);
                command(DATA_FX6_TYPE, DATA_FX6_VALUE, 5);
            }

            _inst = SRAM_ReadPattern(channelPlayingPatternID[mtxCh], playingPatternRow, DATA_INSTRUMENT);

            if (instrumentIsMuted[_inst] == INST_MUTE) // check if instrument is muted. ignore writes, replace note with OFF if so
            {
                _inst = NULL; channelCurrentRowNote[mtxCh] = NOTE_OFF;
            }
            else
            {
                channelCurrentRowNote[mtxCh] = SRAM_ReadPattern(channelPlayingPatternID[mtxCh], playingPatternRow, DATA_NOTE);
            }

            // auto cut note before next note
            if (channelNoteAutoCut[mtxCh])
            {
                if (playingPatternRow == PATTERN_ROW_LAST)
                {
                    if (SRAM_ReadPattern(SRAM_ReadMatrix(mtxCh, playingMatrixRow+1), 0, DATA_NOTE) < NOTES)
                    {
                        channelNoteCut[mtxCh] = channelNoteAutoCut[mtxCh];
                    } else channelNoteCut[mtxCh] = 0;
                }
                else
                {
                    if (SRAM_ReadPattern(channelPlayingPatternID[mtxCh], playingPatternRow+1, DATA_NOTE) < NOTES)
                    {
                        channelNoteCut[mtxCh] = channelNoteAutoCut[mtxCh];
                    } else channelNoteCut[mtxCh] = 0;
                }
            }

            // commands
            if (_inst)
            {
                if (mtxCh < CHANNEL_PSG1) // FM
                {
                    if (_inst != channelPreviousInstrument[mtxCh] || currentScreen != SCREEN_MATRIX) // instrument changes or not on matrix screen
                    {
                        channelPreviousInstrument[mtxCh] = _inst;
                        chInst[mtxCh] = tmpInst[_inst]; // copy from cached preset
                        //StopChannelSound(mtxCh); //? declick
                        SetChannelBaseVolume_FM(mtxCh); // remember preset base TL levels
                        bWriteRegs = FALSE; // disable registers write for effects
                        apply_commands(); // only change chInst
                        if (!bWriteRegs) SetChannelVolume(mtxCh); // if not triggered from command, apply channel attenuation to new instrument
                        WriteYM2612(mtxCh); // rewrite all registers from chInst
                    }
                    else // do effects
                    {
                        bWriteRegs = TRUE; // write commands regs
                        apply_commands();
                    }
                }
                else // PSG
                {
                    channelPreviousInstrument[mtxCh] = _inst;
                    channelArpSeqID[mtxCh] = channelVolSeqID[mtxCh] = _inst; // set seq for PSG as instrument
                    apply_commands(); // will override PSG seq
                }
            }
            else // do effects
            {
                bWriteRegs = TRUE; // write commands regs
                apply_commands();
            }

            // --------- trigger note playback; check empty note later; pass note id: 0..95, 254, 255
            if (channelCurrentRowNote[mtxCh] == NOTE_OFF) // there is OFF
            {
                channelPreviousNote[mtxCh] = channelArp[mtxCh] = NOTE_OFF;
                if (channelNoteRetrigger[mtxCh])
                {
                    channelNoteRetriggerCounter[mtxCh] = channelNoteRetrigger[mtxCh] = 0;
                }
                if (channelRowShift[mtxCh][playingPatternRow]) channelNoteDelayCounter[mtxCh] = channelRowShift[mtxCh][playingPatternRow];
                PlayNoteOff(mtxCh);
            }
            else if (channelCurrentRowNote[mtxCh] < NOTES) // there is a note on a row
            {
                if (channelMatrixTranspose[mtxCh] || channelTranspose[mtxCh])
                {
                    _test = channelCurrentRowNote[mtxCh] + channelMatrixTranspose[mtxCh] + channelTranspose[mtxCh]; // check if out of notes range
                    if (_test < NOTES || _test > -1)
                    {
                        _key = channelPreviousNote[mtxCh] = channelArp[mtxCh] = channelCurrentRowNote[mtxCh] = _test;
                    }
                } else _key = channelPreviousNote[mtxCh] = channelArp[mtxCh] = channelCurrentRowNote[mtxCh];

                if (channelRowShift[mtxCh][playingPatternRow]) channelNoteDelayCounter[mtxCh] = channelRowShift[mtxCh][playingPatternRow];

                channelSEQCounter_VOL[mtxCh] = 0; channelSEQCounter_ARP[mtxCh] = 0;
                seq_vol(mtxCh); seq_arp(mtxCh);

                if (!channelNoteDelayCounter[mtxCh] && !channelNoteRetrigger[mtxCh]) // re-triggered from do_effects
                {
                    PlayNote((u8)_key, mtxCh);
                }
            }
            else _key = NOTE_EMPTY; // empty row
        }
    }

    auto inline void do_effects(u8 mtxCh) {
        if (channelFlags[mtxCh] && channelPreviousNote[mtxCh] < NOTES)
        {
            if (pulseCounter) // only do at sub-pulses; first pulse is separate before PlayNote
            {
                seq_vol(mtxCh); seq_arp(mtxCh);
            }

            if (channelNoteRetrigger[mtxCh])
            {
                channelNoteDelayCounter[mtxCh] = 0; // disable delay
                if (channelNoteRetriggerCounter[mtxCh] == channelNoteRetrigger[mtxCh])
                {
                    PlayNote(channelPreviousNote[mtxCh], mtxCh);
                    channelNoteRetriggerCounter[mtxCh] = 0;
                    channelSEQCounter_VOL[mtxCh] = 0; channelSEQCounter_ARP[mtxCh] = 0;
                }
                channelNoteRetriggerCounter[mtxCh]++;
            }
            //! delay (not work with seq)
            else if (channelNoteDelayCounter[mtxCh])
            {
                if (channelNoteDelayCounter[mtxCh] == 1)
                {
                    PlayNote(channelPreviousNote[mtxCh], mtxCh);
                    channelNoteDelayCounter[mtxCh] = 0;
                } else channelNoteDelayCounter[mtxCh]--;
            }
            // volume effects
            // volume slide (set only by counter)
            if (channelVolumeChangeSpeed[mtxCh])
            {
                if (!channelVolumePulseCounter[mtxCh])
                {
                    channelAttenuation[mtxCh] += channelVolumeChangeSpeed[mtxCh];
                    if (channelAttenuation[mtxCh] > 0x7F) { channelAttenuation[mtxCh] = 0x7F; channelVolumeChangeSpeed[mtxCh] = 0; }
                    else if (channelAttenuation[mtxCh] < 0) { channelAttenuation[mtxCh] = 0; channelVolumeChangeSpeed[mtxCh] = 0; }
                    channelVolumePulseCounter[mtxCh] = channelVolumePulseSkip[mtxCh];
                    if (!(channelTremoloDepth[mtxCh] && channelTremoloSpeed[mtxCh])) SetChannelVolume(mtxCh); //! set later if tremolo
                }
                channelVolumePulseCounter[mtxCh]--;
            }
            // tremolo (set by every pulse)
            if (channelTremoloDepth[mtxCh] && channelTremoloSpeed[mtxCh])
            {
                channelTremolo[mtxCh] = (u8)fix16ToRoundedInt
                (
                    fix16Mul(FIX16(channelTremoloDepth[mtxCh]), fix16Div(fix16Add(cosFix16(channelTremoloPhase[mtxCh]), FIX16(1)), FIX16(2)))
                );

                channelTremoloPhase[mtxCh] += channelTremoloSpeed[mtxCh];
                if (channelTremoloPhase[mtxCh] > 1023) channelTremoloPhase[mtxCh] -= 1024;
                if (!pulseCounter && channelCurrentRowNote[mtxCh] != NOTE_EMPTY) channelTremoloPhase[mtxCh] = TREMOLO_PHASE;
                SetChannelVolume(mtxCh);
            }

            // pitch effects
            if (channelPitchSlideSpeed[mtxCh] || (channelVibratoDepth[mtxCh] && channelVibratoSpeed[mtxCh]))
            {
                // pitch slide
                if (channelPitchSlideSpeed[mtxCh])
                {
                    if (channelPitchSkipStepCounter[mtxCh] < 1)
                    {
                        channelMicrotone[mtxCh] += channelPitchSlideSpeed[mtxCh];
                        while(channelMicrotone[mtxCh] >= MICROTONE_STEPS) // wrap
                        {
                            channelMicrotone[mtxCh] -= MICROTONE_STEPS;
                            channelModNotePitch[mtxCh]++;
                        }
                        while(channelMicrotone[mtxCh] < 0) // wrap
                        {
                            channelMicrotone[mtxCh] += MICROTONE_STEPS;
                            channelModNotePitch[mtxCh]--;
                        }
                        channelPitchSkipStepCounter[mtxCh] = channelPitchSkipStep[mtxCh]; // skip pulses for slower pitch slide
                    }
                    channelPitchSkipStepCounter[mtxCh]--;
                }
                // vibrato
                if (channelVibratoDepth[mtxCh] && channelVibratoSpeed[mtxCh])
                {
                    if (!pulseCounter && channelCurrentRowNote[mtxCh] != NOTE_EMPTY) channelVibratoPhase[mtxCh] = VIBRATO_PHASE;
                    channelVibrato[mtxCh] = vibrato(mtxCh);
                }
                else channelVibrato[mtxCh] = 0;

                channelFinalPitch[mtxCh] = channelMicrotone[mtxCh] + channelVibrato[mtxCh];
                // final pitch check
                if (channelFinalPitch[mtxCh] >= MICROTONE_STEPS)
                {
                    channelFinalPitch[mtxCh] -= MICROTONE_STEPS;
                    channelModNoteVibrato[mtxCh] = 1;
                }
                else if (channelFinalPitch[mtxCh] < 0)
                {
                    channelFinalPitch[mtxCh] += MICROTONE_STEPS;
                    channelModNoteVibrato[mtxCh] = -1;
                }
                else channelModNoteVibrato[mtxCh] = 0;

                if (mtxCh < CHANNEL_PSG1) SetPitchFM(mtxCh, channelArp[mtxCh]);
                else SetPitchPSG(mtxCh, channelArp[mtxCh]);
            }
            else channelFinalPitch[mtxCh] = 0;

            // cut
            if (channelNoteCut[mtxCh] > 1) channelNoteCut[mtxCh]--;
            else if (channelNoteCut[mtxCh] == 1)
            {
                if (FM_CH3_Mode == CH3_SPECIAL_CSM && mtxCh == CHANNEL_FM3_OP4) FM_CH3_Mode = CH3_SPECIAL_CSM_OFF;
                StopChannelSound(mtxCh);
                channelNoteCut[mtxCh] = 0;
                channelPreviousNote[mtxCh] = NOTE_OFF;
            }
        }
    }

    if (bPlayback)
    {
        if (_bBeginPlay)
        {
            SYS_disableInts();
            _bBeginPlay = FALSE;
            ReadMatrixRow();

            // cache instruments and seq
            for (u16 id = 0; id < INSTRUMENTS_TOTAL; id++)
            {
                for (u8 step = 0; step <= SEQ_STEP_LAST; step++)
                {
                    //seqVolValue[id][step] = SRAM_ReadInstrument(id, INST_VOL_TICK_01 + step);
                    //seqArpValue[id][step] = SRAM_ReadInstrument(id, INST_ARP_TICK_01 + step);
                    seqVolValue[id][step] = SRAM_ReadSEQ_VOL(id, step);
                    seqArpValue[id][step] = SRAM_ReadSEQ_ARP(id, step);
                }
                CacheIstrumentToRAM(id);
            }

            // set instruments
            for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
            {
                if (mtxCh < CHANNEL_PSG1) chInst[mtxCh] = tmpInst[channelPreviousInstrument[mtxCh]];
                else
                {
                    channelArpSeqID[mtxCh] = channelVolSeqID[mtxCh] = channelPreviousInstrument[mtxCh];
                }
            }

            // cache sample addresses
            for (u8 bank = 0; bank < 4; bank++)
            {
                for (u8 note = 0; note < NOTES; note++)
                {
                    u32 start, end = 0;
                    start =
                        (SRAM_ReadSampleRegion(bank, note, SAMPLE_START_1) << 16) |
                        (SRAM_ReadSampleRegion(bank, note, SAMPLE_START_2) << 8) |
                         SRAM_ReadSampleRegion(bank, note, SAMPLE_START_3);

                    end =
                        (SRAM_ReadSampleRegion(bank, note, SAMPLE_END_1) << 16) |
                        (SRAM_ReadSampleRegion(bank, note, SAMPLE_END_2) << 8) |
                         SRAM_ReadSampleRegion(bank, note, SAMPLE_END_3);

                    sampleStart[bank][note] = sample_bank_1 + start;
                    sampleLength[bank][note] = (sampleBankSize - start) - (sampleBankSize - end);
                    sampleRate[bank][note] = SRAM_ReadSampleRegion(bank, note, SAMPLE_RATE);
                    samplePan[bank][note] = SRAM_ReadSamplePan(bank, note);
                    sampleLoop[bank][note] = SRAM_ReadSampleRegion(bank, note, SAMPLE_LOOP);
                }
            }

            //! check and apply first found data if playing not from beginning of song !!! too long delay at later rows
            /*if (playingMatrixRow) // not first
            {
                for (u8 _ch = CHANNEL_FM1; _ch < CHANNELS_TOTAL; _ch++)
                {
                    u8 _pData[PATTERN_COLUMNS] = {NOTE_EMPTY,0,0,0,0,0,0,0,0,0,0,0,0,0}; // clear data
                    u8 _effCounter[EFFECTS_TOTAL*2] = {0,0,0,0,0,0,0,0,0,0,0,0}; // reset counters
                    for (s16 rowM = playingMatrixRow-1; rowM >= 0; rowM--) // scan from previous matrix row to first
                    {
                        u16 _pattern = SRAM_ReadMatrix(_ch, rowM);
                        if (_pattern) // pattern is not empty
                        {
                            for (u8 colP = DATA_NOTE; colP < PATTERN_COLUMNS; colP++) // pattern columns
                            {
                                for (s16 rowP = PATTERN_ROW_LAST; rowP >= 0; rowP--) // check row in pattern column from last to first
                                {
                                    u8 _data = SRAM_ReadPattern(_pattern, rowP, colP);

                                    switch (colP)
                                    {
                                        case DATA_NOTE:
                                            if (_data != NOTE_EMPTY && _pData[colP] == NOTE_EMPTY)
                                                { _pData[colP] = _data; break; }
                                            break;
                                        case DATA_INSTRUMENT:
                                            if (_data && !_pData[colP])
                                                { _pData[colP] = _data; break; }
                                            break;
                                        case DATA_FX1_TYPE: case DATA_FX2_TYPE: case DATA_FX3_TYPE:
                                        case DATA_FX4_TYPE: case DATA_FX5_TYPE: case DATA_FX6_TYPE:
                                            _effCounter[colP-2]++;
                                            if (_data && !_pData[colP])
                                                { _pData[colP] = _data; break; }
                                            break;
                                        case DATA_FX1_VALUE: case DATA_FX2_VALUE: case DATA_FX3_VALUE:
                                        case DATA_FX4_VALUE: case DATA_FX5_VALUE: case DATA_FX6_VALUE:
                                            _effCounter[colP-2]++;
                                            if (_data && !_pData[colP] && _effCounter[colP-2] <= _effCounter[colP-3])
                                                { _pData[colP] = _data; break; }
                                            break;
                                        default: break;
                                    }
                                }
                            }
                        }
                        if (_pData[1]
                            &&_pData[2]&&_pData[3]&&_pData[4]&&_pData[5]&&_pData[6]&&_pData[7]
                            &&_pData[8]&&_pData[9]&&_pData[10]&&_pData[11]&&_pData[12]&&_pData[13])
                            break; // all found
                    } // matrix row

                    channelPreviousNote[_ch] = _pData[DATA_NOTE];
                    channelPreviousInstrument[_ch] = _pData[DATA_INSTRUMENT];
                    if (channelPreviousInstrument[_ch]) WriteYM2612(_ch, channelPreviousInstrument[_ch]);

                    for (u8 _eff = 0; _eff < EFFECTS_TOTAL; _eff++)
                    {
                        channelPreviousEffectType[_ch][_eff] = _pData[DATA_FX1_TYPE + _eff*2];
                        channelPreviousEffectValue[_ch][_eff] = _pData[DATA_FX1_VALUE + _eff*2];

                        if (channelPreviousEffectType[_ch][_eff]) // if any effect
                        {
                            ApplyCommand_Common(_ch, channelPreviousEffectType[_ch][_eff], channelPreviousEffectValue[_ch][_eff]);
                            ApplyCommand_FM(_ch, channelPreviousInstrument[_ch], channelPreviousEffectType[_ch][_eff], channelPreviousEffectValue[_ch][_eff]);
                            ApplyCommand_PSG(channelPreviousEffectType[_ch][_eff], channelPreviousEffectValue[_ch][_eff]);
                        }
                    }
                } // channel
            }*/

            // bb: Mode, ResetB ResetA, EnableB EnableA, LoadB LoadA
            //YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, FM_CH3_Mode | 0b00000000);
            // set frame length
            if (playingPatternRow & 1) maxPulse = ppl_1; else maxPulse = ppl_2;

            DrawMatrixPlaybackCursor(FALSE);
            hIntCounter = hIntToSkip; // reset h-int counter
            bDoPulse = FALSE;
            pulseCounter = 0;
            matrixRowJumpTo = OXFF;
            patternRowJumpTo = OXFF;

            SYS_enableInts();
        }

        if (!pulseCounter) // row first pulse; prepare command, note, instrument, draw cursor
        {
            do_row(CHANNEL_FM1);
            do_row(CHANNEL_FM2);
            do_row(CHANNEL_FM3_OP4);
            do_row(CHANNEL_FM3_OP3);
            do_row(CHANNEL_FM3_OP2);
            do_row(CHANNEL_FM3_OP1);
            do_row(CHANNEL_FM4);
            do_row(CHANNEL_FM5);
            do_row(CHANNEL_FM6_DAC);
            do_row(CHANNEL_PSG1);
            do_row(CHANNEL_PSG2);
            do_row(CHANNEL_PSG3);
            do_row(CHANNEL_PSG4_NOISE);

            if (currentScreen == SCREEN_PATTERN) ClearPatternPlaybackCursor();

            if (matrixRowJumpTo != OXFF && currentScreen == SCREEN_MATRIX)
            {
                playingPatternRow = 0x20; // exceed to trigger next condition
            }
            else playingPatternRow++; // next line is..

            // jump to next...
            if (playingPatternRow > patternSize || patternRowJumpTo != OXFF)
            {
                DrawMatrixPlaybackCursor(TRUE); // erase

                if (currentScreen == SCREEN_MATRIX)
                {
                    if (loopStart != OXFF && loopEnd != OXFF)
                    {
                        if (playingMatrixRow == loopEnd) playingMatrixRow = loopStart-1;
                    }
                    else if (matrixRowJumpTo != OXFF)
                    {
                        playingMatrixRow = matrixRowJumpTo-1; // set to row before, then increment
                        matrixRowJumpTo = OXFF;
                    }
                    playingMatrixRow++; // next patterns in matrix is..

                    if (patternRowJumpTo != OXFF)
                    {
                        playingPatternRow = patternRowJumpTo;
                        patternRowJumpTo = OXFF;
                    }
                    else playingPatternRow = 0;

                    if (playingMatrixRow == MATRIX_ROWS)
                    {
                        playingMatrixRow = 0;
                    }

                    ReadMatrixRow(); // fill patterns ID, transpose values
                }
                else if (playingPatternRow > patternSize) // endless cycle pattern if not in matrix editor
                {
                    playingPatternRow = 0;
                    matrixRowJumpTo = OXFF;
                    patternRowJumpTo = OXFF;
                }

                DrawMatrixPlaybackCursor(FALSE);
            }

            if (currentScreen == SCREEN_PATTERN) DrawPatternPlaybackCursor();

            pulseCounter = -1; // to run this part only once when timer expires, not at every while loop tick.
        }

        if (bDoPulse)
        {
            do_effects(CHANNEL_FM1);
            do_effects(CHANNEL_FM2);
            do_effects(CHANNEL_FM3_OP4);
            do_effects(CHANNEL_FM3_OP3);
            do_effects(CHANNEL_FM3_OP2);
            do_effects(CHANNEL_FM3_OP1);
            do_effects(CHANNEL_FM4);
            do_effects(CHANNEL_FM5);
            do_effects(CHANNEL_FM6_DAC);
            do_effects(CHANNEL_PSG1);
            do_effects(CHANNEL_PSG2);
            do_effects(CHANNEL_PSG3);
            do_effects(CHANNEL_PSG4_NOISE);

            if (pulseCounter == -1) pulseCounter = 1; else
            pulseCounter++; // count row sub-pulses
            if (pulseCounter == maxPulse)
            {
                if (playingPatternRow & 1) maxPulse = ppl_1; else maxPulse = ppl_2;
                pulseCounter = 0;
            }
            bDoPulse = FALSE;
        }
    }
    else if (!_bBeginPlay) // need to run only once at playback stopped
    {
        SYS_disableInts();
        _bBeginPlay = TRUE;
        // bb: Mode, ResetB ResetA, EnableB EnableA, LoadB LoadA
        //YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_NORMAL | 0b00000000);
        StopAllSound();
        ClearPatternPlaybackCursor();
        DrawMatrixPlaybackCursor(TRUE);
        SYS_enableInts();
    }
}

inline static void ReadMatrixRow()
{
    for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
    {
        channelMatrixTranspose[mtxCh] = SRAM_ReadMatrixTranspose(mtxCh, playingMatrixRow);
        channelPlayingPatternID[mtxCh] = SRAM_ReadMatrix(mtxCh, playingMatrixRow);
    }
}

inline static s16 FindUnusedPattern()
{
    static bool used = FALSE;
    u8 x = selectedMatrixChannel * 3 + 1;
    u8 y = selectedMatrixScreenRow + 2;

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), x, y);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), x+1, y);

    for (s16 pattern = 1; pattern <= PATTERN_LAST; pattern++) {
        used = FALSE;
        for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++) {
            for (u8 line = 0; line < MATRIX_ROWS; line++) { if (SRAM_ReadMatrix(mtxCh, line) == pattern) { used = TRUE; break; } }
            if (used) break;
        }
        if (!used) return pattern;
    }
    return NULL;
}

static void SetBPM(u16 tempo)
{
    //static u32 microseconds = 0;
    static f32 mcs = 0;

    if (!tempo)
    {
        hIntToSkip = SRAMW_readWord(TEMPO);
    }
    else
    {
        SRAMW_writeWord(TEMPO, tempo); // store
        hIntToSkip = tempo;
    }

    // GUI
    if (IS_PALSYSTEM)
    {
        //microseconds = H_INT_DURATION_PAL * H_INT_SKIP * hIntToSkip; // h-blank = 1/11200 sec;  8.928571428571429e-5; 89.2857 microseconds;
        mcs = fix32Div(FIX32(1.0), FIX32(1.120)) * H_INT_SKIP * hIntToSkip;
    }
    else
    {
        //microseconds = H_INT_DURATION_NTSC * H_INT_SKIP * hIntToSkip; // h-blank = 1/13440 sec; 7.44047619047619e-5; 74.4047 microseconds; 224 * 60
        mcs = fix32Div(FIX32(1.0), FIX32(1.344)) * H_INT_SKIP * hIntToSkip; //193, 140, 0.74404761904761904761904761904762
    }

    // precise BPM: 600000000000 / (1/13440) * 2 * hIntToSkip)) / ppb

    u8 ppb = (ppl_2 + ppl_1) * 2; // pulses per beat

    // beat per minute
    //BPM = (600000000 / microseconds) / ppb;

    // beat per minute (fractional)
    fBPM = (fix32Div(FIX32(60000.0), mcs) / ppb) * 10;
    //BPM = fix32ToRoundedInt(fBPM);
    BPM = fix32ToInt(fix32Int(fBPM));
    fBPM = fix32Frac(fBPM);

    PPS = (((BPM * 1000) / 6) * ppb) / 10000; // pulse per second
    DrawPP();
}

// cursors
void DrawMatrixPlaybackCursor(u8 bClear)
{
    static u8 playingPage = 0;

    playingPage = playingMatrixRow / MATRIX_ROWS_ONPAGE;
    if (playingPage == currentPage)
    {
        if (bClear) { VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + playingMatrixRow), 39, playingMatrixRow - MATRIX_ROWS_ONPAGE * playingPage + 2); }
        else if (bPlayback) { VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 39, playingMatrixRow - MATRIX_ROWS_ONPAGE * playingPage + 2); }
    }
}

void ClearPatternPlaybackCursor()
{
    static s8 line = 0;

    line = playingPatternRow - 1; // previous line
    if (line == -1) line = patternSize;
    if (line < 16) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 40, line+4);
    else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 60, line-12);
}

void DrawPatternPlaybackCursor()
{
    static s8 line = 0;

    line = playingPatternRow - 1; // previous line
    if (line == -1) line = patternSize;
    if (line < 16) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 40, line+4);
    else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 60, line-12);
}

static void ChangeMatrixValue(s16 mod)
{
    static s32 value = 0;

    if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) // matrix
    {
        if (mod)
        {
            value = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow);
            if (!value && lastEnteredPattern) value = lastEnteredPattern;
            else
            {
                value += mod;
                if (value < 1) value = PATTERN_LAST; // last pattern
                else if (value > PATTERN_LAST) value = 1; // first pattern
            }
            SRAM_WriteMatrix(selectedMatrixChannel, selectedMatrixRow, value); bRefreshScreen = TRUE; matrixRowToRefresh = selectedMatrixRow;
            lastEnteredPattern = value;
        }
        else
        {
            value = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow);
            if (value) lastEnteredPattern = value; // accidential delete
            SRAM_WriteMatrix(selectedMatrixChannel, selectedMatrixRow, 0); bRefreshScreen = TRUE; matrixRowToRefresh = selectedMatrixRow;
        }
    }
    else // tempo
    {
        if (mod)
        {
            value = SRAMW_readWord(TEMPO) - mod;
            if (value < TICK_SKIP_MIN) value = TICK_SKIP_MIN;
            else if (value > TICK_SKIP_MAX) value = TICK_SKIP_MAX;
            SetBPM((u16)value);
        }
    }
}

static inline void LoadPreset(u8 id, u8 preset)
{
    SRAM_WriteInstrument(id, INST_MUL1, M_BANK_0[preset]->multiple_1);
    SRAM_WriteInstrument(id, INST_MUL2, M_BANK_0[preset]->multiple_3);
    SRAM_WriteInstrument(id, INST_MUL3, M_BANK_0[preset]->multiple_2);
    SRAM_WriteInstrument(id, INST_MUL4, M_BANK_0[preset]->multiple_4);

    SRAM_WriteInstrument(id, INST_DT1, M_BANK_0[preset]->detune_1);
    SRAM_WriteInstrument(id, INST_DT2, M_BANK_0[preset]->detune_3);
    SRAM_WriteInstrument(id, INST_DT3, M_BANK_0[preset]->detune_2);
    SRAM_WriteInstrument(id, INST_DT4, M_BANK_0[preset]->detune_4);

    SRAM_WriteInstrument(id, INST_TL1, M_BANK_0[preset]->totalLevel_1);
    SRAM_WriteInstrument(id, INST_TL2, M_BANK_0[preset]->totalLevel_3);
    SRAM_WriteInstrument(id, INST_TL3, M_BANK_0[preset]->totalLevel_2);
    SRAM_WriteInstrument(id, INST_TL4, M_BANK_0[preset]->totalLevel_4);

    SRAM_WriteInstrument(id, INST_RS1, M_BANK_0[preset]->rateScaling_1);
    SRAM_WriteInstrument(id, INST_RS2, M_BANK_0[preset]->rateScaling_3);
    SRAM_WriteInstrument(id, INST_RS3, M_BANK_0[preset]->rateScaling_2);
    SRAM_WriteInstrument(id, INST_RS4, M_BANK_0[preset]->rateScaling_4);

    SRAM_WriteInstrument(id, INST_AR1, M_BANK_0[preset]->attackRate_1);
    SRAM_WriteInstrument(id, INST_AR2, M_BANK_0[preset]->attackRate_3);
    SRAM_WriteInstrument(id, INST_AR3, M_BANK_0[preset]->attackRate_2);
    SRAM_WriteInstrument(id, INST_AR4, M_BANK_0[preset]->attackRate_4);

    SRAM_WriteInstrument(id, INST_D1R1, M_BANK_0[preset]->firstDecayRate_1);
    SRAM_WriteInstrument(id, INST_D1R2, M_BANK_0[preset]->firstDecayRate_3);
    SRAM_WriteInstrument(id, INST_D1R3, M_BANK_0[preset]->firstDecayRate_2);
    SRAM_WriteInstrument(id, INST_D1R4, M_BANK_0[preset]->firstDecayRate_4);

    SRAM_WriteInstrument(id, INST_D1L1, M_BANK_0[preset]->secondaryAmplitude_1);
    SRAM_WriteInstrument(id, INST_D1L2, M_BANK_0[preset]->secondaryAmplitude_3);
    SRAM_WriteInstrument(id, INST_D1L3, M_BANK_0[preset]->secondaryAmplitude_2);
    SRAM_WriteInstrument(id, INST_D1L4, M_BANK_0[preset]->secondaryAmplitude_4);

    SRAM_WriteInstrument(id, INST_AM1, M_BANK_0[preset]->amplitudeModulation_1);
    SRAM_WriteInstrument(id, INST_AM2, M_BANK_0[preset]->amplitudeModulation_3);
    SRAM_WriteInstrument(id, INST_AM3, M_BANK_0[preset]->amplitudeModulation_2);
    SRAM_WriteInstrument(id, INST_AM4, M_BANK_0[preset]->amplitudeModulation_4);

    SRAM_WriteInstrument(id, INST_D2R1, M_BANK_0[preset]->secondaryDecayRate_1);
    SRAM_WriteInstrument(id, INST_D2R2, M_BANK_0[preset]->secondaryDecayRate_3);
    SRAM_WriteInstrument(id, INST_D2R3, M_BANK_0[preset]->secondaryDecayRate_2);
    SRAM_WriteInstrument(id, INST_D2R4, M_BANK_0[preset]->secondaryDecayRate_4);

    SRAM_WriteInstrument(id, INST_RR1, M_BANK_0[preset]->releaseRate_1);
    SRAM_WriteInstrument(id, INST_RR2, M_BANK_0[preset]->releaseRate_3);
    SRAM_WriteInstrument(id, INST_RR3, M_BANK_0[preset]->releaseRate_2);
    SRAM_WriteInstrument(id, INST_RR4, M_BANK_0[preset]->releaseRate_4);

    SRAM_WriteInstrument(id, INST_SSGEG1, M_BANK_0[preset]->ssgEg_1);
    SRAM_WriteInstrument(id, INST_SSGEG2, M_BANK_0[preset]->ssgEg_3);
    SRAM_WriteInstrument(id, INST_SSGEG3, M_BANK_0[preset]->ssgEg_2);
    SRAM_WriteInstrument(id, INST_SSGEG4, M_BANK_0[preset]->ssgEg_4);

    SRAM_WriteInstrument(id, INST_FB, M_BANK_0[preset]->feedback);
    SRAM_WriteInstrument(id, INST_ALG, M_BANK_0[preset]->algorithm);
    SRAM_WriteInstrument(id, INST_PAN, M_BANK_0[preset]->stereo);
    SRAM_WriteInstrument(id, INST_FMS, M_BANK_0[preset]->fms);
    SRAM_WriteInstrument(id, INST_AMS, M_BANK_0[preset]->ams);
}

// gamepad interrupts handler
static inline void JoyEvent(u16 joy, u16 changed, u16 state)
{
    static u8 patternColumnShift = 0;
    static s8 inc = 0; // paste increment
    static u8 row = 0; // paste row to
    static s8 col = 0; // pattern color slot
    static s8 transpose = 0; // matrix slot transpose

    u8 muted;

    if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE)
        selectedMatrixRow = selectedMatrixScreenRow + (currentPage * 25);

    auto void stop_playback()
    {
        pulseCounter = 0;
        bPlayback = FALSE;
    }

    auto void selection_clear()
    {
        for (u8 y=4; y<20; y++)
        {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 44, y);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 64, y);
        }
    }

    auto void switch_to_pattern_editor()
    {
        selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow);
        if (selectedPatternID != 0x00) // -- pattern should not be editable
        {
            currentScreen = SCREEN_PATTERN;
            bInitScreen = TRUE;
            bRefreshScreen = TRUE;
            VDP_setHorizontalScroll(BG_A, -320);
            VDP_setHorizontalScroll(BG_B, -320);
        }
    }

    auto void switch_to_instrument_editor()
    {
        currentScreen = SCREEN_INSTRUMENT;
        bInitScreen = TRUE;
        bRefreshScreen = TRUE;
        VDP_setHorizontalScroll(BG_A, -640);
        VDP_setHorizontalScroll(BG_B, -640);

        // go to selected instrument if not --
        if
        (
            (selectedPatternColumn == DATA_INSTRUMENT) ||
            (selectedPatternColumn == DATA_INSTRUMENT + PATTERN_COLUMNS)
        )
        {
            u8 value = SRAM_ReadPattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_INSTRUMENT);
            if (value != 0x00) selectedInstrumentID = value;
        }
    }

    auto void switch_to_matrix_editor()
    {
        selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow);
        currentScreen = SCREEN_MATRIX;
        bInitScreen = TRUE;
        bRefreshScreen = TRUE;
        VDP_setHorizontalScroll(BG_A, 0);
        VDP_setHorizontalScroll(BG_B, 0);
        matrixRowToRefresh = OXFFFF;
    }

    //if (joy == JOY_1 || joy == JOY_2) // any joy really
    //{
        switch (state)
        {
        case BUTTON_START:
            if (bPlayback == FALSE)
            {
                pulseCounter = 0;
                playingPatternRow = 0; // start from the first line of current pattern
                playingMatrixRow = selectedMatrixRow; // actual line in array
                bPlayback = TRUE;
            }
            else
            {
                stop_playback();
            }
            break;

        case BUTTON_MODE:
            if (bPlayback == FALSE)
            {
                pulseCounter = 0;
                if (selectedPatternColumn < PATTERN_COLUMNS) playingPatternRow = selectedPatternRow; // start from the current selected pattern line
                else playingPatternRow = selectedPatternRow + PATTEN_ROWS_PER_SIDE;
                playingMatrixRow = selectedMatrixRow; // actual line in array
                bPlayback = TRUE;
            }
            else
            {
                stop_playback();
            }
            break;
        }
        if (selectedPatternColumn >= PATTERN_COLUMNS) patternColumnShift = PATTEN_ROWS_PER_SIDE; else patternColumnShift = 0;
        /// -------------------------------------------------------------------------------------------------------------------
        // direction hold

        if (changed & BUTTON_DIR) // 1/2/4/8 (true) or 0 (false)
        {
            bDoCount = changed; buttonCounter = GUI_NAVIGATION_DELAY; navigationDirection = state;
        }

        // screens
        switch (currentScreen)
        {
        case SCREEN_MATRIX:
            // single press or buttons combinations
            switch (state)
            {
            case BUTTON_X:
                switch (changed)
                {
                // X+L/R - switch screen
                case BUTTON_RIGHT:
                    switch_to_pattern_editor();
                    break;

                case BUTTON_LEFT:
                    switch_to_instrument_editor();
                    break;
                // X+U/D - mute/un-mute/solo channel
                case BUTTON_UP:
                    channelFlags[selectedMatrixChannel] = FALSE; // mute selected
                    for (u8 ch = 0; ch < CHANNELS_TOTAL; ch++) muted += channelFlags[ch];
                    if (!muted) // all channels muted
                    {
                        for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
                        {
                            channelFlags[mtxCh] = TRUE; // un-mute all
                            SRAM_WriteMatrixChannelEnabled(mtxCh, TRUE);
                            DrawMute(mtxCh);
                        }
                    }
                    else
                    {
                        SRAM_WriteMatrixChannelEnabled(selectedMatrixChannel, FALSE);
                        DrawMute(selectedMatrixChannel);
                    }
                    break;

                case BUTTON_DOWN: // un-mute/solo
                    for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++) { muted += channelFlags[mtxCh]; }
                    if (muted == CHANNELS_TOTAL) // all channels unmuted
                    {
                        for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
                        {
                            channelFlags[mtxCh] = FALSE; // mute all
                            if (mtxCh != selectedMatrixChannel) DrawMute(mtxCh); // set all marks (except selected)
                            SRAM_WriteMatrixChannelEnabled(mtxCh, FALSE);
                        }
                    }
                    channelFlags[selectedMatrixChannel] = TRUE; // un-mute selected
                    SRAM_WriteMatrixChannelEnabled(selectedMatrixChannel, TRUE);
                    DrawMute(selectedMatrixChannel);
                    break;

                case BUTTON_Y: // un-mute all
                    for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
                    {
                        channelFlags[mtxCh] = TRUE; // un-mute all
                        SRAM_WriteMatrixChannelEnabled(mtxCh, TRUE);
                        DrawMute(mtxCh); // clear all marks
                    }
                    break;
                }
                break;

            case BUTTON_Y:
                switch(changed)
                {
                case BUTTON_LEFT: // pattern colors
                    selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow); // select current pattern
                    if (selectedPatternID != NULL)
                    {
                        col = SRAM_ReadPatternColor(selectedPatternID)-1;
                        if (col < 0) col = GUI_PATTERN_COLORS_MAX;
                        SRAM_WritePatternColor(selectedPatternID, col);
                        ReColorsAndTranspose();
                    }
                    break;

                case BUTTON_RIGHT:
                    selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow);
                    if (selectedPatternID != NULL)
                    {
                        col = SRAM_ReadPatternColor(selectedPatternID)+1;
                        if (col > GUI_PATTERN_COLORS_MAX) col = 0;
                        SRAM_WritePatternColor(selectedPatternID, col);
                        ReColorsAndTranspose();
                    }
                    break;

                case BUTTON_UP: // matrix transpose
                    transpose = SRAM_ReadMatrixTranspose(selectedMatrixChannel, selectedMatrixRow);
                    if (transpose < 12)
                    {
                        transpose++; SRAM_WriteMatrixTranspose(selectedMatrixChannel, selectedMatrixRow, transpose);
                        ReColorsAndTranspose();
                    }
                    break;

                case BUTTON_DOWN:
                    transpose = SRAM_ReadMatrixTranspose(selectedMatrixChannel, selectedMatrixRow);
                    if (transpose > -12)
                    {
                        transpose--; SRAM_WriteMatrixTranspose(selectedMatrixChannel, selectedMatrixRow, transpose);
                        ReColorsAndTranspose();
                    }
                    break;

                case BUTTON_C: // clear
                    selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow);
                    if (selectedPatternID != NULL)
                    {
                        col = SRAM_ReadPatternColor(selectedPatternID);
                        if (col != 0)
                        {
                            SRAM_WritePatternColor(selectedPatternID, 0);
                            ReColorsAndTranspose();
                        }
                    }
                    break;
                }
                break;

            case BUTTON_Z:
                switch (changed)
                {
                // Z+L/R - pattern matrix page select
                case BUTTON_RIGHT:
                    if (currentPage < MATRIX_PAGES)
                    {
                        currentPage++;
                        bRefreshScreen = bInitScreen = TRUE;
                        matrixRowToRefresh = OXFFFF;
                        ReColorsAndTranspose();
                    }
                    break;

                case BUTTON_LEFT:
                    if (currentPage > 0)
                    {
                        currentPage--;
                        bRefreshScreen = bInitScreen = TRUE;
                        matrixRowToRefresh = OXFFFF;
                        ReColorsAndTranspose();
                    }
                    break;

                case BUTTON_UP:
                    if (selectedMatrixRow == loopEnd) loopEnd = OXFF;
                    loopStart = selectedMatrixRow;
                    bRefreshScreen = bInitScreen = TRUE;
                    matrixRowToRefresh = OXFFFF;
                    break;

                case BUTTON_DOWN:
                    if (selectedMatrixRow == loopStart) loopStart = OXFF;
                    loopEnd = selectedMatrixRow;
                    bRefreshScreen = bInitScreen = TRUE;
                    matrixRowToRefresh = OXFFFF;
                    break;

                case BUTTON_C:
                    loopStart = loopEnd = OXFF;
                    bRefreshScreen = bInitScreen = TRUE;
                    matrixRowToRefresh = OXFFFF;
                    break;
                }
                break;

            case BUTTON_A:
                // A+L/R/U/D - change pattern in matrix
                switch (changed)
                {
                case BUTTON_B:
                    if (!SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow))
                    {
                        u16 value = FindUnusedPattern();
                        if (value)
                        {
                            SRAM_WriteMatrix(selectedMatrixChannel, selectedMatrixRow, value );
                            bRefreshScreen = TRUE;
                            matrixRowToRefresh = selectedMatrixRow;
                            lastEnteredPattern = value;
                        }
                    }
                    break;
                case BUTTON_RIGHT: ChangeMatrixValue(1);
                    break;
                case BUTTON_LEFT: ChangeMatrixValue(-1);
                    break;
                case BUTTON_UP: ChangeMatrixValue(16);
                    break;
                case BUTTON_DOWN: ChangeMatrixValue(-16);
                    break;
                }
                break;

            case BUTTON_B:
                switch (changed)
                {
                case BUTTON_UP: // delete selected matrix row
                    if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE)
                    {
                        for (u8 row = selectedMatrixRow; row < MATRIX_ROW_LAST-1; row++)
                        {
                            for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++)
                            {
                                SRAM_WriteMatrix(channel, row, SRAM_ReadMatrix(channel, row+1));
                            }
                        }

                        // always clear last row
                        for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++)
                        {
                            SRAM_WriteMatrix(channel, MATRIX_ROWS-1, 0);
                        }

                        matrixRowToRefresh = OXFFFF;
                        bRefreshScreen = TRUE;
                    }
                    break;

                case BUTTON_DOWN: // copy and insert selected matrix row
                    if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE)
                    {
                        for (u8 row = MATRIX_ROWS-2; row >= selectedMatrixRow; row--)
                        {
                            for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++)
                            {
                                SRAM_WriteMatrix(channel, row+1, SRAM_ReadMatrix(channel, row));
                            }
                        }

                        matrixRowToRefresh = OXFFFF;
                        bRefreshScreen = TRUE;
                    }
                    break;
                }
                break;

            case BUTTON_C:
                ChangeMatrixValue(0); // clear
                break;
            // navigate pattern matrix first button press
            case BUTTON_LEFT: case BUTTON_RIGHT: case BUTTON_UP: case BUTTON_DOWN:
                NavigateMatrix(state);
                break;
            }
        break;
        /// -------------------------------------------------------------------------------------------------------------------
        case SCREEN_PATTERN:
            switch (state)
            {
            case BUTTON_X:
                switch (changed)
                {
                // X+L/R - switch screen
                case BUTTON_RIGHT:
                    ClearPatternPlaybackCursor();
                    switch_to_instrument_editor();
                    break;

                case BUTTON_LEFT:
                    ClearPatternPlaybackCursor();
                    switch_to_matrix_editor();
                    break;
                }
                break;

            case BUTTON_Y:
                NavigatePattern(PATTERN_JUMPSIDETRIGGER);
                break;

            /*case BUTTON_Z:
                // Z + D-Pad: select pattern for editing [useless]
                switch (changed)
                {
                case BUTTON_RIGHT:
                    if (selectedPatternID < MAX_PATTERN){ selectedPatternID++; bRefreshScreen = bInitScreen = TRUE; patternRowToRefresh = EVALUATE_0xFF; }
                    if (bPlayback == 1) DrawPatternPlaybackCursor(); // to change color
                    break;
                case BUTTON_LEFT:
                    if (selectedPatternID > 1) { selectedPatternID--; bRefreshScreen = bInitScreen = TRUE; patternRowToRefresh = EVALUATE_0xFF; }
                    if (bPlayback == 1) DrawPatternPlaybackCursor(); // to change color
                    break;
                case BUTTON_UP:
                    if (selectedPatternID < (MAX_PATTERN - 16)) { selectedPatternID += 16; bRefreshScreen = bInitScreen = TRUE; patternRowToRefresh = EVALUATE_0xFF; }
                    if (bPlayback == 1) DrawPatternPlaybackCursor(); // to change color
                    break;
                case BUTTON_DOWN:
                    if (selectedPatternID > 16) { selectedPatternID -= 16; bRefreshScreen = bInitScreen = TRUE; patternRowToRefresh = EVALUATE_0xFF; }
                    if (bPlayback == 1) DrawPatternPlaybackCursor(); // to change color
                    break;
                }
                break;*/

            case BUTTON_A:
                // A + D-Pad: change note; instrument +- 1|16;  effect type/value +- 1|16
                switch (changed)
                {
                // modify selected parameter
                case BUTTON_RIGHT:
                    ChangePatternParameter(1, 1);
                    break;

                case BUTTON_LEFT:
                    ChangePatternParameter(-1, -1);
                    break;

                case BUTTON_UP:
                    ChangePatternParameter(12, 16);
                    break;

                case BUTTON_DOWN:
                    ChangePatternParameter(-12, -16);
                    break;

                // paste all data from selection
                case BUTTON_B:
                    if (patternCopyRangeStart != NOTHING)
                    {
                        for (u8 cnt = patternCopyRangeStart; cnt < patternCopyRangeEnd; cnt++)
                        {
                            row = selectedPatternRow + patternColumnShift + inc;
                            if (row <= PATTERN_ROW_LAST) {
                                SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_NOTE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_INSTRUMENT, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_INSTRUMENT));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX1_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX1_TYPE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX1_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX1_VALUE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX2_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX2_TYPE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX2_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX2_VALUE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX3_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX3_TYPE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX3_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX3_VALUE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX4_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX4_TYPE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX4_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX4_VALUE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX5_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX5_TYPE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX5_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX5_VALUE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX6_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX6_TYPE));
                                SRAM_WritePattern(selectedPatternID, row, DATA_FX6_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX6_VALUE));
                                inc++;
                            }
                        }
                        inc = 0; bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                    }
                    break;

                // paste selected
                case BUTTON_C:
                    if (patternCopyRangeStart != NOTHING)
                    {
                        for (u8 cnt = patternCopyRangeStart; cnt < patternCopyRangeEnd; cnt++)
                        {
                            switch (selectedPatternColumn)
                            {
                            case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_NOTE)); inc++;
                                } //else break;
                                break;

                            case DATA_INSTRUMENT: case (DATA_INSTRUMENT + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_INSTRUMENT, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_INSTRUMENT)); inc++;
                                } //else return;
                                break;

                            case DATA_FX1_TYPE: case (DATA_FX1_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX1_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX1_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX1_VALUE: case (DATA_FX1_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX1_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX1_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX2_TYPE: case (DATA_FX2_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX2_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX2_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX2_VALUE: case (DATA_FX2_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX2_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX2_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX3_TYPE: case (DATA_FX3_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX3_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX3_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX3_VALUE: case (DATA_FX3_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX3_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX3_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX4_TYPE: case (DATA_FX4_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX4_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX4_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX4_VALUE: case (DATA_FX4_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX4_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX4_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX5_TYPE: case (DATA_FX5_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX5_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX5_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX5_VALUE: case (DATA_FX5_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX5_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX5_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX6_TYPE: case (DATA_FX6_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX6_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX6_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX6_VALUE: case (DATA_FX6_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX6_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX6_VALUE)); inc++;
                                } //else return;
                                break;
                            }
                        }
                        inc = 0; bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                    }
                    break;
                }
                break;
            // navigate line
            case BUTTON_LEFT: case BUTTON_RIGHT: case BUTTON_UP: case BUTTON_DOWN:
                NavigatePattern(state);
                break;
            // copy/paste
            case BUTTON_B:
                switch (changed)
                {
                // selected to nothing
                case BUTTON_B: // pressed
                    inc = 0;
                    if (patternCopyRangeStart != NOTHING) { selection_clear(); }
                    patternCopyRangeStart = patternCopyRangeEnd = NOTHING;
                    VDP_setTextPalette(PAL1); VDP_drawText("---", 65, 0);
                    break;
                // select all
                case BUTTON_UP:
                    if (patternCopyRangeStart == NOTHING)
                    {
                        patternCopyFrom = selectedPatternID;
                        patternCopyRangeStart = 0;
                        patternCopyRangeEnd = PATTERN_ROW_LAST + 1;

                        for (u8 y=4; y<20; y++)
                        {
                            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 44, y);
                            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 64, y);
                        }
                        intToHex(selectedPatternID, str, 3); VDP_setTextPalette(PAL1); VDP_drawText(str, 65, 0);
                    }
                    break;
                // set selection
                case BUTTON_DOWN:
                    if (patternCopyRangeStart == NOTHING)
                    {
                        patternCopyFrom = selectedPatternID;
                        patternCopyRangeStart = selectedPatternRow + patternColumnShift;
                        patternCopyRangeEnd = patternCopyRangeStart + 1;

                        if (patternCopyRangeEnd < 16)
                            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 44, patternCopyRangeStart+4);
                        else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 64, patternCopyRangeStart-12);

                        intToHex(selectedPatternID, str, 3); VDP_setTextPalette(PAL1); VDP_drawText(str, 65, 0);
                    }
                    else
                    {
                        if (patternCopyRangeEnd < PATTERN_ROWS)
                        {
                            if (patternCopyRangeEnd < 16)
                                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 44, patternCopyRangeEnd+4);
                            else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 64, patternCopyRangeEnd-12);
                            patternCopyRangeEnd++;
                        }
                    }
                    break;
                // octave -
                case BUTTON_LEFT:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        for (u8 row = 0; row < PATTERN_ROWS; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note < NOTES && note > 11) note -= 12;
                            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, note);
                        }
                        bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                        break;
                    }
                    break;
                // octave +
                case BUTTON_RIGHT:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        for (u8 row = 0; row < PATTERN_ROWS; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note < NOTES && note < NOTES-11) note += 12;
                            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, note);
                        }
                        bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                        break;
                    }
                    break;
                }
                break;
            // clear
            case BUTTON_C:
                switch (selectedPatternColumn)
                {
                case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_NOTE, NOTE_EMPTY);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_INSTRUMENT: case (DATA_INSTRUMENT + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_INSTRUMENT, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX1_TYPE: case (DATA_FX1_TYPE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX1_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX1_VALUE: case (DATA_FX1_VALUE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX1_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX2_TYPE: case (DATA_FX2_TYPE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX2_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX2_VALUE: case (DATA_FX2_VALUE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX2_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX3_TYPE: case (DATA_FX3_TYPE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX3_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX3_VALUE: case (DATA_FX3_VALUE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX3_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX4_TYPE: case (DATA_FX4_TYPE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX4_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX4_VALUE: case (DATA_FX4_VALUE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX4_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX5_TYPE: case (DATA_FX5_TYPE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX5_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX5_VALUE: case (DATA_FX5_VALUE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX5_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX6_TYPE: case (DATA_FX6_TYPE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX6_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX6_VALUE: case (DATA_FX6_VALUE + PATTERN_COLUMNS):
                    SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX6_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                }

                switch (changed)
                {
                // note off
                case BUTTON_B:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        SRAM_WritePattern(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_NOTE, NOTE_OFF);
                        bRefreshScreen = TRUE;
                        patternRowToRefresh = selectedPatternRow + patternColumnShift;
                        break;
                    }
                    break;
                }
                break;
            }
        break;
        /// -------------------------------------------------------------------------------------------------------------------
        case SCREEN_INSTRUMENT:
        {
            switch (state)
            {
            case BUTTON_X:
                switch (changed)
                {
                // X+L/R - switch screen
                case BUTTON_RIGHT:
                    switch_to_matrix_editor();
                    break;

                case BUTTON_LEFT:
                    switch_to_pattern_editor();
                    break;

                case BUTTON_UP: // mute instrument
                    instrumentIsMuted[selectedInstrumentID] = INST_MUTE;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_STATE;
                    break;

                case BUTTON_DOWN: // solo instrument
                    for (u8 i = 0; i < INSTRUMENTS_LAST; i++) instrumentIsMuted[i] = INST_MUTE; // mute all
                    instrumentIsMuted[selectedInstrumentID] = INST_SOLO; // set to solo
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_STATE;
                    break;

                case BUTTON_Y: // un-mute all instruments
                    for (u8 i = 0; i < INSTRUMENTS_LAST; i++) instrumentIsMuted[i] = INST_PLAY;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_STATE;
                    break;
                }
                break;

            case BUTTON_Y:
                // Y + D-Pad: select instrument for editing
                switch (changed)
                {
                case BUTTON_RIGHT:
                    if (selectedInstrumentID < INSTRUMENTS_LAST) { selectedInstrumentID++; bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF; }
                    break;

                case BUTTON_LEFT:
                    if (selectedInstrumentID > 1) { selectedInstrumentID--; bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF; }
                    break;

                case BUTTON_UP:
                    if (selectedInstrumentID < (INSTRUMENTS_LAST - 16)) selectedInstrumentID += 16; else selectedInstrumentID = INSTRUMENTS_LAST;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF;
                    break;

                case BUTTON_DOWN:
                    if (selectedInstrumentID > 16) selectedInstrumentID -= 16; else selectedInstrumentID = 1;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF;
                    break;
                }
                break;

            /*case BUTTON_Z:
                switch (changed)
                {
                    case ???:
                    break;
                }
                break;*/

            case BUTTON_A:
                // A + D-Pad: change parameter value
                switch (changed)
                {
                case BUTTON_RIGHT:
                    ChangeInstrumentParameter(1);
                    break;

                case BUTTON_LEFT:
                    ChangeInstrumentParameter(-1);
                    break;

                case BUTTON_UP:
                    ChangeInstrumentParameter(8);
                    break;

                case BUTTON_DOWN:
                    ChangeInstrumentParameter(-8);
                    break;
                }
                break;
            // navigate parameters
            case BUTTON_LEFT: case BUTTON_RIGHT: case BUTTON_UP: case BUTTON_DOWN:
                NavigateInstrument(state);
                break;

            // ARP and VOL step on/off
            case BUTTON_C:
                if (selectedInstrumentParameter == GUI_INST_PARAM_VOLSEQ)
                {
                    //SRAM_WriteInstrument(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator, SEQ_VOL_SKIP); // VOL
                    SRAM_WriteSEQ_VOL(selectedInstrumentID, selectedInstrumentOperator, SEQ_VOL_SKIP);
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_VOLSEQ;
                }
                else if (selectedInstrumentParameter == GUI_INST_PARAM_ARPSEQ)
                {
                    //SRAM_WriteInstrument(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator, ARP_BASE); // ARP
                    SRAM_WriteSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator, ARP_BASE);
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_ARPSEQ;
                }
                break;

            case BUTTON_B:
                switch (selectedInstrumentParameter)
                {
                case GUI_INST_PARAM_VOLSEQ:
                    //SRAM_WriteInstrument(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator, SEQ_VOL_MIN_ATT); // VOL
                    SRAM_WriteSEQ_VOL(selectedInstrumentID, selectedInstrumentOperator, SEQ_VOL_MIN_ATT); // VOL
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_VOLSEQ;
                    break;
                case GUI_INST_PARAM_ARPSEQ:
                    //SRAM_WriteInstrument(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator, NOTE_EMPTY); // ARP
                    SRAM_WriteSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator, NOTE_EMPTY); // ARP
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_ARPSEQ;
                    break;
                case GUI_INST_PARAM_COPY:
                    // copy instrument routine (FM parameters only)
                    if (instCopyTo != selectedInstrumentID)
                    {
                        for (u8 param = INST_ALG; param < INST_SSGEG4; param++)
                        {
                            SRAM_WriteInstrument(instCopyTo, param, SRAM_ReadInstrument(selectedInstrumentID, param));
                        }
                        VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "OK", GUI_INST_NAME_START, 1);
                    }
                    break;
                case GUI_INST_PARAM_PRESET:
                    LoadPreset(selectedInstrumentID, midiPreset);
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF;
                    break;
                }
                break;
            }
        }
        break;
        }
    //}
}

// draw selection cursor
void DrawSelectionCursor(u8 pos_x, u8 pos_y, u8 bClear)
{
    static s8 offset_x = 1;
    static s8 offset_y = 2;
    static s8 width = 3;

    // matrix
    if (currentScreen == SCREEN_MATRIX)
    {
        offset_x = 1; offset_y = 2; width = 3;
    }
    // pattern
    else if (currentScreen == SCREEN_PATTERN)
    {
        // left
        switch (selectedPatternColumn)
        {
        case GUI_PATTERN_L_NOTE:
            offset_x = 40+2; offset_y = 4; width = 3;
            break;
        case GUI_PATTERN_L_INST:
            offset_x = 40+3; offset_y = 4; width = 2;
            break;
        case GUI_PATTERN_L_FX1_TYPE:
        case GUI_PATTERN_L_FX1_VALUE:
        case GUI_PATTERN_L_FX2_TYPE:
        case GUI_PATTERN_L_FX2_VALUE:
        case GUI_PATTERN_L_FX3_TYPE:
        case GUI_PATTERN_L_FX3_VALUE:
        case GUI_PATTERN_L_FX4_TYPE:
        case GUI_PATTERN_L_FX4_VALUE:
        case GUI_PATTERN_L_FX5_TYPE:
        case GUI_PATTERN_L_FX5_VALUE:
        case GUI_PATTERN_L_FX6_TYPE:
        case GUI_PATTERN_L_FX6_VALUE:
            offset_x = 40+6; offset_y = 4; width = 1;
            break;
        case GUI_PATTERN_R_NOTE:
            offset_x = 40-20; offset_y = 4; width = 3;
            break;
        case GUI_PATTERN_R_INST:
            offset_x = 40-5; offset_y = 4; width = 2;
            break;
        case GUI_PATTERN_R_FX1_TYPE:
        case GUI_PATTERN_R_FX1_VALUE:
        case GUI_PATTERN_R_FX2_TYPE:
        case GUI_PATTERN_R_FX2_VALUE:
        case GUI_PATTERN_R_FX3_TYPE:
        case GUI_PATTERN_R_FX3_VALUE:
        case GUI_PATTERN_R_FX4_TYPE:
        case GUI_PATTERN_R_FX4_VALUE:
        case GUI_PATTERN_R_FX5_TYPE:
        case GUI_PATTERN_R_FX5_VALUE:
        case GUI_PATTERN_R_FX6_TYPE:
        case GUI_PATTERN_R_FX6_VALUE:
            offset_x = 40+12; offset_y = 4; width = 1;
            break;
        default: break;
        }
    }
    // instrument
    else if (currentScreen == SCREEN_INSTRUMENT)
    {
        switch (selectedInstrumentParameter)
        {
        case GUI_INST_PARAM_ALG:
        case GUI_INST_PARAM_FMS:
        case GUI_INST_PARAM_AMS:
        case GUI_INST_PARAM_PAN:
        case GUI_INST_PARAM_FB:
            offset_x = 80+7; offset_y = 2; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_TL:
        case GUI_INST_PARAM_RS:
        case GUI_INST_PARAM_MUL:
        case GUI_INST_PARAM_DT:
            offset_x = 80+14; offset_y = 4; width = 3;
            break;
        case GUI_INST_PARAM_AR:
        case GUI_INST_PARAM_D1R:
        case GUI_INST_PARAM_D1L:
        case GUI_INST_PARAM_D2R:
        case GUI_INST_PARAM_RR:
            offset_x = 80+14; offset_y = 5; width = 3;
            break;
        case GUI_INST_PARAM_AM:
        case GUI_INST_PARAM_SSGEG:
            offset_x = 80+14; offset_y = 6; width = 3;
            break;
        case GUI_INST_PARAM_LFO:
            offset_x = 80+12; offset_y = 7; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_VOLSEQ:
        case GUI_INST_PARAM_ARPSEQ:
            if (pos_x < 8) offset_x = 80+5;
            else if (pos_x < 16) offset_x = 80+6;
            else if (pos_x < 24) offset_x = 80+7;
            else if (pos_x < 32) offset_x = 80+8;
            offset_y = 8; width = 1;
            break;
        case GUI_INST_PARAM_NAME:
            offset_x = 80+17; offset_y = -GUI_INST_PARAM_NAME; width = 1;
            break;
        case GUI_INST_PARAM_COPY:
            offset_x = 80+17; offset_y = 1-GUI_INST_PARAM_COPY; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_PCM_BANK:
            offset_x = 80+33; offset_y = -GUI_INST_PARAM_PCM_BANK; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_PCM_NOTE:
            offset_x = 80+36; offset_y = -GUI_INST_PARAM_PCM_NOTE; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_PCM_START:
        case GUI_INST_PARAM_PCM_END:
            offset_x = 80+33; offset_y = 4-GUI_INST_PARAM_PCM_END; width = 2;
            break;
        case GUI_INST_PARAM_PCM_LOOP:
        case GUI_INST_PARAM_PCM_RATE:
        case GUI_INST_PARAM_PCM_PAN:
            offset_x = 80+33; offset_y = 6-GUI_INST_PARAM_PCM_RATE; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_PRESET:
            offset_x = 80+33; offset_y = GUI_INST_PARAM_PRESET-3; width = 0; selectedInstrumentOperator = 0;
            break;
        default: break;
        }
    }

    auto inline void draw_cursor_1(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
    }

    auto inline void draw_cursor_2(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 1, y);
    }

    auto inline void draw_cursor_3(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 1, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 2, y);
    }

    auto inline void clear_cursor_1(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
    }

    auto inline void clear_cursor_2(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
    }

    auto inline void clear_cursor_2_color(u8 x, u8 y)
    {
        selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow); // select previous pattern on clear
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        if (selectedPatternID != NULL)
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PATTERNCOLORS[SRAM_ReadPatternColor(selectedPatternID)]), x + 1, y);
        else
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
    }

    auto inline void clear_cursor_3(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 2, y);
    }

    // it goes first
    switch (currentScreen)
    {
    case SCREEN_MATRIX:
        if (bClear)
        {
            if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) clear_cursor_2_color(pos_x * width + offset_x, pos_y + offset_y);
            else clear_cursor_3(3, 27); // BPM
        }
        else
        {
            if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y);
            else draw_cursor_3(3, 27); // BPM
        }
        break;
    case SCREEN_PATTERN:
        if (bClear)
        {
            switch (selectedPatternColumn) {
            case GUI_PATTERN_L_NOTE: case GUI_PATTERN_R_NOTE:
                clear_cursor_3(pos_x * width + offset_x - 1, pos_y + offset_y);
                break; // note
            case GUI_PATTERN_L_INST: case GUI_PATTERN_R_INST:
                clear_cursor_2(pos_x * width + offset_x, pos_y + offset_y);
                break; // instrument
            case GUI_PATTERN_L_FX1_VALUE: case GUI_PATTERN_R_FX1_VALUE:
            case GUI_PATTERN_L_FX2_VALUE: case GUI_PATTERN_R_FX2_VALUE:
            case GUI_PATTERN_L_FX3_VALUE: case GUI_PATTERN_R_FX3_VALUE:
            case GUI_PATTERN_L_FX4_VALUE: case GUI_PATTERN_R_FX4_VALUE:
            case GUI_PATTERN_L_FX5_VALUE: case GUI_PATTERN_R_FX5_VALUE:
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), pos_x * width + offset_x, pos_y + offset_y); break; // draw separator
            default: clear_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break; // effects
            }
        }
        else
        {
            switch (selectedPatternColumn) {
            case GUI_PATTERN_L_NOTE: case GUI_PATTERN_R_NOTE: draw_cursor_3(pos_x * width + offset_x - 1, pos_y + offset_y); break; // note
            case GUI_PATTERN_L_INST: case GUI_PATTERN_R_INST: draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y); break; // instrument
            default: draw_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break; // effects
            }
        }
        break;
    case SCREEN_INSTRUMENT:
        if (bClear)
        {
            switch (selectedInstrumentParameter)
            {
            case GUI_INST_PARAM_NAME:
            case GUI_INST_PARAM_PCM_RATE:
            case GUI_INST_PARAM_PCM_PAN:
            case GUI_INST_PARAM_PRESET:
            case GUI_INST_PARAM_VOLSEQ:
            case GUI_INST_PARAM_ARPSEQ:
                clear_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break;
            case GUI_INST_PARAM_PCM_NOTE: clear_cursor_3(pos_x * width + offset_x, pos_y + offset_y); break;
            default: clear_cursor_2(pos_x * width + offset_x, pos_y + offset_y); break;
            }
        }
        else
        {
            switch (selectedInstrumentParameter)
            {
            case GUI_INST_PARAM_NAME:
            case GUI_INST_PARAM_PCM_RATE:
            case GUI_INST_PARAM_PCM_PAN:
            case GUI_INST_PARAM_PRESET:
            case GUI_INST_PARAM_VOLSEQ:
            case GUI_INST_PARAM_ARPSEQ:
                draw_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break;
            case GUI_INST_PARAM_PCM_NOTE: draw_cursor_3(pos_x * width + offset_x, pos_y + offset_y); break;
            default: draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y); break;
            }
        }
        break;
    default: break;
    }
}
// ------------------------------ PATTERN MATRIX
inline void DisplayPatternMatrix()
{
    static u16 patternID = 0;
    static u8 shiftX = 0;
    static u8 shiftY = 0;
    static u8 pageShift = 0;
    static u8 palx = 0;
    static u16 num1 = 0;
    static u16 num2 = 0;
    static u16 num3 = 0;

    if (bInitScreen) // full refresh
    {
        bInitScreen = FALSE;
        // draw page number
        DrawHex2(PAL0, currentPage, 37, 0);
        line = 0; // line is actual line in array
        chan = 0; // channel

    }
    // redraw one value per cycle
    if (bRefreshScreen)
    {
        pageShift = currentPage * MATRIX_ROWS_ONPAGE; // page shift

        if (matrixRowToRefresh != OXFFFF) // if refresh only once only particular line
        {
            line = matrixRowToRefresh - pageShift; // map to 0..24
        }

        patternID = SRAM_ReadMatrix(chan, line + pageShift);

        // display assigned pattern number
        shiftX = chan * 3;
        shiftY = line + 2;

        if (!patternID)
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, 0), shiftX, shiftY); // clear digit 3
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), shiftX + 1, shiftY); // -
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), shiftX + 2, shiftY); // -
        }
        else
        {
            palx = PAL0;
            num1 = (patternID & 0xF00) >> 8;
            num2 = ((patternID & 0x0F0) >> 4) + bgBaseTileIndex[1];
            num3 = (patternID & 0x00F) + bgBaseTileIndex[1];

            if (num1 > 0) num1 += bgBaseTileIndex[3]; // else draw empty tile
            !(line & 1) ? (palx = PAL0) : (palx = PAL1);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num1), shiftX, shiftY); // clear or draw digit 3
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, num2), shiftX + 1, shiftY); // draw digit 2
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, num3), shiftX + 2, shiftY); // draw digit 1
        }
        chan++;

        if (chan > CHANNEL_PSG4_NOISE) // end or row
        {
            if (matrixRowToRefresh == OXFFFF) // redraw the whole matrix
            {
                u8 row;
                row = line + pageShift;
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + row), 39, shiftY); // line number

                if (row == loopStart) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[3] + GUI_LOOP_START), 39, shiftY);
                else if (row == loopEnd) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[3] + GUI_LOOP_END), 39, shiftY);
                else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), 39, shiftY);

                line++;
                chan = 0;
                if (line > MATRIX_ROWS_ONPAGE_LAST)
                {
                    line = 0;
                    bRefreshScreen = FALSE;
                    DrawMatrixPlaybackCursor(FALSE); // if returning from other page during playback
                }
            }
            else // redraw only currently changed edited row
            {
                matrixRowToRefresh = OXFFFF; // set the trigger off
                bRefreshScreen = FALSE;
                chan = 0;
            }
        }
    }
}
// ------------------------------ PATTERN EDITOR
static void ChangePatternParameter(s8 noteMod, s8 parameterMod)
{
    static s16 value = 0;
    static u8 row = 0;

    bRefreshScreen = TRUE;

    auto void write_note(u8 column)
    {
        row = selectedPatternRow + 16 * column;
        value = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
        if (value > NOTE_MAX)
        {
            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, lastEnteredNote);
            patternRowToRefresh = row;
        }
        else
        {
            value += noteMod;
            if (value < 0) value = 0; else if (value > NOTE_MAX) value = NOTE_MAX;
            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, value);
            patternRowToRefresh = row;
            lastEnteredNote = value;
        }
    }

    auto void write_instrument(u8 column)
    {
        row = selectedPatternRow + 16 * column;
        value = SRAM_ReadPattern(selectedPatternID, row, DATA_INSTRUMENT);
        if (!value)
        {
            SRAM_WritePattern(selectedPatternID, row, DATA_INSTRUMENT, lastEnteredInstrumentID);
            patternRowToRefresh = row;
        }
        else
        {
            value += parameterMod;
            if (value < 1) value = INSTRUMENTS_LAST; else if (value > INSTRUMENTS_LAST) value = 1;
            SRAM_WritePattern(selectedPatternID, row, DATA_INSTRUMENT, value);
            patternRowToRefresh = row;
            lastEnteredInstrumentID = value;
        }

        // print info: instrument name
        for (u8 i = 0; i < 8; i++)
        {
            VDP_setTileMapXY(BG_A,
                TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_ALPHABET[SRAM_ReadInstrument(lastEnteredInstrumentID, INST_NAME_1 + i)]),
                GUI_INFO_PRINT_INST_X + i, GUI_INFO_PRINT_INST_Y);
        }
    }

    auto void write_fx_type(u8 id, u8 column)
    {
        row = selectedPatternRow + 16 * column;
        value = SRAM_ReadPattern(selectedPatternID, row, id);
        if (!value)
        {
            SRAM_WritePattern(selectedPatternID, row, id, lastEnteredEffect);
            patternRowToRefresh = row;
        }
        else
        {
            value += parameterMod;
            if (value < 1) value = 0xFF; else if (value > 0xFF) value = 1;
            SRAM_WritePattern(selectedPatternID, row, id, value);
            patternRowToRefresh = row;
            lastEnteredEffect = value;
        }

        // print info: last entered effect description
        if (!strcmp(infoCommands[lastEnteredEffect], STRING_EMPTY))
        {
            VDP_clearTextArea(GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y, 39, 2);
        }
        else
        {
            VDP_setTextPalette(PAL0); VDP_drawText(infoCommands[lastEnteredEffect], GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y);
            VDP_setTextPalette(PAL1); VDP_drawText(infoDescriptions[lastEnteredEffect], GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y + 1);
        }
    }

    auto void write_fx_value(u8 id, u8 column)
    {
        row = selectedPatternRow + 16 * column;
        value = SRAM_ReadPattern(selectedPatternID, row, id);
        if (!value)
        {
            SRAM_WritePattern(selectedPatternID, row, id, lastEnteredEffectValue);
            patternRowToRefresh = row;
        }
        else
        {
            value += parameterMod;
            if (value < 1) value = 0xFF; else if (value > 0xFF) value = 1;
            SRAM_WritePattern(selectedPatternID, row, id, value);
            patternRowToRefresh = row;
            lastEnteredEffectValue = value;
        }
    }

    switch (selectedPatternColumn)
    {
    case DATA_NOTE:                             write_note(0); break;
    case DATA_INSTRUMENT:                       write_instrument(0);break;
    case DATA_FX1_TYPE:                         write_fx_type(DATA_FX1_TYPE, 0); break;
    case DATA_FX1_VALUE:                        write_fx_value(DATA_FX1_VALUE, 0); break;
    case DATA_FX2_TYPE:                         write_fx_type(DATA_FX2_TYPE, 0); break;
    case DATA_FX2_VALUE:                        write_fx_value(DATA_FX2_VALUE, 0); break;
    case DATA_FX3_TYPE:                         write_fx_type(DATA_FX3_TYPE, 0); break;
    case DATA_FX3_VALUE:                        write_fx_value(DATA_FX3_VALUE, 0); break;
    case DATA_FX4_TYPE:                         write_fx_type(DATA_FX4_TYPE, 0); break;
    case DATA_FX4_VALUE:                        write_fx_value(DATA_FX4_VALUE, 0); break;
    case DATA_FX5_TYPE:                         write_fx_type(DATA_FX5_TYPE, 0); break;
    case DATA_FX5_VALUE:                        write_fx_value(DATA_FX5_VALUE, 0); break;
    case DATA_FX6_TYPE:                         write_fx_type(DATA_FX6_TYPE, 0); break;
    case DATA_FX6_VALUE:                        write_fx_value(DATA_FX6_VALUE, 0); break;

    case (DATA_NOTE + PATTERN_COLUMNS):         write_note(1); break;
    case (DATA_INSTRUMENT + PATTERN_COLUMNS):   write_instrument(1); break;
    case (DATA_FX1_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX1_TYPE, 1); break;
    case (DATA_FX1_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX1_VALUE, 1); break;
    case (DATA_FX2_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX2_TYPE, 1); break;
    case (DATA_FX2_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX2_VALUE, 1); break;
    case (DATA_FX3_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX3_TYPE, 1); break;
    case (DATA_FX3_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX3_VALUE, 1); break;
    case (DATA_FX4_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX4_TYPE, 1); break;
    case (DATA_FX4_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX4_VALUE, 1); break;
    case (DATA_FX5_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX5_TYPE, 1); break;
    case (DATA_FX5_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX5_VALUE, 1); break;
    case (DATA_FX6_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX6_TYPE, 1); break;
    case (DATA_FX6_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX6_VALUE, 1); break;
    }
}

inline void DisplayPatternEditor()
{
    if (bInitScreen)
    {
        bInitScreen = 0;
        line = 0;
    }

    if (bRefreshScreen)
    {
        static u8 note = 0;
        static u8 value = 0;
        static u8 side = 0; // left 0 or right 1
        static u8 shiftX = 0;
        static u8 shiftY = 0;
        static u8 palx = PAL0;
        static u8 palminus = PAL1;
        static u8 lineShiftY = 0;

        if (patternRowToRefresh != OXFF) // refresh only selected line
        {
            line = patternRowToRefresh; // actual line selected in pattern array
        }
        else // draw pattern number
        {
            u16 num1 = ((selectedPatternID & 0xF00) >> 8) + bgBaseTileIndex[1];
            u16 num2 = ((selectedPatternID & 0x0F0) >> 4) + bgBaseTileIndex[1];
            u16 num3 = (selectedPatternID & 0x00F) + bgBaseTileIndex[1];
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num1), 50, 0); // draw digit 3
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num2), 51, 0); // draw digit 2
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num3), 52, 0); // draw digit 1
        }

        // display pattern lines
        note = SRAM_ReadPattern(selectedPatternID, line, DATA_NOTE); // notes

        // calculate offsets for the right pattern columns display
        side = (line > 15) ? 1 : 0;
        shiftX = 20 * side;
        shiftY = 4 - (16 * side);
        palx = (line % 4 == 0) ? PAL0 : PAL1;
        lineShiftY = line + shiftY;

        auto void display_fx(u8 id_type, u8 id_value, u8 shift) {
            u8 _fx_t = SRAM_ReadPattern(selectedPatternID, line, id_type);
            u8 _fx_v = SRAM_ReadPattern(selectedPatternID, line, id_value);
            u8 xPos_t = GUI_FX_TYPE_START + shiftX + shift;
            u8 xPos_v = GUI_FX_VALUE_START + shiftX + shift;
            u16 _tile;
            u8 _pal;

            if (!_fx_t) { _pal = PAL1; _tile = bgBaseTileIndex[0]; }
            else { _pal = PAL0; _tile = bgBaseTileIndex[0] + _fx_t; }
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(_pal, 1, FALSE, FALSE, _tile), xPos_t, lineShiftY);

            if (!_fx_v && !_fx_t) { _pal = PAL1; _tile = bgBaseTileIndex[0]; }
            else if (!_fx_v && _fx_t) { _pal = PAL1; _tile = bgBaseTileIndex[3] + GUI_00; }
            else { _pal = PAL1; _tile = bgBaseTileIndex[0] + _fx_v; }
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(_pal, 1, FALSE, FALSE, _tile), xPos_v, lineShiftY);
        }
        // note name
        if (note < NOTE_EMPTY)
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_NOTE_NAMES[0][noteFreqID[note]]), 41 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_NOTE_NAMES[1][noteFreqID[note]]), 42 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + (noteOctave[note])), 43 + shiftX, lineShiftY); // octave number
        }
        else if (note == NOTE_OFF) // 97 is OFF
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_O), 41 + shiftX, lineShiftY); // note off
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_F), 42 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_F), 43 + shiftX, lineShiftY);
        }
        else
        {
            if (palx == PAL0) palminus = PAL1; else palminus = PAL2;
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 41 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 42 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 43 + shiftX, lineShiftY);
        }

        // instrument id
        value = SRAM_ReadPattern(selectedPatternID, line, DATA_INSTRUMENT);
        if (value == 0)
        {
            if (palx == PAL0) palminus = PAL1; else palminus = PAL2;
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 45 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 46 + shiftX, lineShiftY);
        }
        else DrawHex2(palx, value, 45 + shiftX, lineShiftY);

        display_fx(DATA_FX1_TYPE, DATA_FX1_VALUE, 0);
        display_fx(DATA_FX2_TYPE, DATA_FX2_VALUE, 2);
        display_fx(DATA_FX3_TYPE, DATA_FX3_VALUE, 4);
        display_fx(DATA_FX4_TYPE, DATA_FX4_VALUE, 6);
        display_fx(DATA_FX5_TYPE, DATA_FX5_VALUE, 8);
        display_fx(DATA_FX6_TYPE, DATA_FX6_VALUE, 10);
        // refresh all, line by frame; or only currently edited line;
        if (patternRowToRefresh == OXFF) // refresh all
        {
            line++;
            if (line > PATTERN_ROW_LAST)
            {
                line = 0;
                bRefreshScreen = FALSE;
            }
        }
        else // selected line is refreshed, stop here
        {
            patternRowToRefresh = OXFF;
            bRefreshScreen = FALSE;
        }
    }
}
// ------------------------------ INSTRUMENT EDITOR
static void ChangeInstrumentParameter(s8 modifier)
{
    static s16 value = 0;

    bRefreshScreen = TRUE;
    instrumentParameterToRefresh = selectedInstrumentParameter;

    auto void write_pcm(u8 byteNum)
    {
        value = SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, byteNum) + modifier;
        if (value < 0) value = 255; else if (value > 255) value = 0;
        SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, byteNum, value);

        // border check
        u32 sampleLength =
                (SRAM_ReadSampleRegion(activeSampleBank, selectedSampleNote, SAMPLE_END_1) << 16) |
                (SRAM_ReadSampleRegion(activeSampleBank, selectedSampleNote, SAMPLE_END_2) << 8) |
                 SRAM_ReadSampleRegion(activeSampleBank, selectedSampleNote, SAMPLE_END_3);

        if (sampleLength > DAC_DATA_END)
        {
            SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_END_1, (u8)((DAC_DATA_END >> 16) & 0xFF));
            SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_END_2, (u8)((DAC_DATA_END >> 8) & 0xFF));
            SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_END_3, (u8)(DAC_DATA_END & 0xFF));
        }
    }

    switch (selectedInstrumentParameter)
    {
    case GUI_INST_PARAM_ALG:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_ALG) + modifier;
        if (value < 0) value = 0; else if (value > 7) value = 7;
        SRAM_WriteInstrument(selectedInstrumentID, INST_ALG, value);
        break;
    case GUI_INST_PARAM_FMS:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_FMS) + modifier;
        if (value < 0) value = 0; else if (value > 7) value = 7;
        SRAM_WriteInstrument(selectedInstrumentID, INST_FMS, value);
        break;
    case GUI_INST_PARAM_AMS:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_AMS) + modifier;
        if (value < 0) value = 0; else if (value > 3) value = 3;
        SRAM_WriteInstrument(selectedInstrumentID, INST_AMS, value);
        break;
    case GUI_INST_PARAM_PAN:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_PAN) + modifier;
        if (value < 0) value = 0; else if (value > 3) value = 3;
        SRAM_WriteInstrument(selectedInstrumentID, INST_PAN, value);
        break;
    case GUI_INST_PARAM_FB:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_FB) + modifier;
        if (value < 0) value = 0; else if (value > 7) value = 7;
        SRAM_WriteInstrument(selectedInstrumentID, INST_FB, value);
        break;
    case GUI_INST_PARAM_TL:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_TL1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0x7F; else if (value > 0x7F) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_TL1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_RS:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_RS1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 3) value = 3;
            SRAM_WriteInstrument(selectedInstrumentID, INST_RS1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_MUL:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_MUL1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 15) value = 15;
            SRAM_WriteInstrument(selectedInstrumentID, INST_MUL1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_DT:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_DT1 + selectedInstrumentOperator) + modifier;
            if (value < 1) value = 1; else if (value > 7) value = 7;
            SRAM_WriteInstrument(selectedInstrumentID, INST_DT1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_AR:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_AR1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 31) value = 31;
            SRAM_WriteInstrument(selectedInstrumentID, INST_AR1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D1R:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_D1R1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 31) value = 31;
            SRAM_WriteInstrument(selectedInstrumentID, INST_D1R1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D1L:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_D1L1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 15) value = 15;
            SRAM_WriteInstrument(selectedInstrumentID, INST_D1L1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D2R:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_D2R1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 31) value = 31;
            SRAM_WriteInstrument(selectedInstrumentID, INST_D2R1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_RR:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_RR1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 15) value = 15;
            SRAM_WriteInstrument(selectedInstrumentID, INST_RR1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_AM:
        if (selectedInstrumentOperator < 4)
        {
            if (modifier < 0) SRAM_WriteInstrument(selectedInstrumentID, INST_AM1 + selectedInstrumentOperator, FALSE);
            else SRAM_WriteInstrument(selectedInstrumentID, INST_AM1 + selectedInstrumentOperator, TRUE);
        }
        break;
    case GUI_INST_PARAM_SSGEG:
        if (selectedInstrumentOperator < 4)
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_SSGEG1 + selectedInstrumentOperator) + modifier;
            if (value < 7) value = 7; else if (value > 15) value = 15;
            SRAM_WriteInstrument(selectedInstrumentID, INST_SSGEG1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_LFO:
        value = SRAMW_readByte(GLOBAL_LFO) + modifier;
        if (value < 7) value = 7; else if (value > 15) value = 15;
        SRAMW_writeByte(GLOBAL_LFO, value); SetGlobalLFO(value);
        break;
    case GUI_INST_PARAM_VOLSEQ:
        //value = SRAM_ReadInstrument(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator);
        value = SRAM_ReadSEQ_VOL(selectedInstrumentID, selectedInstrumentOperator);
        // align step by 8
        if (value == SEQ_VOL_MIN_ATT)
        {
            if (modifier == -8) modifier = -7;
            else if (modifier == 8) modifier = 7;
        }
        value -= modifier;
        if (value < SEQ_VOL_MIN_ATT)
        {
            if (modifier > 1 ) value = SEQ_VOL_MIN_ATT; else value = SEQ_VOL_MAX_ATT;
        }
        else if (value > SEQ_VOL_MAX_ATT)
        {
            if (modifier < -1) value = SEQ_VOL_MAX_ATT; else value = SEQ_VOL_MIN_ATT;
        }
        //SRAM_WriteInstrument(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator, value);
        SRAM_WriteSEQ_VOL(selectedInstrumentID, selectedInstrumentOperator, value);
        break;
    case GUI_INST_PARAM_ARPSEQ:
        //value = SRAM_ReadInstrument(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator); // need to check first
        value = SRAM_ReadSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator);
        if (value != NOTE_EMPTY)
        {
            value += modifier;
            if (value < 76) value = 76; else if (value > 124) value = 124; // +- 24 semitones
            //SRAM_WriteInstrument(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator, value);
            SRAM_WriteSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_NAME:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_NAME_1 + selectedInstrumentOperator) + modifier;
        if (value < 0) value = 37; else if (value > 37) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_NAME_1 + selectedInstrumentOperator, value);
        break;
    case GUI_INST_PARAM_PCM_BANK:
        value = selectedSampleBank + modifier;
        if (value > 3) value = 3;
        else if (value < 0) value = 0;
        selectedSampleBank = value;
        break;
    case GUI_INST_PARAM_PCM_NOTE:
        value = selectedSampleNote + modifier;
        if (value > NOTE_MAX) value = 0;
        else if (value < 0) value = NOTE_MAX;
        selectedSampleNote = value;
        break;
    case GUI_INST_PARAM_PCM_START:
        write_pcm(selectedInstrumentOperator);
        break;
    case GUI_INST_PARAM_PCM_END:
        write_pcm(selectedInstrumentOperator + SAMPLE_END_1);
        break;
    case GUI_INST_PARAM_PCM_LOOP:
        if (modifier < 0) SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_LOOP, FALSE);
        else SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_LOOP, TRUE);
        break;
    case GUI_INST_PARAM_PCM_RATE:
        value = SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_RATE) + modifier;
        if (value < SOUND_RATE_32000) value = SOUND_RATE_32000; else if (value > SOUND_RATE_8000) value = SOUND_RATE_8000;
        SRAM_WriteSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_RATE, value);
        break;
    case GUI_INST_PARAM_PCM_PAN:
        if (modifier == -1) SRAM_WriteSamplePan(selectedSampleBank, selectedSampleNote, SOUND_PAN_LEFT);
        else if (modifier == 1) SRAM_WriteSamplePan(selectedSampleBank, selectedSampleNote, SOUND_PAN_RIGHT);
        else SRAM_WriteSamplePan(selectedSampleBank, selectedSampleNote, SOUND_PAN_CENTER);
        break;
    case GUI_INST_PARAM_COPY: // tool, not saved to sram
        value = instCopyTo + modifier;
        if (value < 1) instCopyTo = 0xFF; else if (value > 0xFF) instCopyTo = 1; else instCopyTo = value; // guard, wrap
        break;
    case GUI_INST_PARAM_PRESET: // tool, not saved to sram
        value = midiPreset + modifier;
        if (value < 0) midiPreset = 0xFF; else if (value > 0xFF) midiPreset = 0; else midiPreset = value; // guard, wrap
        break;
    }
    CacheIstrumentToRAM(selectedInstrumentID); // update RAM struct
    // apply values at edit
    /*chInst[selectedMatrixChannel] = tmpInst[selectedMatrixChannel];
    if (selectedMatrixChannel < CHANNEL_FM3_OP3)
        WriteYM2612(selectedMatrixChannel, selectedMatrixChannel);
    else if (selectedMatrixChannel > CHANNEL_FM3_OP1 && selectedMatrixChannel < CHANNEL_PSG1)
        WriteYM2612(selectedMatrixChannel, selectedMatrixChannel - 3);*/
}

inline void DisplayInstrumentEditor()
{
    static u8 value = 0; // buffer
    static u8 alg = 0;
    static u8 stepDrawPos = 0;

    auto void draw_pcm_start()
    {
        DrawHex2(PAL0, SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_START_1), 113, 3);
        DrawHex2(PAL0, SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_START_2), 115, 3);
        DrawHex2(PAL0, SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_START_3), 117, 3);
    }

    auto void draw_pcm_end()
    {
        DrawHex2(PAL0, SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_END_1), 113, 4);
        DrawHex2(PAL0, SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_END_2), 115, 4);
        DrawHex2(PAL0, SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_END_3), 117, 4);
    }

    auto void draw_pcm_loop()
    {
        value = SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_LOOP);
        if (value == FALSE) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 113, 5);
        else DrawText(BG_A, PAL0, "ON", 113, 5);
    }

    auto void draw_pcm_rate()
    {
        value = SRAM_ReadSampleRegion(selectedSampleBank, selectedSampleNote, SAMPLE_RATE);
        switch (value)
        {
            case SOUND_RATE_32000: DrawNum(BG_A, PAL1, "32000", 114, 6); break;
            case SOUND_RATE_22050: DrawNum(BG_A, PAL1, "22050", 114, 6); break;
            case SOUND_RATE_16000: DrawNum(BG_A, PAL1, "16000", 114, 6); break;
            case SOUND_RATE_13400: DrawNum(BG_A, PAL1, "13400", 114, 6); break;
            case SOUND_RATE_11025: DrawNum(BG_A, PAL1, "11025", 114, 6); break;
            case SOUND_RATE_8000: DrawNum(BG_A, PAL1, "8000", 114, 6); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, NULL), 118, 6); break;
            default: DrawNum(BG_A, PAL1, "-----", 114, 6); break;
        }
    }

    auto void draw_pcm_pan()
    {
        value = SRAM_ReadSamplePan(selectedSampleBank, selectedSampleNote);
        switch (value)
        {
            case SOUND_PAN_CENTER: VDP_setTextPalette(PAL1); VDP_drawText("C", 114, 7); break;
            case SOUND_PAN_LEFT: VDP_setTextPalette(PAL1); VDP_drawText("L", 114, 7); break;
            case SOUND_PAN_RIGHT: VDP_setTextPalette(PAL1); VDP_drawText("R", 114, 7); break;
            default: VDP_setTextPalette(PAL1); VDP_drawText("-", 114, 7); break;
        }
    }

    if (bInitScreen)
    {
        bInitScreen = FALSE;
        instrumentParameterToRefresh = OXFF;
    }

    if (bRefreshScreen)
    {
        if (instrumentParameterToRefresh == OXFF)
        {
            DrawHex2(PAL1, selectedInstrumentID, 87, 0);
        }

        switch (instrumentParameterToRefresh)
        {
        case GUI_INST_PARAM_ALG: case 255:
            alg = SRAM_ReadInstrument(selectedInstrumentID, INST_ALG);
            DrawHex2(PAL0, alg, 87, 2);

            for(u8 i=0; i<4; i++)
            {
                for(u8 j=0; j<12; j++)
                {
                    switch (GUI_FM_ALG_GRID[alg][i][j])
                    {
                        case GUI_FM_OP:
                        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_ALG_GRID[alg][i][j]), 90+j, 3+i);
                        break;
                        case GUI_DIGIT_1: case GUI_DIGIT_2: case GUI_DIGIT_3: case GUI_DIGIT_4:
                        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_FM_ALG_GRID[alg][i][j]), 90+j, 3+i);
                        break;
                        case GUI_ALG_CORNER_LD: case GUI_ALG_CORNER_LU: case GUI_ALG_CORNER_UDR:
                        case GUI_ALG_CORNER_PLUS: case GUI_ALG_CORNER_DR: case GUI_ALG_CORNER_UR:
                        case GUI_ALG_LINE_H: case GUI_ARROW:
                        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_ALG_GRID[alg][i][j]), 90+j, 3+i);
                        break;
                        default: VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 90+j, 3+i);
                    }
                }
            }
            break;
        case GUI_INST_PARAM_FMS: case 254:
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_FMS);
            if (!value) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_MINUS, 2, 87, 3);
            else DrawHex2(PAL0, value, 87, 3);
            break;
        case GUI_INST_PARAM_AMS: case 253:
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_AMS);
            if (!value) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_MINUS, 2, 87, 4);
            else DrawHex2(PAL0, value, 87, 4);
            break;
        case GUI_INST_PARAM_PAN: case 252:
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_PAN);
            switch (value)
            {
                case 0: FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_MINUS, 2, 87, 5); break;
                case 1:
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 87, 5);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_R), 88, 5);
                    break;
                case 2:
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 88, 5);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_L), 87, 5);
                    break;
                default: DrawText(BG_A, PAL0, "LR", 87, 5); break;
            }
            break;
        case GUI_INST_PARAM_FB: case 251:
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_FB);
            if (!value) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_MINUS, 2, 87, 6);
            else DrawHex2(PAL0, value, 87, 6);
            break;
        case GUI_INST_PARAM_TL: case 250:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, /*0x7F - */SRAM_ReadInstrument(selectedInstrumentID, INST_TL1 + i), 94 + i*3, 9);
            break;
        case GUI_INST_PARAM_RS: case 249:
            for (u8 i=0; i<4; i++)
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + SRAM_ReadInstrument(selectedInstrumentID, INST_RS1 + i)), 95 + i*3, 10);
            break;
        case GUI_INST_PARAM_MUL: case 248:
            for (u8 i=0; i<4; i++)
            {
                u8 mul = SRAM_ReadInstrument(selectedInstrumentID, INST_MUL1 + i);
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + mul), 95 + i*3, 11);
                if (mul > 1)
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + mul + (GUI_MUL-2)), 93 + i*3, 11);
                else
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 93 + i*3, 11);
            }
            break;
        case GUI_INST_PARAM_DT: case 247:
            for (u8 i=0; i<4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_DT1 + i);
                if (value > 4) {
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + value - 4), 95 + i*3, 12);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PLUS), 94 + i*3, 12); }
                else if (value < 4) {
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + 4 - value), 95 + i*3, 12);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 94 + i*3, 12); }
                else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 94 + i*3, 12);
            }
            break;
        case GUI_INST_PARAM_AR: case 246:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, SRAM_ReadInstrument(selectedInstrumentID, INST_AR1 + i), 94 + i*3, 14);
            break;
        case GUI_INST_PARAM_D1R: case 245:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, SRAM_ReadInstrument(selectedInstrumentID, INST_D1R1 + i), 94 + i*3, 15);
            break;
        case GUI_INST_PARAM_D1L: case 244:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, SRAM_ReadInstrument(selectedInstrumentID, INST_D1L1 + i), 94 + i*3, 16);
            break;
        case GUI_INST_PARAM_D2R: case 243:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, SRAM_ReadInstrument(selectedInstrumentID, INST_D2R1 + i), 94 + i*3, 17);
            break;
        case GUI_INST_PARAM_RR: case 242:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, SRAM_ReadInstrument(selectedInstrumentID, INST_RR1 + i), 94 + i*3, 18);
            break;
        case GUI_INST_PARAM_AM: case 241:
            for (u8 i=0; i<4; i++)
                if (SRAM_ReadInstrument(selectedInstrumentID, INST_AM1 + i)) DrawText(BG_A, PAL0, "ON", 94 + i*3, 20);
                else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 94 + i*3, 20);
            break;
        case GUI_INST_PARAM_SSGEG: case 240:
            for (u8 i=0; i<4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_SSGEG1 + i);
                if (value > 7)
                {
                    value -= 7;
                    DrawHex2(PAL0, value, 94 + i*3, 21);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + value + (GUI_SSG-1)), 93 + i*3, 21);
                }
                else
                {
                    FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 94 + i*3, 21);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 93 + i*3, 21);
                }
            }
        case GUI_INST_PARAM_LFO: case 239:
            value = SRAMW_readByte(GLOBAL_LFO);
            if (value > 7) DrawHex2(BG_A, value - 7, 80+12, 23);
            else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 86, 24);
            break;
        case GUI_INST_PARAM_VOLSEQ: case 238:
            for (u8 i = 0; i <= SEQ_STEP_LAST; i++)
            {
                //value = SRAM_ReadInstrument(selectedInstrumentID, INST_VOL_TICK_01 + i);
                value = SRAM_ReadSEQ_VOL(selectedInstrumentID, i);
                //stepDrawPos = 86 + (i*2) + (i/8);
                stepDrawPos = 85 + i + (i/8);

                switch (value)
                {
                case SEQ_VOL_SKIP:
                    //if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 25); // skip dashes
                    //else FillRowRight(BG_A, PAL2, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 25); // dark gray
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 25);
                    break;
                case SEQ_VOL_MAX_ATT:
                    //if (i%4==0) FillRowRight(BG_A, PAL0, FALSE, FALSE, GUI_SEQ_VOL_MAX_ATT, 2, stepDrawPos, 25); // muted dots
                    //else FillRowRight(BG_A, PAL2, FALSE, FALSE, GUI_SEQ_VOL_MAX_ATT, 2, stepDrawPos, 25); // dark gray
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEQ_VOL_MAX_ATT), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEQ_VOL_MAX_ATT), stepDrawPos, 25);
                    break;
                case SEQ_VOL_MIN_ATT:
                    //if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_SEQ_VOL_MIN_ATT, 2, stepDrawPos, 25); // max vol
                    //else FillRowRight(BG_A, PAL2, FALSE, FALSE, GUI_SEQ_VOL_MIN_ATT, 2, stepDrawPos, 25); // dark gray
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEQ_VOL_MIN_ATT), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEQ_VOL_MIN_ATT), stepDrawPos, 25);
                    break;
                default:
                    //if (i%4==0) DrawHex2(PAL3, SEQ_VOL_MAX_ATT - value, stepDrawPos, 25); // inverted
                    //else DrawHex2(PAL0, SEQ_VOL_MAX_ATT - value, stepDrawPos, 25); // white
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[0] + SEQ_VOL_MAX_ATT - value), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[0] + SEQ_VOL_MAX_ATT - value), stepDrawPos, 25);
                    break;
                }
            }
        case GUI_INST_PARAM_ARPSEQ: case 237:
            for (u8 i = 0; i <= SEQ_STEP_LAST; i++)
            {
                //value = SRAM_ReadInstrument(selectedInstrumentID, INST_ARP_TICK_01 + i);
                value = SRAM_ReadSEQ_ARP(selectedInstrumentID, i);
                //stepDrawPos = 86 + (i*2) + (i/8);
                stepDrawPos = 85 + i + (i/8);
                if (value == 100)
                {
                    //if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_BIGDOT, 2, stepDrawPos, 26);
                    //else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, stepDrawPos, 26);
                    //VDP_fillTileMapRect(BG_A, NULL, stepDrawPos, 27, 2, 1);
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), stepDrawPos, 27);
                }
                else if (value == NOTE_EMPTY)
                {
                    //if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 26);
                    //else FillRowRight(BG_A, PAL2, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 26);
                    //VDP_fillTileMapRect(BG_A, NULL, stepDrawPos, 27, 2, 1);
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), stepDrawPos, 27);
                }
                else if (value > 100)
                {
                    //if (i%4==0) DrawHex2(PAL3, value - 100, stepDrawPos, 26);
                    //else DrawHex2(PAL0, value - 100, stepDrawPos, 26);
                    //VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_UP1), stepDrawPos, 27);
                    //VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_UP2), stepDrawPos+1, 27);
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[0] + value - 100), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[0] + value - 100), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_UP1), stepDrawPos, 27);
                }
                else
                {
                    //if (i%4==0) DrawHex2(PAL3, 100 - value, stepDrawPos, 26);
                    //else DrawHex2(PAL0, 100 - value, stepDrawPos, 26);
                    //VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_DOWN1), stepDrawPos, 27);
                    //VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_DOWN2), stepDrawPos+1, 27);
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[0] + 100 - value), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[0] + 100 - value), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_DOWN1), stepDrawPos, 27);
                }
            }
            break;
        case GUI_INST_PARAM_NAME: case 236:
            for (u8 i = 0; i < 8; i++)
            {
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_ALPHABET[SRAM_ReadInstrument(selectedInstrumentID, INST_NAME_1 + i)]), GUI_INST_NAME_START + i, 0);
            }
            break;
        case GUI_INST_PARAM_STATE: case 235:
            switch(instrumentIsMuted[selectedInstrumentID])
            {
                case INST_MUTE: DrawText(BG_A, PAL0, "MUTE", 113, 20); break;
                case INST_PLAY: DrawText(BG_A, PAL0, "PLAY", 113, 20); break;
                case INST_SOLO: DrawText(BG_A, PAL0, "SOLO", 113, 20); break;
            }
        // these parameters are separate from instrument
        case GUI_INST_PARAM_PCM_BANK: case GUI_INST_PARAM_PCM_NOTE:
            DrawHex2(PAL0, selectedSampleBank, 113, 0);

            // note
            u8 noteID = selectedSampleNote;
            while (noteID > 11) noteID -= 12;

            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_NOTE_NAMES[0][noteID]), 116, 0); // key
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_NOTE_NAMES[1][noteID]), 117, 0); // #/-
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + (selectedSampleNote / 12)), 118, 0); // octave number

            draw_pcm_start();
            draw_pcm_end();
            draw_pcm_loop();
            draw_pcm_rate();
            draw_pcm_pan();
            break;
        case GUI_INST_PARAM_PCM_START:
            draw_pcm_start();
            break;
        case GUI_INST_PARAM_PCM_END:
            draw_pcm_end();
            break;
        case GUI_INST_PARAM_PCM_LOOP:
            draw_pcm_loop();
            break;
        case GUI_INST_PARAM_PCM_RATE:
            draw_pcm_rate();
            break;
        case GUI_INST_PARAM_PCM_PAN:
            draw_pcm_pan();
            break;
        case GUI_INST_PARAM_COPY:
            DrawHex2(PAL0, instCopyTo, GUI_INST_NAME_START, 1); // same x position as NAME
            break;
        case GUI_INST_PARAM_PRESET:
            intToStr(midiPreset, str, 3);
            VDP_setTextPalette(PAL1); VDP_drawText(str, 114, 17);
            VDP_drawText(presetName[midiPreset], 106, 18);
            break;
        }

        instrumentParameterToRefresh--; // refresh one parameter by frame to avoid song freeze
        if (instrumentParameterToRefresh < 235) bRefreshScreen = FALSE; // put last case here; redraw only changed parameter
    }
}
// only attenuate, not increase
static inline void SetChannelVolume(u8 mtxCh)
{
    static s16 vol[4] = {0,0,0,0};
    static u8 port = 0, ymCh = 0, psgCh = 0;

    auto inline void set_normal_channel_vol() {
        switch (chInst[mtxCh].ALG)
        {
        case 0: case 1: case 2: case 3:
            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
            }
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        case 4:
            vol[2] =
                channelSlotBaseLevel[mtxCh][2] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
            }
            chInst[mtxCh].TL3 = (u8)vol[2];
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        case 5: case 6:
            vol[1] =
                channelSlotBaseLevel[mtxCh][1] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[1] > 0x7F) vol[1] = 0x7F;

            vol[2] =
                channelSlotBaseLevel[mtxCh][2] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, (u8)vol[1]);
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
            }
            chInst[mtxCh].TL2 = (u8)vol[1];
            chInst[mtxCh].TL3 = (u8)vol[2];
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        case 7:
            vol[0] =
                channelSlotBaseLevel[mtxCh][0] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[0] > 0x7F) vol[0] = 0x7F;

            vol[1] =
                channelSlotBaseLevel[mtxCh][1] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[1] > 0x7F) vol[1] = 0x7F;

            vol[2] =
                channelSlotBaseLevel[mtxCh][2] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelSeqAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + ymCh, (u8)vol[0]);
                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, (u8)vol[1]);
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
            }
            chInst[mtxCh].TL1 = (u8)vol[0];
            chInst[mtxCh].TL2 = (u8)vol[1];
            chInst[mtxCh].TL3 = (u8)vol[2];
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        default: break;
        }
    }

    auto inline void set_special_channel_vol() {
        switch (mtxCh)
        {
        case CHANNEL_FM3_OP4:
            vol[3] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][3] +
                channelAttenuation[CHANNEL_FM3_OP4] +
                channelSeqAttenuation[CHANNEL_FM3_OP4] +
                channelTremolo[CHANNEL_FM3_OP4];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;
        case CHANNEL_FM3_OP3:
            vol[2] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][2] +
                channelAttenuation[CHANNEL_FM3_OP3] +
                channelSeqAttenuation[CHANNEL_FM3_OP3] +
                channelTremolo[CHANNEL_FM3_OP3];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
            chInst[mtxCh].TL3 = (u8)vol[2];
            break;
        case CHANNEL_FM3_OP2:
            vol[1] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][1] +
                channelAttenuation[CHANNEL_FM3_OP2] +
                channelSeqAttenuation[CHANNEL_FM3_OP2] +
                channelTremolo[CHANNEL_FM3_OP2];
            if (vol[1] > 0x7F) vol[1] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, (u8)vol[1]);
            chInst[mtxCh].TL2 = (u8)vol[1];
            break;
        case CHANNEL_FM3_OP1:
            vol[0] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][0] +
                channelAttenuation[CHANNEL_FM3_OP1] +
                channelSeqAttenuation[CHANNEL_FM3_OP1] +
                channelTremolo[CHANNEL_FM3_OP1];
            if (vol[0] > 0x7F) vol[0] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + ymCh, (u8)vol[0]);
            chInst[mtxCh].TL1 = (u8)vol[0];
            break;
        }
    }

    switch (mtxCh)
    {
    case CHANNEL_FM1: case CHANNEL_FM2:
        port = PORT_1; ymCh = mtxCh;
        set_normal_channel_vol();
        break;

    case CHANNEL_FM3_OP4:
        port = PORT_1; ymCh = 2;
        if (FM_CH3_Mode == CH3_NORMAL) { set_normal_channel_vol(); }
        else { set_special_channel_vol(); }
        break;

    case CHANNEL_FM3_OP3: case CHANNEL_FM3_OP2: case CHANNEL_FM3_OP1:
        if (FM_CH3_Mode == CH3_SPECIAL) { port = PORT_1; ymCh = 2; set_special_channel_vol(); }
        break;

    case CHANNEL_FM4: case CHANNEL_FM5: case CHANNEL_FM6_DAC:
        port = PORT_2; ymCh = mtxCh - 6;
        set_normal_channel_vol();
        break;

    default: // PSG
        psgCh = mtxCh - CHANNEL_PSG1;
        vol[0] =
        (
            channelAttenuation[mtxCh] +
            channelSeqAttenuation[mtxCh] +
            channelTremolo[mtxCh]
        ) / 8;

        if (vol[0] > PSG_ENVELOPE_MIN) vol[0] = PSG_ENVELOPE_MIN;
        if (bPsgIsPlayingNote[psgCh]) PSG_setEnvelope(psgCh, (u8)vol[0]);
    break;
    }
    bWriteRegs = TRUE; // trigger
}

static inline void RequestZ80()
{
    if (!Z80_isBusTaken()) Z80_requestBus(TRUE);
}

static inline void ReleaseZ80()
{
    YM2612_write(PORT_1, YM2612REG_DAC); // needed for DAC
    if (Z80_isBusTaken()) Z80_releaseBus();
}

static inline void SetPitchPSG(u8 mtxCh, u8 note)
{
    static s8 key = 0;

    key = note + channelModNotePitch[mtxCh] + channelModNoteVibrato[mtxCh];

    if (key < PSG_LOWEST_NOTE) { key = PSG_LOWEST_NOTE;
        channelPitchSlideSpeed[mtxCh] = 0; }
    else if (key > NOTE_MAX) { key = NOTE_MAX;
        channelPitchSlideSpeed[mtxCh] = 0; }

    if (channelFlags[mtxCh])
    {
        switch (mtxCh)
        {
        case CHANNEL_PSG1:
            SetChannelVolume(mtxCh);
            PSG_setTone(0, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[mtxCh] / 2]); // write tone to PSG3 to supply PSG4 tonal noise
            break;
        case CHANNEL_PSG2:
            SetChannelVolume(mtxCh);
            PSG_setTone(1, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[mtxCh] / 2]);
            break;
        case CHANNEL_PSG3:
            switch (PSG_NoiseMode)
            {
            case PSG_TONAL_CH3_MUTED:
                PSG_setEnvelope(2, PSG_ENVELOPE_MIN); // mute PSG3 channel
                break;
            case PSG_TONAL_CH3_NOT_MUTED: case PSG_FIXED:
                SetChannelVolume(mtxCh);
                PSG_setTone(2, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[mtxCh] / 2]);
                break;
            }
            break;
        case CHANNEL_PSG4_NOISE:
            switch (PSG_NoiseMode)
            {
            case PSG_TONAL_CH3_MUTED:
                SetChannelVolume(mtxCh);
                PSG_setEnvelope(2, PSG_ENVELOPE_MIN); // mute PSG3 channel
                PSG_setTone(2, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[mtxCh] / 2]);
                break;
            case PSG_TONAL_CH3_NOT_MUTED:
                SetChannelVolume(mtxCh);
                PSG_setTone(2, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[mtxCh] / 2]);
                break;
            case PSG_FIXED:
                SetChannelVolume(mtxCh);
                break;
            }
            break;
        }
    }
}

// DAC is also here
static inline void SetPitchFM(u8 mtxCh, u8 note)
{
    static u8 part1 = 0, part2 = 0;
    static s8 key = 0;

    // CSM
    if ((mtxCh == CHANNEL_FM3_OP4) && (FM_CH3_Mode == CH3_SPECIAL_CSM))
    {
       key = FM_CH3_OpFreq[0] + channelModNotePitch[mtxCh] + channelModNoteVibrato[mtxCh];
    }
    // Normal or Special
    else
    {
        key = note + channelModNotePitch[mtxCh] + channelModNoteVibrato[mtxCh];
    }

    if ((key > -1) && (key < NOTES))
    {
        part1 = ((noteOctave[(u8)key]) << 3) | (noteMicrotone[noteFreqID[(u8)key]][(u8)channelFinalPitch[mtxCh]] >> 8);
        part2 = 0b0000000011111111 & noteMicrotone[noteFreqID[(u8)key]][(u8)channelFinalPitch[mtxCh]];

        switch (mtxCh)
        {
        case CHANNEL_FM1:
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH1_FREQ_MSB, part1);
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH1_FREQ_LSB, part2);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110000); // 0
            break;
        case CHANNEL_FM2:
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH2_FREQ_MSB, part1);
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH2_FREQ_LSB, part2);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110001); // 1
            break;
        case CHANNEL_FM3_OP4:
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_FREQ_MSB, part1);
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_FREQ_LSB, part2);

            switch (FM_CH3_Mode)
            {
            case CH3_NORMAL:
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110010); // 2
                break;
            case CH3_SPECIAL:
                BIT_SET(FM_CH3_OpNoteStatus, 7); // 0b1???0010
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus);// 2
                break;
            case CH3_SPECIAL_CSM:
                // Timer A to note pitch
                YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_MSB, (u8)(csmMicrotone[note] >> 2));
                YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_LSB, (u8)(csmMicrotone[note] & 0b0000000000000011));

                // play CSM note
                // bb: Ch3 mode, Reset B, Reset A, Enable B, Enable A, Load B, Load A
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_FREQ_MSB, CH3_SPECIAL_CSM | 0b00001111);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_SPECIAL_CSM | 0b00010101); //!?
                break;
            default: break;
            }
            break;
        case CHANNEL_FM3_OP3:
            if (FM_CH3_Mode == CH3_SPECIAL)
            {
                BIT_SET(FM_CH3_OpNoteStatus, 5); // 0b??1?0010
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP3_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP3_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // 2
            }
            break;
        case CHANNEL_FM3_OP2:
            if (FM_CH3_Mode == CH3_SPECIAL)
            {
                BIT_SET(FM_CH3_OpNoteStatus, 6); // 0b?1??0010
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP2_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP2_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // 2
            }
            break;
        case CHANNEL_FM3_OP1:
            if (FM_CH3_Mode == CH3_SPECIAL)
            {
                BIT_SET(FM_CH3_OpNoteStatus, 4); // 0b???10010
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP1_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP1_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // 2
            }
            break;
        case CHANNEL_FM4:
            YM2612_writeRegZ80(PORT_2, YM2612REG_CH1_FREQ_MSB, part1);
            YM2612_writeRegZ80(PORT_2, YM2612REG_CH1_FREQ_LSB, part2);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110100); // 4
            break;
        case CHANNEL_FM5:
            YM2612_writeRegZ80(PORT_2, YM2612REG_CH2_FREQ_MSB, part1);
            YM2612_writeRegZ80(PORT_2, YM2612REG_CH2_FREQ_LSB, part2);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110101); // 5
            break;
        case CHANNEL_FM6_DAC: // in DAC mode FM is still working normally, but sound output is muted
            if (bDAC_enable)
            {
                if (!FM_CH6_DAC_Pan)
                {
                    SND_startPlay_PCM(sampleStart[activeSampleBank][note],
                                  sampleLength[activeSampleBank][note],
                                  sampleRate[activeSampleBank][note],
                                  samplePan[activeSampleBank][note],
                                  sampleLoop[activeSampleBank][note]);
                }
                else
                {
                    SND_startPlay_PCM(sampleStart[activeSampleBank][note],
                                  sampleLength[activeSampleBank][note],
                                  sampleRate[activeSampleBank][note],
                                  FM_CH6_DAC_Pan,
                                  sampleLoop[activeSampleBank][note]);
                }
            }
            else
            {
                YM2612_writeRegZ80(PORT_2, YM2612REG_CH3_FREQ_MSB, part1);
                YM2612_writeRegZ80(PORT_2, YM2612REG_CH3_FREQ_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110110); // 6
            }
            break;
            default: break;
        }
    }
    else
    {
        channelPitchSlideSpeed[mtxCh]= 0;
    }
}
static inline void PlayNoteOff(u8 mtxCh)
{
        if (FM_CH3_Mode == CH3_SPECIAL_CSM && mtxCh == CHANNEL_FM3_OP4) FM_CH3_Mode = CH3_SPECIAL_CSM_OFF;
        StopChannelSound(mtxCh);
        StopEffects(mtxCh);
}

static inline void PlayNote(u8 note, u8 mtxCh)
{

    if (mtxCh < CHANNEL_PSG1) // FM
    {
        // S1>S3>S2>S4 for common registers and S4>S3>S1>S2 for CH3 frequencies
        StopChannelSound(mtxCh); // need to stop current playing note to write new data
        SetPitchFM(mtxCh, note); // set pitch (or dac), trigger note
    }
    else // PSG
    {
        bPsgIsPlayingNote[mtxCh - CHANNEL_PSG1] = TRUE;
        SetPitchPSG(mtxCh, note);
    }
}

static void StopEffects(u8 mtxCh)
{
    channelNoteCut[mtxCh] = 0;

    channelNoteRetrigger[mtxCh] = 0;
    channelNoteRetriggerCounter[mtxCh] = 0;

    channelNoteDelayCounter[mtxCh] = 0;

    channelTremoloSpeed[mtxCh] = 0;
    channelTremoloSpeedMult[mtxCh] = 0x20;
    channelTremoloDepth[mtxCh] = 0;
    channelTremoloPhase[mtxCh] = TREMOLO_PHASE;

    channelVibratoDepth[mtxCh] = 0;
    channelVibratoSpeed[mtxCh] = 0;
    channelVibratoPhase[mtxCh] = VIBRATO_PHASE;

    channelPitchSlideSpeed[mtxCh] = 0;
    channelFinalPitch[mtxCh] = 0;

    channelModNoteVibrato[mtxCh] = 0;
    channelModNotePitch[mtxCh] = 0;
    channelMicrotone[mtxCh] = 0;

    channelPreviousNote[mtxCh] = NOTE_OFF;

    channelSEQCounter_ARP[mtxCh] = 0;
    channelSEQCounter_VOL[mtxCh] = 0;
}

// stopping sound on matrix channel
static inline void StopChannelSound(u8 mtxCh)
{
    switch (mtxCh)
    {
    case CHANNEL_FM1:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0);
        break;

    case CHANNEL_FM2:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 1);
        break;

    case CHANNEL_FM3_OP4:
        if (FM_CH3_Mode == CH3_NORMAL)
        {
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 2); // all OP Off
        }
        else if (FM_CH3_Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(FM_CH3_OpNoteStatus, 7);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // OP4
        }
        else if (FM_CH3_Mode == CH3_SPECIAL_CSM_OFF)
        {
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 2); // set operators key off for CSM to work
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_SPECIAL_CSM_OFF | 0b00001111);
            FM_CH3_Mode = CH3_SPECIAL_CSM;
        }
        break;

    case CHANNEL_FM3_OP3:
        if (FM_CH3_Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(FM_CH3_OpNoteStatus, 5);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // OP3
        }
        break;

    case CHANNEL_FM3_OP2:
        if (FM_CH3_Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(FM_CH3_OpNoteStatus, 6);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // OP2
        }
        break;

    case CHANNEL_FM3_OP1:
        if (FM_CH3_Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(FM_CH3_OpNoteStatus, 4);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, FM_CH3_OpNoteStatus); // OP1
        }
        break;

    case CHANNEL_FM4:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 4);
        break;

    case CHANNEL_FM5:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 5);
        break;

    case CHANNEL_FM6_DAC:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 6);
        if (SND_isPlaying_PCM()) SND_stopPlay_PCM();
        break;

    case CHANNEL_PSG1:
        PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
        bPsgIsPlayingNote[0] = FALSE;
        break;

    case CHANNEL_PSG2:
        PSG_setEnvelope(1, PSG_ENVELOPE_MIN);
        bPsgIsPlayingNote[1] = FALSE;
        break;

    case CHANNEL_PSG3:
        PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
        bPsgIsPlayingNote[2] = FALSE;
        break;

    case CHANNEL_PSG4_NOISE:
        PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
        bPsgIsPlayingNote[3] = FALSE;
        break;

    default: break;
    }
}

static inline void StopAllSound()
{
    for (u8 mtxCh = CHANNEL_FM1; mtxCh < CHANNELS_TOTAL; mtxCh++)
    {
        StopChannelSound(mtxCh);
        StopEffects(mtxCh);

        // only at playback stop, so note OFF is not affected
        channelVolumeChangeSpeed[mtxCh] = 0;

        channelSeqAttenuation[mtxCh] = SEQ_VOL_MIN_ATT;
        channelCurrentRowNote[mtxCh] =
        channelPreviousNote[mtxCh] = NOTE_EMPTY;

        //channelPreviousInstrument[mtxCh] =
        channelPreviousEffectType[mtxCh][0] =
        channelPreviousEffectType[mtxCh][1] =
        channelPreviousEffectType[mtxCh][2] =
        channelPreviousEffectType[mtxCh][3] =
        channelPreviousEffectType[mtxCh][4] =
        channelPreviousEffectType[mtxCh][5] = NULL;
    }
    // bb: Mode, ResetB ResetA, EnableB EnableA, LoadB LoadA
    //FM_CH3_Mode = CH3_NORMAL;
    YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, FM_CH3_Mode | 0b00010000);
}

static void SetGlobalLFO(u8 freq)
{
    YM2612_writeRegZ80(PORT_1, YM2612REG_GLOBAL_LFO, freq);
}

// cache instrument
static inline void CacheIstrumentToRAM(u8 id)
{
    tmpInst[id].ALG = SRAM_ReadInstrument(id, INST_ALG);
    tmpInst[id].AMS = SRAM_ReadInstrument(id, INST_AMS);
    tmpInst[id].FMS = SRAM_ReadInstrument(id, INST_FMS);
    tmpInst[id].PAN = SRAM_ReadInstrument(id, INST_PAN);
    tmpInst[id].FB = SRAM_ReadInstrument(id, INST_FB);

    tmpInst[id].TL1 = SRAM_ReadInstrument(id, INST_TL1);
    tmpInst[id].TL2 = SRAM_ReadInstrument(id, INST_TL2);
    tmpInst[id].TL3 = SRAM_ReadInstrument(id, INST_TL3);
    tmpInst[id].TL4 = SRAM_ReadInstrument(id, INST_TL4);

    tmpInst[id].RS1 = SRAM_ReadInstrument(id, INST_RS1);
    tmpInst[id].RS2 = SRAM_ReadInstrument(id, INST_RS2);
    tmpInst[id].RS3 = SRAM_ReadInstrument(id, INST_RS3);
    tmpInst[id].RS4 = SRAM_ReadInstrument(id, INST_RS4);

    tmpInst[id].MUL1 = SRAM_ReadInstrument(id, INST_MUL1);
    tmpInst[id].MUL2 = SRAM_ReadInstrument(id, INST_MUL2);
    tmpInst[id].MUL3 = SRAM_ReadInstrument(id, INST_MUL3);
    tmpInst[id].MUL4 = SRAM_ReadInstrument(id, INST_MUL4);

    tmpInst[id].DT1 = SRAM_ReadInstrument(id, INST_DT1);
    tmpInst[id].DT2 = SRAM_ReadInstrument(id, INST_DT2);
    tmpInst[id].DT3 = SRAM_ReadInstrument(id, INST_DT3);
    tmpInst[id].DT4 = SRAM_ReadInstrument(id, INST_DT4);

    tmpInst[id].AR1 = SRAM_ReadInstrument(id, INST_AR1);
    tmpInst[id].AR2 = SRAM_ReadInstrument(id, INST_AR2);
    tmpInst[id].AR3 = SRAM_ReadInstrument(id, INST_AR3);
    tmpInst[id].AR4 = SRAM_ReadInstrument(id, INST_AR4);

    tmpInst[id].D1R1 = SRAM_ReadInstrument(id, INST_D1R1);
    tmpInst[id].D1R2 = SRAM_ReadInstrument(id, INST_D1R2);
    tmpInst[id].D1R3 = SRAM_ReadInstrument(id, INST_D1R3);
    tmpInst[id].D1R4 = SRAM_ReadInstrument(id, INST_D1R4);

    tmpInst[id].D1L1 = SRAM_ReadInstrument(id, INST_D1L1);
    tmpInst[id].D1L2 = SRAM_ReadInstrument(id, INST_D1L2);
    tmpInst[id].D1L3 = SRAM_ReadInstrument(id, INST_D1L3);
    tmpInst[id].D1L4 = SRAM_ReadInstrument(id, INST_D1L4);

    tmpInst[id].D2R1 = SRAM_ReadInstrument(id, INST_D2R1);
    tmpInst[id].D2R2 = SRAM_ReadInstrument(id, INST_D2R2);
    tmpInst[id].D2R3 = SRAM_ReadInstrument(id, INST_D2R3);
    tmpInst[id].D2R4 = SRAM_ReadInstrument(id, INST_D2R4);

    tmpInst[id].RR1 = SRAM_ReadInstrument(id, INST_RR1);
    tmpInst[id].RR2 = SRAM_ReadInstrument(id, INST_RR2);
    tmpInst[id].RR3 = SRAM_ReadInstrument(id, INST_RR3);
    tmpInst[id].RR4 = SRAM_ReadInstrument(id, INST_RR4);

    tmpInst[id].AM1 = SRAM_ReadInstrument(id, INST_AM1);
    tmpInst[id].AM2 = SRAM_ReadInstrument(id, INST_AM2);
    tmpInst[id].AM3 = SRAM_ReadInstrument(id, INST_AM3);
    tmpInst[id].AM4 = SRAM_ReadInstrument(id, INST_AM4);

    tmpInst[id].SSGEG1 = SRAM_ReadInstrument(id, INST_SSGEG1);
    tmpInst[id].SSGEG2 = SRAM_ReadInstrument(id, INST_SSGEG2);
    tmpInst[id].SSGEG3 = SRAM_ReadInstrument(id, INST_SSGEG3);
    tmpInst[id].SSGEG4 = SRAM_ReadInstrument(id, INST_SSGEG4);

    // calculate YM2612 combined registers from module data
    tmpInst[id].FB_ALG = (tmpInst[id].FB << 3) | tmpInst[id].ALG;
    tmpInst[id].PAN_AMS_FMS = (tmpInst[id].PAN << 6) | (tmpInst[id].AMS << 4) | tmpInst[id].FMS;

    tmpInst[id].DT1_MUL1 = (tmpInst[id].DT1 << 4) | tmpInst[id].MUL1;
    tmpInst[id].DT2_MUL2 = (tmpInst[id].DT2 << 4) | tmpInst[id].MUL2;
    tmpInst[id].DT3_MUL3 = (tmpInst[id].DT3 << 4) | tmpInst[id].MUL3;
    tmpInst[id].DT4_MUL4 = (tmpInst[id].DT4 << 4) | tmpInst[id].MUL4;

    tmpInst[id].RS1_AR1 = (tmpInst[id].RS1 << 6) | tmpInst[id].AR1;
    tmpInst[id].RS2_AR2 = (tmpInst[id].RS2 << 6) | tmpInst[id].AR2;
    tmpInst[id].RS3_AR3 = (tmpInst[id].RS3 << 6) | tmpInst[id].AR3;
    tmpInst[id].RS4_AR4 = (tmpInst[id].RS4 << 6) | tmpInst[id].AR4;

    tmpInst[id].AM1_D1R1 = (tmpInst[id].AM1 << 7) | tmpInst[id].D1R1;
    tmpInst[id].AM2_D1R2 = (tmpInst[id].AM2 << 7) | tmpInst[id].D1R2;
    tmpInst[id].AM3_D1R3 = (tmpInst[id].AM3 << 7) | tmpInst[id].D1R3;
    tmpInst[id].AM4_D1R4 = (tmpInst[id].AM4 << 7) | tmpInst[id].D1R4;

    tmpInst[id].D1L1_RR1 = (tmpInst[id].D1L1 << 4) | tmpInst[id].RR1;
    tmpInst[id].D1L2_RR2 = (tmpInst[id].D1L2 << 4) | tmpInst[id].RR2;
    tmpInst[id].D1L3_RR3 = (tmpInst[id].D1L3 << 4) | tmpInst[id].RR3;
    tmpInst[id].D1L4_RR4 = (tmpInst[id].D1L4 << 4) | tmpInst[id].RR4;
}

inline void CalculateCombined(u8 mtxCh, u8 reg)
{
    switch (reg)
    {
        case COMB_FB_ALG:        chInst[mtxCh].FB_ALG = (chInst[mtxCh].FB << 3) | chInst[mtxCh].ALG; break;
        //[L,R,A,A,0,F,F,F]
        case COMB_PAN_AMS_FMS:   chInst[mtxCh].PAN_AMS_FMS = (chInst[mtxCh].PAN << 6) | (chInst[mtxCh].AMS << 4) | chInst[mtxCh].FMS; break;

        case COMB_DT_MUL_1:      chInst[mtxCh].DT1_MUL1 = (chInst[mtxCh].DT1 << 4) | chInst[mtxCh].MUL1; break;
        case COMB_DT_MUL_2:      chInst[mtxCh].DT2_MUL2 = (chInst[mtxCh].DT2 << 4) | chInst[mtxCh].MUL2; break;
        case COMB_DT_MUL_3:      chInst[mtxCh].DT3_MUL3 = (chInst[mtxCh].DT3 << 4) | chInst[mtxCh].MUL3; break;
        case COMB_DT_MUL_4:      chInst[mtxCh].DT4_MUL4 = (chInst[mtxCh].DT4 << 4) | chInst[mtxCh].MUL4; break;

        case COMB_RS_AR_1:       chInst[mtxCh].RS1_AR1 = (chInst[mtxCh].RS1 << 6) | chInst[mtxCh].AR1; break;
        case COMB_RS_AR_2:       chInst[mtxCh].RS2_AR2 = (chInst[mtxCh].RS2 << 6) | chInst[mtxCh].AR2; break;
        case COMB_RS_AR_3:       chInst[mtxCh].RS3_AR3 = (chInst[mtxCh].RS3 << 6) | chInst[mtxCh].AR3; break;
        case COMB_RS_AR_4:       chInst[mtxCh].RS4_AR4 = (chInst[mtxCh].RS4 << 6) | chInst[mtxCh].AR4; break;

        case COMB_AM_D1R_1:      chInst[mtxCh].AM1_D1R1 = (chInst[mtxCh].AM1 << 7) | chInst[mtxCh].D1R1; break;
        case COMB_AM_D1R_2:      chInst[mtxCh].AM2_D1R2 = (chInst[mtxCh].AM2 << 7) | chInst[mtxCh].D1R2; break;
        case COMB_AM_D1R_3:      chInst[mtxCh].AM3_D1R3 = (chInst[mtxCh].AM3 << 7) | chInst[mtxCh].D1R3; break;
        case COMB_AM_D1R_4:      chInst[mtxCh].AM4_D1R4 = (chInst[mtxCh].AM4 << 7) | chInst[mtxCh].D1R4; break;

        case COMB_D1L_RR_1:      chInst[mtxCh].D1L1_RR1 = (chInst[mtxCh].D1L1 << 4) | chInst[mtxCh].RR1; break;
        case COMB_D1L_RR_2:      chInst[mtxCh].D1L2_RR2 = (chInst[mtxCh].D1L2 << 4) | chInst[mtxCh].RR2; break;
        case COMB_D1L_RR_3:      chInst[mtxCh].D1L3_RR3 = (chInst[mtxCh].D1L3 << 4) | chInst[mtxCh].RR3; break;
        case COMB_D1L_RR_4:      chInst[mtxCh].D1L4_RR4 = (chInst[mtxCh].D1L4 << 4) | chInst[mtxCh].RR4; break;
        default: break;
    }
}
// changing instrument will not reset channel attenuation (post-fader)
static inline void SetChannelBaseVolume_FM(u8 mtxCh)
{
    auto inline void set_normal_slots()
    {
        switch (chInst[mtxCh].ALG)
        {
        case 0: case 1: case 2: case 3:
            channelSlotBaseLevel[mtxCh][3] = chInst[mtxCh].TL4;
            break;
        case 4:
            channelSlotBaseLevel[mtxCh][2] = chInst[mtxCh].TL3;
            channelSlotBaseLevel[mtxCh][3] = chInst[mtxCh].TL4;
            break;
        case 5: case 6:
            channelSlotBaseLevel[mtxCh][1] = chInst[mtxCh].TL2;
            channelSlotBaseLevel[mtxCh][2] = chInst[mtxCh].TL3;
            channelSlotBaseLevel[mtxCh][3] = chInst[mtxCh].TL4;
            break;
        case 7:
            channelSlotBaseLevel[mtxCh][0] = chInst[mtxCh].TL1;
            channelSlotBaseLevel[mtxCh][1] = chInst[mtxCh].TL2;
            channelSlotBaseLevel[mtxCh][2] = chInst[mtxCh].TL3;
            channelSlotBaseLevel[mtxCh][3] = chInst[mtxCh].TL4;
            break;
        default: break;
        }
    }

    if (mtxCh == CHANNEL_FM3_OP4 && FM_CH3_Mode != CH3_NORMAL)
    {
        channelSlotBaseLevel[CHANNEL_FM3_OP4][0] = chInst[mtxCh].TL1;
        channelSlotBaseLevel[CHANNEL_FM3_OP4][1] = chInst[mtxCh].TL2;
        channelSlotBaseLevel[CHANNEL_FM3_OP4][2] = chInst[mtxCh].TL3;
        channelSlotBaseLevel[CHANNEL_FM3_OP4][3] = chInst[mtxCh].TL4;
    } else set_normal_slots();
}

// write all YM2612 registers
static inline void WriteYM2612(u8 mtxCh)
{
    static u16 port = 0;
    static u8 ymCh = 0;

    switch (mtxCh)
    {
    case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM3_OP4:
        port = PORT_1; ymCh = mtxCh;
        break;
    case CHANNEL_FM4: case CHANNEL_FM5: case CHANNEL_FM6_DAC:
        port = PORT_2; ymCh = mtxCh - 6;
        break;
    default: return; break;
    }

    switch (ymCh)
    {
    case 0:
        YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0, chInst[mtxCh].FB_ALG);

        YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0, chInst[mtxCh].TL1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0, chInst[mtxCh].TL2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0, chInst[mtxCh].TL3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0, chInst[mtxCh].TL4);

        YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0, chInst[mtxCh].PAN_AMS_FMS);

        YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0, chInst[mtxCh].DT1_MUL1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH0, chInst[mtxCh].DT2_MUL2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH0, chInst[mtxCh].DT3_MUL3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH0, chInst[mtxCh].DT4_MUL4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0, chInst[mtxCh].RS1_AR1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH0, chInst[mtxCh].RS2_AR2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH0, chInst[mtxCh].RS3_AR3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH0, chInst[mtxCh].RS4_AR4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0, chInst[mtxCh].AM1_D1R1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH0, chInst[mtxCh].AM2_D1R2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH0, chInst[mtxCh].AM3_D1R3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH0, chInst[mtxCh].AM4_D1R4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH0, chInst[mtxCh].D2R1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH0, chInst[mtxCh].D2R2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH0, chInst[mtxCh].D2R3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH0, chInst[mtxCh].D2R4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0, chInst[mtxCh].D1L1_RR1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH0, chInst[mtxCh].D1L2_RR2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH0, chInst[mtxCh].D1L3_RR3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH0, chInst[mtxCh].D1L4_RR4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH0, chInst[mtxCh].SSGEG1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH0, chInst[mtxCh].SSGEG2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH0, chInst[mtxCh].SSGEG3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH0, chInst[mtxCh].SSGEG4);
        break;
    case 1:
        YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH1, chInst[mtxCh].FB_ALG);

        YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH1, chInst[mtxCh].TL1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH1, chInst[mtxCh].TL2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH1, chInst[mtxCh].TL3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH1, chInst[mtxCh].TL4);

        YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH1, chInst[mtxCh].PAN_AMS_FMS);

        YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH1, chInst[mtxCh].DT1_MUL1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH1, chInst[mtxCh].DT2_MUL2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH1, chInst[mtxCh].DT3_MUL3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH1, chInst[mtxCh].DT4_MUL4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH1, chInst[mtxCh].RS1_AR1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH1, chInst[mtxCh].RS2_AR2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH1, chInst[mtxCh].RS3_AR3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH1, chInst[mtxCh].RS4_AR4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH1, chInst[mtxCh].AM1_D1R1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH1, chInst[mtxCh].AM2_D1R2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH1, chInst[mtxCh].AM3_D1R3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH1, chInst[mtxCh].AM4_D1R4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH1, chInst[mtxCh].D2R1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH1, chInst[mtxCh].D2R2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH1, chInst[mtxCh].D2R3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH1, chInst[mtxCh].D2R4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH1, chInst[mtxCh].D1L1_RR1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH1, chInst[mtxCh].D1L2_RR2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH1, chInst[mtxCh].D1L3_RR3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH1, chInst[mtxCh].D1L4_RR4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH1, chInst[mtxCh].SSGEG1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH1, chInst[mtxCh].SSGEG2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH1, chInst[mtxCh].SSGEG3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH1, chInst[mtxCh].SSGEG4);
        break;
    case 2:
        YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH2, chInst[mtxCh].FB_ALG);

        YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH2, chInst[mtxCh].TL1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH2, chInst[mtxCh].TL2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH2, chInst[mtxCh].TL3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH2, chInst[mtxCh].TL4);

        YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH2, chInst[mtxCh].PAN_AMS_FMS);

        YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH2, chInst[mtxCh].DT1_MUL1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH2, chInst[mtxCh].DT2_MUL2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH2, chInst[mtxCh].DT3_MUL3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH2, chInst[mtxCh].DT4_MUL4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH2, chInst[mtxCh].RS1_AR1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH2, chInst[mtxCh].RS2_AR2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH2, chInst[mtxCh].RS3_AR3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH2, chInst[mtxCh].RS4_AR4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH2, chInst[mtxCh].AM1_D1R1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH2, chInst[mtxCh].AM2_D1R2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH2, chInst[mtxCh].AM3_D1R3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH2, chInst[mtxCh].AM4_D1R4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH2, chInst[mtxCh].D2R1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH2, chInst[mtxCh].D2R2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH2, chInst[mtxCh].D2R3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH2, chInst[mtxCh].D2R4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH2, chInst[mtxCh].D1L1_RR1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH2, chInst[mtxCh].D1L2_RR2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH2, chInst[mtxCh].D1L3_RR3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH2, chInst[mtxCh].D1L4_RR4);

        YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH2, chInst[mtxCh].SSGEG1);
        YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH2, chInst[mtxCh].SSGEG2);
        YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH2, chInst[mtxCh].SSGEG3);
        YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH2, chInst[mtxCh].SSGEG4);
        break;
    default: break;
    }
}

void DrawPP()
{
    // BPM
    if (BPM < 1000)
    {
        fix32ToStr(fBPM, str, 2);
        VDP_setTextPalette(PAL2);
        VDP_drawTextBG(BG_A, str, 5, 27);
        VDP_drawTextBG(BG_A, str, 45, 27);

        uintToStr(BPM, str, 3);
        DrawNum(BG_A, PAL0, str, 3, 27);
        DrawNum(BG_A, PAL1, str, 43, 27);
    }
    else { VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "OUT    ", 3, 27); VDP_drawTextBG(BG_A, "OUT    ", 43, 27); }

    // PPS
    if (PPS < 1000) { uintToStr(PPS, str, 3); DrawNum(BG_A, PAL1, str, 21, 27); DrawNum(BG_A, PAL1, str, 61, 27); }
    else { VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "OUT", 21, 27); VDP_drawTextBG(BG_A, "OUT", 61, 27); }

    // PPL
    DrawHex(PAL1, ppl_1, 27, 27); DrawHex(PAL1, ppl_2, 29, 27);
    DrawHex(PAL1, ppl_1, 67, 27); DrawHex(PAL1, ppl_2, 69, 27);

    // transpose
    /*if (!channelTranspose)
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_BIGDOT), 10, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_BIGDOT), 50, 27);
    }
    else if (channelTranspose > 0)
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_PLUS), 10, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_PLUS), 50, 27);
    }
    else
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_MINUS), 10, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_MINUS), 50, 27);
    }

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + channelTranspose), 11, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + channelTranspose), 51, 27);*/
}

static inline void ApplyCommand_Common(u8 mtxCh, u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // H-INT callback skip
    case 0x0F:
        if (fxValue) H_INT_SKIP = fxValue; else H_INT_SKIP = 2;
        VDP_setHIntCounter(H_INT_SKIP-1);
        SetBPM(0);
        break;
    // TEMPO
    case 0x13:
        SetBPM(fxValue);
        break;

    // TICKS PER LINE
    case 0x14:
        ppl_1 = (fxValue & 0b11110000) >> 4; if (!ppl_1) ppl_1 = PPL_DEFAULT;
        ppl_2 = fxValue & 0b00001111; if (!ppl_2) ppl_2 = PPL_DEFAULT;
        SetBPM(0);
        break;

    // CHANNEL TRANSPOSE
    case 0x17:
        switch ((fxValue & 0b11110000) >> 4)
        {
            case 1: channelTranspose[mtxCh] = fxValue & 0b00001111; break;
            case 2: channelTranspose[mtxCh] = -(fxValue & 0b00001111); break;
            default: channelTranspose[mtxCh] = 0; break;
        }
        //DrawPP();
        break;

    // ARP SEQUENCE MODE
    case 0x2F:
        if (!fxValue) channelArpSeqMODE[mtxCh] = 0; else channelArpSeqMODE[mtxCh] = 1;
        break;

    // ARP SEQUENCE
    case 0x30:
        channelArpSeqID[mtxCh] = fxValue;
        break;

    // PITCH SLIDE UP
    case 0x31:
        switch (fxValue)
        {
            case 0x00: // stop and reset
                channelPitchSlideSpeed[mtxCh] =
                channelMicrotone[mtxCh] =
                channelModNotePitch[mtxCh] = 0;
                if (!channelVibratoSpeed[mtxCh] && !channelVibratoDepth[mtxCh]) channelFinalPitch[mtxCh] = 0; //! breaks vibrato
                break;
            case 0xFE: // hold
                channelPitchSlideSpeed[mtxCh] = 0;
                break;
            case 0xFF: // reset
                channelPitchSlideSpeed[mtxCh] = channelPitchSlideValue[mtxCh];
                channelMicrotone[mtxCh] =
                channelModNotePitch[mtxCh] = 0;
                break;
            default: // do portamento
                if (fxValue < 0x80) channelPitchSlideSpeed[mtxCh] = channelPitchSlideValue[mtxCh] = (s8)fxValue;
                break;
        }
        break;

    // PITCH SLIDE DOWN
    case 0x32:
        switch (fxValue)
        {
            case 0x00: // stop and reset
                channelPitchSlideSpeed[mtxCh] =
                channelMicrotone[mtxCh] =
                channelModNotePitch[mtxCh] = 0;
                if (!channelVibratoSpeed[mtxCh] && !channelVibratoDepth[mtxCh]) channelFinalPitch[mtxCh] = 0; //! breaks vibrato
                break;
            case 0xFE: // hold
                channelPitchSlideSpeed[mtxCh] = 0;
                break;
            case 0xFF: // reset
                channelPitchSlideSpeed[mtxCh] = channelPitchSlideValue[mtxCh];
                channelMicrotone[mtxCh] =
                channelModNotePitch[mtxCh] = 0;
                break;
            default: // do portamento
                if (fxValue < 0x80) channelPitchSlideSpeed[mtxCh] = channelPitchSlideValue[mtxCh] = (s8)-fxValue;
                break;
        }
        break;

    // VIBRATO
    case 0x33:
        if (fxValue)
        {
            channelVibratoSpeed[mtxCh] = ((fxValue & 0b11110000) >> 4) * channelVibratoSpeedMult[mtxCh];
            channelVibratoDepth[mtxCh] = (fxValue & 0b00001111) * channelVibratoDepthMult[mtxCh];
        }
        else
        {
            channelVibratoSpeed[mtxCh] =
            channelVibratoDepth[mtxCh] =
            channelVibrato[mtxCh] = 0;
            if (!channelPitchSlideSpeed[mtxCh]) channelMicrotone[mtxCh] = channelFinalPitch[mtxCh] = 0; //! break portamento
        }
        channelVibratoPhase[mtxCh] =
        channelModNoteVibrato[mtxCh] = 0;
        break;

    // SET VIBRATO SPEED MULT
    case 0x34:
        if (fxValue)
        {
            channelVibratoSpeedMult[mtxCh] = fxValue;
        }
        else
        {
            channelVibratoSpeedMult[mtxCh] = 0x08;
        }
        channelVibratoSpeed[mtxCh] = ((fxValue & 0b11110000) >> 4) * channelVibratoSpeedMult[mtxCh];
        break;

    // SET VIBRATO DEPTH MULT
    case 0x35:
        if (fxValue)
        {
            channelVibratoDepthMult[mtxCh] = fxValue;
        }
        else
        {
            channelVibratoDepthMult[mtxCh] = 0x02;
        }
        channelVibratoDepth[mtxCh] = (fxValue & 0b00001111) * channelVibratoDepthMult[mtxCh];
        break;

    // SET VIBRATO MODE
    case 0x36:
        if (fxValue < 3) channelVibratoMode[mtxCh] = fxValue;
        break;

    // SET PORTAMENTO SKIP TICKS
    case 0x37:
        channelPitchSkipStep[mtxCh] = fxValue;
        break;

    // VOLUME SEQUENCE MODE
    case 0x3F:
        if (!fxValue) channelVolSeqMODE[mtxCh] = 0; else channelVolSeqMODE[mtxCh] = 1;
        break;

    // VOLUME SEQUENCE
    case 0x40:
        channelVolSeqID[mtxCh] = fxValue;
        if (!fxValue) channelSeqAttenuation[mtxCh] = 0;
        //channelVolumeChangeSpeed[mtxCh] = 0;
        break;

    // VOLUME ATTENUATION
    case 0x41:
        if (fxValue < 0x80)
        {
            channelAttenuation[mtxCh] = fxValue;
            SetChannelVolume(mtxCh);
        }
        channelVolumeChangeSpeed[mtxCh] = 0;
        break;

    // TREMOLO
    case 0x42:
        channelTremoloSpeed[mtxCh] = ((fxValue & 0b11110000) >> 4) * channelTremoloSpeedMult[mtxCh];
        channelTremoloDepth[mtxCh] = (fxValue & 0b00001111) * 2;
        channelTremoloPhase[mtxCh] = 512;
        channelVolumeChangeSpeed[mtxCh] = 0;
        break;

    // TREMOLO SPEED MULT
    case 0x43:
        channelTremoloSpeedMult[mtxCh] = fxValue;
        break;

    // VOLUME INCREASE
    case 0x44:
        channelVolumeChangeSpeed[mtxCh] = fxValue & 0b00001111;
        channelVolumePulseSkip[mtxCh] = ((fxValue & 0b11110000) >> 4) + 1;
        channelVolumePulseCounter[mtxCh] = 0;
        break;

    // VOLUME DECREASE
    case 0x45:
        channelVolumeChangeSpeed[mtxCh] = -(fxValue & 0b00001111);
        channelVolumePulseSkip[mtxCh] = ((fxValue & 0b11110000) >> 4) + 1;
        channelVolumePulseCounter[mtxCh] = 0;
        break;

    // NOTE CUT
    case 0x50:
        channelNoteCut[mtxCh] = fxValue;
        break;

    // NOTE RETRIGGER
    case 0x51:
        channelNoteRetrigger[mtxCh] = fxValue;
        channelNoteRetriggerCounter[mtxCh] = fxValue;
        break;

    // MATRIX JUMP
    case 0x52:
        if (fxValue < MATRIX_ROWS) matrixRowJumpTo = fxValue;
        else matrixRowJumpTo = OXFF;
        break;

    // PATTERN ROW JUMP
    case 0x53:
        if (fxValue < PATTERN_ROWS) patternRowJumpTo = fxValue;
        else patternRowJumpTo = OXFF;
        break;

    // NOTE DELAY
    case 0x54:
        channelNoteDelayCounter[mtxCh] = fxValue;
        break;

    // NOTE AUTO CUT
    case 0x56:
        channelNoteAutoCut[mtxCh] = fxValue;
        break;

    // PATTERN SIZE
    case 0x60:
        if (fxValue > 0 && fxValue < 0x20) patternSize = fxValue;
        break;

    // PATTERN SHIFT 0-15
    case 0x61:
        channelRowShift[mtxCh][fxValue >> 4] = fxValue & 0b00001111;
        break;

    // PATTERN SHIFT 16-31
    case 0x62:
        channelRowShift[mtxCh][(fxValue >> 4) + 16] = fxValue & 0b00001111;
        break;

    default: return; break;
    }
}

static inline void ApplyCommand_DAC(u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // DAC
    case 0x11:
        if (fxValue == 0x00)
        {
            RequestZ80(); YM2612_disableDAC(); ReleaseZ80();
            bDAC_enable = FALSE;
        }
        else if (fxValue == 0x01)
        {
            RequestZ80(); YM2612_enableDAC(); ReleaseZ80();
            bDAC_enable = TRUE;
        }
        break;

    // PAN
    case 0x0E:
        switch (fxValue)
        {
        case 0x10: FM_CH6_DAC_Pan = SOUND_PAN_LEFT;
            break;
        case 0x01: FM_CH6_DAC_Pan = SOUND_PAN_RIGHT;
            break;
        case 0x11: FM_CH6_DAC_Pan = SOUND_PAN_CENTER;
            break;
        default: FM_CH6_DAC_Pan = NULL; // default sample pan
            break;
        }
        break;

    // PCM SAMPLE BANK SET
    case 0x16:
        if (fxValue < 4) activeSampleBank = fxValue;
        break;

    // MSU MD CD audio PLAY ONCE
    case 0x20:
        if (!fxValue)
        {
            *mcd_cmd = MSU_PAUSE;
            *mcd_arg = 0;
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }
        else
        {
            *mcd_cmd = MSU_PLAY | fxValue; // track number
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }
        break;

    // MSU MD CD audio PLAY LOOP
    case 0x21:
        if (!fxValue)
        {
            *mcd_cmd = MSU_PAUSE;
            *mcd_arg = 0;
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }
        else
        {
            *mcd_cmd = MSU_PLAY_LOOP | fxValue;
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }
        break;

    // MSU MD CD audio SEEK TIME EMULATION
    case 0x22:
        if (!fxValue)
        {
            *mcd_cmd = MSU_SEEK_OFF;
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }
        else
        {
            *mcd_cmd = MSU_SEEK_ON;
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }
        break;

    default: break;
    }
}

static inline void ApplyCommand_PSG(u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // PSG N4 MODE
    case 0x15:
        switch (fxValue)
        {
        case 0x10: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_TONE3); PSG_NoiseMode = PSG_TONAL_CH3_MUTED; break;
        case 0x11: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_TONE3); PSG_NoiseMode = PSG_TONAL_CH3_NOT_MUTED; break;
        case 0x12: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_CLOCK2); PSG_NoiseMode = PSG_FIXED; break;
        case 0x13: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_CLOCK4); PSG_NoiseMode = PSG_FIXED; break;
        case 0x14: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_CLOCK8); PSG_NoiseMode = PSG_FIXED; break;
        case 0x20: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3); PSG_NoiseMode = PSG_TONAL_CH3_MUTED; break;
        case 0x21: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3); PSG_NoiseMode = PSG_TONAL_CH3_NOT_MUTED; break;
        case 0x22: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK2); PSG_NoiseMode = PSG_FIXED; break;
        case 0x23: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK4); PSG_NoiseMode = PSG_FIXED; break;
        case 0x24: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK8); PSG_NoiseMode = PSG_FIXED; break;
        default: return; break;
        }
        break;

    // PWM/Waveform
    /*case 0x17:
        if (fxValue < 0x0D) psgPWM = fxValue;
        break;*/
    default: return; break;
    }
}

static inline void ApplyCommand_FM3_SP(u8 mtxCh, u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // CH3 MODE
    case 0x12:
        if (!fxValue)
        {
            FM_CH3_Mode = CH3_NORMAL;
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_NORMAL | 0b00010000);
        }
        else if (fxValue == 1)
        {
            FM_CH3_Mode = CH3_SPECIAL;
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_SPECIAL | 0b00010000);
        }
        else if (fxValue == 2)
        {
            FM_CH3_Mode = CH3_SPECIAL_CSM;
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_SPECIAL_CSM | 0b00000101);
        };
        // bb: Mode, ResetB ResetA, EnableB EnableA, LoadB LoadA
        break;

    // CH3 CSM FILTER
    case 0x55:
        if (fxValue >= 0x09 && fxValue <= NOTE_MAX)
        {
            //switch (mtxCh)
            //{
            //case 2:
                FM_CH3_OpFreq[0] = fxValue;
                //break;
            //case 3: FM_CH3_OpFreq[1] = fxValue; break;
            //case 4: FM_CH3_OpFreq[2] = fxValue; break;
            //case 5: FM_CH3_OpFreq[3] = fxValue; break;
            //}
        }
        break;
    default: return; break;
    }
}

static inline void ApplyCommand_FM(u8 mtxCh, u8 id, u8 fxParam, u8 fxValue)
{
    static u16 port = 0;    // chip port (0..1)
    static u8 ymCh = 0;       // chip channel (0..2)

    // TL; 0 - unused, 000 0000 - TL (0..127) high to low ~0.75db step
    auto inline void write_tl1() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + ymCh, chInst[mtxCh].TL1); }
    auto inline void write_tl2() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, chInst[mtxCh].TL2); }
    auto inline void write_tl3() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, chInst[mtxCh].TL3); }
    auto inline void write_tl4() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, chInst[mtxCh].TL4); }

    // RS, AR
    // 2b - RS (0..3), 1b - unused, 5b - AR (0..31)
    auto inline void write_rs1_ar1()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0 + ymCh, chInst[mtxCh].RS1_AR1);
    }
    auto inline void write_rs2_ar2()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH0 + ymCh, chInst[mtxCh].RS2_AR2);
    }
    auto inline void write_rs3_ar3()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH0 + ymCh, chInst[mtxCh].RS3_AR3);
    }
    auto inline void write_rs4_ar4()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH0 + ymCh, chInst[mtxCh].RS4_AR4);
    }

    // DT, MUL (FM channels 0, 1, 2)
    // 1b - unused, 3b - DT1, 4b - MUL; DT1 = 1..-4+..8, MUL = 0..15
    auto inline void write_dt1_mul1()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT1_MUL1);
    }
    auto inline void write_dt2_mul2()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT2_MUL2);
    }
    auto inline void write_dt3_mul3()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT3_MUL3);

    }
    auto inline void write_dt4_mul4()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT4_MUL4);
    }

    // FB, ALG
    // 2b - unused, 3b (0..7) - FB, 3b - ALG (0..7)
    auto inline void write_fb_alg()
    {
        CalculateCombined(mtxCh, COMB_FB_ALG);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0 + ymCh, chInst[mtxCh].FB_ALG);
    }

    // PAN, AMS, FMS
    // 2b - PAN (1..3), 3b - AMS (0..7), 1b - unused, 2b - FMS (0..3)
    auto inline void write_pan_ams_fms()
    {
        CalculateCombined(mtxCh, COMB_PAN_AMS_FMS);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0 + ymCh, chInst[mtxCh].PAN_AMS_FMS);
    }

    // AM, D1R
    // 1b - AM (0 or 1), 2b - unused, 5b - D1R (0..31)
    auto inline void write_am1_d1r1()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM1_D1R1);
    }
    auto inline void write_am2_d1r2()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM2_D1R2);
    }
    auto inline void write_am3_d1r3()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM3_D1R3);
    }
    auto inline void write_am4_d1r4()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM4_D1R4);
    }

    // SSG-EG
    // 4b - unused, 1b - enable, 3b - SSG-EG (0..7), <8 disable, 8>= enable and set
    // ---------------------------------
    // | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    // |---------------|---|---|---|---|
    // | /   /   /   / | E |ATT|ALT|HLD|
    auto inline void write_ssgeg1() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG1); }
    auto inline void write_ssgeg2() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG2); }
    auto inline void write_ssgeg3() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG3); }
    auto inline void write_ssgeg4() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG4); }

    // D1L, RR
    // 4b - D1L (0..15), 4b - RR (0..15)
    auto inline void write_d1l1_rr1()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L1_RR1);
    }
    auto inline void write_d1l2_rr2()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L2_RR2);
    }
    auto inline void write_d1l3_rr3()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L3_RR3);
    }
    auto inline void write_d1l4_rr4()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L4_RR4);
    }

    // D2R
    // 3b - unused, 5b - D2R (0..31)
    auto inline void write_d2r1() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH0 + ymCh, chInst[mtxCh].D2R1); }
    auto inline void write_d2r2() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH0 + ymCh, chInst[mtxCh].D2R2); }
    auto inline void write_d2r3() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH0 + ymCh, chInst[mtxCh].D2R3); }
    auto inline void write_d2r4() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH0 + ymCh, chInst[mtxCh].D2R4); }

    switch (mtxCh)
    {
    case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM3_OP4:
        port = PORT_1; ymCh = mtxCh;
        break;
    case CHANNEL_FM4: case CHANNEL_FM5: case CHANNEL_FM6_DAC:
        port = PORT_2; ymCh = mtxCh - 6;
        break;
    default: return; break;
    }

    switch (fxParam)
    {
    // TOTAL LEVEL
    case 0x01:
        if (fxValue > 0x7F) chInst[mtxCh].TL1 = tmpInst[id].TL1;
        else chInst[mtxCh].TL1 = /*0x7F - */fxValue;
        write_tl1();
        break;
    case 0x02:
        if (fxValue > 0x7F)chInst[mtxCh].TL2 = tmpInst[id].TL2;
        else chInst[mtxCh].TL2 = /*0x7F - */fxValue;
        write_tl2();
        break;
    case 0x03:
        if (fxValue > 0x7F) chInst[mtxCh].TL3 = tmpInst[id].TL3;
        else chInst[mtxCh].TL3 = /*0x7F - */fxValue;
        write_tl3();
        break;
    case 0x04:
        if (fxValue > 0x7F) chInst[mtxCh].TL4 = tmpInst[id].TL4;
        else chInst[mtxCh].TL4 = /*0x7F - */fxValue;
        write_tl4();
        break;

    // RATE SCALE
    case 0x05:
        switch (fxValue)
        {
        case 0x01: chInst[mtxCh].RS1 = tmpInst[id].RS1; write_rs1_ar1(); break;
        case 0x02: chInst[mtxCh].RS2 = tmpInst[id].RS2; write_rs2_ar2(); break;
        case 0x03: chInst[mtxCh].RS3 = tmpInst[id].RS3; write_rs3_ar3(); break;
        case 0x04: chInst[mtxCh].RS4 = tmpInst[id].RS4; write_rs4_ar4(); break;
        case 0x00:
            chInst[mtxCh].RS1 = tmpInst[id].RS1; chInst[mtxCh].RS2 = tmpInst[id].RS2; chInst[mtxCh].RS3 = tmpInst[id].RS3; chInst[mtxCh].RS4 = tmpInst[id].RS4;
            write_rs1_ar1(); write_rs2_ar2(); write_rs3_ar3(); write_rs4_ar4();
            break;
        default:
                 if ((fxValue > 0x0F) && (fxValue < 0x14)) { chInst[mtxCh].RS1 = fxValue - 0x10; write_rs1_ar1(); }
            else if ((fxValue > 0x1F) && (fxValue < 0x24)) { chInst[mtxCh].RS2 = fxValue - 0x20; write_rs2_ar2(); }
            else if ((fxValue > 0x2F) && (fxValue < 0x34)) { chInst[mtxCh].RS3 = fxValue - 0x30; write_rs3_ar3(); }
            else if ((fxValue > 0x3F) && (fxValue < 0x44)) { chInst[mtxCh].RS4 = fxValue - 0x40; write_rs4_ar4(); }
            else if ((fxValue > 0x4F) && (fxValue < 0x54)) {
                chInst[mtxCh].RS1 = chInst[mtxCh].RS2 = chInst[mtxCh].RS3 = chInst[mtxCh].RS4 = fxValue - 0x50;
                write_rs1_ar1(); write_rs2_ar2(); write_rs3_ar3(); write_rs4_ar4(); }
            break;
        }
        break;

    // MULTIPLIER
    case 0x06:
        switch (fxValue)
        {
        case 0x01: chInst[mtxCh].MUL1 = tmpInst[id].MUL1; write_dt1_mul1(); break;
        case 0x02: chInst[mtxCh].MUL2 = tmpInst[id].MUL1; write_dt2_mul2(); break;
        case 0x03: chInst[mtxCh].MUL3 = tmpInst[id].MUL1; write_dt3_mul3(); break;
        case 0x04: chInst[mtxCh].MUL4 = tmpInst[id].MUL1; write_dt4_mul4(); break;
        case 0x00:
            chInst[mtxCh].MUL1 = tmpInst[id].MUL1;
            chInst[mtxCh].MUL2 = tmpInst[id].MUL2;
            chInst[mtxCh].MUL3 = tmpInst[id].MUL3;
            chInst[mtxCh].MUL4 = tmpInst[id].MUL4;
            write_dt1_mul1(); write_dt2_mul2(); write_dt3_mul3(); write_dt4_mul4();
            break;
        default:
                 if ((fxValue > 0x0F) && (fxValue < 0x20)) { chInst[mtxCh].MUL1 = fxValue - 0x10; write_dt1_mul1(chInst[mtxCh].MUL1); }
            else if ((fxValue > 0x1F) && (fxValue < 0x30)) { chInst[mtxCh].MUL2 = fxValue - 0x20; write_dt2_mul2(chInst[mtxCh].MUL2); }
            else if ((fxValue > 0x2F) && (fxValue < 0x40)) { chInst[mtxCh].MUL3 = fxValue - 0x30; write_dt3_mul3(chInst[mtxCh].MUL3); }
            else if ((fxValue > 0x3F) && (fxValue < 0x50)) { chInst[mtxCh].MUL4 = fxValue - 0x40; write_dt4_mul4(chInst[mtxCh].MUL4); }
            else if ((fxValue > 0x4F) && (fxValue < 0x60)) {
                chInst[mtxCh].MUL1 = chInst[mtxCh].MUL2 = chInst[mtxCh].MUL3 = chInst[mtxCh].MUL4 = fxValue - 0x50;
                write_dt1_mul1(); write_dt2_mul2(); write_dt3_mul3(); write_dt4_mul4(); }
            break;
        }
        break;

    // DETUNE
    case 0x07:
        switch (fxValue)
        {
        case 0x01: chInst[mtxCh].DT1 = tmpInst[id].DT1; write_dt1_mul1(); break;
        case 0x02: chInst[mtxCh].DT2 = tmpInst[id].DT2; write_dt2_mul2(); break;
        case 0x03: chInst[mtxCh].DT3 = tmpInst[id].DT3; write_dt3_mul3(); break;
        case 0x04: chInst[mtxCh].DT4 = tmpInst[id].DT4; write_dt4_mul4(); break;
        case 0x00:
            chInst[mtxCh].DT1 = tmpInst[id].DT1;
            chInst[mtxCh].DT2 = tmpInst[id].DT2;
            chInst[mtxCh].DT3 = tmpInst[id].DT3;
            chInst[mtxCh].DT4 = tmpInst[id].DT4;
            write_dt1_mul1(); write_dt2_mul2(); write_dt3_mul3(); write_dt4_mul4();
            break;
        default:
                 if ((fxValue > 0x0F) && (fxValue < 0x18)) { chInst[mtxCh].DT1 = fxValue - 0x10; write_dt1_mul1(chInst[mtxCh].DT1); }
            else if ((fxValue > 0x1F) && (fxValue < 0x28)) { chInst[mtxCh].DT2 = fxValue - 0x20; write_dt2_mul2(chInst[mtxCh].DT2); }
            else if ((fxValue > 0x2F) && (fxValue < 0x38)) { chInst[mtxCh].DT3 = fxValue - 0x30; write_dt3_mul3(chInst[mtxCh].DT3); }
            else if ((fxValue > 0x3F) && (fxValue < 0x48)) { chInst[mtxCh].DT4 = fxValue - 0x40; write_dt4_mul4(chInst[mtxCh].DT4); }
            else if ((fxValue > 0x4F) && (fxValue < 0x58)) {
                chInst[mtxCh].DT1 = chInst[mtxCh].DT2 = chInst[mtxCh].DT3 = chInst[mtxCh].DT4 = fxValue - 0x50;
                write_dt1_mul1(); write_dt2_mul2(); write_dt3_mul3(); write_dt4_mul4(); }
            break;
        }
        break;

    // LFO FREQUENCY
    case 0x10:
        if (fxValue < 9) SetGlobalLFO(fxValue + 7); // 7 .. F
        break;

    // ATTACK
    case 0xA1:
        if (fxValue > 0x1F) chInst[mtxCh].AR1 = tmpInst[id].AR1;
        else chInst[mtxCh].AR1 = fxValue;
        write_rs1_ar1();
        break;
    case 0xA2:
        if (fxValue > 0x1F) chInst[mtxCh].AR2 = tmpInst[id].AR2;
        else chInst[mtxCh].AR2 = fxValue;
        write_rs2_ar2();
        break;
    case 0xA3:
        if (fxValue > 0x1F) chInst[mtxCh].AR3 = tmpInst[id].AR3;
        else chInst[mtxCh].AR3 = fxValue;
        write_rs3_ar3();
        break;
    case 0xA4:
        if (fxValue > 0x1F) chInst[mtxCh].AR4 = tmpInst[id].AR4;
        else chInst[mtxCh].AR4 = fxValue;
        write_rs4_ar4();
        break;

    // DECAY 1
    case 0xB1:
        if (fxValue > 0x1F) chInst[mtxCh].D1R1 = tmpInst[id].D1R1;
        else chInst[mtxCh].D1R1 = fxValue;
        write_am1_d1r1();
        break;
    case 0xB2:
        if (fxValue > 0x1F) chInst[mtxCh].D1R2 = tmpInst[id].D1R2;
        else chInst[mtxCh].D1R2 = fxValue;
        write_am2_d1r2();
        break;
    case 0xB3:
        if (fxValue > 0x1F) chInst[mtxCh].D1R3 = tmpInst[id].D1R3;
        else chInst[mtxCh].D1R3 = fxValue;
        write_am3_d1r3();
        break;
    case 0xB4:
        if (fxValue > 0x1F) chInst[mtxCh].D1R4 = tmpInst[id].D1R4;
        else chInst[mtxCh].D1R4 = fxValue;
        write_am4_d1r4();
        break;

    // SUSTAIN
    case 0xC1:
        if (fxValue > 0x0F) chInst[mtxCh].D1L1 = tmpInst[id].D1L1;
        else chInst[mtxCh].D1L1 = fxValue;
        write_d1l1_rr1();
        break;
    case 0xC2:
        if (fxValue > 0x0F) chInst[mtxCh].D1L2 = tmpInst[id].D1L2;
        else chInst[mtxCh].D1L2 = fxValue;
        write_d1l2_rr2();
        break;
    case 0xC3:
        if (fxValue > 0x0F) chInst[mtxCh].D1L3 = tmpInst[id].D1L3;
        else chInst[mtxCh].D1L3 = fxValue;
        write_d1l3_rr3();
        break;
    case 0xC4:
        if (fxValue > 0x0F) chInst[mtxCh].D1L4 = tmpInst[id].D1L4;
        else chInst[mtxCh].D1L4 = fxValue;
        write_d1l4_rr4();
        break;

    // DECAY 2
    case 0xD1:
        if (fxValue > 0x1F) chInst[mtxCh].D2R1 = tmpInst[id].D2R1;
        else chInst[mtxCh].D2R1 = fxValue;
        write_d2r1();
        break;
    case 0xD2:
        if (fxValue > 0x1F) chInst[mtxCh].D2R2 = tmpInst[id].D2R2;
        else chInst[mtxCh].D2R2 = fxValue;
        write_d2r2();
        break;
    case 0xD3:
        if (fxValue > 0x1F) chInst[mtxCh].D2R3 = tmpInst[id].D2R3;
        else chInst[mtxCh].D2R3 = fxValue;
        write_d2r3();
        break;
    case 0xD4:
        if (fxValue > 0x1F) chInst[mtxCh].D2R4 = tmpInst[id].D2R4;
        else chInst[mtxCh].D2R4 = fxValue;
        write_d2r4();
        break;

    // RELEASE
    case 0xE1:
        if (fxValue > 0x0F) chInst[mtxCh].RR1 = tmpInst[id].RR1;
        else chInst[mtxCh].RR1 = fxValue;
        write_d1l1_rr1();
        break;
    case 0xE2:
        if (fxValue > 0x0F) chInst[mtxCh].RR2 = tmpInst[id].RR2;
        else chInst[mtxCh].RR2 = fxValue;
        write_d1l2_rr2();
        break;
    case 0xE3:
        if (fxValue > 0x0F) chInst[mtxCh].RR3 = tmpInst[id].RR3;
        else chInst[mtxCh].RR3 = fxValue;
        write_d1l3_rr3();
        break;
    case 0xE4:
        if (fxValue > 0x0F) chInst[mtxCh].RR4 = tmpInst[id].RR4;
        else chInst[mtxCh].RR4 = fxValue;
        write_d1l4_rr4();
        break;

    // AMPLITUDE MODULATION
    case 0x08:
        switch(fxValue)
        {
            case 0x01: { chInst[mtxCh].AM1 = tmpInst[id].AM1; write_am1_d1r1(); }
                break;
            case 0x11: { chInst[mtxCh].AM1 = 1; write_am1_d1r1(); }
                break;
            case 0x10: { chInst[mtxCh].AM1 = 0; write_am1_d1r1(); }
                break;
            case 0x02: { chInst[mtxCh].AM2 = tmpInst[id].AM2; write_am2_d1r2(); }
                break;
            case 0x21: { chInst[mtxCh].AM2 = 1; write_am2_d1r2(); }
                break;
            case 0x20: { chInst[mtxCh].AM2 = 0; write_am2_d1r2(); }
                break;
            case 0x03: { chInst[mtxCh].AM3 = tmpInst[id].AM3; write_am3_d1r3(); }
                break;
            case 0x31: { chInst[mtxCh].AM3 = 1; write_am3_d1r3(); }
                break;
            case 0x30: { chInst[mtxCh].AM3 = 0; write_am3_d1r3(); }
                break;
            case 0x04: { chInst[mtxCh].AM4 = tmpInst[id].AM4; write_am4_d1r4(); }
                break;
            case 0x41: { chInst[mtxCh].AM4 = 1; write_am4_d1r4(); }
                break;
            case 0x40: { chInst[mtxCh].AM4 = 0; write_am4_d1r4(); }
                break;
            case 0x00:
                chInst[mtxCh].AM1 = tmpInst[id].AM1;
                chInst[mtxCh].AM2 = tmpInst[id].AM2;
                chInst[mtxCh].AM3 = tmpInst[id].AM3;
                chInst[mtxCh].AM4 = tmpInst[id].AM4;
                write_am1_d1r1(); write_am2_d1r2(); write_am3_d1r3(); write_am4_d1r4();
                break;
            case 0x51:
                chInst[mtxCh].AM1 = chInst[mtxCh].AM2 = chInst[mtxCh].AM3 = chInst[mtxCh].AM4 = 1;
                write_am1_d1r1(); write_am2_d1r2(); write_am3_d1r3(); write_am4_d1r4();
                break;
            case 0x50:
                chInst[mtxCh].AM1 = chInst[mtxCh].AM2 = chInst[mtxCh].AM3 = chInst[mtxCh].AM4 = 0;
                write_am1_d1r1(); write_am2_d1r2(); write_am3_d1r3(); write_am4_d1r4();
                break;
            default: return; break;
        }
        break;

    // SSG-EG
    case 0x09:
        switch (fxValue)
        {
        case 0x01: chInst[mtxCh].SSGEG1 = tmpInst[id].SSGEG1; write_ssgeg1(); break;
        case 0x02: chInst[mtxCh].SSGEG2 = tmpInst[id].SSGEG2; write_ssgeg2(); break;
        case 0x03: chInst[mtxCh].SSGEG3 = tmpInst[id].SSGEG3; write_ssgeg3(); break;
        case 0x04: chInst[mtxCh].SSGEG4 = tmpInst[id].SSGEG4; write_ssgeg4(); break;
        case 0x00:
            chInst[mtxCh].SSGEG1 = tmpInst[id].SSGEG1;
            chInst[mtxCh].SSGEG2 = tmpInst[id].SSGEG2;
            chInst[mtxCh].SSGEG3 = tmpInst[id].SSGEG3;
            chInst[mtxCh].SSGEG4 = tmpInst[id].SSGEG4;
            write_ssgeg1(); write_ssgeg2(); write_ssgeg3(); write_ssgeg4();
            break;
        default:
                 if ((fxValue > 0x09) && (fxValue < 0x19)) { chInst[mtxCh].SSGEG1 = fxValue - 0x09; write_ssgeg1(); }
            else if ((fxValue > 0x1F) && (fxValue < 0x29)) { chInst[mtxCh].SSGEG2 = fxValue - 0x19; write_ssgeg2(); }
            else if ((fxValue > 0x2F) && (fxValue < 0x39)) { chInst[mtxCh].SSGEG3 = fxValue - 0x29; write_ssgeg3(); }
            else if ((fxValue > 0x3F) && (fxValue < 0x49)) { chInst[mtxCh].SSGEG4 = fxValue - 0x39; write_ssgeg4(); }
            else if ((fxValue > 0x4F) && (fxValue < 0x59)) {
                chInst[mtxCh].SSGEG1 = chInst[mtxCh].SSGEG2 = chInst[mtxCh].SSGEG3 = chInst[mtxCh].SSGEG4 = fxValue - 0x49;
                write_ssgeg1(); write_ssgeg2(); write_ssgeg3(); write_ssgeg4(); }
            break;
        }
        break;

    // ALGORITHM
    case 0x0A:
        if (fxValue > 7) chInst[mtxCh].ALG = tmpInst[id].ALG;
        else chInst[mtxCh].ALG = fxValue;
        write_fb_alg();
        break;

    // OP1 FEEDBACK
    case 0x0B:
        if (fxValue > 7) chInst[mtxCh].FB = tmpInst[id].FB;
        else chInst[mtxCh].FB = fxValue;
        write_fb_alg();
        break;

    // AMS = amplitude modulation sensitivity/scale
    case 0x0C:
        if (fxValue > 3) chInst[mtxCh].AMS = tmpInst[id].AMS;
        else chInst[mtxCh].AMS = fxValue;
        write_pan_ams_fms();
        break;

    // FMS = frequency modulation sensitivity/scale (actually PMS  = phase modulation sensitivity/scale)
    case 0x0D:
        if (fxValue > 7) chInst[mtxCh].FMS = tmpInst[id].FMS;
        else chInst[mtxCh].FMS = fxValue;
        write_pan_ams_fms();
        break;

    // PAN
    case 0x0E:
        switch (fxValue)
        {
        case 0x00: chInst[mtxCh].PAN = tmpInst[id].PAN; break;
        case 0x10: chInst[mtxCh].PAN = 2; break;
        case 0x01: chInst[mtxCh].PAN = 1; break;
        case 0x11: chInst[mtxCh].PAN = 3; break;
        default: chInst[mtxCh].PAN = 0; break;
        }
        write_pan_ams_fms();
        break;
    default: return; break;
    }
}

void DrawText(u8 plane, u8 pal, const char *str, u8 x, u8 y) // letters only
{
    u16 len = strlen(str);
    while(len--) VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, asciiBaseLetters + str[len]), x + len, y);
}

void DrawNum(u8 plane, u8 pal, const char *str, u8 x, u8 y) // numbers only
{
    u16 len = strlen(str);
    while(len--) VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, asciiBaseNumbers + str[len]), x + len, y);
}

void DrawHex2(u8 pal, u16 number, u8 x, u8 y) // u8 hex
{
    u16 dH = ((number & 0x0F0) >> 4) + bgBaseTileIndex[1];
    u16 dL = (number & 0x00F) + bgBaseTileIndex[1];
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, dH), x, y);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, dL), x+1, y);
}

void DrawHex(u8 pal, u8 number, u8 x, u8 y) // u8 hex
{
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, number + bgBaseTileIndex[1]), x, y);
}

void FillRowRight(u8 plane, u8 pal, u8 flipV, u8 flipH, u8 guiSymbol, u8 fillCount, u8 startX, u8 y) // fill with row symbol
{
    for (u8 x = startX; x < (startX + fillCount); x++)
        VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 1, flipV, flipH, bgBaseTileIndex[2] + guiSymbol), x, y);
}

void ReColorsAndTranspose() // on color change
{
    static u16 pt = 0;
    static s8 tr = 0;

    for (u8 ch = CHANNEL_FM1; ch <= CHANNEL_PSG4_NOISE; ch++)
    {
        for (u8 row = 0; row < MATRIX_ROWS_ONPAGE; row++)
        {
            pt = SRAM_ReadMatrix(ch, row + currentPage * MATRIX_ROWS_ONPAGE);
            tr = SRAM_ReadMatrixTranspose(ch, row + currentPage * MATRIX_ROWS_ONPAGE);
            if (pt != NULL)
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PATTERNCOLORS[SRAM_ReadPatternColor(pt)]), ch*3+2, row+2);
            }
            else
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), ch*3+2, row+2);
            }
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[3] + GUI_TRANSPOSE + tr), ch*3, row+2);
        }
    }
}

// instrument
static inline u8 SRAM_ReadInstrument(u8 id, u16 param) { return SRAMW_readByte((u32)INSTRUMENT_DATA + (id * INST_SIZE) + param); }
void SRAM_WriteInstrument(u8 id, u16 param, u8 data) { SRAMW_writeByte((u32)INSTRUMENT_DATA + (id * INST_SIZE) + param, data); }

// seq
static inline u8 SRAM_ReadSEQ_VOL(u8 id, u8 step) { return SRAMW_readByte((u32)SEQ_VOL_START + (id*32) + step); }
void SRAM_WriteSEQ_VOL(u8 id, u8 step, u8 data) { SRAMW_writeByte((u32)SEQ_VOL_START + (id*32) + step, data); }
static inline u8 SRAM_ReadSEQ_ARP(u8 id, u8 step) { return SRAMW_readByte((u32)SEQ_ARP_START + (id*32) + step); }
void SRAM_WriteSEQ_ARP(u8 id, u8 step, u8 data) { SRAMW_writeByte((u32)SEQ_ARP_START + (id*32) + step, data); }

// pattern
static inline u8 SRAM_ReadPattern(u16 id, u8 line, u8 param) { return SRAMW_readByte((u32)PATTERN_DATA + (id * PATTERN_SIZE) + (line * PATTERN_COLUMNS) + param); }
void SRAM_WritePattern(u16 id, u8 line, u8 param, u8 data) { SRAMW_writeByte((u32)PATTERN_DATA + (id * PATTERN_SIZE) + (line * PATTERN_COLUMNS) + param, data); }

inline u8 SRAM_ReadPatternColor(u16 id) { return SRAMW_readByte((u32)PATTERN_COLOR + id); }
void SRAM_WritePatternColor(u16 id, u8 color) { SRAMW_writeByte((u32)PATTERN_COLOR + id, color); }

// matrix
static inline u16 SRAM_ReadMatrix(u8 channel, u8 line) { return SRAMW_readWord((u32)PATTERN_MATRIX + ((channel * MATRIX_ROWS) + line) * 2); }
void SRAM_WriteMatrix(u8 channel, u8 line, u16 data) { SRAMW_writeWord((u32)PATTERN_MATRIX + ((channel * MATRIX_ROWS) + line) * 2, data); }

static inline s8 SRAM_ReadMatrixTranspose(u8 channel, u8 line) { return SRAMW_readByte((u32)MATRIX_TRANSPOSE + ((channel * MATRIX_ROWS) + line)); }
void SRAM_WriteMatrixTranspose(u8 channel, u8 line, s8 transpose) { SRAMW_writeByte((u32)MATRIX_TRANSPOSE + ((channel * MATRIX_ROWS) + line), transpose); }

static inline u8 SRAM_ReadMatrixChannelEnabled(u8 channel) { return SRAMW_readByte((u32)MUTE_CHANNEL + channel); }
void SRAM_WriteMatrixChannelEnabled(u8 channel, u8 state) { SRAMW_writeByte((u32)MUTE_CHANNEL + channel, state); }

// pcm
static inline u32 SRAM_ReadSampleRegion(u8 bank, u8 note, u8 byteNum) { return (u32)SRAMW_readByte((u32)SAMPLE_DATA + (bank * NOTES * SAMPLE_DATA_SIZE) + (note * SAMPLE_DATA_SIZE) + byteNum); }
void SRAM_WriteSampleRegion(u8 bank, u8 note, u8 byteNum, u8 data) { SRAMW_writeByte((u32)SAMPLE_DATA + (bank * NOTES * SAMPLE_DATA_SIZE) + (note * SAMPLE_DATA_SIZE) + byteNum, data); }

static inline u8 SRAM_ReadSamplePan(u8 bank, u8 note){ return SRAMW_readByte((u32)SAMPLE_PAN + (bank * NOTES) + note); }
void SRAM_WriteSamplePan(u8 bank, u8 note, u8 data) { SRAMW_writeByte((u32)SAMPLE_PAN + (bank * NOTES) + note, data); }

// other
static inline void YM2612_writeRegZ80(u16 part, u8 reg, u8 data)
{
    RequestZ80();
    YM2612_writeReg(part, reg, data);
    ReleaseZ80();
}

void DrawMute(u8 mtxCh)
{
    if (channelFlags[mtxCh]) VDP_fillTileMapRect(BG_B, NULL, (mtxCh * 3) + 1, 1, 2, 1);
    else VDP_fillTileMapRect(BG_B, bgBaseTileIndex[2] + GUI_MUTE, (mtxCh * 3) + 1, 1, 2, 1);
}

void InitTracker()
{
    SYS_disableInts();
    /*
    0 $A130F1 	SRAM access register
    1 $A130F3 	Bank register for address $80000-$FFFFF
    2 $A130F5 	Bank register for address $100000-$17FFFF
    3 $A130F7 	Bank register for address $180000-$1FFFFF
    4 $A130F9 	Bank register for address $200000-$27FFFF
    5 $A130FB 	Bank register for address $280000-$2FFFFF
    6 $A130FD 	Bank register for address $300000-$37FFFF
    7 $A130FF 	Bank register for address $380000-$3FFFFF

    Mega Everdrive X3, X5
        Bank 28 can be used for saves. First 32Kbyte of this bank will be copied to SD card.
    Mega Everdrive X7
        Bank 31 can be used for saves. Upper 256K of this bank mapped to battery SRAM.
    Mega Everdrive PRO
        Backup ram mapped to the last 31th bank.
    */

    ssf_init();
    ssf_set_rom_bank(7, 31); // 4
    ssf_rom_wr_on();

    msu_resp = msu_drv();
    //if (msu_resp == 0) // Function will return 0 if driver loaded successfully or 1 if MCD hardware not detected.
    //{
        //while (*mcd_stat != 1); // Init driver ... 0-ready, 1-init, 2-cmd busy
        //while (*mcd_stat == 1); // Wait till sub CPU finish initialization
    //}

    VDP_init();
    VDP_setDMAEnabled(TRUE);
    VDP_setHInterrupt(TRUE);
    VDP_setHIntCounter(H_INT_SKIP-1);
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    VDP_setHilightShadow(FALSE);
    VDP_setScanMode(INTERLACED_NONE);

    // each plane can be a maximum of 4096 tiles in memory
    // at dimensions 32x32, 32x64, 64x64, or 32x128, with up to 40x28 (1120) visible on screen
    VDP_setPlaneSize(128, 32, TRUE);
    VDP_setBGBAddress(0xC000);          // * $2000; 0xC000 default
    VDP_setWindowAddress(0x9000);       // * $1000; 0xD000 default; WINDOW replaces BG_A when drawn, but ignores scrolling;
    VDP_setBGAAddress(0xE000);          // * $2000; 0xE000 default
    VDP_setHScrollTableAddress(0xB800); // * $400; 0xF000 default
    VDP_setSpriteListAddress(0xBC00);   // * $400; 0xF400 default
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_COLUMN);
    VDP_setTextPlane(BG_A);
    VDP_loadFont(&custom_font, DMA); VDP_waitDMACompletion();
    VDP_setWindowHPos(FALSE, 0);
    VDP_setWindowVPos(FALSE, 0);

    // BGR palettes
    PAL_setPaletteColorsDMA(0, &palette_gui);
    PAL_setPaletteColorsDMA(16, &palette_1);
    PAL_setPaletteColorsDMA(32, &palette_2);
    PAL_setPaletteColorsDMA(48, &palette_3);

    // Double digit font 00(--)..FF
    u16 ind;
    ind = TILE_USERINDEX;
    bgBaseTileIndex[0] = ind;
    VDP_loadTileSet(&numfont, ind, DMA);
    ind += numfont.numTile;
    // Normal font 0..Z
    bgBaseTileIndex[1] = ind; asciiBaseLetters = ind - 55; asciiBaseNumbers = ind - 48;
    VDP_loadTileSet(&numletters, ind, DMA);
    ind += numletters.numTile;
    // GUI 1
    bgBaseTileIndex[2] = ind;
    VDP_loadTileSet(&tileset_gui, ind, DMA);
    ind += tileset_gui.numTile;
    // GUI 2
    bgBaseTileIndex[3] = ind;
    VDP_loadTileSet(&tileset_gui2, ind, DMA);
    ind += tileset_gui2.numTile;

    VDP_setTextPalette(PAL0);

    SRAM_enable();

    PSG_init();
    YM2612_reset();
    Z80_init();
    Z80_loadDriver(Z80_DRIVER_PCM, TRUE);

    JOY_setSupport(PORT_1, JOY_SUPPORT_6BTN);
    JOY_setSupport(PORT_2, JOY_SUPPORT_6BTN);
    JOY_setEventHandler(JoyEvent);

    ReColorsAndTranspose(); // need SRAM
    //MDT_HEADER = "MDT100"; // 4D 00 44 00 54 00 31 00 30 00 30 00

    // if there is no SRAM file, needs fresh init.
    if (SRAMW_readWord(FILE_CHECKER) != MDT_CHECKER)
    {
        VDP_setTextPalette(PAL0); VDP_drawText("GENERATING MODULE DATA", 3, 3);
        for (u8 i = 0; i < 6; i++) SRAM_writeByte(i, MDT_HEADER[i]); // write file version
        for (u16 inst = 1; inst <= INSTRUMENTS_LAST; inst++)
        {
            LoadPreset(inst, 0);
            for (u8 t = 0; t < 16; t++)
            {
                if (!t)
                {
                    SRAM_WriteSEQ_VOL(inst, 0, SEQ_VOL_MIN_ATT);
                    SRAM_WriteSEQ_ARP(inst, 0, ARP_BASE);
                    //SRAM_WriteInstrument(inst, INST_VOL_TICK_01, SEQ_VOL_MIN_ATT); // no volume attenuation
                    //SRAM_WriteInstrument(inst, INST_ARP_TICK_01, ARP_BASE); // base note
                }
                else
                {
                    SRAM_WriteSEQ_VOL(inst, t, SEQ_VOL_SKIP);
                    SRAM_WriteSEQ_ARP(inst, t, NOTE_EMPTY);
                    //SRAM_WriteInstrument(inst, INST_VOL_TICK_01 + t, SEQ_VOL_SKIP); // skip step
                    //SRAM_WriteInstrument(inst, INST_ARP_TICK_01 + t, NOTE_EMPTY); // empty note
                }

                if (t < 8) SRAM_WriteInstrument(inst, INST_NAME_1 + t, NULL); // "--------" by default
            }
        }

        // init matrix
        for (u8 channel = CHANNEL_FM1; channel < CHANNELS_TOTAL; channel++)
        {
            SRAM_WriteMatrixChannelEnabled(channel, TRUE); // enable channel
            channelFlags[channel] = TRUE;
            VDP_fillTileMapRect(BG_B, NULL, (channel * 3) + 1, 1, 2, 1);

            for (u8 row = 0; row < MATRIX_ROWS; row++) // 250
            {
                SRAM_WriteMatrix(channel, row, NULL);
            }
        }

        // initialize patterns
        for (u16 pattern = 0; pattern <= PATTERN_LAST; pattern++)
        {
            SRAM_WritePatternColor(pattern, 0);
            for (u8 row = 0; row <= PATTERN_ROW_LAST; row++)
            {
                SRAM_WritePattern(pattern, row, DATA_NOTE, NOTE_EMPTY);
                SRAM_WritePattern(pattern, row, DATA_INSTRUMENT, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX1_TYPE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX1_VALUE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX2_TYPE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX2_VALUE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX3_TYPE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX3_VALUE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX4_TYPE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX4_VALUE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX5_TYPE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX5_VALUE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX6_TYPE, NULL);
                SRAM_WritePattern(pattern, row, DATA_FX6_VALUE, NULL);
            }
        }

        // dac default pan init
        for (u8 bank = 0; bank < 4; bank++)
        {
            for (u8 note = 0; note < NOTES; note++)
            {
                SRAM_WriteSamplePan(bank, note, SOUND_PAN_CENTER);
            }
        }

        SetBPM(DEFAULT_TEMPO);
        SRAMW_writeByte(GLOBAL_LFO, 7);
        SRAMW_writeWord(FILE_CHECKER, MDT_CHECKER);
        //SRAMW_writeByte(SONG_TRANSPOSE, (s8)0);
    }
    else
    {
        // legacy beta stuff
        for (u8 i = 0; i < 6; i++) str[i] = SRAM_readByte(i);

        if (strcmp(str, MDT_HEADER) != 0) // not equal
        {
            Legacy();
        }
        SetBPM(0); // reads BPM from SRAM
        //songTranspose = (s8)SRAMW_readByte(SONG_TRANSPOSE);
    }

    // init
    for (u8 mtxCh = CHANNEL_FM1; mtxCh < CHANNELS_TOTAL; mtxCh++)
    {
        channelPreviousInstrument[mtxCh] =
        channelPreviousEffectType[mtxCh][0] =
        channelPreviousEffectType[mtxCh][1] =
        channelPreviousEffectType[mtxCh][2] =
        channelArpSeqID[mtxCh] =
        channelVibratoMode[mtxCh] =
        channelVolSeqID[mtxCh] =
        channelNoteCut[mtxCh] = 0;

        channelPreviousNote[mtxCh] = NOTE_OFF;

        channelTremoloSpeedMult[mtxCh] = 0x20;
        channelVibratoSpeedMult[mtxCh] = 0x08;
        channelVibratoDepthMult[mtxCh] = 0x02;

        channelFlags[mtxCh] = SRAM_ReadMatrixChannelEnabled(mtxCh);
        DrawMute(mtxCh);
    }

    sampleBankSize = sizeof(sample_bank_1);
    /*
    Vertical interrupt (V-INT): level 6
    Horizontal interrupt (H-INT): level 4
    External interrupt (EX-INT): level 2
    */
    SYS_setInterruptMaskLevel(2);
    SYS_setHIntCallback(*hIntCallback);
    SYS_setVIntCallback(*vIntCallback);

    // global LFO frequency (0..7) 3.98 5.56 6.02 6.37 6.88 9.63 48.1 72.2
    // 0000 - unused, 0 - enable, 000 - frequency
    // 8< - disable; 9..15 enable and set frequency
    YM2612_writeRegZ80(PORT_1, YM2612REG_GLOBAL_LFO, SRAMW_readByte(GLOBAL_LFO));

    // CH3 mode:
    //|Mode| Behavior
    //| 00 | Normal
    //| 01 | Special
    //| 10 | Special + CSM
    //| 11 | Special

    // CSM mode is where Timer A performs automatic key on/off for channel 3
    // bb - CSM / CH3 mode, bb bbbb - Timers: Reset B, Reset A, Enable B, Enable A, Load B, Load A
    // b - CH3 normal mode; 64 - CH3 special mode; timers are need to be used to

    // set timer A time; 1111 1111 11 = 0.018 ms (minimum step), 0000 0000 00 = 18.4 ms; 1024 values; 18 * (1024 - Timer A) microseconds
    // 3FFh CSM: always key-on
    // timer A is for CSM. timer B usually is for song playback and tempo
    YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_MSB, 0); // 8 bit MSB
    YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_LSB, 0); // 2 bit LSB
    // timer B; 1111 1111 = 0.288 ms (minimum step), 0000 0000 = 73.44 ms; 288 * (256 - Timer B ) microseconds
    YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_NORMAL | 0b00111100);
    YM2612_enableDAC();

    PSG_NoiseMode = PSG_TONAL_CH3_MUTED;
    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3);

    for (u16 id = 0; id <= INSTRUMENTS_LAST; id++)
    {
        CacheIstrumentToRAM(id);
        instrumentIsMuted[id] = INST_PLAY;
    }

    DrawStaticHeaders();
    VDP_waitDMACompletion();

    SYS_enableInts();
}

void DrawStaticHeaders()
{
    // draw default initial brackets
    currentScreen = 2; DrawSelectionCursor(0, 0, 0); // instrument
    currentScreen = 1; DrawSelectionCursor(0, 0, 0); // pattern
    currentScreen = 0; DrawSelectionCursor(0, 0, 0); // matrix

    //VDP_clearTileMap(WINDOW, 0, 40*28, TRUE); // BG_B is still visible

    // ----------------------------------- matrix editor
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_VERSION),     38, 27); // version
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_VERSION+1),   39, 27);

    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i, 0); // MD.TRACKER logo

    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 27, 0, 0); // top line
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 2, 27, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 29, 0);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_P), 31, 0); // PAGE:
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_A), 32, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_G), 33, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_E), 34, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON),    35, 0);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      1, 1); // fm1 fm2 fm3
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_1), 2, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      4, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_2), 5, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      7, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_3), 8, 1);

    for (u8 i=0; i<8; i++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_CH3 + i), i + 10, 1); // special mode

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      19, 1); // fm4 fm5 fm6
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_4), 20, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      22, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_5), 23, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      25, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_6), 26, 1);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_SQUARE),  28, 1); // sq1 sq2 sq3 n4
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_1),     29, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_SQUARE),  31, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_2),     32, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_SQUARE),  34, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_3),     35, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_NOISE),   37, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_4),     38, 1);

    /*for (u8 i=0; i<4; i++) // special mode op4 op3 op2 op1
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_OP), (i * 3) + 7, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_4 - i), (i * 3) + 8, 27);
    }*/

    FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_SLASH, 3, 34, 27); // bottom line
    FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_LOWLINE, 3, 37, 27);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM), 1, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM + 1), 2, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_FPS), 13, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_FPS + 1), 14, 27);
            DrawNum(BG_A, PAL1, "999", 15, 27); // default FPS
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPS), 19, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPS + 1), 20, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL), 25, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL + 1), 26, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH), 28, 27);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM), 41, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM + 1), 42, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_FPS), 53, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_FPS + 1), 54, 27);
            DrawNum(BG_A, PAL1, "999", 55, 27); // default FPS
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPS), 59, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPS + 1), 60, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL), 65, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL + 1), 66, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH), 68, 27);


    //! for (u8 y=2; y<27; y++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 27, y); // fm/psg

    // ----------------------------------- pattern editor
    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i + 72, 22); // MD.TRACKER logo

    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 29, 40, 22); // low line
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_SLASH), 69, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 70, 22);
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 9, 71, 22);

    for (u8 y=4; y<20; y++) // pattern effects separator dots, numbers
    {
        u8 pal;
        if (y%4) pal = PAL3; else pal = PAL0;
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, bgBaseTileIndex[0] + y-4), 44, y);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, bgBaseTileIndex[0] + y+12), 64, y);

        for (u8 x=49; x<59; x+=2)
        {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), x, y);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), x + 20, y);
        }
    };
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_00), 44, 4); // 00

    FillRowRight(BG_B, PAL3, FALSE, TRUE, GUI_LOWLINE, 19, 41, 3); // pattern high lines
    FillRowRight(BG_B, PAL3, FALSE, FALSE, GUI_LOWLINE, 19, 61, 3);

    VDP_setTextPalette(PAL3); VDP_drawText("PATTERN:", 41, 0); VDP_drawText("COPY FROM:", 54, 0);
    VDP_setTextPalette(PAL1); VDP_drawText("---", 65, 0);

    DrawText(BG_A, PAL3, "KEY", 41, 2);
    DrawText(BG_A, PAL3, "IN", 45, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FX_SYM), 48, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_FX_SYM), 59, 2);
    DrawText(BG_A, PAL3, "COMMANDS", 50, 2);
    DrawText(BG_A, PAL3, "KEY", 61, 2);
    DrawText(BG_A, PAL3, "IN", 65, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FX_SYM), 68, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_FX_SYM), 79, 2);
    DrawText(BG_A, PAL3, "COMMANDS", 70, 2);

    DrawText(BG_A, PAL3, "INST", 41, 23); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 45, 23);
    //VDP_drawTextBG(BG_A, "INST: --------", 41, 23);

    // ----------------------------------- instrument editor
    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i + 112, 22); // MD.TRACKER logo

    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 29, 80, 22); // low line
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_SLASH), 109, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 110, 22);
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 9, 111, 22);

    for (u8 y=0; y<22; y++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 105, y); // inst/sample separator

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_LOWLINE), 80, 1); // under INST:
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 7, 81, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 88, 1);

    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 9, 106, 1); // under SAMPLE
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 3, 115, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 118, 1);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_LOWLINE), 80, 8); // above TOTAL LEVEL
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 11, 81, 8);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 92, 8);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, TRUE, TRUE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 106, 2); // left from REGION
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 4, 107, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 111, 2);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_LOWLINE), 106, 19); // above STATE
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 6, 107, 19);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 113, 19);
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 6, 114, 19);

    DrawText(BG_A, PAL3, "INST", 81, 0); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 85, 0);
    //
    DrawText(BG_A, PAL3, "ALG", 81, 2);
    DrawText(BG_A, PAL3, "FMS", 81, 3);
    DrawText(BG_A, PAL3, "AMS", 81, 4);
    DrawText(BG_A, PAL3, "PAN", 81, 5);
    DrawText(BG_A, PAL3, "FB", 81, 6);
    for (u8 y=2; y<7; y++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 85, y);

    DrawText(BG_A, PAL3, "NAME", 91, 0); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 95, 0);
    DrawText(BG_A, PAL3, "COPY", 91, 1); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 95, 1);
    DrawText(BG_A, PAL3, "OK", 97, 1); VDP_setTextPalette(PAL2); VDP_drawText("(B)", 99, 1);

    DrawText(BG_A, PAL3, "SAMPLE", 106, 0);
    //
    DrawText(BG_A, PAL3, "REGION", 113, 2);
    DrawText(BG_A, PAL3, "START", 106, 3);
    DrawText(BG_A, PAL3, "END", 106, 4);
    DrawText(BG_A, PAL3, "LOOP", 106, 5);
    DrawText(BG_A, PAL3, "RATE", 106, 6); VDP_setTextPalette(PAL1); VDP_drawText(">", 113, 6);
    DrawText(BG_A, PAL3, "PAN", 106, 7); VDP_setTextPalette(PAL1); VDP_drawText(">", 113, 7);
    for (u8 y=3; y<8; y++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 111, y); // sample colons

    VDP_setTextPalette(PAL3); VDP_drawText("PRESET", 106, 17);
    VDP_setTextPalette(PAL1); VDP_drawText(">", 113, 17); VDP_drawText("000", 114, 17);
    VDP_drawText(presetName[0], 106, 18);
    VDP_setTextPalette(PAL2); VDP_drawText("(B)", 117, 17);

    DrawText(BG_A, PAL3, "STATE", 106, 20); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 111, 20);
    DrawText(BG_A, PAL0, "PLAY", 113, 20);

    for (u8 i=0; i<4; i++) // op4 op3 op2 op1
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_OP), (i * 3) + 94, 8);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_1 + i), (i * 3) + 95, 8);
    }

    DrawText(BG_A, PAL3, "TOTAL", 81, 9); DrawText(BG_A, PAL3, "LEVEL", 87, 9);
    DrawText(BG_A, PAL3, "RATE", 81, 10); DrawText(BG_A, PAL3, "SCALE", 86, 10);
    DrawText(BG_A, PAL3, "MULTIPLIER", 81, 11);
    DrawText(BG_A, PAL3, "DETUNE", 81, 12);

    DrawText(BG_A, PAL3, "ATTACK", 81, 14);
    DrawText(BG_A, PAL3, "DECAY", 81, 15); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_1), 87, 15);
    DrawText(BG_A, PAL3, "SUSTAIN", 81, 16);
    DrawText(BG_A, PAL3, "DECAY", 81, 17); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_2), 87, 17);
    DrawText(BG_A, PAL3, "RELEASE", 81, 18);

    for (u8 i=0; i<4; i++)
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_0), 94 + i*3, 10);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_0), 94 + i*3, 11);
    }

    DrawText(BG_A, PAL3, "MODULATION", 81, 20);// DrawText(BG_A, PAL3, "LFO", 84, 20);
    DrawText(BG_A, PAL3, "SSG", 81, 21); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 84, 21);
        DrawText(BG_A, PAL3, "EG", 85, 21);
    for (u8 y=9; y<22; y++)
    {
        switch (y)
        {
            case 13: case 19: break;
            default: VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 92, y);
        }
    }
    VDP_setTextPalette(PAL3);
    VDP_drawText("AM LFO RATE:", 80, 23);
    VDP_drawText("VOL:", 80, 25);
    VDP_drawText("ARP:", 80, 26);
}

void Legacy()
{
    // update seq to 32 steps
    for (u16 id = 0; id < INSTRUMENTS_TOTAL; id++)
    {
        for (u8 step = 0; step < 32; step++)
        {
            if (step < 16)
            {
                SRAM_WriteSEQ_VOL(id, step, SRAM_ReadInstrument(id, 49+step));
                SRAM_WriteSEQ_ARP(id, step, SRAM_ReadInstrument(id, 65+step));
            }
            else
            {
                SRAM_WriteSEQ_VOL(id, step, SEQ_VOL_SKIP);
                SRAM_WriteSEQ_ARP(id, step, NOTE_EMPTY);
            }
        }
    }
    // 00th instrument SEQ
    for (u8 t = 0; t < 16; t++)
    {
        SRAM_WriteSEQ_VOL(0, t, SEQ_VOL_SKIP); // skip step
        SRAM_WriteSEQ_ARP(0, t, NOTE_EMPTY); // empty note
    }
    for (u8 i = 0; i < 6; i++) SRAM_writeByte(i, MDT_HEADER[i]); // write file version
}

void ForceResetVariables()
{
    playingMatrixRow=
    selectedMatrixScreenRow=
    selectedMatrixRow=
    currentPage=
    selectedPatternRow=
    selectedPatternColumn=
    selectedPatternID=
    line=
    chan=
    playingPatternRow=
    selectedInstrumentParameter=
    selectedInstrumentOperator=
    currentScreen=
    pulseCounter=
    PSG_NoiseMode=
    bPlayback=
    FM_CH3_Mode=
    bPsgIsPlayingNote[0]=
    bPsgIsPlayingNote[1]=
    bPsgIsPlayingNote[2]=
    bPsgIsPlayingNote[3]=
    FM_CH3_OpFreq[0]=
    FM_CH3_OpFreq[1]=
    FM_CH3_OpFreq[2]=
    FM_CH3_OpFreq[3]=
    samplesSize=
    sampleToPlay=
    sampleSeekTime=
    selectedSampleBank=
    selectedSampleNote=
    sampleBankSize=
    activeSampleBank=
    hIntToSkip=
    hIntCounter=
    bDoPulse=
    asciiBaseLetters=
    asciiBaseNumbers=
    bDoCount=
    selectedMatrixChannel=0;

    lastEnteredEffect=
    lastEnteredEffectValue=
    lastEnteredInstrumentID=
    lastEnteredPattern=
    bInitScreen=
    bRefreshScreen=
    bDAC_enable=
    bWriteRegs=
    patternCopyFrom=
    instCopyTo=
    updateCursor=
    selectedInstrumentID=1;

    H_INT_SKIP = 2;

    patternRowToRefresh=
    instrumentParameterToRefresh=
    matrixRowJumpTo=
    loopStart=
    loopEnd=
    patternRowJumpTo=OXFF;

    matrixRowToRefresh=OXFFFF;

    lastEnteredNote=45;

    FM_CH3_OpNoteStatus=0b00000010;
    ppl_1=ppl_2=maxPulse=4;

    patternCopyRangeStart=
    patternCopyRangeEnd=-1;

    buttonCounter=GUI_NAVIGATION_DELAY;
    navigationDirection=BUTTON_RIGHT;
    FM_CH6_DAC_Pan=NULL; // sample pan by default

    patternSize = 0x1F;

    for (u16 in=0; in<=INSTRUMENTS_LAST; in++)
    {
        instrumentIsMuted[in]=INST_PLAY;
    }

    for (u8 ch=0; ch<CHANNELS_TOTAL; ch++)
    {
        channelTranspose[ch]=
        channelPreviousNote[ch]=
        channelArpSeqID[ch]=
        channelVolSeqID[ch]=
        channelCurrentRowNote[ch]=
        channelSEQCounter_VOL[ch]=
        channelSEQCounter_ARP[ch]=
        channelPitchSlideSpeed[ch]=
        channelPitchSlideValue[ch]=
        channelMicrotone[ch]=
        channelArp[ch]=
        channelPitchSkipStep[ch]=
        channelPitchSkipStepCounter[ch]=
        channelVibratoMode[ch]=
        channelVibratoDepth[ch]=
        channelVibratoDepthMult[ch]=
        channelVibratoSpeed[ch]=
        channelVibratoSpeedMult[ch]=
        channelVibratoPhase[ch]=
        channelFinalPitch[ch]=
        channelModNoteVibrato[ch]=
        channelModNotePitch[ch]=
        channelTremoloDepth[ch]=
        channelTremoloSpeed[ch]=
        channelTremoloSpeedMult[ch]=
        channelTremoloPhase[ch]=
        channelTremolo[ch]=
        channelBaseVolume[ch]=
        channelSeqAttenuation[ch]=
        channelAttenuation[ch]=
        channelSlotBaseLevel[ch][0]=
        channelSlotBaseLevel[ch][1]=
        channelSlotBaseLevel[ch][2]=
        channelSlotBaseLevel[ch][3]=
        channelVolumeChangeSpeed[ch]=
        channelVolumePulseSkip[ch]=
        channelVolumePulseCounter[ch]=
        channelNoteCut[ch]=
        channelNoteRetrigger[ch]=
        channelNoteDelayCounter[ch]=
        channelNoteAutoCut[ch]=
        channelMatrixTranspose[ch]=
        channelPlayingPatternID[ch]=
        channelNoteRetriggerCounter[ch]=0;

        channelFlags[ch]=
        channelArpSeqMODE[ch]=
        channelVolSeqMODE[ch]=1;

        for (u8 ef=0; ef<EFFECTS_TOTAL; ef++)
        {
            channelPreviousEffectType[ch][ef]=0;
        }

        for (u8 row=0; row<32; row++)
        {
            channelRowShift[ch][row]=0;
        }
    }
}
