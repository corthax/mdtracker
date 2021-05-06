#include <genesis.h>
//#include <maths.h>
//#include <everdrive.h>    // ED V1
#include <sram.h>
//#include <segalib.h>
#include <ssf.h>            // PRO

#include "fontdata.h"
//#include "sprites.h"
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

#define MD_TRACKER_VERSION  5

#define STRING_EMPTY    ""

u8 playingMatrixRow = 0; // current played line
u8 selectedMatrixScreenRow = 0; // selected matrix line on SCREEN
u8 selectedMatrixRow = 0; // selected pattern matrix index according to page
u8 selectedMatrixChannel = 0; // playback channel
u8 updateCursor = 1; // playback cursor
s8 currentPage = 0; // pattern matrix page

u8 selectedPatternRow = 0;
u8 selectedPatternColumn = 0;
u16 selectedPatternID = 0;

u8 patternRowToRefresh = EVALUATE_0xFF; // to refresh only edited pattern line; 255 = refresh everything
u16 matrixRowToRefresh = EVALUATE_0xFFFF; //! change
u8 instrumentParameterToRefresh = EVALUATE_0xFF; // 255-- counter to refresh one instrument parameter per frame; avoid slowdown

u8 lastEnteredNote = 45; // A-3 by default
u8 lastEnteredEffect = 1; // TL1
u8 lastEnteredEffectValue = 1;
u8 lastEnteredInstrumentID = 1;
u16 lastEnteredPattern = 1;

u8 line = 0; // redraw screen data line counter
u8 chan = 0; // draw only one cell per VBlank to avoid slowdown?

u8 playingPatternRow = 0; // current played pattern row

u8 channelPreviousInstrument[CHANNELS_TOTAL]; // 255 is used to write instrument when parameters changed
u8 channelPreviousEffect[CHANNELS_TOTAL][EFFECTS_TOTAL];
u8 channelPreviousNote[CHANNELS_TOTAL];
u8 channelArpSeqID[CHANNELS_TOTAL];
u8 channelArpSeqMODE[CHANNELS_TOTAL];
u8 channelVolSeqID[CHANNELS_TOTAL];
u8 channelVolSeqMODE[CHANNELS_TOTAL];
u8 channelCurrentNote[CHANNELS_TOTAL];

u8 selectedInstrumentID = 1; // 0 instrument is empty
u8 selectedInstrumentParameter = 0; // 0..53
u8 selectedInstrumentOperator = 0; // 0..53

// screen
u8 currentScreen = 0;
u8 bInitScreen = TRUE; // redraw selection brackets
u8 bRefreshScreen = TRUE; // refresh data of current screen

char str[6]; //! symbol buffer !!!may cause crash if overflowed!!!

// engine
u8 ch3Mode = CH3_NORMAL; // global
u8 ch3OpNoteStatus = 0b00000010; // ch3 each of operators status, note on or off
u16 channelFlags = 0b0001111111111111; // mute/unmute channels

u8 psg_noise_mode = 0;

bool bPlayback = FALSE;
u8 ticksPerOddRow = 16;
u8 ticksPerEvenRow = 16;
u8 maxFrame = 16;
s8 frameCounter = 0; // 8 PPL

u16 subTicksToSkip = 0;

// channel effects
u8 channelPitchSlideSpeed[CHANNELS_TOTAL];
s8 channelMicrotone[CHANNELS_TOTAL];
u8 channelArpNote[CHANNELS_TOTAL];

s8 channelPitchSkipStep[CHANNELS_TOTAL];
s8 channelPitchSkipStepCounter[CHANNELS_TOTAL];
u8 channelVibratoMode[CHANNELS_TOTAL];
u16 channelVibratoDepth[CHANNELS_TOTAL];
u8 channelVibratoDepthMult[CHANNELS_TOTAL];
u16 channelVibratoSpeed[CHANNELS_TOTAL];
u8 channelVibratoSpeedMult[CHANNELS_TOTAL];
u16 channelVibratoPhase[CHANNELS_TOTAL];
s8 channelFinalPitch[CHANNELS_TOTAL];
s8 channelModNoteVibrato[CHANNELS_TOTAL];
s8 channelModNotePitch[CHANNELS_TOTAL];

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
u16 channelNoteRetriggerCounter[CHANNELS_TOTAL];

u8 bPsgIsPlayingNote[4];

s16 matrixRowJumpTo = EVALUATE_0xFF;
u8 patternRowJumpTo = EVALUATE_0xFF;
u8 channelNoteDelay[CHANNELS_TOTAL];

u8 ch3OpFreq[4];

u8 instrumentIsMuted[MAX_INSTRUMENT+1]; // 1 = mute

// dac
u32 samplesSize = 0;
u8 sampleToPlay = 0;
u32 sampleSeekTime = 0;

u8 selectedSampleBank = 0;
u8 selectedSampleNote = 0;
u32 sampleBankSize = 0;
u8 activeSampleBank = 0;

u8 bDAC_enable = TRUE; // global, 0xF0 to enable. 0 to disable
//u8 DAC_softsynth = 0; // 0 is disabled, 0< to set mode
//static void WriteDAC(u8 sample);
//static void PCMPlay();
//static u8 SoftsynthRoutine();
//u16 softsynthFreq = 1;
//u8 sampleStack[1024];

// sys
bool bBusTaken = FALSE;

// copy/paste
u16 patternCopyFrom = 1;
s8 patternCopyRangeStart = NOTHING;
s8 patternCopyRangeEnd = NOTHING;

u16 hIntToSkip = 0;
s16 hIntCounter = 0;

u16 bgBaseTileIndex[3];
u16 asciiBaseLetters, asciiBaseNumbers;
u8 instCopyTo = 0x01; // instrument copy

s8 buttonCounter = GUI_NAVIGATION_DELAY; // signed just in case of overflow
u8 doCount = FALSE;
u8 navigationDirection = BUTTON_RIGHT;

u8 psgPWM = FALSE;
u8 psgPulseCounter = 0;
u8 dacPan = SOUND_PAN_CENTER;

Instrument tmpInst[MAX_INSTRUMENT+1]; // cache instruments to RAM for faster access

u16 msu_drv();
vu16 *mcd_cmd = (vu16 *) 0xA12010;  // command
vu32 *mcd_arg = (vu32 *) 0xA12012;  // argument
vu8 *mcd_cmd_ck = (vu8 *) 0xA1201F; // increment for command execution
vu8 *mcd_stat = (vu8 *) 0xA12020;   // Driver ready for commands processing when 0xA12020 sets to 0
u16 msu_resp;

// cant put in header. build error

const u8 GUI_PATTERNCOLORS[14] = { 42, 43, 44, 45, 46, 47, 56, 57, 58, 59, 60, 61, 62, 63 };

const u8 GUI_FM_ALG_GRID[8][4][12] =
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

const u8 GUI_NOTE_NAMES[2][12] =
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

const s16 GUI_ALPHABET[38] =
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

int main()
{
    InitTracker();
	while(1) { DoEngine(); }
	return(0);
}

// must not be very cpu hungry
static void hIntCallback()
{
    //static u8 flipEnvelope = 0;
    //static u8 hIntSkipCounter = 0;

    /*if (!hIntSkipCounter)
    {
        hIntSkipCounter = hIntToSkip;*/

        if (bPlayback)
        {
            hIntCounter--;
            // PSG PWM
            /*if (psgPWM)
            {
                if (!psgPulseCounter)
                {
                    if (flipEnvelope) { PSG_setEnvelope(0, PSG_ENVELOPE_MAX); flipEnvelope = FALSE; }
                    else { PSG_setEnvelope(0, PSG_ENVELOPE_MIN); flipEnvelope = TRUE; }
                    psgPulseCounter = pwmNoteTableNTSC[psgPWM]; // 0..11 C .. B notes
                }
                psgPulseCounter--;
            }*/
        }

    /*}
    else if (bPlayback)
    {
        // PSG PWM
        if (psgPWM)
        {
            if (!psgPulseCounter)
            {
                if (flipEnvelope) { PSG_setEnvelope(0, PSG_ENVELOPE_MAX); flipEnvelope = FALSE; }
                else { PSG_setEnvelope(0, PSG_ENVELOPE_MIN); flipEnvelope = TRUE; }
                psgPulseCounter = pwmNoteTableNTSC[psgPWM]; // 0..11 C .. B notes
            }
            psgPulseCounter--;
        }
    }
    hIntSkipCounter--;*/

    // fast GUI, playback slowdown when updating screen
    switch (currentScreen)
    {
    case SCREEN_MATRIX: DisplayPatternMatrix(); break;
    case SCREEN_PATTERN: DisplayPatternEditor(); break;
    case SCREEN_INSTRUMENT: DisplayInstrumentEditor(); break;
    }
}

static void vIntCallback()
{
    SYS_doVBlankProcessEx(IMMEDIATELY);
    // fast navigation
    if (doCount)
    {
        buttonCounter--;
        if (buttonCounter <= 0)
        {
            buttonCounter = GUI_NAVIGATION_SPEED_COUNTER;
            switch (currentScreen)
            {
            case SCREEN_MATRIX: NavigateMatrix(navigationDirection); break;
            case SCREEN_PATTERN: NavigatePattern(navigationDirection); break;
            case SCREEN_INSTRUMENT: NavigateInstrument(navigationDirection); break;
            }
        }
    }
    // slow GUI, less CPU (not that much really...)
    /*switch (currentScreen)
    {
    case 0: DisplayPatternMatrix(); break;
    case 1: DisplayPatternEditor(); break;
    case 2: DisplayInstrumentEditor(); break;
    }*/
}

// -------------------------------------------------------------------------------------------------------------
void NavigateMatrix(u8 direction)
{
    switch (direction)
    {
    case BUTTON_LEFT:
        if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS) { // not BPM
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
            if (selectedMatrixChannel > CHANNEL_FM1) selectedMatrixChannel--; else selectedMatrixChannel = CHANNELS_TOTAL-1;
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        }
        break;
    case BUTTON_RIGHT:
        if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS) { // not BPM
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
            if (selectedMatrixChannel < CHANNELS_TOTAL-1) selectedMatrixChannel++; else selectedMatrixChannel = CHANNEL_FM1;
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        }
        break;
    case BUTTON_UP:
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
        if (selectedMatrixScreenRow > 0) selectedMatrixScreenRow--; else selectedMatrixScreenRow = LAST_MATRIX_SCREEN_ROW;
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        selectedMatrixRow = selectedMatrixScreenRow + currentPage * MATRIX_SCREEN_ROWS;
        break;
    case BUTTON_DOWN:
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
        if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS) selectedMatrixScreenRow++; else selectedMatrixScreenRow = 0;
        DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        selectedMatrixRow = selectedMatrixScreenRow + currentPage * MATRIX_SCREEN_ROWS;
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
    case PATTERN_JUMP_SIDE: // jump to other side
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
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = 15;
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
            if (selectedInstrumentOperator < 15) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
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
static void DoEngine()
{
    static u8 arptick_value = 0;
    static u8 voltick_value = 0;
    static u8 fxtype_value = 0;
    static u8 fxval_value = 0;
    static u16 playingPatternID = 0;
    static u8 beginPlay = TRUE;

    // vibrato tool
    auto s8 vibrato(u8 channel)
    {
        if (channelVibratoSpeed[channel] > 0 && channelVibratoDepth[channel] > 0)
        {
            static s8 vib = 0;

            switch (channelVibratoMode[channel])
            {
            case 1:
                vib = abs((s8)fix16ToRoundedInt(fix16Mul(FIX16(channelVibratoDepth[channel]), sinFix16(channelVibratoPhase[channel]))));
                break;
            case 2:
                vib = -abs((s8)fix16ToRoundedInt(fix16Mul(FIX16(channelVibratoDepth[channel]), sinFix16(channelVibratoPhase[channel]))));
                break;
            default:
                vib = (s8)fix16ToRoundedInt(fix16Mul(FIX16(channelVibratoDepth[channel]), sinFix16(channelVibratoPhase[channel])));
                break;
            }

            channelVibratoPhase[channel] += channelVibratoSpeed[channel];
            if (channelVibratoPhase[channel] > 1023) channelVibratoPhase[channel] -= 1024;

            return vib;
        }
        else return 0;
    }

    if (bPlayback)
    {
        // need to be this way to avoid playback cursor glitches and hang notes at stop
        if (beginPlay)
        {
            SYS_disableInts();
            beginPlay = FALSE;

            for (u16 id = 0; id <= MAX_INSTRUMENT; id++) { CacheIstrumentToRAM(id); } // reset [tempInst]

            // start timer B
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, ch3Mode | 0b00001111);

            // set frame length
            if (playingPatternRow & 1) maxFrame = ticksPerOddRow; // ticks per line
            else maxFrame = ticksPerEvenRow;

            DrawMatrixPlaybackCursor(FALSE);
            SYS_enableInts();
        }

        if (hIntCounter <= 0) // ticks counter
        {
            hIntCounter = hIntToSkip;
            if (frameCounter == -1) frameCounter = 1; else frameCounter++;
            if (frameCounter == maxFrame)
            {
                if (playingPatternRow & 1) maxFrame = ticksPerOddRow; else maxFrame = ticksPerEvenRow;
                frameCounter = 0;
            }

            //effects ---------------------------------------------------------------------------------------------------------------
            for (u8 channel = CHANNEL_FM1; channel < CHANNELS_TOTAL; channel++)
            {
                //sequencers
                if (channelPreviousNote[channel] <= MAX_NOTE && frameCounter > 0) // notes only, not at 1st frame
                {
                    // attenuation sequence
                    u8 value = ReadInstrumentSRAM(channelVolSeqID[channel], INST_VOL_TICK_01 + frameCounter);
                    if (channelVolSeqMODE[channel] == 0 || (channelVolSeqMODE[channel] == 1 && channelCurrentNote[channel] != NOTE_EMPTY)) // loop or once
                    {
                        if (value < 128)
                        {
                            channelSeqAttenuation[channel] = value;
                            SetChannelVolume(channel);
                        }
                    }

                    // note
                    value = ReadInstrumentSRAM(channelArpSeqID[channel], INST_ARP_TICK_01 + frameCounter);
                    if (value != NOTE_EMPTY)
                    {
                        if (channelArpSeqMODE[channel] == 0 || (channelArpSeqMODE[channel] == 1 && channelCurrentNote[channel] != NOTE_EMPTY)) // loop or once
                        {
                            if (value > ARP_BASE)
                            {
                                channelArpNote[channel] = channelPreviousNote[channel] + (value - ARP_BASE);
                            }
                            else if (value < ARP_BASE)
                            {
                                channelArpNote[channel] = channelPreviousNote[channel] - (ARP_BASE - value);
                            }
                            else
                            {
                                channelArpNote[channel] = channelPreviousNote[channel];
                            }
                            PlayNote(channelArpNote[channel], channel);
                        }
                    }
                }

                // delay
                if (channelNoteDelay[channel] > 0)
                {
                    channelNoteDelay[channel]--;
                    if (channelNoteDelay[channel] == 1)
                    {
                        PlayNote(channelPreviousNote[channel], channel);
                        channelNoteDelay[channel] = 0;
                    }
                }

                // re-trigger
                if (channelNoteRetrigger[channel] > 0)
                {
                    channelNoteRetriggerCounter[channel]++;
                    if (channelNoteRetriggerCounter[channel] % channelNoteRetrigger[channel] == 0)
                    {
                        PlayNote(channelPreviousNote[channel], channel);
                    }
                }

                // volume slide
                if (channelVolumeChangeSpeed[channel] != 0)
                {
                    if (channelVolumePulseCounter[channel] == 0)
                    {
                        channelAttenuation[channel] += channelVolumeChangeSpeed[channel];
                        if (channelAttenuation[channel] > 0x7F) { channelAttenuation[channel] = 0x7F; channelVolumeChangeSpeed[channel] = 0; }
                        else if (channelAttenuation[channel] < 0) { channelAttenuation[channel] = 0; channelVolumeChangeSpeed[channel] = 0; }
                        SetChannelVolume(channel);
                        channelVolumePulseCounter[channel] = channelVolumePulseSkip[channel];
                    }
                    channelVolumePulseCounter[channel]--;
                }

                // tremolo
                if (channelTremoloDepth[channel] > 0 && channelTremoloSpeed[channel] > 0)
                {
                    channelTremolo[channel] = (u8)fix16ToRoundedInt(fix16Mul(FIX16(channelTremoloDepth[channel]), fix16Div(fix16Add(cosFix16(channelTremoloPhase[channel]), FIX16(1)), FIX16(2))));

                    channelTremoloPhase[channel] += channelTremoloSpeed[channel];
                    if (channelTremoloPhase[channel] > 1023) channelTremoloPhase[channel] -= 1024;
                    SetChannelVolume(channel);
                }

                // pitch
                if (channelPitchSlideSpeed[channel] > 0 || channelVibratoDepth[channel] > 0 || channelVibratoSpeed[channel] > 0)
                {
                    // portamento
                    if (channelPitchSkipStepCounter[channel] <= 0)
                    {
                        channelMicrotone[channel] += channelPitchSlideSpeed[channel];

                        while(channelMicrotone[channel] >= MICROTONE_STEPS) // wrap
                        {
                            channelMicrotone[channel] -= MICROTONE_STEPS;
                            channelModNotePitch[channel]++;
                        }

                        while(channelMicrotone[channel] < 0) // wrap
                        {
                            channelMicrotone[channel] += MICROTONE_STEPS;
                            channelModNotePitch[channel]--;
                        }

                        channelPitchSkipStepCounter[channel] = channelPitchSkipStep[channel]; // skip pulses for slower pitch slide
                    }

                    channelPitchSkipStepCounter[channel]--;

                    // vibrato
                    channelFinalPitch[channel] = channelMicrotone[channel] + vibrato(channel);

                    if (channelFinalPitch[channel] >= MICROTONE_STEPS)
                    {
                        channelFinalPitch[channel] -= MICROTONE_STEPS;
                        channelModNoteVibrato[channel] = 1;
                    }
                    else if (channelFinalPitch[channel] < 0)
                    {
                        channelFinalPitch[channel] += MICROTONE_STEPS;
                        channelModNoteVibrato[channel] = -1;
                    }
                    else  channelModNoteVibrato[channel] = 0;

                    if (channel < CHANNEL_PSG1) SetPitchFM(channel, channelArpNote[channel]);
                    else SetPitchPSG(channel, channelArpNote[channel]);
                }
                else
                {
                    channelFinalPitch[channel] = 0;
                }

                // cut
                if (channelNoteCut[channel] > 1) channelNoteCut[channel]--;
                else if (channelNoteCut[channel] == 1)
                {
                    // kill note
                    StopChannelSound(channel);
                    channelNoteCut[channel] = 0;
                }
            }
        }

        // playback engine
        if (frameCounter == 0)
        {
            for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++) // 13 channels; 0 .. 12
            {
                playingPatternID = ReadMatrixSRAM(channel, playingMatrixRow);
                if ((playingPatternID != 0) && (BIT_CHECK(channelFlags, channel) == 1)) // if there is a pattern, and channel is not muted
                {
                    static u8 inst = 0;
                    static s16 key = 0;
                    auto void command(u8 type, u8 val, u8 effect)
                    {
                        fxtype_value = ReadPatternSRAM(playingPatternID, playingPatternRow, type);
                        fxval_value = ReadPatternSRAM(playingPatternID, playingPatternRow, val);
                        if (fxtype_value != NULL) {
                            ApplyCommand_Common(channel, fxtype_value, fxval_value);
                            if (channel < CHANNEL_PSG1) ApplyCommand_FM(channel, channelPreviousInstrument[channel], fxtype_value, fxval_value);
                            else ApplyCommand_PSG(fxtype_value, fxval_value);
                            channelPreviousEffect[channel][effect] = fxtype_value;
                        }
                        else if (fxval_value != NULL) {
                            ApplyCommand_Common(channel, channelPreviousEffect[channel][effect], fxval_value);
                            if (channel < CHANNEL_PSG1) ApplyCommand_FM(channel, channelPreviousInstrument[channel], channelPreviousEffect[channel][effect], fxval_value);
                            else ApplyCommand_PSG(channelPreviousEffect[channel][effect], fxval_value);
                        }
                    }

                    // apply commands to temp RAM instrument
                    auto void apply_commands()
                    {
                        command(DATA_FX1_TYPE, DATA_FX1_VALUE, 0);
                        command(DATA_FX2_TYPE, DATA_FX2_VALUE, 1);
                        command(DATA_FX3_TYPE, DATA_FX3_VALUE, 2);
#if (MD_TRACKER_VERSION == 5)
                        command(DATA_FX4_TYPE, DATA_FX4_VALUE, 3);
                        command(DATA_FX5_TYPE, DATA_FX5_VALUE, 4);
                        command(DATA_FX6_TYPE, DATA_FX6_VALUE, 5);
#endif
                    }

                    voltick_value = ReadInstrumentSRAM(channelVolSeqID[channel], INST_VOL_TICK_01); // volume tick sequencer
                    if (voltick_value < 0x80)
                    {
                        channelSeqAttenuation[channel] = voltick_value;
                    }
                    arptick_value = ReadInstrumentSRAM(channelArpSeqID[channel], INST_ARP_TICK_01); // note tick sequencer

                    // write only when instrument is changed and not empty
                    inst = ReadPatternSRAM(playingPatternID, playingPatternRow, DATA_INSTRUMENT);

                    if (instrumentIsMuted[inst] == INST_MUTE) // check if instrument is muted. ignore writes, replace note with OFF if so
                    {
                        inst = NULL; channelCurrentNote[channel] = NOTE_OFF;
                    }
                    else
                    {
                        channelCurrentNote[channel] = ReadPatternSRAM(playingPatternID, playingPatternRow, DATA_NOTE);;
                    }

                    if (inst != NULL)
                    {
                        channelPreviousInstrument[channel] = inst; // change current channel instrument for later use
                        apply_commands(); // update [tmpInst] if any commands, write command *registers*, recalculate *combined registers*
                        WriteYM2612(channel, inst); // write all FM registers from [tmpInst]; all [tmpInst] are re-cached at play from SRAM to RAM
                    }
                    else
                    {
                        apply_commands();
                    }

                    // --------- trigger note playback; check empty note later; pass note id: 0..95, 254, 255

                    key = channelCurrentNote[channel];

                    if (channelCurrentNote[channel] != NOTE_EMPTY)
                    {
                        channelPreviousNote[channel] = channelCurrentNote[channel]; // note or OFF
                        channelArpNote[channel] = channelCurrentNote[channel];
                        if (channelNoteRetrigger[channel] > 0) channelNoteRetriggerCounter[channel] = 0;
                    }

                    if (arptick_value != NOTE_EMPTY) // ARP is not empty
                    {
                        if (channelCurrentNote[channel] <= MAX_NOTE) // if there is note, modify it with ARP seq
                        {
                            if (arptick_value > ARP_BASE) key = channelCurrentNote[channel] + (arptick_value - ARP_BASE);
                            else if (arptick_value < ARP_BASE) key = channelCurrentNote[channel] - (ARP_BASE - arptick_value);
                            if (key < 0 || key > MAX_NOTE) key = channelCurrentNote[channel]; // return same note if ARP is out of range
                        }
                        else if (channelCurrentNote[channel] == NOTE_EMPTY && channelPreviousNote[channel] != NOTE_OFF)
                        {
                            if (arptick_value > ARP_BASE) key = channelPreviousNote[channel] + (arptick_value - ARP_BASE);
                            else if (arptick_value < ARP_BASE) key = channelPreviousNote[channel] - (ARP_BASE - arptick_value);
                            if (key < 0 || key > MAX_NOTE || arptick_value == ARP_BASE) key = channelPreviousNote[channel];
                        }
                    }

                    if (channelNoteDelay[channel] == 0) PlayNote(key, channel);
                }
                else // empty pattern or muted channel
                {
                    if (channel == CHANNEL_FM3_OP4)
                    {
                       if (ch3Mode == CH3_SPECIAL_CSM || ch3Mode == CH3_SPECIAL_CSM_OFF) ch3Mode = CH3_NORMAL;
                    }
                }
            }

            ClearPatternPlaybackCursor();

            if (matrixRowJumpTo != EVALUATE_0xFF && currentScreen == SCREEN_MATRIX)
            {
                playingPatternRow = PATTERN_ROW_LAST + 1; // exceed to trigger next condition
            }
            else playingPatternRow++; // next line is..

            // jump to next...
            if (playingPatternRow > PATTERN_ROW_LAST || patternRowJumpTo != EVALUATE_0xFF)
            {
                DrawMatrixPlaybackCursor(TRUE); // erase

                // endless cycle pattern if not in matrix editor
                if (currentScreen == SCREEN_MATRIX)
                {
                    if (matrixRowJumpTo != EVALUATE_0xFF)
                    {
                        playingMatrixRow = matrixRowJumpTo - 1; // set to before, then increment
                        matrixRowJumpTo = EVALUATE_0xFF;
                    }
                    playingMatrixRow++; // next patterns in matrix is..

                    if (patternRowJumpTo != EVALUATE_0xFF)
                    {
                        playingPatternRow = patternRowJumpTo;
                        patternRowJumpTo = EVALUATE_0xFF;
                    }
                    else playingPatternRow = 0;

                    if (playingMatrixRow == MAX_MATRIX_ROWS)
                    {
                        playingMatrixRow = 0;
                    }
                }
                else if (playingPatternRow > PATTERN_ROW_LAST)
                {
                    playingPatternRow = 0;
                    matrixRowJumpTo = EVALUATE_0xFF;
                    patternRowJumpTo = EVALUATE_0xFF;
                }

                DrawMatrixPlaybackCursor(FALSE);
            }

            DrawPatternPlaybackCursor();
            frameCounter = -1; // changes are applied only when timer expires, not at every while loop ticks
        }
    }
    else if (!beginPlay) // need to run only once at playback stopped
    {
        SYS_disableInts();
        beginPlay = TRUE;
        // stop timer A (load: 1 to start, 0 to stop; enable: 1 to set register flag when overflowed, 0 to keep cycling without setting flag)
        // reset read register flag, timer overflowing is enabled
        YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, ch3Mode | 0b00111100);

        StopAllSound();
        ClearPatternPlaybackCursor();
        DrawMatrixPlaybackCursor(TRUE);
        SYS_enableInts();
    }
}

