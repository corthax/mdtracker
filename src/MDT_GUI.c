#include <genesis.h>
#include <sram.h>
#include "MDT_GUI.h"
#include "MDT_Main.h"
#include "MDT_SRAM.h"
#include "MDT_ModuleData.h"
// note tables and preset data (extern refs only - defined elsewhere)
extern const u8 noteFreqID[96];
extern const u8 noteOctave[96];
extern const char* presetName[256];
#include "samples.h"
#include "MDT_SampleSettings.h"
#include "midi/midi_sync.h"

// ---------------------------------------------------------------------------
// GUI-owned globals
// ---------------------------------------------------------------------------
u8 line = 0;
u8 chan = 0;
bool bInitScreen = TRUE;
u16 bgBaseTileIndex[5]; // set in main.c InitTracker
u16 asciiBaseLetters, asciiBaseNumbers;
u8 instCopyTo = 0x01;
s8 buttonCounter = GUI_NAVIGATION_DELAY;
bool bDoCount = FALSE;
u8 navigationDirection = BUTTON_RIGHT;
u16 patternCopyFrom = 1;
s8 patternCopyRangeStart = NOTHING;
s8 patternCopyRangeEnd = NOTHING;
char sampleName[] = "--------------";
bool bReColorsAndTranspose = TRUE;
u8 rcat_ch = CHANNEL_FM1;
u8 rcat_row = 0;

// Palettes:
// PAL0: 0 transparent, 1..14 gui graphics, 15 text
// PAL1: 0 transparent, 1..14 pattern colors, 15 text
// PAL2: 0 transparent, 1..14 pattern colors, 15 text
// PAL3: 0 transparent, 1..14 pattern colors, 15 text

// ---------------------------------------------------------------------------
// Static const data tables
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Navigation
// ---------------------------------------------------------------------------
void NavigateMatrix(u8 direction)
{
    switch (direction)
    {
    case BUTTON_LEFT:
        if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) {
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, TRUE);
            if (selectedMatrixChannel > CHANNEL_FM1) selectedMatrixChannel--; else selectedMatrixChannel = CHANNEL_LAST;
            DrawSelectionCursor(selectedMatrixChannel, selectedMatrixScreenRow, FALSE);
        }
        break;
    case BUTTON_RIGHT:
        if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) {
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
        else selectedPatternColumn = GUI_PATTERN_COLUMN_LAST;
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case BUTTON_RIGHT:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternColumn < GUI_PATTERN_COLUMN_LAST) selectedPatternColumn++;
        else selectedPatternColumn = 0;
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case BUTTON_UP:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternRow > 0) selectedPatternRow--;
        else selectedPatternRow = GUI_PATTERN_ROW_LAST;
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case BUTTON_DOWN:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternRow < GUI_PATTERN_ROW_LAST) selectedPatternRow++;
        else selectedPatternRow = 0;
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, FALSE);
        break;
    case PATTERN_JUMPSIDETRIGGER:
        DrawSelectionCursor(selectedPatternColumn, selectedPatternRow, TRUE);
        if (selectedPatternColumn < GUI_PATTERN_SIDE_ROWS) selectedPatternColumn += GUI_PATTERN_SIDE_ROWS;
        else selectedPatternColumn -= GUI_PATTERN_SIDE_ROWS;
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
        case GUI_INST_PARAM_PARSEQ:
        case GUI_INST_PARAM_ARPSEQ:
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = SEQ_STEP_LAST;
            break;
        case GUI_INST_PARAM_NAME:
            if (selectedInstrumentOperator > 0) selectedInstrumentOperator--; else selectedInstrumentOperator = 7;
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
        case GUI_INST_PARAM_PARSEQ:
        case GUI_INST_PARAM_ARPSEQ:
            if (selectedInstrumentOperator < SEQ_STEP_LAST) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
            break;
        case GUI_INST_PARAM_NAME:
            if (selectedInstrumentOperator < 7) selectedInstrumentOperator++; else selectedInstrumentOperator = 0;
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
            selectedInstrumentOperator = 0;
            break;
        }

        if (selectedInstrumentParameter > 0) selectedInstrumentParameter--; else selectedInstrumentParameter = GUI_INST_MAX_PARAMETER;
        /*if (selectedInstrumentParameter >= GUI_INST_PARAM_PCM_TYPE && selectedInstrumentParameter <= GUI_INST_PARAM_PCM_PAN)
            selectedInstrumentParameter = GUI_INST_PARAM_PCM_NOTE;*/
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, FALSE);
        break;

    case BUTTON_DOWN:
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, TRUE);
        switch (selectedInstrumentParameter)
        {
        case GUI_INST_PARAM_ARPSEQ:
        case GUI_INST_PARAM_NAME:
        case GUI_INST_PARAM_COPY:
            selectedInstrumentOperator = 0;
            break;
        }

        if (selectedInstrumentParameter < GUI_INST_MAX_PARAMETER) selectedInstrumentParameter++; else selectedInstrumentParameter = 0;
        /*if (selectedInstrumentParameter >= GUI_INST_PARAM_PCM_TYPE && selectedInstrumentParameter <= GUI_INST_PARAM_PCM_PAN)
            selectedInstrumentParameter = GUI_INST_PARAM_PRESET;*/
        DrawSelectionCursor(selectedInstrumentOperator, selectedInstrumentParameter, FALSE);
        break;
    }
}