static s16 FindUnusedPattern()
{
    static bool used = FALSE;
    for (s16 pattern = 1; pattern <= MAX_PATTERN; pattern++) {
        used = FALSE;
        for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++) {
            for (u8 line = 0; line < MAX_MATRIX_ROWS; line++) { if (ReadMatrixSRAM(channel, line) == pattern) { used = TRUE; break; } }
            if (used) break;
        }
        if (!used) return pattern;
    }
    return NULL;
}

static void SetBPM(u16 counter)
{
    static u16 BPM = 0;
    static u32 microseconds = 0;

    /*YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_B, counter);
    microseconds = 3003 * (256 - counter); // timer B = 300.34 microseconds*/

    if (!IS_PALSYSTEM) microseconds = (744 * (H_INT_SKIP + 1)) * counter; // h-blank = 1/13440 sec; 74.4047 microseconds;
    else microseconds = (892 * (H_INT_SKIP + 1)) * counter; // h-blank = 1/11200 sec; 89.2857 microseconds;
    hIntToSkip = counter;

    //! software cpu subtick. very unstable
    //microseconds = 130 * timer; // sub-tick = 1/76800 sec; 13.0208333 microseconds
    //subTicksToSkip = timer;

    BPM = (600000000 / microseconds) / ((ticksPerEvenRow + ticksPerOddRow) * 2);
    SRAMW_writeWord(TEMPO, counter); // store

    if (BPM < 1000)
    {
        uintToStr(BPM, str, 3); // show timer value
        DrawNum(BG_A, PAL0, str, 3, 27);
    }
    else { VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "BAD", 3, 27); }
}

// cursors
void DrawMatrixPlaybackCursor(u8 bClear)
{
    static u8 playingPage = 0;

    playingPage = playingMatrixRow / MATRIX_SCREEN_ROWS;
    if (playingPage == currentPage)
    {
        if (bClear) { VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + playingMatrixRow), 39, playingMatrixRow - MATRIX_SCREEN_ROWS * playingPage + 2); }
        else if (bPlayback) { VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 39, playingMatrixRow - MATRIX_SCREEN_ROWS * playingPage + 2); }
    }
}

void ClearPatternPlaybackCursor()
{
    static s8 line = 0;

    line = playingPatternRow - 1; // previous line
    if (line == -1) line = 31;
    if (line < 16) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 40, line+4);
    else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 60, line-12);
}

void DrawPatternPlaybackCursor()
{
    static s8 line = 0;

    line = playingPatternRow - 1; // previous line
    if (line == -1) line = 31;
    if (line < 16) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 40, line+4);
    else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 60, line-12);
}

static void ChangeMatrixValue(s16 mod)
{
    static s16 value = 0;

    if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS) // matrix
    {
        if (mod != 0)
        {
            value = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow);
            if (value == NULL && lastEnteredPattern != NULL) value = lastEnteredPattern;
            else
            {
                value += mod;
                if (value < 1) value = MAX_PATTERN; // last pattern
                else if (value > MAX_PATTERN) value = 1; // first pattern
            }
            WriteMatrixSRAM(selectedMatrixChannel, selectedMatrixRow, value); bRefreshScreen = TRUE; matrixRowToRefresh = selectedMatrixRow;
            lastEnteredPattern = value;
        }
        else
        {
            WriteMatrixSRAM(selectedMatrixChannel, selectedMatrixRow, 0); bRefreshScreen = TRUE; matrixRowToRefresh = selectedMatrixRow;
        }
    }
    else // tempo
    {
        if (mod != 0)
        {
            value = SRAMW_readWord(TEMPO) - mod;
            if (value < TICK_SKIP_MIN) value = TICK_SKIP_MIN;
            else if (value > TICK_SKIP_MAX) value = TICK_SKIP_MAX;
            SetBPM((u16)value);
        }
    }
}

// input
static void JoyEvent(u16 joy, u16 changed, u16 state)
{
    static u8 patternColumnShift = 0;
    static s8 inc = 0; // paste increment
    static u8 row = 0; // paste row to
    static u8 col = 0; // pattern color slot

    if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS)
        selectedMatrixRow = selectedMatrixScreenRow + (currentPage * 25);

    auto void stop_playback()
    {
        frameCounter = 0;
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
        selectedPatternID = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow);
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
            u8 value = ReadPatternSRAM(selectedPatternID, selectedPatternRow, DATA_INSTRUMENT);
            if (value != 0x00) selectedInstrumentID = value;
        }
    }

    auto void switch_to_matrix_editor()
    {
        selectedPatternID = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow);
        currentScreen = SCREEN_MATRIX;
        bInitScreen = TRUE;
        bRefreshScreen = TRUE;
        VDP_setHorizontalScroll(BG_A, 0);
        VDP_setHorizontalScroll(BG_B, 0);
        matrixRowToRefresh = EVALUATE_0xFFFF;
    }

    if (joy == JOY_1 || joy == JOY_2)
    {
        switch (state)
        {
        case BUTTON_START:
            if (bPlayback == FALSE)
            {
                frameCounter = 0;
                playingPatternRow = 0; // start from the first line of current pattern
                playingMatrixRow = selectedMatrixRow; // actual line in array
                bPlayback = TRUE;
                //ssf_led_on(); // breaks PicoDrive (no sound, then hang on)
            }
            else
            {
                stop_playback();
                //ssf_led_off();
            }
            break;

        case BUTTON_MODE:
            if (bPlayback == FALSE)
            {
                frameCounter = 0;
                if (selectedPatternColumn < PATTERN_COLUMNS) playingPatternRow = selectedPatternRow; // start from the current selected pattern line
                else playingPatternRow = selectedPatternRow + PATTEN_ROWS_PER_SIDE;
                playingMatrixRow = selectedMatrixRow; // actual line in array
                bPlayback = TRUE;
                //ssf_led_on();
            }
            else
            {
                stop_playback();
                //ssf_led_off();
            }
            break;
        }
        if (selectedPatternColumn >= PATTERN_COLUMNS) patternColumnShift = PATTEN_ROWS_PER_SIDE; else patternColumnShift = 0;
        /// -------------------------------------------------------------------------------------------------------------------
        // direction hold

        if (changed & BUTTON_DIR) // 1/2/4/8 (true) or 0 (false)
        {
            doCount = changed; buttonCounter = GUI_NAVIGATION_DELAY; navigationDirection = state;
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
                    BIT_CLEAR(channelFlags, selectedMatrixChannel); // mute selected
                    if (channelFlags == 0) // all channels muted
                    {
                        channelFlags = 0b0001111111111111; // un-mute all
                        for (u8 i=0; i<CHANNELS_TOTAL; i++)
                            VDP_fillTileMapRect(BG_B, NULL, (i * 3) + 1, 1, 2, 1); // clear all marks
                    }
                    else
                    {
                        FillRowRight(BG_B, PAL1, FALSE, FALSE, GUI_MUTE, 2,(selectedMatrixChannel * 3) + 1, 1 ); // set mark
                    }
                    break;

                case BUTTON_DOWN: // un-mute/solo
                    if (channelFlags == 0b0001111111111111) // all channels unmuted
                    {
                        channelFlags = 0; // mute all
                        for (u8 i=0; i<CHANNELS_TOTAL; i++)
                            if (i != selectedMatrixChannel) FillRowRight(BG_B, PAL1, FALSE, FALSE, GUI_MUTE, 2,(i * 3) + 1, 1 ); // set all marks (except selected)
                    }
                    BIT_SET(channelFlags, selectedMatrixChannel); // un-mute selected
                    VDP_fillTileMapRect(BG_B, NULL, (selectedMatrixChannel * 3) + 1, 1, 2, 1); // clear mark
                    break;

                case BUTTON_Y: // un-mute all
                    channelFlags = 0b0001111111111111; // un-mute all
                    for (u8 i=0; i<CHANNELS_TOTAL; i++)
                        VDP_fillTileMapRect(BG_B, NULL, (i * 3) + 1, 1, 2, 1); // clear all marks
                    break;
                }
                break;

            case BUTTON_Y: // pattern colors
                switch(changed)
                {
                case BUTTON_LEFT:
                    selectedPatternID = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow); // select current pattern
                    if (selectedPatternID != NULL)
                    {
                        col = ReadPatternColorSRAM(selectedPatternID)-1;
                        if (col < 1) col = GUI_PATTERN_COLORS_MAX;
                        WritePatternColorSRAM(selectedPatternID, col);
                        RefreshPatternColors();
                    }
                    break;
                case BUTTON_RIGHT:
                    selectedPatternID = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow);
                    if (selectedPatternID != NULL)
                    {
                        col = ReadPatternColorSRAM(selectedPatternID)+1;
                        if (col > GUI_PATTERN_COLORS_MAX) col = 1;
                        WritePatternColorSRAM(selectedPatternID, col);
                        RefreshPatternColors();
                    }
                    break;
                case BUTTON_C: // clear
                    selectedPatternID = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow);
                    if (selectedPatternID != NULL)
                    {
                        col = ReadPatternColorSRAM(selectedPatternID);
                        if (col != 0)
                        {
                            WritePatternColorSRAM(selectedPatternID, 0);
                            RefreshPatternColors();
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
                    if (currentPage < MAX_MATRIX_PAGE)
                    {
                        currentPage++;
                        bRefreshScreen = bInitScreen = TRUE;
                        matrixRowToRefresh = EVALUATE_0xFFFF;
                        RefreshPatternColors();
                    }
                    break;

                case BUTTON_LEFT:
                    if (currentPage > 0)
                    {
                        currentPage--;
                        bRefreshScreen = bInitScreen = TRUE;
                        matrixRowToRefresh = EVALUATE_0xFFFF;
                        RefreshPatternColors();
                    }
                    break;

                case BUTTON_UP:
                    currentPage += 4;
                    if (currentPage > MAX_MATRIX_PAGE) currentPage = MAX_MATRIX_PAGE;
                    bRefreshScreen = bInitScreen = TRUE;
                    matrixRowToRefresh = EVALUATE_0xFFFF;
                    RefreshPatternColors();
                    break;

                case BUTTON_DOWN:
                    currentPage -= 4;
                    if (currentPage < 0) currentPage = 0;
                    bRefreshScreen = bInitScreen = TRUE;
                    matrixRowToRefresh = EVALUATE_0xFFFF;
                    RefreshPatternColors();
                    break;
                }
                break;

            case BUTTON_A:
                // A+L/R/U/D - change pattern in matrix
                switch (changed)
                {
                case BUTTON_B:
                    if (!ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow))
                    {
                        u16 value = FindUnusedPattern();
                        if (value)
                        {
                            WriteMatrixSRAM(selectedMatrixChannel, selectedMatrixRow, value );
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
                /*if (changed & BUTTON_RIGHT) { ChangeMatrixValue(1); }
                else if (changed & BUTTON_LEFT) { ChangeMatrixValue(-1); }
                else if (changed & BUTTON_UP) { ChangeMatrixValue(16); }
                else if (changed & BUTTON_DOWN) { ChangeMatrixValue(-16); }*/
                break;

            case BUTTON_B:
                switch (changed)
                {
                case BUTTON_UP: // delete selected matrix row
                    if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS)
                    {
                        for (u8 row = selectedMatrixRow; row < MAX_MATRIX_ROWS-2; row++)
                        {
                            for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++)
                            {
                                WriteMatrixSRAM(channel, row, ReadMatrixSRAM(channel, row+1));
                            }
                        }

                        // always clear last row
                        for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++)
                        {
                            WriteMatrixSRAM(channel, MAX_MATRIX_ROWS-1, 0);
                        }

                        matrixRowToRefresh = EVALUATE_0xFFFF;
                        bRefreshScreen = TRUE;
                    }
                    break;

                case BUTTON_DOWN: // copy and insert selected matrix row
                    if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS)
                    {
                        for (u8 row = MAX_MATRIX_ROWS-2; row >= selectedMatrixRow; row--)
                        {
                            for (u8 channel = 0; channel < CHANNELS_TOTAL; channel++)
                            {
                                WriteMatrixSRAM(channel, row+1, ReadMatrixSRAM(channel, row));
                            }
                        }

                        matrixRowToRefresh = EVALUATE_0xFFFF;
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
                    switch_to_instrument_editor();
                    break;

                case BUTTON_LEFT:
                    switch_to_matrix_editor();
                    break;
                }
                break;

            case BUTTON_Y:
                NavigatePattern(PATTERN_JUMP_SIDE);
                /*switch (changed)
                {
                case BUTTON_Y:

                    break;
                }*/
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
                                WritePatternSRAM(selectedPatternID, row, DATA_NOTE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_NOTE));
                                WritePatternSRAM(selectedPatternID, row, DATA_INSTRUMENT, ReadPatternSRAM(patternCopyFrom, cnt, DATA_INSTRUMENT));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX1_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX1_TYPE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX1_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX1_VALUE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX2_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX2_TYPE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX2_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX2_VALUE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX3_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX3_TYPE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX3_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX3_VALUE));

#if (MD_TRACKER_VERSION == 5)
                                WritePatternSRAM(selectedPatternID, row, DATA_FX4_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX4_TYPE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX4_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX4_VALUE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX5_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX5_TYPE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX5_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX5_VALUE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX6_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX6_TYPE));
                                WritePatternSRAM(selectedPatternID, row, DATA_FX6_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX6_VALUE));
#endif
                                inc++;
                            }
                        }
                        inc = 0; bRefreshScreen = TRUE; patternRowToRefresh = EVALUATE_0xFF;
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
                                    WritePatternSRAM(selectedPatternID, row, DATA_NOTE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_NOTE)); inc++;
                                } //else break;
                                break;

                            case DATA_INSTRUMENT: case (DATA_INSTRUMENT + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_INSTRUMENT, ReadPatternSRAM(patternCopyFrom, cnt, DATA_INSTRUMENT)); inc++;
                                } //else return;
                                break;

                            case DATA_FX1_TYPE: case (DATA_FX1_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX1_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX1_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX1_VALUE: case (DATA_FX1_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX1_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX1_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX2_TYPE: case (DATA_FX2_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX2_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX2_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX2_VALUE: case (DATA_FX2_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX2_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX2_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX3_TYPE: case (DATA_FX3_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX3_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX3_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX3_VALUE: case (DATA_FX3_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX3_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX3_VALUE)); inc++;
                                } //else return;
                                break;

#if (MD_TRACKER_VERSION == 5)
                            case DATA_FX4_TYPE: case (DATA_FX4_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX4_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX4_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX4_VALUE: case (DATA_FX4_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX4_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX4_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX5_TYPE: case (DATA_FX5_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX5_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX5_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX5_VALUE: case (DATA_FX5_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX5_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX5_VALUE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX6_TYPE: case (DATA_FX6_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX6_TYPE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX6_TYPE)); inc++;
                                } //else return;
                                break;

                            case DATA_FX6_VALUE: case (DATA_FX6_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    WritePatternSRAM(selectedPatternID, row, DATA_FX6_VALUE, ReadPatternSRAM(patternCopyFrom, cnt, DATA_FX6_VALUE)); inc++;
                                } //else return;
                                break;
#endif
                            }
                        }
                        inc = 0; bRefreshScreen = TRUE; patternRowToRefresh = EVALUATE_0xFF;
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
                    }
                    else
                    {
                        if (patternCopyRangeEnd <= PATTERN_ROW_LAST)
                        {
                            if (patternCopyRangeEnd < 16)
                                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 44, patternCopyRangeEnd+4);
                            else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), 64, patternCopyRangeEnd-12);
                            patternCopyRangeEnd++;
                        }
                    }
                    break;
                }
                break;
            // clear
            case BUTTON_C:
                switch (selectedPatternColumn)
                {
                case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_NOTE, NOTE_EMPTY);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_INSTRUMENT: case (DATA_INSTRUMENT + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_INSTRUMENT, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX1_TYPE: case (DATA_FX1_TYPE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX1_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX1_VALUE: case (DATA_FX1_VALUE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX1_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX2_TYPE: case (DATA_FX2_TYPE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX2_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX2_VALUE: case (DATA_FX2_VALUE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX2_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX3_TYPE: case (DATA_FX3_TYPE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX3_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX3_VALUE: case (DATA_FX3_VALUE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX3_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
#if (MD_TRACKER_VERSION == 5)
                case DATA_FX4_TYPE: case (DATA_FX4_TYPE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX4_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX4_VALUE: case (DATA_FX4_VALUE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX4_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX5_TYPE: case (DATA_FX5_TYPE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX5_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX5_VALUE: case (DATA_FX5_VALUE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX5_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX6_TYPE: case (DATA_FX6_TYPE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX6_TYPE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
                case DATA_FX6_VALUE: case (DATA_FX6_VALUE + PATTERN_COLUMNS):
                    WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_FX6_VALUE, NULL);
                    bRefreshScreen = TRUE;
                    patternRowToRefresh = selectedPatternRow + patternColumnShift;
                    break;
#endif
                }

                switch (changed)
                {
                // note off
                case BUTTON_B:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        WritePatternSRAM(selectedPatternID, selectedPatternRow + patternColumnShift, DATA_NOTE, NOTE_OFF);
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
                    for (u8 i = 0; i < MAX_INSTRUMENT; i++) instrumentIsMuted[i] = INST_MUTE; // mute all
                    instrumentIsMuted[selectedInstrumentID] = INST_SOLO; // set to solo
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_STATE;
                    break;

                case BUTTON_Y: // un-mute all instruments
                    for (u8 i = 0; i < MAX_INSTRUMENT; i++) instrumentIsMuted[i] = INST_PLAY;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_STATE;
                    break;
                }
                break;

            case BUTTON_Y:
                // Y + D-Pad: select instrument for editing
                switch (changed)
                {
                case BUTTON_RIGHT:
                    if (selectedInstrumentID < MAX_INSTRUMENT) { selectedInstrumentID++; bRefreshScreen = TRUE; instrumentParameterToRefresh = EVALUATE_0xFF; }
                    break;

                case BUTTON_LEFT:
                    if (selectedInstrumentID > 1) { selectedInstrumentID--; bRefreshScreen = TRUE; instrumentParameterToRefresh = EVALUATE_0xFF; }
                    break;

                case BUTTON_UP:
                    if (selectedInstrumentID < (MAX_INSTRUMENT - 16)) selectedInstrumentID += 16; else selectedInstrumentID = MAX_INSTRUMENT;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = EVALUATE_0xFF;
                    break;

                case BUTTON_DOWN:
                    if (selectedInstrumentID > 16) selectedInstrumentID -= 16; else selectedInstrumentID = 1;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = EVALUATE_0xFF;
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
                    WriteInstrumentSRAM(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator, 0x7F); // VOL
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_VOLSEQ;
                }
                else if (selectedInstrumentParameter == GUI_INST_PARAM_ARPSEQ)
                {
                    WriteInstrumentSRAM(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator, ARP_BASE); // ARP
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_ARPSEQ;
                }
                break;

            case BUTTON_B:
                switch (selectedInstrumentParameter)
                {
                case GUI_INST_PARAM_VOLSEQ:
                    WriteInstrumentSRAM(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator, 0xFF); // VOL
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_VOLSEQ;
                    break;
                case GUI_INST_PARAM_ARPSEQ:
                    WriteInstrumentSRAM(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator, NOTE_EMPTY); // ARP
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_ARPSEQ;
                    break;
                case GUI_INST_PARAM_COPY:
                    // copy instrument routine (FM parameters only)
                    if (instCopyTo != selectedInstrumentID)
                    {
                        for (u8 param = INST_ALG; param < INST_SSGEG4; param++)
                        {
                            WriteInstrumentSRAM(instCopyTo, param, ReadInstrumentSRAM(selectedInstrumentID, param));
                        }
                        VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "OK", GUI_INST_NAME_START, 1);
                    }
                    break;
                }
                break;
            }
        }
        break;
        }
    }
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
#if (MD_TRACKER_VERSION == 5)
        case GUI_PATTERN_L_FX4_TYPE:
        case GUI_PATTERN_L_FX4_VALUE:
        case GUI_PATTERN_L_FX5_TYPE:
        case GUI_PATTERN_L_FX5_VALUE:
        case GUI_PATTERN_L_FX6_TYPE:
        case GUI_PATTERN_L_FX6_VALUE:
#endif
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
#if (MD_TRACKER_VERSION == 5)
        case GUI_PATTERN_R_FX4_TYPE:
        case GUI_PATTERN_R_FX4_VALUE:
        case GUI_PATTERN_R_FX5_TYPE:
        case GUI_PATTERN_R_FX5_VALUE:
        case GUI_PATTERN_R_FX6_TYPE:
        case GUI_PATTERN_R_FX6_VALUE:
#endif
            offset_x = 40+12; offset_y = 4; width = 1;
            break;
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
            offset_x = 80+6; offset_y = 8; width = 0; selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_VOLSEQ:
        case GUI_INST_PARAM_ARPSEQ:
            if (pos_x > 7) offset_x = 80+7; else offset_x = 80+6; offset_y = 8; width = 2;
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
            offset_x = 80+33; offset_y = 6-GUI_INST_PARAM_PCM_RATE; width = 0; selectedInstrumentOperator = 0;
            break;
        }
    }

    auto void draw_cursor_1(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
    }

    auto void draw_cursor_2(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 1, y);
    }

    auto void draw_cursor_3(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 1, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 2, y);
    }

    auto void clear_cursor_1(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
    }

    auto void clear_cursor_2(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
    }

    auto void clear_cursor_2_color(u8 x, u8 y)
    {
        selectedPatternID = ReadMatrixSRAM(selectedMatrixChannel, selectedMatrixRow); // select previous pattern on clear
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        if (selectedPatternID != NULL)
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PATTERNCOLORS[ReadPatternColorSRAM(selectedPatternID)]), x + 1, y);
        else
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
    }

    auto void clear_cursor_3(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 2, y);
    }

    // it goes first
    switch (currentScreen) {
    case SCREEN_MATRIX:
        if (bClear)
        {
            if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS) clear_cursor_2_color(pos_x * width + offset_x, pos_y + offset_y);
            else clear_cursor_3(3, 27); // BPM
        }
        else
        {
            if (selectedMatrixScreenRow < MATRIX_SCREEN_ROWS) draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y);
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
#if (MD_TRACKER_VERSION == 5)
            case GUI_PATTERN_L_FX4_VALUE: case GUI_PATTERN_R_FX4_VALUE:
            case GUI_PATTERN_L_FX5_VALUE: case GUI_PATTERN_R_FX5_VALUE:
#endif
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
            case GUI_INST_PARAM_PCM_RATE: clear_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break;
            case GUI_INST_PARAM_PCM_NOTE: clear_cursor_3(pos_x * width + offset_x, pos_y + offset_y); break;
            default: clear_cursor_2(pos_x * width + offset_x, pos_y + offset_y); break;
            }
        }
        else
        {
            switch (selectedInstrumentParameter)
            {
            case GUI_INST_PARAM_NAME:
            case GUI_INST_PARAM_PCM_RATE: draw_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break;
            case GUI_INST_PARAM_PCM_NOTE: draw_cursor_3(pos_x * width + offset_x, pos_y + offset_y); break;
            default: draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y); break;
            }
        }
    break;
    }
}
// ------------------------------ PATTERN MATRIX
void DisplayPatternMatrix()
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
        pageShift = currentPage * MATRIX_SCREEN_ROWS; // page shift

        if (matrixRowToRefresh != EVALUATE_0xFFFF) // if refresh only once only particular line
        {
            line = matrixRowToRefresh - pageShift; // map to 0..24
        }

        patternID = ReadMatrixSRAM(chan, line + pageShift);

        // display assigned pattern number
        shiftX = chan * 3;
        shiftY = line + 2;

        if (patternID == 0)
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

            if (num1 > 0) num1 += bgBaseTileIndex[1]; // else draw empty tile
            !(line & 1) ? (palx = PAL0) : (palx = PAL1);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num1), shiftX, shiftY); // clear or draw digit 3
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, num2), shiftX + 1, shiftY); // draw digit 2
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, num3), shiftX + 2, shiftY); // draw digit 1
        }
        chan++;

        if (chan > CHANNEL_PSG4_NOISE) // end or row
        {
            if (matrixRowToRefresh == EVALUATE_0xFFFF) // redraw the whole matrix
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + line + pageShift), 39, shiftY); // line number
                line++;
                chan = 0;
                if (line >= MATRIX_SCREEN_ROWS)
                {
                    line = 0;
                    bRefreshScreen = FALSE;
                    DrawMatrixPlaybackCursor(FALSE); // if returning from other page during playback
                }
            }
            else // redraw only currently changed edited row
            {
                matrixRowToRefresh = EVALUATE_0xFFFF; // set the trigger off
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
        value = ReadPatternSRAM(selectedPatternID, row, DATA_NOTE);
        if (value > MAX_NOTE)
        {
            WritePatternSRAM(selectedPatternID, row, DATA_NOTE, lastEnteredNote);
            patternRowToRefresh = row;
        }
        else
        {
            value += noteMod;
            if (value < 0) value = 0; else if (value > MAX_NOTE) value = MAX_NOTE;
            WritePatternSRAM(selectedPatternID, row, DATA_NOTE, value);
            patternRowToRefresh = row;
            lastEnteredNote = value;
        }
    }

    auto void write_instrument(u8 column)
    {
        row = selectedPatternRow + 16 * column;
        value = ReadPatternSRAM(selectedPatternID, row, DATA_INSTRUMENT);
        if (value == 0)
        {
            WritePatternSRAM(selectedPatternID, row, DATA_INSTRUMENT, lastEnteredInstrumentID);
            patternRowToRefresh = row;
        }
        else
        {
            value += parameterMod;
            if (value < 1) value = MAX_INSTRUMENT; else if (value > MAX_INSTRUMENT) value = 1;
            WritePatternSRAM(selectedPatternID, row, DATA_INSTRUMENT, value);
            patternRowToRefresh = row;
            lastEnteredInstrumentID = value;
        }

        // print info: instrument name
        VDP_clearTextArea(GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y + 1, 28, 3);
        for (u8 i = 0; i < 8; i++)
        {
            VDP_setTileMapXY(BG_A,
                TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_ALPHABET[ReadInstrumentSRAM(lastEnteredInstrumentID, INST_NAME_1 + i)]),
                GUI_INFO_PRINT_INST_X + i, GUI_INFO_PRINT_INST_Y);
        }
        //VDP_clearText(GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y, 25); // clear rest of line (33-8)
    }

    auto void write_fx_type(u8 id, u8 column)
    {
        row = selectedPatternRow + 16 * column;
        value = ReadPatternSRAM(selectedPatternID, row, id);
        if (value == 0)
        {
            WritePatternSRAM(selectedPatternID, row, id, lastEnteredEffect);
            patternRowToRefresh = row;
        }
        else
        {
            value += parameterMod;
            if (value < 1) value = 0xFF; else if (value > 0xFF) value = 1;
            WritePatternSRAM(selectedPatternID, row, id, value);
            patternRowToRefresh = row;
            lastEnteredEffect = value;
        }

        // print info: last entered effect description
        if (strcmp(infoCommands[lastEnteredEffect], STRING_EMPTY) == 0)
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
        value = ReadPatternSRAM(selectedPatternID, row, id);
        if (value == 0)
        {
            WritePatternSRAM(selectedPatternID, row, id, lastEnteredEffectValue);
            patternRowToRefresh = row;
        }
        else
        {
            value += parameterMod;
            if (value < 1) value = 0xFF; else if (value > 0xFF) value = 1;
            WritePatternSRAM(selectedPatternID, row, id, value);
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
#if (MD_TRACKER_VERSION == 5)
    case DATA_FX4_TYPE:                         write_fx_type(DATA_FX4_TYPE, 0); break;
    case DATA_FX4_VALUE:                        write_fx_value(DATA_FX4_VALUE, 0); break;
    case DATA_FX5_TYPE:                         write_fx_type(DATA_FX5_TYPE, 0); break;
    case DATA_FX5_VALUE:                        write_fx_value(DATA_FX5_VALUE, 0); break;
    case DATA_FX6_TYPE:                         write_fx_type(DATA_FX6_TYPE, 0); break;
    case DATA_FX6_VALUE:                        write_fx_value(DATA_FX6_VALUE, 0); break;
#endif

    case (DATA_NOTE + PATTERN_COLUMNS):         write_note(1); break;
    case (DATA_INSTRUMENT + PATTERN_COLUMNS):   write_instrument(1); break;
    case (DATA_FX1_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX1_TYPE, 1); break;
    case (DATA_FX1_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX1_VALUE, 1); break;
    case (DATA_FX2_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX2_TYPE, 1); break;
    case (DATA_FX2_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX2_VALUE, 1); break;
    case (DATA_FX3_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX3_TYPE, 1); break;
    case (DATA_FX3_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX3_VALUE, 1); break;
#if (MD_TRACKER_VERSION == 5)
    case (DATA_FX4_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX4_TYPE, 1); break;
    case (DATA_FX4_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX4_VALUE, 1); break;
    case (DATA_FX5_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX5_TYPE, 1); break;
    case (DATA_FX5_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX5_VALUE, 1); break;
    case (DATA_FX6_TYPE + PATTERN_COLUMNS):     write_fx_type(DATA_FX6_TYPE, 1); break;
    case (DATA_FX6_VALUE + PATTERN_COLUMNS):    write_fx_value(DATA_FX6_VALUE, 1); break;
#endif
    }
}

void DisplayPatternEditor()
{
    if (bInitScreen)
    {
        bInitScreen = 0;
        line = 0;
    }

    if (bRefreshScreen)
    {
        static u8 note = 0;
        static u8 noteFreqID = 0;
        static u8 value = 0;
        static u8 side = 0; // left 0 or right 1
        static u8 shiftX = 0;
        static u8 shiftY = 0;
        static u8 palx = PAL0;
        static u8 palminus = PAL1;
        static u8 lineShiftY = 0;

        if (patternRowToRefresh != EVALUATE_0xFF) // refresh only selected line
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
        note = ReadPatternSRAM(selectedPatternID, line, DATA_NOTE); // notes
        noteFreqID = note;
        while (noteFreqID > 11) noteFreqID -= 12;

        // calculate offsets for the right pattern columns display
        side = (line > 15) ? 1 : 0;
        shiftX = 20 * side;
        shiftY = 4 - (16 * side);
        palx = (line % 4 == 0) ? PAL0 : PAL1;
        lineShiftY = line + shiftY;

        auto void display_fx_type(u8 id, u8 shift)
        {
            value = ReadPatternSRAM(selectedPatternID, line, id);
            u8 xPos = GUI_FX_TYPE_START + shiftX + shift;

            if (value == 0)
            {
                if (palx == PAL0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0]), xPos, lineShiftY);
                else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[0]), xPos, lineShiftY);
            }
            else
            {
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, value + bgBaseTileIndex[0]), xPos, lineShiftY);
            }
        }

        auto void display_fx_value(u8 id, u8 shift)
        {
            value = ReadPatternSRAM(selectedPatternID, line, id);
            u8 xPos = GUI_FX_VALUE_START + shiftX + shift;

            if (value == 0)
            {
                if (palx == PAL0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0]), xPos, lineShiftY);
                else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[0]), xPos, lineShiftY);
            }
            else
            {
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, value + bgBaseTileIndex[0]), xPos, lineShiftY);
            }
        }

        // note name
        if (note < NOTE_EMPTY)
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_NOTE_NAMES[0][noteFreqID]), 41 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_NOTE_NAMES[1][noteFreqID]), 42 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + (note / 12)), 43 + shiftX, lineShiftY); // octave number
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
        value = ReadPatternSRAM(selectedPatternID, line, DATA_INSTRUMENT);
        if (value == 0)
        {
            if (palx == PAL0) palminus = PAL1; else palminus = PAL2;
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 45 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palminus, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 46 + shiftX, lineShiftY);
        }
        else DrawHex2(palx, value, 45 + shiftX, lineShiftY);

        display_fx_type(DATA_FX1_TYPE, 0);
        display_fx_value(DATA_FX1_VALUE, 0);
        display_fx_type(DATA_FX2_TYPE, 2);
        display_fx_value(DATA_FX2_VALUE, 2);
        display_fx_type(DATA_FX3_TYPE, 4);
        display_fx_value(DATA_FX3_VALUE, 4);
#if (MD_TRACKER_VERSION == 5)
        display_fx_type(DATA_FX4_TYPE, 6);
        display_fx_value(DATA_FX4_VALUE, 6);
        display_fx_type(DATA_FX5_TYPE, 8);
        display_fx_value(DATA_FX5_VALUE, 8);
        display_fx_type(DATA_FX6_TYPE, 10);
        display_fx_value(DATA_FX6_VALUE, 10);