// ---------------------------------------------------------------------------
// Display / drawing
// ---------------------------------------------------------------------------
void DrawBPM()
{
    if (BPM < 1000)
    {
        uintToStr(BPM, str, 3);
        DrawNum(BG_A, PAL0, str, 3, 27);
        DrawNum(BG_A, PAL1, str, 43, 27);
    }
    else { VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "999    ", 3, 27); VDP_drawTextBG(BG_A, "999    ", 43, 27); }
}

void DrawInfo()
{
    MidiSyncMode _midiMode = midi_sync_get_mode();

    if (useExternalSync && _midiMode == MIDI_SYNC_OFF)
    {
        VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "MODE   ", 3, 27); VDP_drawTextBG(BG_A, "MODE   ", 43, 27);
        return;
    }

    if (_midiMode == MIDI_SYNC_CLOCK)
    {
        VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "M-CLK  ", 3, 27); VDP_drawTextBG(BG_A, "M-CLK  ", 43, 27);
        return;
    }

    if (_midiMode == MIDI_SYNC_NOTE)
    {
        VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "M-NOTE ", 3, 27); VDP_drawTextBG(BG_A, "M-NOTE ", 43, 27);
        return;
    }

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
    else { VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "999    ", 3, 27); VDP_drawTextBG(BG_A, "999    ", 43, 27); }

    DrawHex(PAL1, ppl_1, 12, 27); DrawHex(PAL1, ppl_2, 14, 27);
    DrawHex(PAL1, ppl_1, 52, 27); DrawHex(PAL1, ppl_2, 54, 27);

    u32 _ds = sramUsed >= 999999 ? 999999 : sramUsed;
    uintToStr(_ds, str, 6);
    VDP_setTextPalette(PAL1);
    VDP_drawTextBG(BG_A, str, 18, 27);
    VDP_drawTextBG(BG_A, str, 58, 27);
}

// cursors
void DrawMatrixPlaybackCursor(u8 bClear, u8 palette, s8 offset, u8 row)
{
    static u8 playingPage = 0;

    playingPage = row / MATRIX_ROWS_ONPAGE;
    if (playingPage == currentPage)
    {
        if (bClear) { VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + row), 39, row - MATRIX_ROWS_ONPAGE * playingPage + 2 + offset); }
        else if (bPlayback) { VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 39, row - MATRIX_ROWS_ONPAGE * playingPage + 2 + offset); }
    }
}

void ClearPatternPlaybackCursor()
{
    static s8 line = 0;

    line = playingPatternRow - 1;
    if (line == -1) line = patternSize;
    if (line < 16) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 40, line+4);
    else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), 60, line-12);
}

void DrawPatternPlaybackCursor()
{
    static s8 line = 0;

    line = playingPatternRow - 1;
    if (line == -1) line = patternSize;
    if (line < 16) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 40, line+4);
    else VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_PLAYCURSOR), 60, line-12);
}