#endif
        // refresh all, line by frame; or only currently edited line;
        if (patternRowToRefresh == EVALUATE_0xFF) // refresh all
        {
            line++;
            if (line > 31)
            {
                line = 0;
                bRefreshScreen = FALSE;
            }
        }
        else // selected line is refreshed, stop here
        {
            patternRowToRefresh = EVALUATE_0xFF;
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
        value = ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, byteNum) + modifier;
        if (value < 0) value = 255; else if (value > 255) value = 0;
        WriteSampleRegionSRAM(selectedSampleBank, selectedSampleNote, byteNum, value);

        // border check
        u32 sampleEnd =
                (ReadSampleRegionSRAM(activeSampleBank, selectedSampleNote, 3) << 16) |
                (ReadSampleRegionSRAM(activeSampleBank, selectedSampleNote, 4) << 8) |
                ReadSampleRegionSRAM(activeSampleBank, selectedSampleNote, 5);

        if (sampleEnd > DAC_DATA_END)
        {
            WriteSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 3, (u8)((DAC_DATA_END >> 16) & 0xFF));
            WriteSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 4, (u8)((DAC_DATA_END >> 8) & 0xFF));
            WriteSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 5, (u8)(DAC_DATA_END & 0xFF));
        }
    }

    switch (selectedInstrumentParameter)
    {
    case GUI_INST_PARAM_ALG:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_ALG) + modifier;
        if (value < 0) value = 0; else if (value > 7) value = 7;
        WriteInstrumentSRAM(selectedInstrumentID, INST_ALG, value);
        break;
    case GUI_INST_PARAM_FMS:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_FMS) + modifier;
        if (value < 0) value = 0; else if (value > 7) value = 7;
        WriteInstrumentSRAM(selectedInstrumentID, INST_FMS, value);
        break;
    case GUI_INST_PARAM_AMS:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_AMS) + modifier;
        if (value < 0) value = 0; else if (value > 3) value = 3;
        WriteInstrumentSRAM(selectedInstrumentID, INST_AMS, value);
        break;
    case GUI_INST_PARAM_PAN:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_PAN) + modifier;
        if (value < 0) value = 0; else if (value > 3) value = 3;
        WriteInstrumentSRAM(selectedInstrumentID, INST_PAN, value);
        break;
    case GUI_INST_PARAM_FB:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_FB) + modifier;
        if (value < 0) value = 0; else if (value > 7) value = 7;
        WriteInstrumentSRAM(selectedInstrumentID, INST_FB, value);
        break;
    case GUI_INST_PARAM_TL:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_TL1 + selectedInstrumentOperator) - modifier;
            if (value < 0) value = 0x7F; else if (value > 0x7F) value = 0;
            WriteInstrumentSRAM(selectedInstrumentID, INST_TL1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_RS:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_RS1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 3) value = 3;
            WriteInstrumentSRAM(selectedInstrumentID, INST_RS1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_MUL:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_MUL1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 15) value = 15;
            WriteInstrumentSRAM(selectedInstrumentID, INST_MUL1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_DT:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_DT1 + selectedInstrumentOperator) + modifier;
            if (value < 1) value = 1; else if (value > 7) value = 7;
            WriteInstrumentSRAM(selectedInstrumentID, INST_DT1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_AR:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_AR1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 31) value = 31;
            WriteInstrumentSRAM(selectedInstrumentID, INST_AR1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D1R:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_D1R1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 31) value = 31;
            WriteInstrumentSRAM(selectedInstrumentID, INST_D1R1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D1L:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_D1L1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 15) value = 15;
            WriteInstrumentSRAM(selectedInstrumentID, INST_D1L1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D2R:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_D2R1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 31) value = 31;
            WriteInstrumentSRAM(selectedInstrumentID, INST_D2R1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_RR:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_RR1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0; else if (value > 15) value = 15;
            WriteInstrumentSRAM(selectedInstrumentID, INST_RR1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_AM:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_AM1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = FALSE; else if (value > 1) value = TRUE;
            WriteInstrumentSRAM(selectedInstrumentID, INST_AM1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_SSGEG:
        if (selectedInstrumentOperator < 4)
        {
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_SSGEG1 + selectedInstrumentOperator) + modifier;
            if (value < 7) value = 7; else if (value > 15) value = 15;
            WriteInstrumentSRAM(selectedInstrumentID, INST_SSGEG1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_LFO:
        value = SRAMW_readByte(GLOBAL_LFO) + modifier;
        if (value < 7) value = 7; else if (value > 15) value = 15;
        SRAMW_writeByte(GLOBAL_LFO, value); SetGlobalLFO(value);
        break;
    case GUI_INST_PARAM_VOLSEQ:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator) - modifier;
        if (value < 0) value = 0x7F; else if (value > 0x7F) value = 0;
        WriteInstrumentSRAM(selectedInstrumentID, INST_VOL_TICK_01 + selectedInstrumentOperator, value);
        break;
    case GUI_INST_PARAM_ARPSEQ:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator); // need to check first
        if (value != NOTE_EMPTY)
        {
            value += modifier;
            if (value < 76) value = 76; else if (value > 124) value = 124; // +- 24 semitones
            WriteInstrumentSRAM(selectedInstrumentID, INST_ARP_TICK_01 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_NAME:
        value = ReadInstrumentSRAM(selectedInstrumentID, INST_NAME_1 + selectedInstrumentOperator) + modifier;
        if (value < 0) value = 37; else if (value > 37) value = 0;
        WriteInstrumentSRAM(selectedInstrumentID, INST_NAME_1 + selectedInstrumentOperator, value);
        break;
    case GUI_INST_PARAM_PCM_BANK:
        value = selectedSampleBank + modifier;
        if (value > 3) value = 3;
        else if (value < 0) value = 0;
        selectedSampleBank = value;
        break;
    case GUI_INST_PARAM_PCM_NOTE:
        value = selectedSampleNote + modifier;
        if (value > MAX_NOTE) value = 0;
        else if (value < 0) value = MAX_NOTE;
        selectedSampleNote = value;
        break;
    case GUI_INST_PARAM_PCM_START:
        write_pcm(selectedInstrumentOperator);
        break;
    case GUI_INST_PARAM_PCM_END:
        write_pcm(selectedInstrumentOperator + 3);
        break;
    case GUI_INST_PARAM_PCM_LOOP:
        value = ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 6) + modifier;
        if (value < FALSE) value = FALSE; else if (value > TRUE) value = TRUE;
        WriteSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 6, value);
        break;
    case GUI_INST_PARAM_PCM_RATE:
        value = ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 7) + modifier;
        if (value < SOUND_RATE_32000) value = SOUND_RATE_32000; else if (value > SOUND_RATE_8000) value = SOUND_RATE_8000;
        WriteSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 7, value);
        break;
    case GUI_INST_PARAM_COPY: // global, not saved to sram
        value = instCopyTo + modifier;
        if (value < 0x01) instCopyTo = 0xFF; else if (value > 0xFF) instCopyTo = 0x01; else instCopyTo = value; // guard, wrap
        break;
    }
    CacheIstrumentToRAM(selectedInstrumentID); // update RAM struct
}

void DisplayInstrumentEditor()
{
    static u8 value = 0; // buffer
    static u8 alg = 0;
    static u8 stepDrawPos = 0;

    auto void draw_pcm_start()
    {
        DrawHex2(PAL0, ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 0), 113, 3);
        DrawHex2(PAL0, ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 1), 115, 3);
        DrawHex2(PAL0, ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 2), 117, 3);
    }

    auto void draw_pcm_end()
    {
        DrawHex2(PAL0, ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 3), 113, 4);
        DrawHex2(PAL0, ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 4), 115, 4);
        DrawHex2(PAL0, ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 5), 117, 4);
    }

    auto void draw_pcm_loop()
    {
        value = ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 6);
        if (value == FALSE) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 113, 5);
        else DrawText(BG_A, PAL0, "ON", 113, 5);
    }

    auto void draw_pcm_rate()
    {
        value = ReadSampleRegionSRAM(selectedSampleBank, selectedSampleNote, 7);
        switch (value)
        {
            case SOUND_RATE_32000: DrawNum(BG_A, PAL1, "32000", 114, 6); break;
            case SOUND_RATE_22050: DrawNum(BG_A, PAL1, "22050", 114, 6); break;
            case SOUND_RATE_16000: DrawNum(BG_A, PAL1, "16000", 114, 6); break;
            case SOUND_RATE_13400: DrawNum(BG_A, PAL1, "13400", 114, 6); break;
            case SOUND_RATE_11025: DrawNum(BG_A, PAL1, "11025", 114, 6); break;
            case SOUND_RATE_8000: DrawNum(BG_A, PAL1, "8000", 114, 6); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, NULL), 118, 6); break;
        }
    }

    if (bInitScreen)
    {
        bInitScreen = FALSE;
        instrumentParameterToRefresh = EVALUATE_0xFF;
    }

    if (bRefreshScreen)
    {
        if (instrumentParameterToRefresh == EVALUATE_0xFF)
        {
            DrawHex2(PAL1, selectedInstrumentID, 87, 0);
        }

        switch (instrumentParameterToRefresh)
        {
        case GUI_INST_PARAM_ALG: case 255:
            alg = ReadInstrumentSRAM(selectedInstrumentID, INST_ALG);
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
                        case GUI_ALG_LINE_H: case GUI_ALG_LINE_V: case GUI_ARROW:
                        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_ALG_GRID[alg][i][j]), 90+j, 3+i);
                        break;
                        default: VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 90+j, 3+i);
                    }
                }
            }
            break;
        case GUI_INST_PARAM_FMS: case 254:
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_FMS);
            if (value == 0) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_MINUS, 2, 87, 3);
            else DrawHex2(PAL0, value, 87, 3);
            break;
        case GUI_INST_PARAM_AMS: case 253:
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_AMS);
            if (value == 0) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_MINUS, 2, 87, 4);
            else DrawHex2(PAL0, value, 87, 4);
            break;
        case GUI_INST_PARAM_PAN: case 252:
            value = ReadInstrumentSRAM(selectedInstrumentID, INST_PAN);
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
            DrawHex2(PAL0, ReadInstrumentSRAM(selectedInstrumentID, INST_FB), 87, 6);
            break;
        case GUI_INST_PARAM_TL: case 250:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, 0x80 - ReadInstrumentSRAM(selectedInstrumentID, INST_TL1 + i), 94 + i*3, 9);
            break;
        case GUI_INST_PARAM_RS: case 249:
            for (u8 i=0; i<4; i++)
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + ReadInstrumentSRAM(selectedInstrumentID, INST_RS1 + i)), 95 + i*3, 10);
            break;
        case GUI_INST_PARAM_MUL: case 248:
            for (u8 i=0; i<4; i++)
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + ReadInstrumentSRAM(selectedInstrumentID, INST_MUL1 + i)), 95 + i*3, 11);
            break;
        case GUI_INST_PARAM_DT: case 247:
            for (u8 i=0; i<4; i++)
            {
                value = ReadInstrumentSRAM(selectedInstrumentID, INST_DT1 + i);
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
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, 1+ ReadInstrumentSRAM(selectedInstrumentID, INST_AR1 + i), 94 + i*3, 14);
            break;
        case GUI_INST_PARAM_D1R: case 245:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, 1+ ReadInstrumentSRAM(selectedInstrumentID, INST_D1R1 + i), 94 + i*3, 15);
            break;
        case GUI_INST_PARAM_D1L: case 244:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, 1+ ReadInstrumentSRAM(selectedInstrumentID, INST_D1L1 + i), 94 + i*3, 16);
            break;
        case GUI_INST_PARAM_D2R: case 243:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, 1+ ReadInstrumentSRAM(selectedInstrumentID, INST_D2R1 + i), 94 + i*3, 17);
            break;
        case GUI_INST_PARAM_RR: case 242:
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, 1+ ReadInstrumentSRAM(selectedInstrumentID, INST_RR1 + i), 94 + i*3, 18);
            break;
        case GUI_INST_PARAM_AM: case 241:
            for (u8 i=0; i<4; i++)
                if (ReadInstrumentSRAM(selectedInstrumentID, INST_AM1 + i) == 1) DrawText(BG_A, PAL0, "ON", 94 + i*3, 20);
                else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 94 + i*3, 20);
            break;
        case GUI_INST_PARAM_SSGEG: case 240:
            for (u8 i=0; i<4; i++)
            {
                value = ReadInstrumentSRAM(selectedInstrumentID, INST_SSGEG1 + i);
                if (value > 7) DrawHex2(PAL0, value - 7, 94 + i*3, 21);
                else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 94 + i*3, 21);
            }
        case GUI_INST_PARAM_LFO: case 239:
            value = SRAMW_readByte(GLOBAL_LFO);
            if (value > 7) DrawHex2(BG_A, value - 7, 86, 24);
            else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 86, 24);
            break;
        case GUI_INST_PARAM_VOLSEQ: case 238:
            for (u8 i = 0; i < 16; i++)
            {
                value = ReadInstrumentSRAM(selectedInstrumentID, INST_VOL_TICK_01 + i);
                stepDrawPos = 86 + (i*2) + (i/8);
                if (value == 255)
                {
                    if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 25);
                    else FillRowRight(BG_A, PAL2, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 25);
                }
                else
                {
                    if (i%4==0) DrawHex2(PAL3, 0x7F - value, stepDrawPos, 25);
                    else DrawHex2(PAL0, 0x7F - value, stepDrawPos, 25);
                }
            }
        case GUI_INST_PARAM_ARPSEQ: case 237:
            for (u8 i = 0; i < 16; i++)
            {
                value = ReadInstrumentSRAM(selectedInstrumentID, INST_ARP_TICK_01 + i);
                stepDrawPos = 86 + (i*2) + (i/8);
                if (value == 100)
                {
                    if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_BIGDOT, 2, stepDrawPos, 26);
                    else FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, stepDrawPos, 26);
                    VDP_fillTileMapRect(BG_A, NULL, stepDrawPos, 27, 2, 1);
                }
                else if (value == NOTE_EMPTY)
                {
                    if (i%4==0) FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 26);
                    else FillRowRight(BG_A, PAL2, FALSE, FALSE, GUI_MINUS, 2, stepDrawPos, 26);
                    VDP_fillTileMapRect(BG_A, NULL, stepDrawPos, 27, 2, 1);
                }
                else if (value > 100)
                {
                    if (i%4==0) DrawHex2(PAL3, value - 100, stepDrawPos, 26);
                    else DrawHex2(PAL0, value - 100, stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_UP1), stepDrawPos, 27);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_UP2), stepDrawPos+1, 27);
                }
                else
                {
                    if (i%4==0) DrawHex2(PAL3, 100 - value, stepDrawPos, 26);
                    else DrawHex2(PAL0, 100 - value, stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_DOWN1), stepDrawPos, 27);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_DOWN2), stepDrawPos+1, 27);
                }
            }
            break;
        case GUI_INST_PARAM_NAME: case 236:
            for (u8 i = 0; i < 8; i++)
            {
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_ALPHABET[ReadInstrumentSRAM(selectedInstrumentID, INST_NAME_1 + i)]), GUI_INST_NAME_START + i, 0);
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
        case GUI_INST_PARAM_COPY:
            DrawHex2(PAL0, instCopyTo, GUI_INST_NAME_START, 1); // same x position as NAME
            break;
        }

        instrumentParameterToRefresh--; // refresh one parameter by frame to avoid song freeze
        if (instrumentParameterToRefresh < 235) bRefreshScreen = FALSE; // put last case here; redraw only changed parameter
    }
}
//! slow
static void SetChannelVolume(u8 matrixChannel)
{
    static s16 volT = 0, volT1 = 0, volT2 = 0, volT3 =0 , volT4 = 0; // volume, tremolo

    if (matrixChannel > 8) // PSG
    {
        if (bPsgIsPlayingNote[matrixChannel - CHANNEL_PSG1] == TRUE)
        {
            volT =// channelBaseVolume[matrixChannel] +
            channelAttenuation[matrixChannel] / 8 +
            channelSeqAttenuation[matrixChannel] / 8 +
            channelTremolo[matrixChannel];

            if (volT > PSG_ENVELOPE_MIN) volT = PSG_ENVELOPE_MIN;
            //else if (volT < PSG_ENVELOPE_MAX) volT = PSG_ENVELOPE_MAX;

            PSG_setEnvelope(matrixChannel - CHANNEL_PSG1, (u8)volT);
        }
    }
    else // FM
    {
        static u8 port = 0;
        static u8 fmChannel = 0;

        auto void set_normal_channel_vol()
        {
            switch (tmpInst[channelPreviousInstrument[matrixChannel]].ALG)
            {
            case 0: case 1: case 2: case 3:
                volT4 =
                    channelSlotBaseLevel[matrixChannel][3] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT4 > 0x7F) volT4 = 0x7F;
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + fmChannel, (u8)volT4);
                break;
            case 4:
                volT3 =
                    channelSlotBaseLevel[matrixChannel][2] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT3 > 0x7F) volT3 = 0x7F;

                volT4 =
                    channelSlotBaseLevel[matrixChannel][3] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT4 > 0x7F) volT4 = 0x7F;
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + fmChannel, (u8)volT3);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + fmChannel, (u8)volT4);
                break;
            case 5: case 6:
                volT2 =
                    channelSlotBaseLevel[matrixChannel][1] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT2 > 0x7F) volT2 = 0x7F;

                volT3 =
                    channelSlotBaseLevel[matrixChannel][2] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT3 > 0x7F) volT3 = 0x7F;

                volT4 =
                    channelSlotBaseLevel[matrixChannel][3] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT4 > 0x7F) volT4 = 0x7F;

                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + fmChannel, (u8)volT2);
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + fmChannel, (u8)volT3);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + fmChannel, (u8)volT4);
                break;
            case 7:
                volT1 =
                    channelSlotBaseLevel[matrixChannel][0] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT1 > 0x7F) volT1 = 0x7F;

                volT2 =
                    channelSlotBaseLevel[matrixChannel][1] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT2 > 0x7F) volT2 = 0x7F;

                volT3 =
                    channelSlotBaseLevel[matrixChannel][2] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT3 > 0x7F) volT3 = 0x7F;

                volT4 =
                    channelSlotBaseLevel[matrixChannel][3] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT4 > 0x7F) volT4 = 0x7F;

                YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + fmChannel, (u8)volT1);
                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + fmChannel, (u8)volT2);
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + fmChannel, (u8)volT3);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + fmChannel, (u8)volT4);
                break;
            }
        }

        auto void set_special_channel_vol()
        {
            switch (matrixChannel)
            {
            case CHANNEL_FM3_OP4:
                volT4 =
                    channelSlotBaseLevel[matrixChannel][3] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT4 > 0x7F) volT4 = 0x7F;
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + fmChannel, (u8)volT4);
                break;
            case CHANNEL_FM3_OP3:
                volT3 =
                    channelSlotBaseLevel[matrixChannel][2] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT3 > 0x7F) volT3 = 0x7F;
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + fmChannel, (u8)volT3);
                break;
            case CHANNEL_FM3_OP2:
                volT2 =
                    channelSlotBaseLevel[matrixChannel][1] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT2 > 0x7F) volT2 = 0x7F;
                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + fmChannel, (u8)volT2);
                break;
            case CHANNEL_FM3_OP1:
                volT1 =
                    channelSlotBaseLevel[matrixChannel][0] +
                    channelAttenuation[matrixChannel] +
                    channelSeqAttenuation[matrixChannel] +
                    channelTremolo[matrixChannel];
                if (volT1 > 0x7F) volT1 = 0x7F;
                YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + fmChannel, (u8)volT1);
                break;
            }
        }

        switch (matrixChannel)
        {
        case CHANNEL_FM1: case CHANNEL_FM2:
            port = PORT_1; fmChannel = matrixChannel;
            set_normal_channel_vol();
            break;
        case CHANNEL_FM3_OP4:
            port = PORT_1; fmChannel = 2;
            if (ch3Mode == CH3_NORMAL) { set_normal_channel_vol(); }
            else { set_special_channel_vol(); }
            break;
        case CHANNEL_FM3_OP3: case CHANNEL_FM3_OP2: case CHANNEL_FM3_OP1:
            if (ch3Mode == CH3_SPECIAL) { port = PORT_1; fmChannel = 2; set_special_channel_vol();  }
            break;
        case CHANNEL_FM4: case CHANNEL_FM5: case CHANNEL_FM6_DAC:
            port = PORT_2; fmChannel = matrixChannel - 6;
            set_normal_channel_vol();
            break;
        }
    }
}

static void RequestZ80()
{
    bBusTaken = Z80_isBusTaken();
    if (!bBusTaken) Z80_requestBus(TRUE);
}

static void ReleaseZ80()
{
    bBusTaken = Z80_isBusTaken();
    if (bDAC_enable) YM2612_write(PORT_1, YM2612REG_DAC); // needed for DAC
    if (bBusTaken) Z80_releaseBus();
}

static void SetPitchPSG(u8 matrixChannel, u8 note)
{
    static s8 key = 0;

    key = note + channelModNotePitch[matrixChannel] + channelModNoteVibrato[matrixChannel];

    if (key < PSG_LOWEST_NOTE) { key = PSG_LOWEST_NOTE;
        channelPitchSlideSpeed[matrixChannel] = 0; }
    else if (key > MAX_NOTE) { key = MAX_NOTE;
        channelPitchSlideSpeed[matrixChannel] = 0; }

    auto void setvol()
    {
        channelBaseVolume[matrixChannel] = PSG_ENVELOPE_MAX;
        if (channelVolSeqID[matrixChannel] == 0)
        {
            channelSeqAttenuation[matrixChannel] = 0;
        }
        else
        {
            channelSeqAttenuation[matrixChannel] = ReadInstrumentSRAM(channelVolSeqID[matrixChannel], INST_VOL_TICK_01 + frameCounter);
        }
        SetChannelVolume(matrixChannel);
    }

    switch (matrixChannel)
    {
    case CHANNEL_PSG1: case CHANNEL_PSG2:
        setvol();
        PSG_setTone(matrixChannel - 9, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[matrixChannel] / 2]);
        break;
    case CHANNEL_PSG3:
        switch (psg_noise_mode)
        {
        case PSG_TONAL_CH3_MUTED:
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN); // mute PSG3 channel
            break;
        case PSG_TONAL_CH3_NOT_MUTED: case PSG_FIXED:
            setvol();
            PSG_setTone(2, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[matrixChannel] / 2]); // write tone to PSG3 to supply PSG4 tonal noise
            break;
        }
        break;
    case CHANNEL_PSG4_NOISE:
        switch (psg_noise_mode)
        {
        case PSG_TONAL_CH3_MUTED:
            setvol();
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN); // mute PSG3 channel
            PSG_setTone(2, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[matrixChannel] / 2]); // write tone to PSG3 to supply PSG4 tonal noise
            break;
        case PSG_TONAL_CH3_NOT_MUTED:
            setvol();
            PSG_setTone(2, psgNoteMicrotone[(u8)key][(u8)channelFinalPitch[matrixChannel] / 2]); // write tone to PSG3 to supply PSG4 tonal noise
            break;
         case PSG_FIXED:
            setvol();
            break;
        }
        break;
    }
}

// DAC is also here
static void SetPitchFM(u8 matrixChannel, u8 note)
{
    static u8 part1 = 0, part2 = 0, noteFreqID = 0;
    static s8 key = 0;

    auto void csm_pitch() // bus requested later
    {
        // Timer A to note pitch
        YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_MSB, csmMicrotone[note] >> 2);
        YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_LSB, csmMicrotone[note] & 0b0000000000000011);

        // play CSM note
        YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_FREQ_MSB, 0b10001111);
        //YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_FREQ_MSB, CH3_SPECIAL_CSM | 0b00001111); // bb Ch3 mode, Reset B, Reset A, Enable B, Enable A, Load B, Load A
        ch3Mode = CH3_SPECIAL_CSM;
    }

    // CSM
    if ((matrixChannel == CHANNEL_FM3_OP4) && (ch3Mode == CH3_SPECIAL_CSM || ch3Mode == CH3_SPECIAL_CSM_OFF))
    {
       key = ch3OpFreq[0] + channelModNotePitch[matrixChannel] + channelModNoteVibrato[matrixChannel];
    }
    // Normal or Special
    else
    {
        key = note + channelModNotePitch[matrixChannel] + channelModNoteVibrato[matrixChannel];
    }

    if (key >= 0 && key <= MAX_NOTE)
    {
        noteFreqID = key;
        while (noteFreqID > 11) noteFreqID -= 12;

        part1 = ((key / 12) << 3) | (noteMicrotone[noteFreqID][(u8)channelFinalPitch[matrixChannel]] >> 8);
        part2 = 0b0000000011111111 & noteMicrotone[noteFreqID][(u8)channelFinalPitch[matrixChannel]];

        switch (matrixChannel)
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

            switch (ch3Mode)
            {
            case CH3_NORMAL:
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110010); // 2
                break;
            case CH3_SPECIAL:
                BIT_SET(ch3OpNoteStatus, 7); // 0b1???0010
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus);// 2
                break;
            case CH3_SPECIAL_CSM: case CH3_SPECIAL_CSM_OFF:
                csm_pitch();
                break;
            default:
                break;
            }
            break;
        case CHANNEL_FM3_OP3:
            if (ch3Mode == CH3_SPECIAL)
            {
                BIT_SET(ch3OpNoteStatus, 5); // 0b??1?0010
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP3_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP3_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // 2
            }
            break;
        case CHANNEL_FM3_OP2:
            if (ch3Mode == CH3_SPECIAL)
            {
                BIT_SET(ch3OpNoteStatus, 6); // 0b?1??0010
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP2_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP2_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // 2
            }
            break;
        case CHANNEL_FM3_OP1:
            if (ch3Mode == CH3_SPECIAL)
            {
                BIT_SET(ch3OpNoteStatus, 4); // 0b???10010
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP1_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP1_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // 2
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
                static u32 sampleStart = 0, sampleEnd = 0;

                sampleStart =
                (ReadSampleRegionSRAM(activeSampleBank, note, 0) << 16) |
                (ReadSampleRegionSRAM(activeSampleBank, note, 1) << 8) |
                ReadSampleRegionSRAM(activeSampleBank, note, 2);

                sampleEnd =
                (ReadSampleRegionSRAM(activeSampleBank, note, 3) << 16) |
                (ReadSampleRegionSRAM(activeSampleBank, note, 4) << 8) |
                ReadSampleRegionSRAM(activeSampleBank, note, 5);

                SND_startPlay_PCM(sample_bank_1 + sampleStart,
                                  (sampleBankSize - sampleStart) - (sampleBankSize - sampleEnd),
                                  ReadSampleRegionSRAM(activeSampleBank, note, 7),
                                  dacPan,
                                  ReadSampleRegionSRAM(activeSampleBank, note, 6));
            }
            else
            {
                YM2612_writeRegZ80(PORT_2, YM2612REG_CH3_FREQ_MSB, part1);
                YM2612_writeRegZ80(PORT_2, YM2612REG_CH3_FREQ_LSB, part2);
                YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0b11110110); // 6
            }
            break;
        }
    }
    else
    {
        channelPitchSlideSpeed[matrixChannel]= 0;
    }
}

static void PlayNote(u8 note, u8 matrixChannel)
{
    if (note <= MAX_NOTE)
    {
        channelVibratoPhase[matrixChannel] = 0; // neutral state
        channelTremoloPhase[matrixChannel] = 512; // neutral state

        if (matrixChannel < CHANNEL_PSG1) // FM
        {
            // S1>S3>S2>S4 for common registers and S4>S3>S1>S2 for CH3 frequencies
            StopChannelSound(matrixChannel); // need to stop current playing note to write new data; OFF needed also for CSM
            SetPitchFM(matrixChannel, note);
        }
        else // PSG
        {
            bPsgIsPlayingNote[matrixChannel - CHANNEL_PSG1] = TRUE;
            SetPitchPSG(matrixChannel, note);
        }
    }
    else if (note == NOTE_OFF)
    {
        StopChannelSound(matrixChannel);
        StopEffects(matrixChannel);
    }
}

static void StopEffects(u8 matrixChannel)
{
    channelNoteCut[matrixChannel] = 0;
    channelNoteRetrigger[matrixChannel] = 0;
    channelNoteRetriggerCounter[matrixChannel] = 0;
    channelNoteDelay[matrixChannel] = 0;

    channelTremoloSpeed[matrixChannel] = 0;
        channelTremoloSpeedMult[matrixChannel] = 0x20;
    channelTremoloDepth[matrixChannel] = 0;
    channelTremoloPhase[matrixChannel] = 512;

    channelVibratoMode[matrixChannel] = 0;
    channelVibratoSpeed[matrixChannel] = 0;
        channelVibratoSpeedMult[matrixChannel] = 0x08;
    channelVibratoDepth[matrixChannel] = 0;
        channelVibratoDepthMult[matrixChannel] = 0x02;
    channelVibratoPhase[matrixChannel] = 0;
    channelPitchSlideSpeed[matrixChannel] = 0;
    channelFinalPitch[matrixChannel] = 0;

    channelModNoteVibrato[matrixChannel] = 0;
    channelModNotePitch[matrixChannel] = 0;
    channelMicrotone[matrixChannel] = 0;

    channelArpSeqID[matrixChannel] = 0;
    channelVolSeqID[matrixChannel] = 0;

    channelPreviousNote[matrixChannel] = NOTE_OFF;

    matrixRowJumpTo = EVALUATE_0xFF;
    patternRowJumpTo = EVALUATE_0xFF;
}

// stopping sound on matrix channel
static void StopChannelSound(u8 matrixChannel)
{
    switch (matrixChannel)
    {
    case CHANNEL_FM1:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 0);
        break;

    case CHANNEL_FM2:
        YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 1);
        break;

    case CHANNEL_FM3_OP4:
        if (ch3Mode == CH3_NORMAL)
        {
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 2); // all OP Off
        }
        else if (ch3Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(ch3OpNoteStatus, 7);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // OP4
        }
        else if (ch3Mode == CH3_SPECIAL_CSM || ch3Mode == CH3_SPECIAL_CSM_OFF)
        {
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, 2); // set operators key off for CSM to work
            YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_SPECIAL_CSM_OFF | 0b00001111);
            ch3Mode = CH3_SPECIAL_CSM_OFF;
        }
        break;

    case CHANNEL_FM3_OP3:
        if (ch3Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(ch3OpNoteStatus, 5);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // OP3
        }
        break;

    case CHANNEL_FM3_OP2:
        if (ch3Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(ch3OpNoteStatus, 6);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // OP2
        }
        break;

    case CHANNEL_FM3_OP1:
        if (ch3Mode == CH3_SPECIAL)
        {
            BIT_CLEAR(ch3OpNoteStatus, 4);
            YM2612_writeRegZ80(PORT_1, YM2612REG_KEY, ch3OpNoteStatus); // OP1
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

static void StopAllSound()
{
    for (u8 matrixChannel = CHANNEL_FM1; matrixChannel < CHANNELS_TOTAL; matrixChannel++)
    {
        StopChannelSound(matrixChannel);
        StopEffects(matrixChannel);

        // only at playback stop, so note OFF is not affected
        channelAttenuation[matrixChannel] = 0;
        channelVolumeChangeSpeed[matrixChannel] = 0;
    }
}

static void SetGlobalLFO(u8 freq)
{
    YM2612_writeRegZ80(PORT_1, YM2612REG_GLOBAL_LFO, freq);
}

// cache instrument
static void CacheIstrumentToRAM(u8 id)
{
    tmpInst[id].ALG = ReadInstrumentSRAM(id, INST_ALG);
    tmpInst[id].AMS = ReadInstrumentSRAM(id, INST_FMS);
    tmpInst[id].FMS = ReadInstrumentSRAM(id, INST_AMS);
    tmpInst[id].PAN = ReadInstrumentSRAM(id, INST_PAN);
    tmpInst[id].FB = ReadInstrumentSRAM(id, INST_FB);

    tmpInst[id].TL1 = ReadInstrumentSRAM(id, INST_TL1);
    tmpInst[id].TL2 = ReadInstrumentSRAM(id, INST_TL2);
    tmpInst[id].TL3 = ReadInstrumentSRAM(id, INST_TL3);
    tmpInst[id].TL4 = ReadInstrumentSRAM(id, INST_TL4);

    tmpInst[id].RS1 = ReadInstrumentSRAM(id, INST_RS1);
    tmpInst[id].RS2 = ReadInstrumentSRAM(id, INST_RS2);
    tmpInst[id].RS3 = ReadInstrumentSRAM(id, INST_RS3);
    tmpInst[id].RS4 = ReadInstrumentSRAM(id, INST_RS4);

    tmpInst[id].MUL1 = ReadInstrumentSRAM(id, INST_MUL1);
    tmpInst[id].MUL2 = ReadInstrumentSRAM(id, INST_MUL2);
    tmpInst[id].MUL3 = ReadInstrumentSRAM(id, INST_MUL3);
    tmpInst[id].MUL4 = ReadInstrumentSRAM(id, INST_MUL4);

    tmpInst[id].DT1 = ReadInstrumentSRAM(id, INST_DT1);
    tmpInst[id].DT2 = ReadInstrumentSRAM(id, INST_DT2);
    tmpInst[id].DT3 = ReadInstrumentSRAM(id, INST_DT3);
    tmpInst[id].DT4 = ReadInstrumentSRAM(id, INST_DT4);

    tmpInst[id].AR1 = ReadInstrumentSRAM(id, INST_AR1);
    tmpInst[id].AR2 = ReadInstrumentSRAM(id, INST_AR2);
    tmpInst[id].AR3 = ReadInstrumentSRAM(id, INST_AR3);
    tmpInst[id].AR4 = ReadInstrumentSRAM(id, INST_AR4);

    tmpInst[id].D1R1 = ReadInstrumentSRAM(id, INST_D1R1);
    tmpInst[id].D1R2 = ReadInstrumentSRAM(id, INST_D1R2);
    tmpInst[id].D1R3 = ReadInstrumentSRAM(id, INST_D1R3);
    tmpInst[id].D1R4 = ReadInstrumentSRAM(id, INST_D1R4);

    tmpInst[id].D1L1 = ReadInstrumentSRAM(id, INST_D1L1);
    tmpInst[id].D1L2 = ReadInstrumentSRAM(id, INST_D1L2);
    tmpInst[id].D1L3 = ReadInstrumentSRAM(id, INST_D1L3);
    tmpInst[id].D1L4 = ReadInstrumentSRAM(id, INST_D1L4);

    tmpInst[id].D2R1 = ReadInstrumentSRAM(id, INST_D2R1);
    tmpInst[id].D2R2 = ReadInstrumentSRAM(id, INST_D2R2);
    tmpInst[id].D2R3 = ReadInstrumentSRAM(id, INST_D2R3);
    tmpInst[id].D2R4 = ReadInstrumentSRAM(id, INST_D2R4);

    tmpInst[id].RR1 = ReadInstrumentSRAM(id, INST_RR1);
    tmpInst[id].RR2 = ReadInstrumentSRAM(id, INST_RR2);
    tmpInst[id].RR3 = ReadInstrumentSRAM(id, INST_RR3);
    tmpInst[id].RR4 = ReadInstrumentSRAM(id, INST_RR4);

    tmpInst[id].AM1 = ReadInstrumentSRAM(id, INST_AM1);
    tmpInst[id].AM2 = ReadInstrumentSRAM(id, INST_AM2);
    tmpInst[id].AM3 = ReadInstrumentSRAM(id, INST_AM3);
    tmpInst[id].AM4 = ReadInstrumentSRAM(id, INST_AM4);

    tmpInst[id].SSGEG1 = ReadInstrumentSRAM(id, INST_SSGEG1);
    tmpInst[id].SSGEG2 = ReadInstrumentSRAM(id, INST_SSGEG2);
    tmpInst[id].SSGEG3 = ReadInstrumentSRAM(id, INST_SSGEG3);
    tmpInst[id].SSGEG4 = ReadInstrumentSRAM(id, INST_SSGEG4);

    // calculate YM2612 combined registers from module data
    CalculateCombined(id, COMB_FB_ALG);
    CalculateCombined(id, COMB_PAN_AMS_FMS);

    CalculateCombined(id, COMB_DT_MUL_1);
    CalculateCombined(id, COMB_DT_MUL_2);
    CalculateCombined(id, COMB_DT_MUL_3);
    CalculateCombined(id, COMB_DT_MUL_4);

    CalculateCombined(id, COMB_RS_AR_1);
    CalculateCombined(id, COMB_RS_AR_2);
    CalculateCombined(id, COMB_RS_AR_3);
    CalculateCombined(id, COMB_RS_AR_4);

    CalculateCombined(id, COMB_AM_D1R_1);
    CalculateCombined(id, COMB_AM_D1R_2);
    CalculateCombined(id, COMB_AM_D1R_3);
    CalculateCombined(id, COMB_AM_D1R_4);

    CalculateCombined(id, COMB_D1L_RR_1);
    CalculateCombined(id, COMB_D1L_RR_2);
    CalculateCombined(id, COMB_D1L_RR_3);
    CalculateCombined(id, COMB_D1L_RR_4);
}

void CalculateCombined(u8 id, u8 reg)
{
    switch (reg)
    {
        case COMB_FB_ALG:        tmpInst[id].FB_ALG = (tmpInst[id].FB << 3) | tmpInst[id].ALG; break;
        case COMB_PAN_AMS_FMS:   tmpInst[id].PAN_AMS_FMS = ((tmpInst[id].PAN << 6) | (tmpInst[id].AMS << 3)) | tmpInst[id].FMS; break;

        case COMB_DT_MUL_1:      tmpInst[id].DT1_MUL1 = (tmpInst[id].DT1 << 4) | tmpInst[id].MUL1; break;
        case COMB_DT_MUL_2:      tmpInst[id].DT2_MUL2 = (tmpInst[id].DT2 << 4) | tmpInst[id].MUL2; break;
        case COMB_DT_MUL_3:      tmpInst[id].DT3_MUL3 = (tmpInst[id].DT3 << 4) | tmpInst[id].MUL3; break;
        case COMB_DT_MUL_4:      tmpInst[id].DT4_MUL4 = (tmpInst[id].DT4 << 4) | tmpInst[id].MUL4; break;

        case COMB_RS_AR_1:       tmpInst[id].RS1_AR1 = (tmpInst[id].RS1 << 6) | tmpInst[id].AR1; break;
        case COMB_RS_AR_2:       tmpInst[id].RS2_AR2 = (tmpInst[id].RS2 << 6) | tmpInst[id].AR2; break;
        case COMB_RS_AR_3:       tmpInst[id].RS3_AR3 = (tmpInst[id].RS3 << 6) | tmpInst[id].AR3; break;
        case COMB_RS_AR_4:       tmpInst[id].RS4_AR4 = (tmpInst[id].RS4 << 6) | tmpInst[id].AR4; break;

        case COMB_AM_D1R_1:      tmpInst[id].AM1_D1R1 = (tmpInst[id].AM1 << 7) | tmpInst[id].D1R1; break;
        case COMB_AM_D1R_2:      tmpInst[id].AM2_D1R2 = (tmpInst[id].AM2 << 7) | tmpInst[id].D1R2; break;
        case COMB_AM_D1R_3:      tmpInst[id].AM3_D1R3 = (tmpInst[id].AM3 << 7) | tmpInst[id].D1R3; break;
        case COMB_AM_D1R_4:      tmpInst[id].AM4_D1R4 = (tmpInst[id].AM4 << 7) | tmpInst[id].D1R4; break;

        case COMB_D1L_RR_1:      tmpInst[id].D1L1_RR1 = (tmpInst[id].D1L1 << 4) | tmpInst[id].RR1; break;
        case COMB_D1L_RR_2:      tmpInst[id].D1L2_RR2 = (tmpInst[id].D1L2 << 4) | tmpInst[id].RR2; break;
        case COMB_D1L_RR_3:      tmpInst[id].D1L3_RR3 = (tmpInst[id].D1L3 << 4) | tmpInst[id].RR3; break;
        case COMB_D1L_RR_4:      tmpInst[id].D1L4_RR4 = (tmpInst[id].D1L4 << 4) | tmpInst[id].RR4; break;
    }
}

static void SetChannelBaseVolume(u8 matrixChannel, u8 id)
{
    auto void set_normal_slots()
    {
        if (matrixChannel < CHANNEL_PSG1) // FM only
        {
            switch (tmpInst[id].ALG)
            {
            case 0: case 1: case 2: case 3:
                channelSlotBaseLevel[matrixChannel][3] = tmpInst[id].TL4;
                tmpInst[id].TL4 += channelAttenuation[matrixChannel];
                if (tmpInst[id].TL4 > 0x7F) tmpInst[id].TL4 = 0x7F;
                break;
            case 4:
                channelSlotBaseLevel[matrixChannel][2] = tmpInst[id].TL3;
                channelSlotBaseLevel[matrixChannel][3] = tmpInst[id].TL4;
                tmpInst[id].TL3 += channelAttenuation[matrixChannel];
                tmpInst[id].TL4 += channelAttenuation[matrixChannel];
                if (tmpInst[id].TL3 > 0x7F) tmpInst[id].TL3 = 0x7F;
                if (tmpInst[id].TL4 > 0x7F) tmpInst[id].TL4 = 0x7F;
                break;
            case 5: case 6:
                channelSlotBaseLevel[matrixChannel][1] = tmpInst[id].TL2;
                channelSlotBaseLevel[matrixChannel][2] = tmpInst[id].TL3;
                channelSlotBaseLevel[matrixChannel][3] = tmpInst[id].TL4;
                tmpInst[id].TL2 += channelAttenuation[matrixChannel];
                tmpInst[id].TL3 += channelAttenuation[matrixChannel];
                tmpInst[id].TL4 += channelAttenuation[matrixChannel];
                if (tmpInst[id].TL2 > 0x7F) tmpInst[id].TL2 = 0x7F;
                if (tmpInst[id].TL3 > 0x7F) tmpInst[id].TL3 = 0x7F;
                if (tmpInst[id].TL4 > 0x7F) tmpInst[id].TL4 = 0x7F;
                break;
            case 7:
                channelSlotBaseLevel[matrixChannel][0] = tmpInst[id].TL1;
                channelSlotBaseLevel[matrixChannel][1] = tmpInst[id].TL2;
                channelSlotBaseLevel[matrixChannel][2] = tmpInst[id].TL3;
                channelSlotBaseLevel[matrixChannel][3] = tmpInst[id].TL4;
                tmpInst[id].TL1 += channelAttenuation[matrixChannel];
                tmpInst[id].TL2 += channelAttenuation[matrixChannel];
                tmpInst[id].TL3 += channelAttenuation[matrixChannel];
                tmpInst[id].TL4 += channelAttenuation[matrixChannel];
                if (tmpInst[id].TL1 > 0x7F) tmpInst[id].TL1 = 0x7F;
                if (tmpInst[id].TL2 > 0x7F) tmpInst[id].TL2 = 0x7F;
                if (tmpInst[id].TL3 > 0x7F) tmpInst[id].TL3 = 0x7F;
                if (tmpInst[id].TL4 > 0x7F) tmpInst[id].TL4 = 0x7F;
                break;
            }
        }
    }

    switch (matrixChannel)
    {
    case CHANNEL_FM3_OP4:
        if (ch3Mode == CH3_NORMAL) set_normal_slots();
        else
        {
            channelSlotBaseLevel[matrixChannel][3] = tmpInst[id].TL4;
            tmpInst[id].TL4 += channelAttenuation[matrixChannel];
            if (tmpInst[id].TL4 > 0x7F) tmpInst[id].TL1 = 0x7F;
        }
        break;
    case CHANNEL_FM3_OP3:
        if (ch3Mode != CH3_NORMAL)
        {
            channelSlotBaseLevel[matrixChannel][2] = tmpInst[id].TL3;
            tmpInst[id].TL3 += channelAttenuation[matrixChannel];
            if (tmpInst[id].TL3 > 0x7F) tmpInst[id].TL1 = 0x7F;
        }
        break;
    case CHANNEL_FM3_OP2:
        if (ch3Mode != CH3_NORMAL)
        {
            channelSlotBaseLevel[matrixChannel][1] = tmpInst[id].TL2;
            tmpInst[id].TL2 += channelAttenuation[matrixChannel];
            if (tmpInst[id].TL2 > 0x7F) tmpInst[id].TL1 = 0x7F;
        }
        break;
    case CHANNEL_FM3_OP1:
        if (ch3Mode != CH3_NORMAL)
        {
            channelSlotBaseLevel[matrixChannel][0] = tmpInst[id].TL1;
            tmpInst[id].TL1 += channelAttenuation[matrixChannel];
            if (tmpInst[id].TL1 > 0x7F) tmpInst[id].TL1 = 0x7F;
        }
        break;
    default:
        set_normal_slots();
        break;
    }
    SetChannelVolume(matrixChannel); // if there is vol command, resulting same values
}

// write all YM2612 registers
static void WriteYM2612(u8 matrixChannel, u8 id)
{
    static u16 port = 0;
    static u8 fmChannel = 0;

    if (matrixChannel < CHANNEL_PSG1) // FM channel
    {
        switch (matrixChannel)
        {
        case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM3_OP4:
            port = PORT_1; fmChannel = matrixChannel; // 0, 1, 2
            break;
        case CHANNEL_FM3_OP3: case CHANNEL_FM3_OP2: case CHANNEL_FM3_OP1:
            port = PORT_1; fmChannel = CHANNEL_FM3_OP4; // 2
            break;
        case CHANNEL_FM4: case CHANNEL_FM5: case CHANNEL_FM6_DAC:
            port = PORT_2; fmChannel = matrixChannel - 6; // 0, 1, 2
            break;
        }
        SetChannelBaseVolume(matrixChannel, id); //! SLOW!

        switch (fmChannel)
        {
        case 0:
            YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0, tmpInst[id].FB_ALG);

            YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0, tmpInst[id].TL1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0, tmpInst[id].TL2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0, tmpInst[id].TL3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0, tmpInst[id].TL4);

            YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0, tmpInst[id].PAN_AMS_FMS);

            YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0, tmpInst[id].DT1_MUL1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH0, tmpInst[id].DT2_MUL2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH0, tmpInst[id].DT3_MUL3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH0, tmpInst[id].DT4_MUL4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0, tmpInst[id].RS1_AR1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH0, tmpInst[id].RS2_AR2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH0, tmpInst[id].RS3_AR3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH0, tmpInst[id].RS4_AR4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0, tmpInst[id].AM1_D1R1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH0, tmpInst[id].AM2_D1R2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH0, tmpInst[id].AM3_D1R3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH0, tmpInst[id].AM4_D1R4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH0, tmpInst[id].D2R1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH0, tmpInst[id].D2R2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH0, tmpInst[id].D2R3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH0, tmpInst[id].D2R4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0, tmpInst[id].D1L1_RR1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH0, tmpInst[id].D1L2_RR2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH0, tmpInst[id].D1L3_RR3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH0, tmpInst[id].D1L4_RR4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH0, tmpInst[id].SSGEG1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH0, tmpInst[id].SSGEG2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH0, tmpInst[id].SSGEG3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH0, tmpInst[id].SSGEG4);
            break;
        case 1:
            YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH1, tmpInst[id].FB_ALG);

            YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH1, tmpInst[id].TL1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH1, tmpInst[id].TL2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH1, tmpInst[id].TL3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH1, tmpInst[id].TL4);

            YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH1, tmpInst[id].PAN_AMS_FMS);

            YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH1, tmpInst[id].DT1_MUL1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH1, tmpInst[id].DT2_MUL2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH1, tmpInst[id].DT3_MUL3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH1, tmpInst[id].DT4_MUL4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH1, tmpInst[id].RS1_AR1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH1, tmpInst[id].RS2_AR2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH1, tmpInst[id].RS3_AR3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH1, tmpInst[id].RS4_AR4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH1, tmpInst[id].AM1_D1R1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH1, tmpInst[id].AM2_D1R2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH1, tmpInst[id].AM3_D1R3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH1, tmpInst[id].AM4_D1R4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH1, tmpInst[id].D2R1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH1, tmpInst[id].D2R2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH1, tmpInst[id].D2R3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH1, tmpInst[id].D2R4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH1, tmpInst[id].D1L1_RR1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH1, tmpInst[id].D1L2_RR2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH1, tmpInst[id].D1L3_RR3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH1, tmpInst[id].D1L4_RR4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH1, tmpInst[id].SSGEG1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH1, tmpInst[id].SSGEG2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH1, tmpInst[id].SSGEG3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH1, tmpInst[id].SSGEG4);
            break;
        case 2:
            YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH2, tmpInst[id].FB_ALG);

            YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH2, tmpInst[id].TL1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH2, tmpInst[id].TL2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH2, tmpInst[id].TL3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH2, tmpInst[id].TL4);

            YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH2, tmpInst[id].PAN_AMS_FMS);

            YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH2, tmpInst[id].DT1_MUL1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH2, tmpInst[id].DT2_MUL2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH2, tmpInst[id].DT3_MUL3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH2, tmpInst[id].DT4_MUL4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH2, tmpInst[id].RS1_AR1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH2, tmpInst[id].RS2_AR2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH2, tmpInst[id].RS3_AR3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH2, tmpInst[id].RS4_AR4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH2, tmpInst[id].AM1_D1R1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH2, tmpInst[id].AM2_D1R2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH2, tmpInst[id].AM3_D1R3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH2, tmpInst[id].AM4_D1R4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH2, tmpInst[id].D2R1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH2, tmpInst[id].D2R2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH2, tmpInst[id].D2R3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH2, tmpInst[id].D2R4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH2, tmpInst[id].D1L1_RR1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH2, tmpInst[id].D1L2_RR2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH2, tmpInst[id].D1L3_RR3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH2, tmpInst[id].D1L4_RR4);

            YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH2, tmpInst[id].SSGEG1);
            YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH2, tmpInst[id].SSGEG2);
            YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH2, tmpInst[id].SSGEG3);
            YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH2, tmpInst[id].SSGEG4);
            break;
        }
    }
}