void DrawSelectionCursor(u8 pos_x, u8 pos_y, u8 bClear)
{
    static s8 offset_x = 1;
    static s8 offset_y = 2;
    static s8 width = 3;

    if (currentScreen == SCREEN_MATRIX)
    {
        offset_x = 1; offset_y = 2; width = 3;
    }
    else if (currentScreen == SCREEN_PATTERN)
    {
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
        case GUI_INST_PARAM_PARSEQ:
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
        case GUI_INST_PARAM_PRESET:
            offset_x = 80+33; offset_y = GUI_INST_PARAM_PRESET+7; width = 0; selectedInstrumentOperator = 0;
            break;
        default: break;
        }
    }

    auto void draw_cursor_1(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
    }

    auto void draw_cursor_2(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 1, y);
    }

    auto void draw_cursor_3(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 1, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_CURSOR), x + 2, y);
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
        selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        if (selectedPatternID != NULL)
        {
            u8 rawColor = SRAM_ReadPatternColor(selectedPatternID);
            if (rawColor)
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(rawColor >> 4, 1, FALSE, FALSE, bgBaseTileIndex[4] + (rawColor & 0xF)), x + 1, y);
            else
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
        }
        else
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
    }

    auto void clear_cursor_3(u8 x, u8 y)
    {
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 1, y);
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), x + 2, y);
    }

    switch (currentScreen)
    {
    case SCREEN_MATRIX:
        if (bClear)
        {
            if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) clear_cursor_2_color(pos_x * width + offset_x, pos_y + offset_y);
            else clear_cursor_3(3, 27);
        }
        else
        {
            if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y);
            else draw_cursor_3(3, 27);
        }
        break;
    case SCREEN_PATTERN:
        if (bClear)
        {
            switch (selectedPatternColumn) {
            case GUI_PATTERN_L_NOTE: case GUI_PATTERN_R_NOTE:
                clear_cursor_3(pos_x * width + offset_x - 1, pos_y + offset_y);
                break;
            case GUI_PATTERN_L_INST: case GUI_PATTERN_R_INST:
                clear_cursor_2(pos_x * width + offset_x, pos_y + offset_y);
                break;
            case GUI_PATTERN_L_FX1_VALUE: case GUI_PATTERN_R_FX1_VALUE:
            case GUI_PATTERN_L_FX2_VALUE: case GUI_PATTERN_R_FX2_VALUE:
            case GUI_PATTERN_L_FX3_VALUE: case GUI_PATTERN_R_FX3_VALUE:

            case GUI_PATTERN_L_FX4_VALUE: case GUI_PATTERN_R_FX4_VALUE:
            case GUI_PATTERN_L_FX5_VALUE: case GUI_PATTERN_R_FX5_VALUE:

                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), pos_x * width + offset_x, pos_y + offset_y); break;
            default: clear_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break;
            }
        }
        else
        {
            switch (selectedPatternColumn) {
            case GUI_PATTERN_L_NOTE: case GUI_PATTERN_R_NOTE: draw_cursor_3(pos_x * width + offset_x - 1, pos_y + offset_y); break;
            case GUI_PATTERN_L_INST: case GUI_PATTERN_R_INST: draw_cursor_2(pos_x * width + offset_x, pos_y + offset_y); break;
            default: draw_cursor_1(pos_x * width + offset_x, pos_y + offset_y); break;
            }
        }
        break;
    case SCREEN_INSTRUMENT:
        if (bClear)
        {
            switch (selectedInstrumentParameter)
            {
            case GUI_INST_PARAM_NAME:
            case GUI_INST_PARAM_PRESET:
            case GUI_INST_PARAM_PARSEQ:
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
            case GUI_INST_PARAM_PRESET:
            case GUI_INST_PARAM_PARSEQ:
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

    //DrawUnsavedMark(matrixDirty, SCREEN_MATRIX);

    if (bInitScreen)
    {
        bInitScreen = FALSE;
        DrawHex2(PAL0, currentPage, 37, 0);
        line = 0;
        chan = 0;
    }
    if (bRefreshScreen)
    {
        pageShift = currentPage * MATRIX_ROWS_ONPAGE;

        if (matrixRowToRefresh != OXFFFF)
        {
            line = matrixRowToRefresh - pageShift;
        }

        patternID = SRAM_ReadMatrix(chan, line + pageShift) & 0x3FF;

        shiftX = chan * 3;
        shiftY = line + 2;

        if (!patternID)
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, 0), shiftX, shiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), shiftX + 1, shiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), shiftX + 2, shiftY);
        }
        else
        {
            palx = PAL0;
            num1 = (patternID & 0xF00) >> 8;
            num2 = ((patternID & 0x0F0) >> 4) + bgBaseTileIndex[1];
            num3 = (patternID & 0x00F) + bgBaseTileIndex[1];

            if (num1 > 0) num1 += bgBaseTileIndex[3];
            !(line & 1) ? (palx = PAL0) : (palx = PAL1);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num1), shiftX, shiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, num2), shiftX + 1, shiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, num3), shiftX + 2, shiftY);
        }
        chan++;

        if (chan > CHANNEL_PSG4_NOISE)
        {
            if (matrixRowToRefresh == OXFFFF)
            {
                u8 row;
                row = line + pageShift;
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + row), 39, shiftY);

                if (row == loopStart) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[3] + GUI_LOOP_START), 39, shiftY);
                else if (row == loopEnd) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[3] + GUI_LOOP_END), 39, shiftY);
                else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), 39, shiftY);

                line++;
                chan = 0;
                if (line > MATRIX_ROWS_ONPAGE_LAST)
                {
                    line = 0;
                    bRefreshScreen = FALSE;
                    DrawMatrixPlaybackCursor(FALSE, PAL0, 0, playingMatrixRow);
                }
            }
            else
            {
                matrixRowToRefresh = OXFFFF;
                bRefreshScreen = FALSE;
                chan = 0;
            }
        }
    }
}
// ------------------------------ PATTERN EDITOR
void DisplayPatternEditor()
{
    //DrawUnsavedMark(patternDirty, SCREEN_PATTERN);

    if (bInitScreen)
    {
        bInitScreen = 0;
        line = 0;
    }

    if (bRefreshScreen)
    {
        static u8 note = 0;
        static u8 value = 0;
        static u8 side = 0;
        static u8 shiftX = 0;
        static u8 shiftY = 0;
        static u8 palx = PAL0;
        static u8 palminus = PAL1;
        static u8 lineShiftY = 0;

        if (patternRowToRefresh != OXFF)
        {
            line = patternRowToRefresh;
        }
        else
        {
            u16 num1 = ((selectedPatternID & 0xF00) >> 8) + bgBaseTileIndex[1];
            u16 num2 = ((selectedPatternID & 0x0F0) >> 4) + bgBaseTileIndex[1];
            u16 num3 = (selectedPatternID & 0x00F) + bgBaseTileIndex[1];
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num1), 50, 0);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num2), 51, 0);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, num3), 52, 0);
        }

        note = SRAM_ReadPattern(selectedPatternID, line, DATA_NOTE);

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

        if (note < NOTE_EMPTY)
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_NOTE_NAMES[0][noteFreqID[note]]), 41 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_NOTE_NAMES[1][noteFreqID[note]]), 42 + shiftX, lineShiftY);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + (noteOctave[note])), 43 + shiftX, lineShiftY);
        }
        else if (note == NOTE_OFF)
        {
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(palx, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_O), 41 + shiftX, lineShiftY);
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

        if (patternRowToRefresh == OXFF)
        {
            line++;
            if (line > PATTERN_ROW_LAST)
            {
                line = 0;
                bRefreshScreen = FALSE;
            }
        }
        else
        {
            patternRowToRefresh = OXFF;
            bRefreshScreen = FALSE;
        }
    }
}
// ------------------------------ INSTRUMENT EDITOR
inline void DisplayInstrumentEditor()
{
    static u8 value = 0;
    static u8 alg = 0;
    static u8 stepDrawPos = 0;

    auto void draw_pcm_settings()
    {
        const struct SampleSettings* s = GetSampleSettings(selectedSampleBank, selectedSampleNote);

        // start
        /*DrawHex2(PAL0, (s->startOffset >> 16) & 0xFF, 113, 3);
        DrawHex2(PAL0, (s->startOffset >> 8) & 0xFF, 115, 3);
        DrawHex2(PAL0, s->startOffset & 0xFF, 117, 3);*/

        // end
        /*DrawHex2(PAL0, (s->endOffset >> 16) & 0xFF, 113, 4);
        DrawHex2(PAL0, (s->endOffset >> 8) & 0xFF, 115, 4);
        DrawHex2(PAL0, s->endOffset & 0xFF, 117, 4);*/

        if (s->type == 0) { VDP_setTextPalette(PAL1); VDP_drawTextBG(BG_A, "PCM  ", 113, GUI_INST_POSY_SAMPLE_TYPE); }
        else { VDP_setTextPalette(PAL1); VDP_drawTextBG(BG_A, "ADPCM", 113, GUI_INST_POSY_SAMPLE_TYPE); }

        /*if (s->looped == FALSE) FillRowRight(BG_A, PAL1, FALSE, FALSE, GUI_BIGDOT, 2, 113, GUI_INST_POSY_SAMPLE_LOOP);
        else DrawText(BG_A, PAL1, "ON", 113, GUI_INST_POSY_SAMPLE_LOOP);*/

        if (s->looped == FALSE) { VDP_setTextPalette(PAL1); VDP_drawTextBG(BG_A, "NO ", 113, GUI_INST_POSY_SAMPLE_LOOP); }
        else { VDP_setTextPalette(PAL1); VDP_drawTextBG(BG_A, "YES", 113, GUI_INST_POSY_SAMPLE_LOOP); }

        switch (s->rate)
        {
            case SOUND_PCM_RATE_32000: DrawNum(BG_A, PAL1, "32000", 113, GUI_INST_POSY_SAMPLE_RATE); break;
            case SOUND_PCM_RATE_22050: DrawNum(BG_A, PAL1, "22050", 113, GUI_INST_POSY_SAMPLE_RATE); break;
            case SOUND_PCM_RATE_16000: DrawNum(BG_A, PAL1, "16000", 113, GUI_INST_POSY_SAMPLE_RATE); break;
            case SOUND_PCM_RATE_13400: DrawNum(BG_A, PAL1, "13400", 113, GUI_INST_POSY_SAMPLE_RATE); break;
            case SOUND_PCM_RATE_11025: DrawNum(BG_A, PAL1, "11025", 113, GUI_INST_POSY_SAMPLE_RATE); break;
            case SOUND_PCM_RATE_8000: DrawNum(BG_A, PAL1, "8000", 113, GUI_INST_POSY_SAMPLE_RATE);
                VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, NULL), 113+4, GUI_INST_POSY_SAMPLE_RATE); break;
            default: DrawNum(BG_A, PAL1, "-----", 114, 6); break;
        }

        switch (s->pan)
        {
            case SOUND_PAN_CENTER: VDP_setTextPalette(PAL1); VDP_drawText("C", 113, GUI_INST_POSY_SAMPLE_PAN); break;
            case SOUND_PAN_LEFT: VDP_setTextPalette(PAL1); VDP_drawText("L", 113, GUI_INST_POSY_SAMPLE_PAN); break;
            case SOUND_PAN_RIGHT: VDP_setTextPalette(PAL1); VDP_drawText("R", 113, GUI_INST_POSY_SAMPLE_PAN); break;
            default: VDP_setTextPalette(PAL1); VDP_drawText("-", 113, GUI_INST_POSY_SAMPLE_PAN); break;
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
                        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_ALG_GRID[alg][i][j]), 90+j, 3+i);
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
            for (u8 i=0; i<4; i++) DrawHex2(PAL0, SRAM_ReadInstrument(selectedInstrumentID, INST_TL1 + i), 94 + i*3, 9);
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
            value = SRAMW_readByte(SRAM_GLOBAL_LFO);
            DrawHex2(BG_A, value - 7, 80+12, 23);
            break;
        case GUI_INST_PARAM_PARSEQ: case 238:
            for (u8 i = 0; i <= SEQ_STEP_LAST; i++)
            {
                value = SRAM_ReadSEQ_PAR(selectedInstrumentID, i);
                stepDrawPos = 85 + i + (i/8);

                if (value == SEQ_SKIP)
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 25);
                }
                else if (value == 0)
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEQ_VOL_MAX_ATT), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEQ_VOL_MAX_ATT), stepDrawPos, 25);
                }
                else
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[0] + value), stepDrawPos, 25);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[0] + value), stepDrawPos, 25);
                }
            }
        case GUI_INST_PARAM_ARPSEQ: case 237:
            for (u8 i = 0; i <= SEQ_STEP_LAST; i++)
            {
                value = SRAM_ReadSEQ_ARP(selectedInstrumentID, i);
                stepDrawPos = 85 + i + (i/8);
                if (value == 100)
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), stepDrawPos, 27);
                }
                else if (value == NOTE_EMPTY)
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_MINUS), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, NULL), stepDrawPos, 27);
                }
                else if (value > 100)
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[0] + value - 100), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[0] + value - 100), stepDrawPos, 26);
                    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_UP1), stepDrawPos, 27);
                }
                else
                {
                    if (i%4==0) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[0] + 100 - value), stepDrawPos, 26);
                    else VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[0] + 100 - value), stepDrawPos, 26);
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
        case GUI_INST_PARAM_PCM_BANK: case GUI_INST_PARAM_PCM_NOTE:
            DrawHex2(PAL0, selectedSampleBank, 113, 0);

            u8 noteID = selectedSampleNote;
            while (noteID > 11) noteID -= 12;

            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_NOTE_NAMES[0][noteID]), 116, 0);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_NOTE_NAMES[1][noteID]), 117, 0);
            VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + (selectedSampleNote / 12)), 118, 0);

            draw_pcm_settings();
            break;
        case GUI_INST_PARAM_COPY:
            DrawHex2(PAL0, instCopyTo, GUI_INST_NAME_START, 1);
            break;
        case GUI_INST_PARAM_PRESET:
            intToStr(midiPreset, str, 3);
            VDP_setTextPalette(PAL1); VDP_drawText(str, 114, 17);
            VDP_drawText(presetName[midiPreset], 106, 18);
            break;
        }

        instrumentParameterToRefresh--;
        if (instrumentParameterToRefresh < 235) bRefreshScreen = FALSE;
    }
}