static void ApplyCommand_Common(u8 matrixChannel, u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // TEMPO
    case 0x13:
        if (fxValue == 0) SetBPM(SRAMW_readWord(TEMPO));
        else SetBPM(fxValue);
        break;

    // TICKS PER LINE
    case 0x14:
        ticksPerOddRow = 1 + ((fxValue & 0b11110000) >> 4);
        ticksPerEvenRow = 1 + (fxValue & 0b00001111);
        break;

    // PCM SAMPLE BANK SET
    case 0x16:
        if (fxValue < 4) activeSampleBank = fxValue;
        break;

    // MSU MD CD audio PLAY ONCE
    case 0x20:
        if (fxValue == 0)
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
        if (fxValue == 0)
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
        if (fxValue == 0)
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

    // ------------------------------------------------------------------------

    // ARP SEQUENCE MODE
    case 0x2F:
        if (fxValue == 0) channelArpSeqMODE[matrixChannel] = 0; else channelArpSeqMODE[matrixChannel] = 1;
        break;

    // ARP SEQUENCE
    case 0x30:
        channelArpSeqID[matrixChannel] = fxValue;
        break;

    // PITCH SLIDE UP
    case 0x31:
        switch (fxValue)
        {
            case 0x00: // stop and reset
                channelPitchSlideSpeed[matrixChannel] = 0;
                channelMicrotone[matrixChannel] = 0;
                channelModNotePitch[matrixChannel] = 0;
                break;
            case 0xFE: // stop
                channelPitchSlideSpeed[matrixChannel] = 0;
                break;
            case 0xFF: // reset
                channelMicrotone[matrixChannel] = 0;
                channelModNotePitch[matrixChannel] = 0;
                break;
            default: // do portamento
                if (fxValue < 0x80) channelPitchSlideSpeed[matrixChannel] = fxValue;
                break;
        }
        break;

    // PITCH SLIDE DOWN
    case 0x32:
        switch (fxValue)
        {
            case 0x00: // stop and reset
                channelPitchSlideSpeed[matrixChannel] = 0;
                channelMicrotone[matrixChannel] = 0;
                channelModNotePitch[matrixChannel] = 0;
                break;
            case 0xFE: // stop
                channelPitchSlideSpeed[matrixChannel] = 0;
                break;
            case 0xFF: // reset
                channelMicrotone[matrixChannel] = 0;
                channelModNotePitch[matrixChannel] = 0;
                break;
            default: // do portamento
                if (fxValue < 0x80) channelPitchSlideSpeed[matrixChannel] = -fxValue;
                break;
        }
        break;

    // VIBRATO
    case 0x33:
        channelVibratoSpeed[matrixChannel] = ((fxValue & 0b11110000) >> 4) * channelVibratoSpeedMult[matrixChannel];
        channelVibratoDepth[matrixChannel] = (fxValue & 0b00001111) * channelVibratoDepthMult[matrixChannel];
        channelVibratoPhase[matrixChannel] = 0;
        channelModNoteVibrato[matrixChannel] = 0;
        break;

    // VIBRATO SPEED MULT
    case 0x34:
        if (fxValue > 0)
        {
            channelVibratoSpeedMult[matrixChannel] = fxValue;
        }
        else
        {
            channelVibratoSpeedMult[matrixChannel] = 0x08;
        }
        channelVibratoSpeed[matrixChannel] = ((fxValue & 0b11110000) >> 4) * channelVibratoSpeedMult[matrixChannel];
        break;

    // VIBRATO DEPTH MULT
    case 0x35:
        if (fxValue > 0)
        {
            channelVibratoDepthMult[matrixChannel] = fxValue;
        }
        else
        {
            channelVibratoDepthMult[matrixChannel] = 0x02;
        }
        channelVibratoDepth[matrixChannel] = (fxValue & 0b00001111) * channelVibratoDepthMult[matrixChannel];
        break;

    // VIBRATO MODE
    case 0x36:
        if (fxValue < 3) channelVibratoMode[matrixChannel] = fxValue;
        break;

    // PORTAMENTO SKIP TICKS
    case 0x37:
        channelPitchSkipStep[matrixChannel] = fxValue;
        break;

    // ------------------------------------------------------------------------

    // VOLUME SEQUENCE MODE
    case 0x3F:
        if (fxValue == 0) channelVolSeqMODE[matrixChannel] = 0; else channelVolSeqMODE[matrixChannel] = 1;
        break;

    // VOLUME SEQUENCE
    case 0x40:
        channelVolSeqID[matrixChannel] = fxValue;
        if (fxValue == 0) channelSeqAttenuation[matrixChannel] = 0;
        channelVolumeChangeSpeed[matrixChannel] = 0;
        break;

    // VOLUME ATTENUATION
    case 0x41:
        if (fxValue < 0x80)
        {
            channelAttenuation[matrixChannel] = fxValue;
            SetChannelVolume(matrixChannel);
        }
        channelVolumeChangeSpeed[matrixChannel] = 0;
        break;

    // TREMOLO
    case 0x42:
        channelTremoloSpeed[matrixChannel] = ((fxValue & 0b11110000) >> 4) * channelTremoloSpeedMult[matrixChannel];
        channelTremoloDepth[matrixChannel] = (fxValue & 0b00001111) * 2;
        channelTremoloPhase[matrixChannel] = 512;
        channelVolumeChangeSpeed[matrixChannel] = 0;
        break;

    // TREMOLO SPEED MULT
    case 0x43:
        channelTremoloSpeedMult[matrixChannel] = fxValue;
        break;

    // VOLUME INCREASE
    case 0x44:
        channelVolumeChangeSpeed[matrixChannel] = fxValue & 0b00001111;
        channelVolumePulseSkip[matrixChannel] = ((fxValue & 0b11110000) >> 4) + 1;
        channelVolumePulseCounter[matrixChannel] = 0;
        break;

    // VOLUME DECREASE
    case 0x45:
        channelVolumeChangeSpeed[matrixChannel] = -(fxValue & 0b00001111);
        channelVolumePulseSkip[matrixChannel] = ((fxValue & 0b11110000) >> 4) + 1;
        channelVolumePulseCounter[matrixChannel] = 0;
        break;

    // ------------------------------------------------------------------------

    // NOTE CUT
    case 0x50:
        channelNoteCut[matrixChannel] = fxValue;
        break;

    // NOTE RETRIGGER
    case 0x51:
        channelNoteRetrigger[matrixChannel] = fxValue;
        break;

    // MATRIX JUMP
    case 0x52:
        if (fxValue > 0 && fxValue <= MAX_MATRIX_ROWS) matrixRowJumpTo = fxValue - 1;
        else if (fxValue == 0) matrixRowJumpTo = playingMatrixRow + 1;
        else matrixRowJumpTo = EVALUATE_0xFF;
        break;

    // PATTERN ROW JUMP
    case 0x53:
        if (fxValue <= PATTERN_ROW_LAST) patternRowJumpTo = fxValue;
        else patternRowJumpTo = EVALUATE_0xFF;
        break;

    // NOTE DELAY
    case 0x54:
        channelNoteDelay[matrixChannel] = fxValue;
        break;

    // CH3 CSM FILTER (OP4 only)
    case 0x55:
        if (fxValue >= 0x09 && fxValue <= MAX_NOTE)
        {
            //switch (matrixChannel)
            //{
            //case 2:
                ch3OpFreq[0] = fxValue;
                //break;
            //case 3: ch3OpFreq[1] = fxValue; break;
            //case 4: ch3OpFreq[2] = fxValue; break;
            //case 5: ch3OpFreq[3] = fxValue; break;
            //}
        }
        break;
    }
}

static void ApplyCommand_PSG(u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // PSG N4 MODE
    case 0x15:
        switch (fxValue)
        {
        case 0x10: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_TONE3); psg_noise_mode = PSG_TONAL_CH3_MUTED; break;
        case 0x11: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_TONE3); psg_noise_mode = PSG_TONAL_CH3_NOT_MUTED; break;
        case 0x12: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_CLOCK2); psg_noise_mode = PSG_FIXED; break;
        case 0x13: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_CLOCK4); psg_noise_mode = PSG_FIXED; break;
        case 0x14: PSG_setNoise(PSG_NOISE_TYPE_PERIODIC, PSG_NOISE_FREQ_CLOCK8); psg_noise_mode = PSG_FIXED; break;
        case 0x20: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3); psg_noise_mode = PSG_TONAL_CH3_MUTED; break;
        case 0x21: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3); psg_noise_mode = PSG_TONAL_CH3_NOT_MUTED; break;
        case 0x22: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK2); psg_noise_mode = PSG_FIXED; break;
        case 0x23: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK4); psg_noise_mode = PSG_FIXED; break;
        case 0x24: PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK8); psg_noise_mode = PSG_FIXED; break;
        }
        break;

    // PWM/Waveform
    case 0x17:
        if (fxValue < 0x0D) psgPWM = fxValue;
        break;
    }
}

static void ApplyCommand_FM(u8 matrixChannel, u8 id, u8 fxParam, u8 fxValue) // matrix channel; instrument id
{
    static u16 port = 0;
    static u8 fmChannel = 0;

    // auto functions used only for effects, not for instrument change

    // TL; 0 - unused, 000 0000 - TL (0..127) high to low ~0.75db step
    auto void write_tl1() { YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + fmChannel, tmpInst[id].TL1); }
    auto void write_tl2() { YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + fmChannel, tmpInst[id].TL2); }
    auto void write_tl3() { YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + fmChannel, tmpInst[id].TL3); }
    auto void write_tl4() { YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + fmChannel, tmpInst[id].TL4); }

    // RS, AR
    // 2b - RS (0..3), 1b - unused, 5b - AR (0..31)
    auto void write_rs1_ar1()
    {
        CalculateCombined(id, COMB_RS_AR_1);
        YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0 + fmChannel, tmpInst[id].RS1_AR1);
    }
    auto void write_rs2_ar2()
    {
        CalculateCombined(id, COMB_RS_AR_2);
        YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH0 + fmChannel, tmpInst[id].RS2_AR2);
    }
    auto void write_rs3_ar3()
    {
        CalculateCombined(id, COMB_RS_AR_3);
        YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH0 + fmChannel, tmpInst[id].RS3_AR3);
    }
    auto void write_rs4_ar4()
    {
        CalculateCombined(id, COMB_RS_AR_4);
        YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH0 + fmChannel, tmpInst[id].RS4_AR4);
    }

    // DT, MUL (FM channels 0, 1, 2)
    // 1b - unused, 3b - DT1, 4b - MUL; DT1 = 1..-4+..8, MUL = 0..15
    auto void write_dt1_mul1()
    {
        CalculateCombined(id, COMB_DT_MUL_1);
        YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0 + fmChannel, tmpInst[id].DT1_MUL1);
    }
    auto void write_dt2_mul2()
    {
        CalculateCombined(id, COMB_DT_MUL_2);
        YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH0 + fmChannel, tmpInst[id].DT2_MUL2);
    }
    auto void write_dt3_mul3()
    {
        CalculateCombined(id, COMB_DT_MUL_3);
        YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH0 + fmChannel, tmpInst[id].DT3_MUL3);

    }
    auto void write_dt4_mul4()
    {
        CalculateCombined(id, COMB_DT_MUL_4);
        YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH0 + fmChannel, tmpInst[id].DT4_MUL4);
    }

    // FB, ALG
    // 2b - unused, 3b (0..7) - FB, 3b - ALG (0..7)
    auto void write_fb_alg()
    {
        CalculateCombined(id, COMB_FB_ALG);
        YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0 + fmChannel, tmpInst[id].FB_ALG);
    }

    // PAN, AMS, FMS
    // 2b - PAN (1..3), 3b - AMS (0..7), 1b - unused, 2b - FMS (0..3)
    auto void write_pan_ams_fms()
    {
        CalculateCombined(id, COMB_PAN_AMS_FMS);
        YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0 + fmChannel, tmpInst[id].PAN_AMS_FMS);

    }

    // AM, D1R
    // 1b - AM (0 or 1), 2b - unused, 5b - D1R (0..31)
    auto void write_am1_d1r1()
    {
        CalculateCombined(id, COMB_AM_D1R_1);
        YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0 + fmChannel, tmpInst[id].AM1_D1R1);
    }
    auto void write_am2_d1r2()
    {
        CalculateCombined(id, COMB_AM_D1R_2);
        YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH0 + fmChannel, tmpInst[id].AM2_D1R2);
    }
    auto void write_am3_d1r3()
    {
        CalculateCombined(id, COMB_AM_D1R_3);
        YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH0 + fmChannel, tmpInst[id].AM3_D1R3);
    }
    auto void write_am4_d1r4()
    {
        CalculateCombined(id, COMB_AM_D1R_4);
        YM2612_writeRegZ80(port, YM2612REG_OP4_AM_D1R_CH0 + fmChannel, tmpInst[id].AM4_D1R4);
    }

    // SSG-EG
    // 4b - unused, 1b - enable, 3b - SSG-EG (0..7), <8 disable, 8>= enable and set
    // ---------------------------------
    // | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    // |---------------|---|---|---|---|
    // | /   /   /   / | E |ATT|ALT|HLD|
    auto void write_ssgeg1() { YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH0 + fmChannel, tmpInst[id].SSGEG1); }
    auto void write_ssgeg2() { YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH0 + fmChannel, tmpInst[id].SSGEG2); }
    auto void write_ssgeg3() { YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH0 + fmChannel, tmpInst[id].SSGEG3); }
    auto void write_ssgeg4() { YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH0 + fmChannel, tmpInst[id].SSGEG4); }

    // D1L, RR
    // 4b - D1L (0..15), 4b - RR (0..15)
    auto void write_d1l1_rr1()
    {
        CalculateCombined(id, COMB_D1L_RR_1);
        YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0 + fmChannel, tmpInst[id].D1L1_RR1);
    }
    auto void write_d1l2_rr2()
    {
        CalculateCombined(id, COMB_D1L_RR_2);
        YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH0 + fmChannel, tmpInst[id].D1L2_RR2);
    }
    auto void write_d1l3_rr3()
    {
        CalculateCombined(id, COMB_D1L_RR_3);
        YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH0 + fmChannel, tmpInst[id].D1L3_RR3);
    }
    auto void write_d1l4_rr4()
    {
        CalculateCombined(id, COMB_D1L_RR_4);
        YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH0 + fmChannel, tmpInst[id].D1L4_RR4);
    }

    // D2R
    // 3b - unused, 5b - D2R (0..31)
    auto void write_d2r1() { YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH0 + fmChannel, tmpInst[id].D2R1); }
    auto void write_d2r2() { YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH0 + fmChannel, tmpInst[id].D2R2); }
    auto void write_d2r3() { YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH0 + fmChannel, tmpInst[id].D2R3); }
    auto void write_d2r4() { YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH0 + fmChannel, tmpInst[id].D2R4); }

    switch (matrixChannel)
    {
    case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM3_OP4:
        port = PORT_1; fmChannel = matrixChannel; // 0, 1, 2
        break;
    case CHANNEL_FM3_OP3: case CHANNEL_FM3_OP2: case CHANNEL_FM3_OP1:
        port = PORT_1; fmChannel = CHANNEL_FM3_OP4; // 2
        break;
    case CHANNEL_FM4: case CHANNEL_FM5: case CHANNEL_FM6_DAC:
        port = PORT_2; fmChannel = matrixChannel - 6; // 0, 1, 2
        break;
    }

    switch (fxParam)
    {
    // TOTAL LEVEL
    case 0x01:
        if (fxValue == 0) tmpInst[id].TL1 = ReadInstrumentSRAM(id, INST_TL1);
        else if (fxValue <= 0x80) tmpInst[id].TL1 = 0x80 - fxValue;
        else return;
        write_tl1();
        break;
    case 0x02:
        if (fxValue == 0)tmpInst[id].TL2 = ReadInstrumentSRAM(id, INST_TL2);
        else if (fxValue <= 0x80) tmpInst[id].TL2 = 0x80 - fxValue;
        else return;
        write_tl2();
        break;
    case 0x03:
        if (fxValue == 0) tmpInst[id].TL3 = ReadInstrumentSRAM(id, INST_TL3);
        else if (fxValue <= 0x80) tmpInst[id].TL3 = 0x80 - fxValue;
        else return;
        write_tl3();
        break;
    case 0x04:
        if (fxValue == 0) tmpInst[id].TL4 = ReadInstrumentSRAM(id, INST_TL4);
        else if (fxValue <= 0x80) tmpInst[id].TL4 = 0x80 - fxValue;
        else return;
        write_tl4();
        break;

    // RATE SCALE
    case 0x05:
        if (fxValue == 0x10) { tmpInst[id].RS1 = ReadInstrumentSRAM(id, INST_RS1); write_rs1_ar1(); }
        else if ((fxValue >= 0x11) && (fxValue <= 0x14)) { tmpInst[id].RS1 = fxValue - 0x11; write_rs1_ar1(); }

        else if (fxValue == 0x20) { tmpInst[id].RS2 = ReadInstrumentSRAM(id, INST_RS2); write_rs2_ar2(); }
        else if ((fxValue >= 0x21) && (fxValue <= 0x24)) { tmpInst[id].RS2 = fxValue - 0x21; write_rs2_ar2(); }

        else if (fxValue == 0x30) { tmpInst[id].RS3 = ReadInstrumentSRAM(id, INST_RS3); write_rs3_ar3(); }
        else if ((fxValue >= 0x31) && (fxValue <= 0x34)) { tmpInst[id].RS3 = fxValue - 0x31; write_rs3_ar3(); }

        else if (fxValue == 0x40) { tmpInst[id].RS4 = ReadInstrumentSRAM(id, INST_RS4); write_rs4_ar4(); }
        else if ((fxValue >= 0x41) && (fxValue <= 0x44)) { tmpInst[id].RS4 = fxValue - 0x41; write_rs4_ar4(); }

        else if (fxValue == 0x00 || fxValue == 0x50) // reset all
        {
            tmpInst[id].RS1 = ReadInstrumentSRAM(id, INST_RS1);
            tmpInst[id].RS2 = ReadInstrumentSRAM(id, INST_RS2);
            tmpInst[id].RS3 = ReadInstrumentSRAM(id, INST_RS3);
            tmpInst[id].RS4 = ReadInstrumentSRAM(id, INST_RS4);
            write_rs1_ar1(); write_rs2_ar2(); write_rs3_ar3(); write_rs4_ar4();
        }
        else if ((fxValue >= 0x51) && (fxValue <= 0x54))
        {
            tmpInst[id].RS1 = tmpInst[id].RS2 = tmpInst[id].RS3 = tmpInst[id].RS4 = fxValue - 0x51;
            write_rs1_ar1(); write_rs2_ar2(); write_rs3_ar3(); write_rs4_ar4();
        }
        break;

    // MULTIPLIER
    case 0x06:
        if (fxValue == 0x01) { tmpInst[id].MUL1 = ReadInstrumentSRAM(id, INST_MUL1); write_dt1_mul1(); }
        else if ((fxValue >= 0x10) && (fxValue < 0x20)) { tmpInst[id].MUL1 = fxValue - 0x10; write_dt1_mul1(tmpInst[id].MUL1); }

        else if (fxValue == 0x02) { tmpInst[id].MUL2 = ReadInstrumentSRAM(id, INST_MUL2); write_dt2_mul2(); }
        else if ((fxValue >= 0x20) && (fxValue < 0x30)) { tmpInst[id].MUL2 = fxValue - 0x20; write_dt2_mul2(tmpInst[id].MUL2); }

        else if (fxValue == 0x03) { tmpInst[id].MUL3 = ReadInstrumentSRAM(id, INST_MUL3); write_dt3_mul3(); }
        else if ((fxValue >= 0x30) && (fxValue < 0x40)) { tmpInst[id].MUL3 = fxValue - 0x30; write_dt3_mul3(tmpInst[id].MUL3); }

        else if (fxValue == 0x04) { tmpInst[id].MUL4 = ReadInstrumentSRAM(id, INST_MUL4); write_dt4_mul4(); }
        else if ((fxValue >= 0x40) && (fxValue < 0x50)) { tmpInst[id].MUL4 = fxValue - 0x40; write_dt4_mul4(tmpInst[id].MUL4); }


        else if (fxValue == 0x00) // reset all
        {
            tmpInst[id].MUL1 = ReadInstrumentSRAM(id, INST_MUL1);
            tmpInst[id].MUL2 = ReadInstrumentSRAM(id, INST_MUL2);
            tmpInst[id].MUL3 = ReadInstrumentSRAM(id, INST_MUL3);
            tmpInst[id].MUL4 = ReadInstrumentSRAM(id, INST_MUL4);
            write_dt1_mul1(); write_dt2_mul2(); write_dt3_mul3(); write_dt4_mul4();
        }
        break;

    // DETUNE
    case 0x07:
        if (fxValue == 0x10) { tmpInst[id].DT1 = ReadInstrumentSRAM(id, INST_DT1); write_dt1_mul1(); }
        else if ((fxValue >= 0x11) && (fxValue <= 0x18)) { tmpInst[id].DT1 = fxValue - 0x11; write_dt1_mul1(tmpInst[id].DT1); }

        else if (fxValue == 0x20) { tmpInst[id].DT2 = ReadInstrumentSRAM(id, INST_DT2); write_dt2_mul2(); }
        else if ((fxValue >= 0x21) && (fxValue <= 0x28)) { tmpInst[id].DT2 = fxValue - 0x21; write_dt2_mul2(tmpInst[id].DT2); }

        else if (fxValue == 0x30) { tmpInst[id].DT3 = ReadInstrumentSRAM(id, INST_DT3); write_dt3_mul3(); }
        else if ((fxValue >= 0x31) && (fxValue <= 0x38)) { tmpInst[id].DT3 = fxValue - 0x31; write_dt3_mul3(tmpInst[id].DT3); }

        else if (fxValue == 0x40) { tmpInst[id].DT4 = ReadInstrumentSRAM(id, INST_DT4); write_dt4_mul4(); }
        else if ((fxValue >= 0x41) && (fxValue <= 0x48)) { tmpInst[id].DT4 = fxValue - 0x41; write_dt4_mul4(tmpInst[id].DT4); }

        else if (fxValue == 0x00) // reset all
        {
            tmpInst[id].DT1 = ReadInstrumentSRAM(id, INST_DT1);
            tmpInst[id].DT2 = ReadInstrumentSRAM(id, INST_DT2);
            tmpInst[id].DT3 = ReadInstrumentSRAM(id, INST_DT3);
            tmpInst[id].DT4 = ReadInstrumentSRAM(id, INST_DT4);
            write_dt1_mul1(); write_dt2_mul2(); write_dt3_mul3(); write_dt4_mul4();
        }
        break;

    // LFO FREQUENCY
    case 0x10:
        if (fxValue < 9) SetGlobalLFO(fxValue + 7); // 7 .. F
        break;

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

    // CH3 MODE
    case 0x12:
        if (fxValue == 0) ch3Mode = CH3_NORMAL; // special
        else if (fxValue == 1) ch3Mode = CH3_SPECIAL; // normal
        else if (fxValue == 2) ch3Mode = CH3_SPECIAL_CSM; // special+CSM
        else return;
        YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, ch3Mode | 0b00001111);
        break;

    // ATTACK
    case 0xA1:
        if (fxValue == 0) tmpInst[id].AR1 = ReadInstrumentSRAM(id, INST_AR1);
        else if (fxValue <= 0x20) tmpInst[id].AR1 = fxValue - 1;
        else return;
        write_rs1_ar1();
        break;
    case 0xA2:
        if (fxValue == 0) tmpInst[id].AR2 = ReadInstrumentSRAM(id, INST_AR2);
        else if (fxValue <= 0x20) tmpInst[id].AR2 = fxValue - 1;
        else return;
        write_rs2_ar2();
        break;
    case 0xA3:
        if (fxValue == 0) tmpInst[id].AR3 = ReadInstrumentSRAM(id, INST_AR3);
        else if (fxValue <= 0x20) tmpInst[id].AR3 = fxValue - 1;
        else return;
        write_rs3_ar3();
        break;
    case 0xA4:
        if (fxValue == 0) tmpInst[id].AR4 = ReadInstrumentSRAM(id, INST_AR4);
        else if (fxValue <= 0x20) tmpInst[id].AR4 = fxValue - 1;
        else return;
        write_rs4_ar4();
        break;

    // DECAY 1
    case 0xB1:
        if (fxValue == 0) tmpInst[id].D1R1 = ReadInstrumentSRAM(id, INST_D1R1);
        else if (fxValue <= 0x20) tmpInst[id].D1R1 = fxValue - 1;
        else return;
        write_am1_d1r1();
        break;
    case 0xB2:
        if (fxValue == 0) tmpInst[id].D1R2 = ReadInstrumentSRAM(id, INST_D1R2);
        else if (fxValue <= 0x20) tmpInst[id].D1R2 = fxValue - 1;
        else return;
        write_am2_d1r2();
        break;
    case 0xB3:
        if (fxValue == 0) tmpInst[id].D1R3 = ReadInstrumentSRAM(id, INST_D1R3);
        else if (fxValue <= 0x20) tmpInst[id].D1R3 = fxValue - 1;
        else return;
        write_am3_d1r3();
        break;
    case 0xB4:
        if (fxValue == 0) tmpInst[id].D1R4 = ReadInstrumentSRAM(id, INST_D1R4);
        else if (fxValue <= 0x20) tmpInst[id].D1R4 = fxValue - 1;
        else return;
        write_am4_d1r4();
        break;

    // SUSTAIN
    case 0xC1:
        if (fxValue == 0) tmpInst[id].D1L1 = ReadInstrumentSRAM(id, INST_D1L1);
        else if (fxValue <= 0x10) tmpInst[id].D1L1 = fxValue - 1;
        else return;
        write_d1l1_rr1();
        break;
    case 0xC2:
        if (fxValue == 0) tmpInst[id].D1L2 = ReadInstrumentSRAM(id, INST_D1L2);
        else if (fxValue <= 0x10) tmpInst[id].D1L2 = fxValue - 1;
        else return;
        write_d1l2_rr2();
        break;
    case 0xC3:
        if (fxValue == 0) tmpInst[id].D1L3 = ReadInstrumentSRAM(id, INST_D1L3);
        else if (fxValue <= 0x10) tmpInst[id].D1L3 = fxValue - 1;
        else return;
        write_d1l3_rr3();
        break;
    case 0xC4:
        if (fxValue == 0) tmpInst[id].D1L4 = ReadInstrumentSRAM(id, INST_D1L4);
        else if (fxValue <= 0x10) tmpInst[id].D1L4 = fxValue - 1;
        else return;
        write_d1l4_rr4();
        break;

    // DECAY 2
    case 0xD1:
        if (fxValue == 0) tmpInst[id].D2R1 = ReadInstrumentSRAM(id, INST_D2R1);
        else if (fxValue <= 0x20) tmpInst[id].D2R1 = fxValue - 1;
        else return;
        write_d2r1();
        break;
    case 0xD2:
        if (fxValue == 0) tmpInst[id].D2R2 = ReadInstrumentSRAM(id, INST_D2R2);
        else if (fxValue <= 0x20) tmpInst[id].D2R2 = fxValue - 1;
        else return;
        write_d2r2();
        break;
    case 0xD3:
        if (fxValue == 0) tmpInst[id].D2R3 = ReadInstrumentSRAM(id, INST_D2R3);
        else if (fxValue <= 0x20) tmpInst[id].D2R3 = fxValue - 1;
        else return;
        write_d2r3();
        break;
    case 0xD4:
        if (fxValue == 0) tmpInst[id].D2R4 = ReadInstrumentSRAM(id, INST_D2R4);
        else if (fxValue <= 0x20) tmpInst[id].D2R4 = fxValue - 1;
        else return;
        write_d2r4();
        break;

    // RELEASE
    case 0xE1:
        if (fxValue == 0) tmpInst[id].RR1 = ReadInstrumentSRAM(id, INST_RR1);
        else if (fxValue <= 0x10) tmpInst[id].RR1 = fxValue - 1;
        else return;
        write_d1l1_rr1();
        break;
    case 0xE2:
        if (fxValue == 0) tmpInst[id].RR2 = ReadInstrumentSRAM(id, INST_RR2);
        else if (fxValue <= 0x10) tmpInst[id].RR2 = fxValue - 1;
        else return;
        write_d1l2_rr2();
        break;
    case 0xE3:
        if (fxValue == 0) tmpInst[id].RR3 = ReadInstrumentSRAM(id, INST_RR3);
        else if (fxValue <= 0x10) tmpInst[id].RR3 = fxValue - 1;
        else return;
        write_d1l3_rr3();
        break;
    case 0xE4:
        if (fxValue == 0) tmpInst[id].RR4 = ReadInstrumentSRAM(id, INST_RR4);
        else if (fxValue <= 0x10) tmpInst[id].RR4 = fxValue - 1;
        else return;
        write_d1l4_rr4();
        break;

    // AMPLITUDE MODULATION
    case 0x08:
        switch(fxValue)
        {
            case 0x10: { tmpInst[id].AM1 = ReadInstrumentSRAM(id, INST_AM1); write_am1_d1r1(); }
                break;
            case 0x11: { tmpInst[id].AM1 = 1; write_am1_d1r1(); }
                break;
            case 0x12: { tmpInst[id].AM1 = 0; write_am1_d1r1(); }
                break;
            case 0x20: { tmpInst[id].AM2 = ReadInstrumentSRAM(id, INST_AM2); write_am2_d1r2(); }
                break;
            case 0x21: { tmpInst[id].AM2 = 1; write_am2_d1r2(); }
                break;
            case 0x22: { tmpInst[id].AM2 = 0; write_am2_d1r2(); }
                break;
            case 0x30: { tmpInst[id].AM3 = ReadInstrumentSRAM(id, INST_AM3); write_am3_d1r3(); }
                break;
            case 0x31: { tmpInst[id].AM3 = 1; write_am3_d1r3(); }
                break;
            case 0x32: { tmpInst[id].AM3 = 0; write_am3_d1r3(); }
                break;
            case 0x40: { tmpInst[id].AM4 = ReadInstrumentSRAM(id, INST_AM4); write_am4_d1r4(); }
                break;
            case 0x41: { tmpInst[id].AM4 = 1; write_am4_d1r4(); }
                break;
            case 0x42: { tmpInst[id].AM4 = 0; write_am4_d1r4(); }
                break;
            case 0x00: case 0x50:
                tmpInst[id].AM1 = ReadInstrumentSRAM(id, INST_AM1);
                tmpInst[id].AM2 = ReadInstrumentSRAM(id, INST_AM2);
                tmpInst[id].AM3 = ReadInstrumentSRAM(id, INST_AM3);
                tmpInst[id].AM4 = ReadInstrumentSRAM(id, INST_AM4);
                write_am1_d1r1(); write_am2_d1r2(); write_am3_d1r3(); write_am4_d1r4();
                break;
            case 0x51:
                tmpInst[id].AM1 = tmpInst[id].AM2 = tmpInst[id].AM3 = tmpInst[id].AM4 = 1;
                write_am1_d1r1(); write_am2_d1r2(); write_am3_d1r3(); write_am4_d1r4();
                break;
            case 0x52:
                tmpInst[id].AM1 = tmpInst[id].AM2 = tmpInst[id].AM3 = tmpInst[id].AM4 = 0;
                write_am1_d1r1(); write_am2_d1r2(); write_am3_d1r3(); write_am4_d1r4();
                break;
            default: return; break;
        }
        break;

    // SSG-EG
    case 0x09:
             if (fxValue == 0x10) { tmpInst[id].SSGEG1 = ReadInstrumentSRAM(id, INST_SSGEG1); write_ssgeg1(); }
        else if ((fxValue >= 0x11) && (fxValue <= 0x19)) { tmpInst[id].SSGEG1 = fxValue - 0x0A; write_ssgeg1(); }

        else if (fxValue == 0x20) { tmpInst[id].SSGEG2 = ReadInstrumentSRAM(id, INST_SSGEG2); write_ssgeg2(); }
        else if ((fxValue >= 0x21) && (fxValue <= 0x29)) { tmpInst[id].SSGEG2 = fxValue - 0x1A; write_ssgeg2(); }

        else if (fxValue == 0x30) { tmpInst[id].SSGEG3 = ReadInstrumentSRAM(id, INST_SSGEG3); write_ssgeg3(); }
        else if ((fxValue >= 0x31) && (fxValue <= 0x39)) { tmpInst[id].SSGEG3 = fxValue - 0x2A; write_ssgeg3(); }

        else if (fxValue == 0x40) { tmpInst[id].SSGEG4 = ReadInstrumentSRAM(id, INST_SSGEG4); write_ssgeg4(); }
        else if ((fxValue >= 0x41) && (fxValue <= 0x49)) { tmpInst[id].SSGEG4 = fxValue - 0x3A; write_ssgeg4(); }

        else if (fxValue == 0x00 || fxValue == 0x50) // reset all
        {
            tmpInst[id].SSGEG1 = ReadInstrumentSRAM(id, INST_SSGEG1);
            tmpInst[id].SSGEG2 = ReadInstrumentSRAM(id, INST_SSGEG2);
            tmpInst[id].SSGEG3 = ReadInstrumentSRAM(id, INST_SSGEG3);
            tmpInst[id].SSGEG4 = ReadInstrumentSRAM(id, INST_SSGEG4);
            write_ssgeg1(); write_ssgeg2(); write_ssgeg3(); write_ssgeg4();
        }
        else if ((fxValue >= 0x51) && (fxValue <= 0x59))
        {
            tmpInst[id].SSGEG1 = tmpInst[id].SSGEG2 = tmpInst[id].SSGEG3 = tmpInst[id].SSGEG4 = fxValue - 0x4A;
            write_ssgeg1(); write_ssgeg2(); write_ssgeg3(); write_ssgeg4();
        }
        else return;
        break;

    // ALGORITHM
    case 0x0A:
        if (fxValue == 0) tmpInst[id].ALG = ReadInstrumentSRAM(id, INST_ALG);
        else if (fxValue < 9) tmpInst[id].ALG = fxValue - 1;
        else return;
        write_fb_alg();
        break;

    // OP1 FEEDBACK
    case 0x0B:
        if (fxValue == 0) tmpInst[id].FB = ReadInstrumentSRAM(id, INST_FB);
        else if (fxValue < 9) tmpInst[id].FB = fxValue - 1;
        else return;
        write_fb_alg();
        break;

    // AMS
    case 0x0C:
        if (fxValue == 0) tmpInst[id].AMS = ReadInstrumentSRAM(id, INST_FMS);
        else if (fxValue < 8) tmpInst[id].AMS = fxValue;
        else if (fxValue == 0x0F) tmpInst[id].AMS = 0;
        else return;
        write_pan_ams_fms();
        break;

    // FMS
    case 0x0D:
        if (fxValue == 0) tmpInst[id].FMS = ReadInstrumentSRAM(id, INST_AMS);
        else if (fxValue < 4) tmpInst[id].FMS = fxValue;
        else if (fxValue == 0x0F) tmpInst[id].FMS = 0;
        else return;
        write_pan_ams_fms();
        break;

    // PAN
    case 0x0E:
        switch (fxValue)
        {
        case 0x00: tmpInst[id].PAN = ReadInstrumentSRAM(id, INST_PAN);
            break;
        case 0x10: tmpInst[id].PAN = 2; if (matrixChannel == CHANNEL_FM6_DAC) dacPan = SOUND_PAN_LEFT;
            break;
        case 0x01: tmpInst[id].PAN = 1; if (matrixChannel == CHANNEL_FM6_DAC) dacPan = SOUND_PAN_RIGHT;
            break;
        case 0x11: tmpInst[id].PAN = 3; if (matrixChannel == CHANNEL_FM6_DAC) dacPan = SOUND_PAN_CENTER;
            break;
        case 0xFF: tmpInst[id].PAN = 0;
            break;
        default: return; break;
        }
        write_pan_ams_fms();
        break;
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