// ---------------------------------------------------------------------------
// Sample name
// ---------------------------------------------------------------------------
void DisplaySampleName(u8 xPos, u8 yPos, u8 note, u8 bank)
{
    const struct SampleSettings* s = GetSampleSettings(bank, note);
    for (u8 i = 0; i < 14; i++)
    {
        sampleName[i] = s->name[i];
    }

    VDP_setTextPalette(PAL0);
    VDP_drawTextBG(BG_A, sampleName, xPos, yPos);
}

// ---------------------------------------------------------------------------
// Color / recolor
// ---------------------------------------------------------------------------
void RedrawMarks()
{
    bReColorsAndTranspose = TRUE;
    rcat_ch = CHANNEL_FM1;
    rcat_row = 0;
}

void ReColorsAndTranspose()
{
    if (bReColorsAndTranspose)
    {
        if (rcat_row == MATRIX_ROWS_ONPAGE)
        {
            rcat_ch++;
            rcat_row = 0;

            if (rcat_ch > CHANNEL_PSG4_NOISE)
            {
                bReColorsAndTranspose = FALSE;
                return;
            }
        }

        u16 combined = SRAM_ReadMatrix(rcat_ch, rcat_row + currentPage * MATRIX_ROWS_ONPAGE);
        u16 pt = combined & 0x3FF;
        u8 rawTr = (combined >> 10) & 0x3F;
        s8 tr = (s8)(rawTr >= 32 ? rawTr - 64 : rawTr);

        if (pt != NULL)
        {
            u8 rawColor = SRAM_ReadPatternColor(pt);
            if (rawColor)
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(rawColor >> 4, 0, FALSE, FALSE, bgBaseTileIndex[4] + (rawColor & 0xF)), rcat_ch*3+2, rcat_row+2);
            }
            else
            {
                VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), rcat_ch*3+2, rcat_row+2);
            }
        }
        else
        {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, NULL), rcat_ch*3+2, rcat_row+2);
        }
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, bgBaseTileIndex[3] + GUI_TRANSPOSE + tr), rcat_ch*3, rcat_row+2);

        rcat_row++;
    }
}