void FillRowRight(u8 plane, u8 pal, u8 flipV, u8 flipH, u8 guiSymbol, u8 fillCount, u8 startX, u8 y) // fill with row symbol
{
    for (u8 x = startX; x < (startX + fillCount); x++)
        VDP_setTileMapXY(plane, TILE_ATTR_FULL(pal, 1, flipV, flipH, bgBaseTileIndex[2] + guiSymbol), x, y);
}

void RefreshPatternColors() // on color change
{
    static u16 pt = 0;

    for (u8 ch = CHANNEL_FM1; ch <= CHANNEL_PSG4_NOISE; ch++)
    {
        for (u8 row = 0; row < MATRIX_SCREEN_ROWS; row++)
        {
            pt = ReadMatrixSRAM(ch, row + currentPage * MATRIX_SCREEN_ROWS);
            if (pt != NULL)
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PATTERNCOLORS[ReadPatternColorSRAM(pt)]), ch*3+2, row+2);
            }
            else
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), ch*3+2, row+2);
            }
        }
    }
}

// instrument
u8 ReadInstrumentSRAM(u8 id, u16 param)
{
    return SRAMW_readByte((u32)INSTRUMENT_DATA + (id * INST_SIZE) + param);
}

void WriteInstrumentSRAM(u8 id, u16 param, u8 data)
{
    SRAMW_writeByte((u32)INSTRUMENT_DATA + (id * INST_SIZE) + param, data);
}

// pattern
u8 ReadPatternSRAM(u16 id, u8 line, u8 param)
{
    return SRAMW_readByte((u32)PATTERN_DATA + (id * PATTERN_SIZE) + (line * PATTERN_COLUMNS) + param);
}

void WritePatternSRAM(u16 id, u8 line, u8 param, u8 data)
{
    SRAMW_writeByte((u32)PATTERN_DATA + (id * PATTERN_SIZE) + (line * PATTERN_COLUMNS) + param, data);
}

u8 ReadPatternColorSRAM(u16 id)
{
    return SRAMW_readByte((u32)PATTERN_COLOR + id);
}

void WritePatternColorSRAM(u16 id, u8 color)
{
    SRAMW_writeByte((u32)PATTERN_COLOR + id, color);
}

// matrix
u16 ReadMatrixSRAM(u8 channel, u8 line)
{
    return SRAMW_readWord((u32)PATTERN_MATRIX + ((channel * MAX_MATRIX_ROWS) + line) * 2);
}

void WriteMatrixSRAM(u8 channel, u8 line, u16 data)
{
    SRAMW_writeWord((u32)PATTERN_MATRIX + ((channel * MAX_MATRIX_ROWS) + line) * 2, data);
}

// pcm
u32 ReadSampleRegionSRAM(u8 bank, u8 note, u8 byteNum)
{
    return (u32)SRAMW_readByte((u32)SAMPLE_DATA + (bank * MAX_NOTES * SAMPLE_DATA_SIZE) + (note * SAMPLE_DATA_SIZE) + byteNum);
}

void WriteSampleRegionSRAM(u8 bank, u8 note, u8 byteNum, u8 data)
{
    SRAMW_writeByte((u32)SAMPLE_DATA + (bank * MAX_NOTES * SAMPLE_DATA_SIZE) + (note * SAMPLE_DATA_SIZE) + byteNum, data);
}

static void YM2612_writeRegZ80(u16 part, u8 reg, u8 data)
{
    RequestZ80();
    YM2612_writeReg(part, reg, data);
    ReleaseZ80();
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
    5 $A130FB 	Bank register for address $280000-$2FFFFF sram
    6 $A130FD 	Bank register for address $300000-$37FFFF
    7 $A130FF 	Bank register for address $380000-$3FFFFF
    */
    //if (msu_resp == 0) // Function will return 0 if driver loaded successfully or 1 if MCD hardware not detected.
    //{
        //while (*mcd_stat != 1); // Init driver ... 0-ready, 1-init, 2-cmd busy
        //while (*mcd_stat == 1); // Wait till sub CPU finish initialization
    //}

    ssf_init();
    // X3 X5
    // Bank 28 can be used for saves. First 32Kbyte of this bank will be copied to SD card.
    // X7
    // Bank 31 can be used for saves. Upper 256K of this bank mapped to battery SRAM.
    // PRO
    // Backup ram mapped to the last 31th bank.
    ssf_set_rom_bank(4, 31);
    ssf_rom_wr_on();

    msu_resp = msu_drv();

    VDP_init();
    VDP_setDMAEnabled(TRUE);
    VDP_setHInterrupt(TRUE);
    VDP_setHIntCounter(H_INT_SKIP);
    VDP_setScreenWidth320();
    VDP_setScreenHeight224();
    VDP_setHilightShadow(FALSE);
    VDP_setScanMode(INTERLACED_NONE);

    // each plane can be a maximum of 4096 tiles in memory
    // at dimensions 32x32, 32x64, 64x64, or 32x128, with up to 40x28 (1120) visible on screen
    VDP_setPlaneSize(128, 32, TRUE);
    VDP_setBGBAddress(0xC000);          // * $2000; 0xC000 default
    VDP_setWindowAddress(0xA000);       // * $1000; 0xD000 default; WINDOW replaces BG_A when drawn, but ignores scrolling;
    VDP_setBGAAddress(0xE000);          // * $2000; 0xE000 default
    VDP_setHScrollTableAddress(0xB800); // * $400; 0xF000 default
    VDP_setSpriteListAddress(0xBC00);   // * $400; 0xF400 default
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_COLUMN);
    VDP_setTextPlane(BG_A);
    VDP_setTextPalette(PAL0);
    VDP_loadFont(&custom_font, DMA);
    VDP_setWindowHPos(FALSE, 0); // disable window, TRUE to enable
    VDP_setWindowVPos(FALSE, 0);

    // test
    /*for (u8 x=0; x<40; x++)
    {
        for (u8 y=0; y<32; y++)
        {
            VDP_setTileMapXY(WINDOW, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, 0), x, y);
        }
    }*/

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
    // GUI symbols
    bgBaseTileIndex[2] = ind;
    VDP_loadTileSet(&tileset_gui, ind, DMA);
    ind += numletters.numTile;
    // Matrix pattern colors
    /*bgBaseTileIndex[3] = ind;
    VDP_loadTileSet(&tileset_patterns, ind, DMA);
    ind += numletters.numTile;*/

    // BGR
    PAL_setPaletteColorsDMA(0, &palette_gui); // PAL0
    // 0        background
    // 1 - 15   pattern colors
    // 16       normal text
    // PAL1
    PAL_setColor(15+16, 0x0BBB);    // shade 1 text
    // PAL2
    PAL_setColor(15+32, 0x0555);    // shade 2 text
    // PAL3
    PAL_setColor(15+48, 0x0ADE);    // static text

    DrawStaticHeaders();

    SRAM_enable();

    PSG_init();
    YM2612_reset();
    Z80_init();
    Z80_loadDriver(Z80_DRIVER_PCM, TRUE);

    JOY_setSupport(PORT_1, JOY_SUPPORT_6BTN);
    JOY_setSupport(PORT_2, JOY_SUPPORT_6BTN);
    JOY_setEventHandler(JoyEvent);

    RefreshPatternColors(); // need SRAM

    // init
    for (u8 channel = CHANNEL_FM1; channel < CHANNELS_TOTAL; channel++)
    {
        channelPreviousInstrument[channel] = NULL;
        channelPreviousEffect[channel][0] = NULL;
        channelPreviousEffect[channel][1] = NULL;
        channelPreviousEffect[channel][2] = NULL;
        channelPreviousNote[channel] = NOTE_OFF;
        channelArpSeqID[channel] = NULL;
        channelVolSeqID[channel] = NULL;

        channelTremoloSpeedMult[channel] = 0x20;
        channelVibratoSpeedMult[channel] = 0x08;
        channelVibratoDepthMult[channel] = 0x02;
        channelVibratoMode[channel] = 0;

        channelNoteCut[channel]  = 0;
    }

    if (SRAMW_readWord(DEAD_INSTRUMENT) != 0xDEAD) // there is no SRAM file, needs fresh init.
    {
        SetBPM(H_INT_SKIP * 0x2D); // 140 BPM NTSC
        // init with default instrument; 49 non-global parameters (5 for whole channel, 11*4 per operator)
        DrawText(BG_A, PAL0, "GENERATING", 3, 3); DrawText(BG_A, PAL0, "MODULE", 14, 3); DrawText(BG_A, PAL0, "DATA", 21, 3);
        for (u16 i = 0; i <= MAX_INSTRUMENT; i++)
        {
            // create default instruments
            WriteInstrumentSRAM(i, INST_MUL1, 1);
            WriteInstrumentSRAM(i, INST_MUL2, 4);
            WriteInstrumentSRAM(i, INST_MUL3, 2);
            WriteInstrumentSRAM(i, INST_MUL4, 1);
            //{
            //0 	0 	0 	No change
            //0 	0 	1 	*(1+e)
            //0 	1 	0 	*(1+2e)
            //0 	1 	1 	*(1+3e)
            //1 	0 	0 	No change
            //1 	0 	1 	*(1-e)
            //1 	1 	0 	*(1-2e)
            //1 	1 	1 	*(1-3e)
            //}
            WriteInstrumentSRAM(i, INST_DT1, 4);
            WriteInstrumentSRAM(i, INST_DT2, 4);
            WriteInstrumentSRAM(i, INST_DT3, 4);
            WriteInstrumentSRAM(i, INST_DT4, 4);

            WriteInstrumentSRAM(i, INST_TL1, 0x20);
            WriteInstrumentSRAM(i, INST_TL2, 0x10);
            WriteInstrumentSRAM(i, INST_TL3, 0x09);
            WriteInstrumentSRAM(i, INST_TL4, 0x03);

            WriteInstrumentSRAM(i, INST_RS1, 1);
            WriteInstrumentSRAM(i, INST_RS2, 2);
            WriteInstrumentSRAM(i, INST_RS3, 1);
            WriteInstrumentSRAM(i, INST_RS4, 1);

            WriteInstrumentSRAM(i, INST_AR1, 31);
            WriteInstrumentSRAM(i, INST_AR2, 28);
            WriteInstrumentSRAM(i, INST_AR3, 25);
            WriteInstrumentSRAM(i, INST_AR4, 31);

            WriteInstrumentSRAM(i, INST_D1R1, 5);
            WriteInstrumentSRAM(i, INST_D1R2, 5);
            WriteInstrumentSRAM(i, INST_D1R3, 5);
            WriteInstrumentSRAM(i, INST_D1R4, 7);

            WriteInstrumentSRAM(i, INST_D1L1, 2);
            WriteInstrumentSRAM(i, INST_D1L2, 3);
            WriteInstrumentSRAM(i, INST_D1L3, 2);
            WriteInstrumentSRAM(i, INST_D1L4, 10);

            WriteInstrumentSRAM(i, INST_AM1, 0);
            WriteInstrumentSRAM(i, INST_AM2, 0);
            WriteInstrumentSRAM(i, INST_AM3, 0);
            WriteInstrumentSRAM(i, INST_AM4, 0);

            WriteInstrumentSRAM(i, INST_D2R1, 2);
            WriteInstrumentSRAM(i, INST_D2R2, 3);
            WriteInstrumentSRAM(i, INST_D2R3, 2);
            WriteInstrumentSRAM(i, INST_D2R4, 4);

            WriteInstrumentSRAM(i, INST_RR1, 15);
            WriteInstrumentSRAM(i, INST_RR2, 15);
            WriteInstrumentSRAM(i, INST_RR3, 15);
            WriteInstrumentSRAM(i, INST_RR4, 15);

            WriteInstrumentSRAM(i, INST_SSGEG1, 7);// 4 bit (off + 8 values)
            WriteInstrumentSRAM(i, INST_SSGEG2, 7);
            WriteInstrumentSRAM(i, INST_SSGEG3, 7);
            WriteInstrumentSRAM(i, INST_SSGEG4, 7);

            WriteInstrumentSRAM(i, INST_FB, 3);
            WriteInstrumentSRAM(i, INST_ALG, 3);
            WriteInstrumentSRAM(i, INST_PAN, 3);
            WriteInstrumentSRAM(i, INST_FMS, 0);
            WriteInstrumentSRAM(i, INST_AMS, 0);

            for (u8 j = 0; j < 16; j++)
            {
                if (j == 0) WriteInstrumentSRAM(i, INST_VOL_TICK_01, 0x00); // no volume attenuation
                else WriteInstrumentSRAM(i, INST_VOL_TICK_01 + j, EVALUATE_0xFF); // skip step

                WriteInstrumentSRAM(i, INST_ARP_TICK_01 + j, NOTE_EMPTY);

                if (j < 8) WriteInstrumentSRAM(i, INST_NAME_1 + j, 0); // "--------" by default
            }
        }

        SRAMW_writeByte(GLOBAL_LFO, 7);
        SRAMW_writeWord(DEAD_INSTRUMENT, 0xDEAD); // checker

        // init matrix
        for (u8 i = CHANNEL_FM1; i < CHANNELS_TOTAL; i++) // 12
        {
            for (u8 j = 0; j < MAX_MATRIX_ROWS; j++) // 250
            {
                WriteMatrixSRAM(i, j, 0x0000);
            }
        }

        // initialize patterns
        for (u16 i = 0; i <= MAX_PATTERN; i++)
        {
            WritePatternColorSRAM(i, 0);
            for (u8 j = 0; j <= PATTERN_ROW_LAST; j++)
            {
                WritePatternSRAM(i, j, DATA_NOTE, NOTE_EMPTY);
                WritePatternSRAM(i, j, DATA_INSTRUMENT, 0x00);
                WritePatternSRAM(i, j, DATA_FX1_TYPE, 0x00);
                WritePatternSRAM(i, j, DATA_FX1_VALUE, 0x00);
                WritePatternSRAM(i, j, DATA_FX2_TYPE, 0x00);
                WritePatternSRAM(i, j, DATA_FX2_VALUE, 0x00);
                WritePatternSRAM(i, j, DATA_FX3_TYPE, 0x00);
                WritePatternSRAM(i, j, DATA_FX3_VALUE, 0x00);
#if (MD_TRACKER_VERSION == 5)
                WritePatternSRAM(i, j, DATA_FX4_TYPE, 0x00);
                WritePatternSRAM(i, j, DATA_FX4_VALUE, 0x00);
                WritePatternSRAM(i, j, DATA_FX5_TYPE, 0x00);
                WritePatternSRAM(i, j, DATA_FX5_VALUE, 0x00);
                WritePatternSRAM(i, j, DATA_FX6_TYPE, 0x00);
                WritePatternSRAM(i, j, DATA_FX6_VALUE, 0x00);
#endif
            }
        }
    }
    else
    {
        SetBPM(SRAMW_readWord(TEMPO));
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
    // 00 - CSM / CH3 mode, 00 0000 - Timers: Reset B, Reset A, Enable B, Enable A, Load B, Load A
    // 0 - CH3 normal mode; 64 - CH3 special mode; timers are need to be used to

    // set timer A time; 1111 1111 11 = 0.018 ms (minimum step), 0000 0000 00 = 18.4 ms; 1024 values; 18 * (1024 - Timer A) microseconds
    // 3FFh CSM: always key-on
    // timer A is for CSM. timer B usually is for song playback and tempo
    YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_MSB, 0); // 8 bit MSB
    YM2612_writeRegZ80(PORT_1, YM2612REG_TIMER_A_LSB, 0); // 2 bit LSB
    // timer B; 1111 1111 = 0.288 ms (minimum step), 0000 0000 = 73.44 ms; 288 * (256 - Timer B ) microseconds
    YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_NORMAL | 0b00111100);
    YM2612_enableDAC();

    psg_noise_mode = PSG_TONAL_CH3_MUTED;
    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_TONE3);

    for (u16 id = 0; id <= MAX_INSTRUMENT; id++)
    {
        CacheIstrumentToRAM(id);
        instrumentIsMuted[id] = INST_PLAY;
    }

    SYS_enableInts();
}

void DrawStaticHeaders()
{
    // draw default initial brackets
    currentScreen = 2; DrawSelectionCursor(0, 0, 0); // instrument
    currentScreen = 1; DrawSelectionCursor(0, 0, 0); // pattern
    currentScreen = 0; DrawSelectionCursor(0, 0, 0); // matrix

    // ----------------------------------- matrix editor
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_VERSION),     38, 27); // version
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_VERSION+1),   39, 27);

    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i, 0); // MD.TRACKER logo

    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 27, 0, 0); // top line
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_SLASH, 2, 27, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 29, 0);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_P),        31, 0); // PAGE:
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_A),        32, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_G),        33, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_E),        34, 0);
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

    for (u8 i=0; i<4; i++) // op4 op3 op2 op1
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_OP), (i * 3) + 7, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_4 - i), (i * 3) + 8, 27);
    }

    FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_SLASH, 3, 19, 27); // bottom line
    FillRowRight(BG_A, PAL3, FALSE, FALSE, GUI_LOWLINE, 18, 22, 27);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BPM), 1, 27); // bpm
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BPM + 1), 2, 27);

    //! for (u8 y=2; y<27; y++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 27, y); // fm/psg

    // ----------------------------------- pattern editor
    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i + 72, 22); // MD.TRACKER logo

    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 29, 40, 22); // low line
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_SLASH), 69, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 70, 22);
    FillRowRight(BG_A, PAL3, FALSE, TRUE, GUI_LOWLINE, 9, 71, 22);

    for (u8 y=4; y<20; y++) // pattern effects separator dots
    {
#if (MD_TRACKER_VERSION == 5)
        for (u8 x=49; x<59; x+=2)
#elif (MD_TRACKER_VERSION == 2)
        for (u8 x=49; x<55; x+=2)
#endif
        {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), x, y);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), x + 20, y);
        }
    }

    FillRowRight(BG_B, PAL3, FALSE, TRUE, GUI_LOWLINE, 19, 41, 3); // pattern high lines
    FillRowRight(BG_B, PAL3, FALSE, FALSE, GUI_LOWLINE, 19, 61, 3);

    //DrawText(BG_A, PAL3, "PT", 41, 0); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 43, 0);
    VDP_setTextPalette(PAL3); VDP_drawText("PATTERN:", 41, 0);
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
    DrawText(BG_A, PAL3, "OK", 97, 1);

    DrawText(BG_A, PAL3, "SAMPLE", 106, 0);
    //
    DrawText(BG_A, PAL3, "REGION", 113, 2);
    DrawText(BG_A, PAL3, "START", 106, 3);
    DrawText(BG_A, PAL3, "END", 106, 4);
    DrawText(BG_A, PAL3, "LOOP", 106, 5);
    DrawText(BG_A, PAL3, "RATE", 106, 6); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_ARROW), 113, 6);
    for (u8 y=3; y<7; y++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 111, y);

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

    DrawText(BG_A, PAL3, "AM", 81, 20); DrawText(BG_A, PAL3, "LFO", 84, 20);
    DrawText(BG_A, PAL3, "SSG", 81, 21); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), 84, 21);
        DrawText(BG_A, PAL3, "EG", 85, 21);
    for (u8 y=9; y<22; y++)
    {
        switch (y)
        {
            case 13: case 19: break;
            default : VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 92, y);
        }
    }

    DrawText(BG_A, PAL3, "LFO", 81, 24); DrawText(BG_A, PAL1, "GLOBAL", 89, 24);
    DrawText(BG_A, PAL3, "VOL", 81, 25);
    DrawText(BG_A, PAL3, "ARP", 81, 26);
    for (u8 y=24; y<27; y++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 84, y);
}