// ---------------------------------------------------------------------------
// Mute / static GUI
// ---------------------------------------------------------------------------
void DrawMute(u8 mtxCh)
{
    if (channelFlags[mtxCh]) VDP_fillTileMapRect(BG_B, NULL, (mtxCh * 3) + 1, 1, 2, 1);
    else VDP_fillTileMapRect(BG_B, bgBaseTileIndex[2] + GUI_MUTE, (mtxCh * 3) + 1, 1, 2, 1);
}

void DrawStaticGUI()
{
    // bottom right version
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_VERSION),     38, 27);
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_VERSION+1),   39, 27);

    // MD.Tracker logo
    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i, 0);

    // top line
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 27, 0, 0);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_SLASH, 2, 27, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 29, 0);

    // matrix page
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_P), 31, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_A), 32, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_G), 33, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_LETTER_E), 34, 0);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON),    35, 0);

    // fm1 .. fm3
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      1, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_1), 2, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      4, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_2), 5, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      7, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_3), 8, 1);

    // special mode title
    for (u8 i=0; i<8; i++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_CH3 + i), i + 10, 1);

    // fm4 .. fm6
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      19, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_4), 20, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      22, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_5), 23, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM),      25, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_6), 26, 1);

    // psg
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_SQUARE),  28, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_1),     29, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_SQUARE),  31, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_2),     32, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_SQUARE),  34, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_3),     35, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_PSG_NOISE),   37, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_DIGIT_4),     38, 1);

    FillRowRight(BG_A, PAL0, FALSE, FALSE, GUI_SLASH, 3, 34, 27);
    FillRowRight(BG_A, PAL0, FALSE, FALSE, GUI_LOWLINE, 3, 37, 27);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM), 1, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM + 1), 2, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL), 10, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL + 1), 11, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH), 13, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_SRAM), 16, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_SRAM + 1), 17, 27);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM), 41, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_BPM + 1), 42, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL), 50, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_PPL + 1), 51, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH), 53, 27);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_SRAM), 56, 27);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_SRAM + 1), 57, 27);

    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i + 72, 22);

    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 29, 40, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_SLASH), 69, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 70, 22);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 9, 71, 22);

    // pattern grid
    for (u8 y=4; y<20; y++)
    {
        u8 pal;
        if (y%4) pal = PAL3; else pal = PAL0;
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, bgBaseTileIndex[0] + y-4), 44, y);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(pal, 1, FALSE, FALSE, bgBaseTileIndex[0] + y+12), 64, y);

        for (u8 x=49; x<59; x+=2)
        {
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), x, y);
            VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SEPARATOR), x + 20, y);
        }
    };

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[3] + GUI_00), 44, 4);

    FillRowRight(BG_B, PAL0, FALSE, TRUE, GUI_LOWLINE, 19, 41, 3);
    FillRowRight(BG_B, PAL0, FALSE, FALSE, GUI_LOWLINE, 19, 61, 3);

    VDP_setTextPalette(PAL0); VDP_drawText("PATTERN:", 41, 0); VDP_drawText("COPY FROM:", 54, 0);
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

    VDP_drawTextBG(BG_A, "INST: --------", 41, 23);

    for (u8 i=0; i<7; i++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_LOGO + i), i + 112, 22);

    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 29, 80, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_SLASH), 109, 22);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 110, 22);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 9, 111, 22);

    for (u8 y=0; y<22; y++) VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 105, y);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_LOWLINE), 80, 1);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_SLASH, 7, 81, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 88, 1);

    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 9, 106, 1);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_SLASH, 3, 115, 1);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 118, 1);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_LOWLINE), 80, 8);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_SLASH, 11, 81, 8);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 92, 8);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, TRUE, TRUE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 106, 2);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_SLASH, 4, 107, 2);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 111, 2);

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, TRUE, bgBaseTileIndex[2] + GUI_LOWLINE), 106, 19);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_SLASH, 6, 107, 19);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_SLASH_FAT), 113, 19);
    FillRowRight(BG_A, PAL0, FALSE, TRUE, GUI_LOWLINE, 6, 114, 19);

    DrawText(BG_A, PAL3, "INST", 81, 0); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 85, 0);

    DrawText(BG_A, PAL3, "ALG", 81, 2);
    DrawText(BG_A, PAL3, "FMS", 81, 3);
    DrawText(BG_A, PAL3, "AMS", 81, 4);
    DrawText(BG_A, PAL3, "PAN", 81, 5);
    DrawText(BG_A, PAL3, "FB", 81, 6);
    for (u8 y=2; y<7; y++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 85, y);

    DrawText(BG_A, PAL3, "NAME", 91, 0); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 95, 0);
    DrawText(BG_A, PAL3, "COPY", 91, 1); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 95, 1);
    DrawText(BG_A, PAL3, "OK", 97, 1); VDP_setTextPalette(PAL2); VDP_drawText("(B)", 99, 1);

    //DrawText(BG_A, PAL3, "SAMPLE", 106, 0);
    VDP_setTextPalette(PAL3); VDP_drawTextBG(BG_A, "S.BANK", 106, 0);

    DrawText(BG_A, PAL3, "TYPE", 106, GUI_INST_POSY_SAMPLE_TYPE);
    DrawText(BG_A, PAL3, "LOOP", 106, GUI_INST_POSY_SAMPLE_LOOP);
    DrawText(BG_A, PAL3, "RATE", 106, GUI_INST_POSY_SAMPLE_RATE); //VDP_setTextPalette(PAL1); VDP_drawText(">", 113, GUI_INST_POSY_SAMPLE_RATE);
    DrawText(BG_A, PAL3, "PAN", 106, GUI_INST_POSY_SAMPLE_PAN); //VDP_setTextPalette(PAL1); VDP_drawText(">", 113, GUI_INST_POSY_SAMPLE_PAN);
    DrawText(BG_A, PAL3, "NAME", 106, GUI_INST_POSY_SAMPLE_NAME);
    DisplaySampleName(106, GUI_INST_POSY_SAMPLE_NAME+1, 0, 0);

    for (u8 y=3; y<7; y++) VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL3, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 111, y);

    VDP_setTextPalette(PAL0); VDP_drawText("PRESET", 106, 17);
    VDP_setTextPalette(PAL1);
        VDP_drawText(">", 113, 17); VDP_drawText("000", 114, 17);
        VDP_drawText(presetName[0], 106, 18);
    VDP_setTextPalette(PAL2); VDP_drawText("(B)", 117, 17);


    DrawText(BG_A, PAL3, "STATE", 106, 20); VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL1, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_COLON), 111, 20);
    DrawText(BG_A, PAL0, "PLAY", 113, 20);

    for (u8 i=0; i<4; i++)
    {
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_FM_OP), (i * 3) + 94, 8);
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

    DrawText(BG_A, PAL3, "MODULATION", 81, 20);
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
    VDP_drawText("SEQ:", 80, 25);
    VDP_drawText("ARP:", 80, 26);

    currentScreen = 2; DrawSelectionCursor(0, 0, 0);
    currentScreen = 1; DrawSelectionCursor(0, 0, 0);
    currentScreen = 0; DrawSelectionCursor(0, 0, 0);
}
//! Draw unsaved mark [not enough VRAM]
/*void DrawUnsavedMark(u8 dirty, u8 screen)
{
    u16 mark = bgBaseTileIndex[3];
    if (dirty) mark = bgBaseTileIndex[2] + GUI_UNSAVED;

    switch (screen)
    {
    case SCREEN_MATRIX:
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, mark), 39, 0);
        break;
    case SCREEN_PATTERN:
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, mark), 79, 0);
        break;
    case SCREEN_INSTRUMENT:
        VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL0, 0, FALSE, FALSE, mark), 119, 0);
        break;
    }
}*/

// ---------------------------------------------------------------------------
// Reset GUI state (called from ForceResetVariables in main.c)
// ---------------------------------------------------------------------------
void ForceResetGUI()
{
    line = 0;
    chan = 0;
    bInitScreen = TRUE;
    bReColorsAndTranspose = TRUE;
    rcat_ch = CHANNEL_FM1;
    rcat_row = 0;
    buttonCounter = GUI_NAVIGATION_DELAY;
    bDoCount = FALSE;
    navigationDirection = BUTTON_RIGHT;
    patternCopyFrom = 1;
    patternCopyRangeStart = NOTHING;
    patternCopyRangeEnd = NOTHING;
    instCopyTo = 0x01;
    asciiBaseLetters = 0;
    asciiBaseNumbers = 0;
}
