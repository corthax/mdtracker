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
#include "MDT_SampleSettings.h"
#include "MDT_Info.h"
#include "MDT_Presets.h"
#include "MDT_Version.h"

// midi
#include "midi/comm_midi.h"
#include "midi/midi_rx.h"
#include "midi/midi_sync.h"

#define MDT_HEADER              "MDT105"
#define STRING_EMPTY            ""
#define NTSC_ACTIVE_LINES       224
#define PAL_ACTIVE_LINES        224
#define NTSC_FPS                60
#define PAL_FPS                 50

#define TICK_SKIP_MIN           0x01 // fast tempo limit, 1 tick = H_INT_SKIP h-Blanks; 6
#define TICK_SKIP_MAX           0xFF // slow tempo limit; 128

u8 H_INT_CALLS_SKIP = 2; // 1 == no skip

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

u8 playingPatternRow = 0; // current played pattern row

s8 channelTranspose[CHANNELS_TOTAL];

u16 channelPlayingPatternID[CHANNELS_TOTAL];
s8 channelMatrixTranspose[CHANNELS_TOTAL];

u8 channelPreviousInstrument[CHANNELS_TOTAL];
u8 channelPreviousEffectType[CHANNELS_TOTAL][EFFECTS_TOTAL];
u8 channelPreviousNote[CHANNELS_TOTAL];
u8 channelArpSeqID[CHANNELS_TOTAL];
u8 channelArpSeqActive[CHANNELS_TOTAL];
u8 channelArpSeqPlayMODE[CHANNELS_TOTAL];
u8 channelArpSeqTriggerType[CHANNELS_TOTAL];
u8 channelParSeqID[CHANNELS_TOTAL];
u8 channelParSeqActive[CHANNELS_TOTAL];
u8 channelParSeqPlayMODE[CHANNELS_TOTAL];
u8 channelParSeqTYPE[CHANNELS_TOTAL];
u8 channelCurrentRowNote[CHANNELS_TOTAL]; // affected by channelMatrixTranspose
u8 channelNoteAutoCut[CHANNELS_TOTAL];
u8 channelNoteTriggerType[CHANNELS_TOTAL];

u8 channelSeqSkipStep[CHANNELS_TOTAL];
u8 channelArpSkipStep[CHANNELS_TOTAL];
u8 channelSeqSkipStepCounter[CHANNELS_TOTAL];
u8 channelArpSkipStepCounter[CHANNELS_TOTAL];

s8 channelSEQCounter_PAR[CHANNELS_TOTAL];
s8 channelSEQCounter_ARP[CHANNELS_TOTAL];

u8 channelRowShift[CHANNELS_TOTAL][PATTERN_ROWS];

u8 selectedInstrumentID = 1; // 0 instrument is empty
u8 selectedInstrumentParameter = 0; // 0..53
u8 selectedInstrumentOperator = 0; // 0..53

u8 seqArpValue[INSTRUMENTS_TOTAL][SEQ_STEP_LAST+1];
u8 seqParValue[INSTRUMENTS_TOTAL][SEQ_STEP_LAST+1];

// screen
u8 currentScreen = SCREEN_MATRIX;
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
u8 channelFineTune[CHANNELS_TOTAL];
s8 channelModNoteVibrato[CHANNELS_TOTAL];

u8 channelTremoloDepth[CHANNELS_TOTAL];
u8 channelTremoloSpeed[CHANNELS_TOTAL];
u8 channelTremoloSpeedMult[CHANNELS_TOTAL];
u16 channelTremoloPhase[CHANNELS_TOTAL];
u8 channelTremolo[CHANNELS_TOTAL];

u8 channelBaseVolume[CHANNELS_TOTAL];
u8 channelVolumeAttenuation[CHANNELS_TOTAL];
s16 channelAttenuation[CHANNELS_TOTAL];
u8 channelSlotBaseLevel[CHANNELS_TOTAL][4]; // need less
s16 channelVolumeChangeSpeed[CHANNELS_TOTAL];
u8 channelVolumePulseSkip[CHANNELS_TOTAL];
u8 channelVolumePulseCounter[CHANNELS_TOTAL];

u8 channelNoteCut[CHANNELS_TOTAL];
u8 channelNoteRetrigger[CHANNELS_TOTAL];
u8 channelNoteRetriggerCounter[CHANNELS_TOTAL];

u8 channelAutoGlide[CHANNELS_TOTAL];
u16 channelAutoGlideTicksLeft[CHANNELS_TOTAL];
s8 channelAutoGlideStartPitch[CHANNELS_TOTAL];

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
u8 FM_CH6_DAC_Pan = SOUND_PAN_CENTER;

// copy/paste

u16 hIntToSkip = 0;
u16 hIntCounter = 0;
bool bDoPulse = FALSE;

bool bBusTaken = FALSE;

//u32 frames_counter = 0; // to measure fps
u32 BPM = 0; // rough beats per minute
f32 fBPM = 0; // very bad precision
u8 useExternalSync = FALSE;

u8 patternSize = 0x1F;

// New expandable pattern storage
u16 patternOffset[PATTERN_LAST+1];     // SRAM offset of each pattern's block, relative to patternRegionBase (0=empty)
u8* editBuffer = NULL;                 // current pattern being edited (allocated on demand)
u16 patternEditID = 0xFFFF;            // which pattern is in editBuffer
u16 chEventIdx[CHANNELS_TOTAL];        // per-channel event read pointer during playback

// Expandable block variables
u32 instBlockEnd;                      // start of sequencers block = end of instruments
u32 seqBlockEnd;                       // start of patterns block = end of sequencers
u32 patternRegionBase;                 // = seqBlockEnd (cached)
u32 sramUsed;                          // total used SRAM bytes (from addr 0 through end of patterns)
u16 instDataAddr[INSTRUMENTS_TOTAL];   // SRAM addr of each modified instrument's compact record (0=ROM)
u16 seqDataAddr[INSTRUMENTS_TOTAL];    // SRAM addr of each modified sequencer's compact record (0=default)
u16 seqEditID = 0xFFFF;                // which instrument's SEQ is in seqEditBuffer (0xFFFF=none)
u8  seqEditBuffer[64];                 // SEQ edit buffer: [0..31] VOL, [32..63] ARP

// Maps SRAM INST_* param (0..48) to byte offset within Preset_FM struct for ROM-mode reads.
// Handles the OP1,OP3,OP2,OP4 ordering difference between Preset_FM and SRAM layout.
static const u8 paramToPresetByte[INST_DATA_SIZE] = {
     0, 4, 3, 2, 1,    // INST_ALG, FMS, AMS, PAN, FB
    14,36,25,47,       // INST_TL1..TL4 (OP1,OP3,OP2,OP4)
     8,30,19,41,       // INST_RS1..RS4
     5,27,16,38,       // INST_MUL1..MUL4
     6,28,17,39,       // INST_DT1..DT4
     7,29,18,40,       // INST_AR1..AR4
     9,31,20,42,       // INST_D1R1..D1R4
    11,33,22,44,       // INST_D1L1..D1L4
    12,34,23,45,       // INST_D2R1..D2R4
    13,35,24,46,       // INST_RR1..RR4
    10,32,21,43,       // INST_AM1..AM4
    15,37,26,48        // INST_SSGEG1..SSGEG4
};

Instrument tmpInst[INSTRUMENTS_TOTAL]; // cache instruments to RAM for faster access
Instrument chInst[CHANNELS_TOTAL]; // to apply commands; FM only

u8 midiPreset = 0;

/*
u16 msu_drv();
vu16 *mcd_cmd = (vu16 *) 0xA12010;  // command
vu32 *mcd_arg = (vu32 *) 0xA12012;  // argument
vu8 *mcd_cmd_ck = (vu8 *) 0xA1201F; // increment for command execution
vu8 *mcd_stat = (vu8 *) 0xA12020;   // Driver ready for commands processing when 0xA12020 sets to 0
u16 msu_resp;
*/

// static forward declarations (engine-internal)
static void DoEngine();
static void SetPitchFM(u8 mtxCh, u8 note);
static void SetPitchPSG(u8 mtxCh, u8 note);
static void PlayNote(u8 note, u8 mtxCh, u8 glide);
static void PlayNoteOff(u8 mtxCh);
static void StopChannelSound(u8 mtxCh);
static void StopAllSound();
static void StopEffects(u8 mtxCh);
static void SetGlobalLFO(u8 freq);
static void SetChannelVolume(u8 mtxCh);
static void SetChannelBaseVolume_FM(u8 mtxCh);
static void SetBPM(u16 tempo);
static void ProcessMidiSync();
static void WriteYM2612(u8 mtxCh);
static void CacheInstrumentToRAM(u8 id);
static void ApplyCommand_FM(u8 mtxCh, u8 id, u8 fxParam, u8 fxValue);
static void ApplyCommand_FM3_SP(u8 mtxCh, u8 fxParam, u8 fxValue);
static void ApplyCommand_DAC(u8 fxParam, u8 fxValue);
static void ApplyCommand_Common(u8 mtxCh, u8 fxParam, u8 fxValue);
static void ApplyCommand_PSG(u8 fxParam, u8 fxValue);
static void ChangeInstrumentParameter(s8 modifier, u8 changeAll);
static void ChangePatternParameter(s8 note, s8 par);
static void ChangeMatrixValue(s16 mod, u8 externalSync);
static void RequestZ80();
static void ReleaseZ80();
static void JoyEvent(u16 joy, u16 changed, u16 state);
static void YM2612_writeRegZ80(u16 part, u8 reg, u8 data);
static s16 FindUnusedPattern();
static void ReadMatrixRow();

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
        case SCREEN_MATRIX: DisplayPatternMatrix(); ReColorsAndTranspose(); break;
        case SCREEN_PATTERN: DisplayPatternEditor(); break;
        case SCREEN_INSTRUMENT: DisplayInstrumentEditor(); break;
        default: break;
        }
    }
	return(0);
}

/*FORCE_INLINE static inline void CountPulses()
{
    hIntCounter--;
    if (!hIntCounter)
    {
        bDoPulse = TRUE;
        hIntCounter = hIntToSkip;
    }
}*/

HINTERRUPT_CALLBACK hIntCallback()
{
    //CountPulses();
    hIntCounter++;
}

void vIntCallback()
{
    //VDP_setHInterrupt(FALSE);
    SYS_doVBlankProcessEx(IMMEDIATELY);
    //CountPulses();

    /*
    static u8 _vInts_counter;
    _vInts_counter++;
    if (_vInts_counter > 59) // measure every second
    {
        if (frames_counter < 1000)
        {
            uintToStr(frames_counter, str, 3);

            if (frames_counter < PPS)
            {
                DrawNum(BG_A, PAL0, str, 15, 27);
                DrawNum(BG_A, PAL0, str, 55, 27);
            }
            else
            {
                DrawNum(BG_A, PAL1, str, 15, 27);
                DrawNum(BG_A, PAL1, str, 55, 27);
            }
        }
        frames_counter = 0;
        _vInts_counter = 0;
    }*/

    // slower GUI updates. not helping much
    /*switch (currentScreen)
    {
    case SCREEN_MATRIX: DisplayPatternMatrix(); break;
    case SCREEN_PATTERN: DisplayPatternEditor(); break;
    case SCREEN_INSTRUMENT: DisplayInstrumentEditor(); break;
    default: break;
    }*/

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
            case SCREEN_PATTERN: NavigatePattern(navigationDirection); PrintSelectedPositionInfo(); break;
            case SCREEN_INSTRUMENT: NavigateInstrument(navigationDirection); break;
            default: break;
            }
        }
    }

    //VDP_setHInterrupt(TRUE);
}

// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------
static void DoEngine()
{
    static u8 _seqValue = 0;
    static u8 _fxType = 0;
    static u8 _fxValue = 0;
    static bool _bBeginPlay = TRUE;
    static u8 _inst = 0;
    static s16 _key = 0;
    static s8 _test = 0;
    static s16 _oldNote = 0;

    // vibrato tool
    auto s8 vibrato(u8 mtxCh) {
        static s8 vib = 0;
        switch (channelVibratoMode[mtxCh])
        {
        case 1:
            vib = abs((s8)F16_toRoundedInt(F16_mul(FIX16(channelVibratoDepth[mtxCh]), sinFix16(channelVibratoPhase[mtxCh]))));
            break;
        case 2:
            vib = -abs((s8)F16_toRoundedInt(F16_mul(FIX16(channelVibratoDepth[mtxCh]), sinFix16(channelVibratoPhase[mtxCh]))));
            break;
        default:
            vib = (s8)F16_toRoundedInt(F16_mul(FIX16(channelVibratoDepth[mtxCh]), sinFix16(channelVibratoPhase[mtxCh])));
            break;
        }

        channelVibratoPhase[mtxCh] += channelVibratoSpeed[mtxCh];
        if (channelVibratoPhase[mtxCh] > 1023) channelVibratoPhase[mtxCh] -= 1024;

        return vib;
    }

    /*auto void pitch_slide(u8 mtxCh)
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
    }*/

    auto void seq_par(u8 mtxCh) {
        if (!channelParSeqActive[mtxCh]) return;
        if (!channelParSeqPlayMODE[mtxCh] || (channelParSeqPlayMODE[mtxCh] && (channelSEQCounter_PAR[mtxCh] <= SEQ_STEP_LAST)))
        {
            if (!channelSeqSkipStepCounter[mtxCh]) { channelSeqSkipStepCounter[mtxCh] = channelSeqSkipStep[mtxCh]; }
            else { channelSeqSkipStepCounter[mtxCh]--; return; }

            _seqValue = seqParValue[channelParSeqID[mtxCh]][(u8)channelSEQCounter_PAR[mtxCh]]; channelSEQCounter_PAR[mtxCh]++;

            if (_seqValue != SEQ_SKIP)
            {
                switch (channelParSeqTYPE[mtxCh])
                {
                case SEQ_TYPE_VOL: // channel volume attenuation
                    channelVolumeAttenuation[mtxCh] = _seqValue;
                    SetChannelVolume(mtxCh);
                    break;
                case SEQ_TYPE_MICROTONE: // channel micro-tone fine tune. will work only on new note triggered
                    if (_seqValue < 0x20) channelFineTune[mtxCh] = _seqValue;
                    break;
                default:
                    if (mtxCh < CHANNEL_PSG1) // FM commands only
                    {
                        bWriteRegs = TRUE;
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], channelParSeqTYPE[mtxCh], _seqValue);
                    }
                    break;
                }

                /*if (channelParSeqTYPE[mtxCh] == SEQ_TYPE_VOL) // common FM/PSG volume attenuation
                {
                    channelSeqAttenuation[mtxCh] = _seqValue;
                    SetChannelVolume(mtxCh);
                }
                else if (channelParSeqTYPE[mtxCh] == SEQ_TYPE_MICROTONE)
                {
                    channelMicrotone[mtxCh] = _seqValue;
                }
                else if (mtxCh < CHANNEL_PSG1) // FM commands only
                {
                    bWriteRegs = TRUE;
                    ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], channelParSeqTYPE[mtxCh], _seqValue);
                }*/
            }
        }
        if (!channelParSeqPlayMODE[mtxCh] && (channelSEQCounter_PAR[mtxCh] > SEQ_STEP_LAST)) channelSEQCounter_PAR[mtxCh] = 0;
    }

    auto void seq_arp_modify_key(u8 mtxCh)
    {
        if (!channelArpSeqActive[mtxCh]) return;
        _seqValue = seqArpValue[channelArpSeqID[mtxCh]][0];
        if (_seqValue > ARP_BASE) _key = channelCurrentRowNote[mtxCh] + (_seqValue - ARP_BASE);
        else if (_seqValue < ARP_BASE) _key = channelCurrentRowNote[mtxCh] - (ARP_BASE - _seqValue);
        if (_key < 0 || _key > NOTE_MAX || _seqValue == ARP_BASE) _key = channelCurrentRowNote[mtxCh];
    }

    auto void seq_arp(u8 mtxCh) {
        if (!channelArpSeqActive[mtxCh]) return;
        if (channelSEQCounter_ARP[mtxCh] < 0)
        {
            //channelSEQCounter_ARP[mtxCh]++;
            return;
        }

        if (!channelArpSeqPlayMODE[mtxCh] || (channelArpSeqPlayMODE[mtxCh] && (channelSEQCounter_ARP[mtxCh] <= SEQ_STEP_LAST)))
        {
            // make seq slower if set
            if (!channelArpSkipStepCounter[mtxCh]) { channelArpSkipStepCounter[mtxCh] = channelArpSkipStep[mtxCh]; }
            else { channelArpSkipStepCounter[mtxCh]--; return; }

            _seqValue = seqArpValue[channelArpSeqID[mtxCh]][(u8)channelSEQCounter_ARP[mtxCh]]; channelSEQCounter_ARP[mtxCh]++;

            if (!pulseCounter) // first tick only
            {
                if (_seqValue != NOTE_EMPTY)
                {
                    if (_seqValue > ARP_BASE) _key = channelPreviousNote[mtxCh] + (_seqValue - ARP_BASE);
                    else if (_seqValue < ARP_BASE) _key = channelPreviousNote[mtxCh] - (ARP_BASE - _seqValue);
                    if (_key < 0 || _key > NOTE_MAX || _seqValue == ARP_BASE) _key = channelPreviousNote[mtxCh];

                    if (channelCurrentRowNote[mtxCh] == NOTE_EMPTY)
                        PlayNote(_key, mtxCh, channelArpSeqTriggerType[mtxCh]);
                }
            }
            else // other ticks
            {
                if (_seqValue != NOTE_EMPTY)
                {
                    if (_seqValue > ARP_BASE) channelArp[mtxCh] = channelPreviousNote[mtxCh] + (_seqValue - ARP_BASE);
                    else if (_seqValue < ARP_BASE) channelArp[mtxCh] = channelPreviousNote[mtxCh] - (ARP_BASE - _seqValue);
                    if (channelArp[mtxCh] < 0 || channelArp[mtxCh] > NOTE_MAX || _seqValue == ARP_BASE)
                        channelArp[mtxCh] = channelPreviousNote[mtxCh];
                    PlayNote(channelArp[mtxCh], mtxCh, channelArpSeqTriggerType[mtxCh]);
                }
            }
        }
        // loop mode
        if (!channelArpSeqPlayMODE[mtxCh] && (channelSEQCounter_ARP[mtxCh] > SEQ_STEP_LAST)) channelSEQCounter_ARP[mtxCh] = 0;
    }

    auto void do_row(u8 mtxCh) {
        u8 _rowData[PATTERN_COLUMNS];
        if (channelFlags[mtxCh])
        {
            SRAM_ReadRowToBuffer(channelPlayingPatternID[mtxCh], playingPatternRow, _rowData);

            // detect per-row ARP (0x30) and VOL (0x40) effects for auto-clearing
            u8 hasArpOnRow = 0, hasParOnRow = 0;
            for (u8 _eff = 0; _eff < EFFECTS_TOTAL; _eff++)
            {
                u8 _ft = _rowData[DATA_FX1_TYPE + _eff*2];
                u8 _fv = _rowData[DATA_FX1_VALUE + _eff*2];
                if (_ft == 0x30 || (!_ft && _fv && channelPreviousEffectType[mtxCh][_eff] == 0x30)) hasArpOnRow = 1;
                if (_ft == 0x40 || (!_ft && _fv && channelPreviousEffectType[mtxCh][_eff] == 0x40)) hasParOnRow = 1;
            }

            auto void command(u8 type, u8 val, u8 effect) { // slow!
                //if (channelDoEffects[mtxCh])
                //{
                    _fxType = _rowData[type];
                    _fxValue = _rowData[val];
                //}

                if (_fxType)
                {
                    channelPreviousEffectType[mtxCh][effect] = _fxType;
                    switch (mtxCh)
                    {
                    case CHANNEL_FM1: case CHANNEL_FM2: case CHANNEL_FM4: case CHANNEL_FM5:
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], _fxType, _fxValue);
                        break;
                    case CHANNEL_FM6_DAC:
                        //ApplyCommand_DAC(_fxType, _fxValue);
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], _fxType, _fxValue);
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
                    ApplyCommand_DAC(_fxType, _fxValue); // to be able to play multichannel PCM on any channel
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
                        //if (Z80_getLoadedDriver() == Z80_DRIVER_PCM4) pcmNote[0] = pcmNote[1] = pcmNote[2] = pcmNote[3] = NOTE_EMPTY;
                        ApplyCommand_DAC(channelPreviousEffectType[CHANNEL_FM6_DAC][effect], _fxValue);
                        //if (!bDAC_enable)
                        ApplyCommand_FM(mtxCh, channelPreviousInstrument[mtxCh], channelPreviousEffectType[mtxCh][effect], _fxValue);
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

            auto void apply_commands() { // slow!
                command(DATA_FX1_TYPE, DATA_FX1_VALUE, 0);
                command(DATA_FX2_TYPE, DATA_FX2_VALUE, 1);
                command(DATA_FX3_TYPE, DATA_FX3_VALUE, 2);

                command(DATA_FX4_TYPE, DATA_FX4_VALUE, 3);
                command(DATA_FX5_TYPE, DATA_FX5_VALUE, 4);
                command(DATA_FX6_TYPE, DATA_FX6_VALUE, 5);
            }

            _inst = _rowData[DATA_INSTRUMENT];

            channelCurrentRowNote[mtxCh] = _rowData[DATA_NOTE];

            // auto cut note before next note
            if (channelNoteAutoCut[mtxCh])
            {
                if (playingPatternRow == PATTERN_ROW_LAST)
                {
                    if (SRAM_ReadPatternFromEvents(SRAM_ReadMatrix(mtxCh, playingMatrixRow+1) & 0x3FF, 0, DATA_NOTE) < NOTES)
                    {
                        channelNoteCut[mtxCh] = channelNoteAutoCut[mtxCh];
                    } else channelNoteCut[mtxCh] = 0;
                }
                else
                {
                    if (SRAM_ReadPatternFromEvents(channelPlayingPatternID[mtxCh], playingPatternRow+1, DATA_NOTE) < NOTES)
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
                    channelArpSeqID[mtxCh] = channelParSeqID[mtxCh] = _inst; // set seq for PSG as instrument
                    channelArpSeqActive[mtxCh] = channelParSeqActive[mtxCh] = 1;
                    apply_commands(); // will override PSG seq
                }
            }
            else // do effects
            {
                bWriteRegs = TRUE; // write commands regs
                apply_commands();
            }

            // clear per-row sequencer flags when command absent (FM only; PSG uses instrument default)
            if (!hasArpOnRow && mtxCh < CHANNEL_PSG1) { channelArpSeqActive[mtxCh] = 0; channelSEQCounter_ARP[mtxCh] = 0; }
            if (!hasParOnRow && mtxCh < CHANNEL_PSG1) { channelParSeqActive[mtxCh] = 0; channelSEQCounter_PAR[mtxCh] = 0; }

            // --------- trigger note playback; check empty note later; pass note id: 0..95, 254, 255
            /*if (mtxCh == CHANNEL_FM6_DAC &&
                bDAC_enable &&
                Z80_getLoadedDriver() == Z80_DRIVER_PCM4 &&
                (pcmNote[0] != NOTE_EMPTY ||
                 pcmNote[1] != NOTE_EMPTY ||
                 pcmNote[2] != NOTE_EMPTY ||
                 pcmNote[3] != NOTE_EMPTY) &&
                channelCurrentRowNote[mtxCh] != NOTE_OFF)
                    channelCurrentRowNote[mtxCh] = 0; // default C-0*/

            if (channelCurrentRowNote[mtxCh] == NOTE_OFF || instrumentIsMuted[channelPreviousInstrument[mtxCh]] == INST_MUTE) // there is OFF
            {
                channelPreviousNote[mtxCh] = channelArp[mtxCh] = NOTE_OFF;
                //if (channelNoteRetrigger[mtxCh])
                //{
                    //channelNoteRetriggerCounter[mtxCh] =
                    channelNoteRetrigger[mtxCh] = 0;
                //}
                if (channelRowShift[mtxCh][playingPatternRow]) channelNoteDelayCounter[mtxCh] = channelRowShift[mtxCh][playingPatternRow];
                PlayNoteOff(mtxCh);
            }
            else if (channelCurrentRowNote[mtxCh] < NOTES) // there is a note on a row
            {
                _oldNote = channelPreviousNote[mtxCh];

                if (channelMatrixTranspose[mtxCh] || channelTranspose[mtxCh])
                {
                    _test = channelCurrentRowNote[mtxCh] + channelMatrixTranspose[mtxCh] + channelTranspose[mtxCh]; // check if out of notes range
                    if (_test < NOTES || _test > -1)
                    {
                        _key = channelPreviousNote[mtxCh] = channelArp[mtxCh] = channelCurrentRowNote[mtxCh] = _test;
                    }
                } else _key = channelPreviousNote[mtxCh] = channelArp[mtxCh] = channelCurrentRowNote[mtxCh];

                // auto glide: if active and valid previous note, start gliding from old pitch
                if (channelAutoGlide[mtxCh] && _oldNote < NOTES)
                {
                    channelModNotePitch[mtxCh] = (s8)(_oldNote - _key);
                    channelAutoGlideStartPitch[mtxCh] = channelModNotePitch[mtxCh];
                    channelAutoGlideTicksLeft[mtxCh] = channelAutoGlide[mtxCh] + 1;
                }

                // shift row playback by some pulses if set on channel. re-trigger will ignore it
                if (channelRowShift[mtxCh][playingPatternRow] && !channelNoteRetrigger[mtxCh])
                    channelNoteDelayCounter[mtxCh] = channelRowShift[mtxCh][playingPatternRow];

                seq_arp_modify_key(mtxCh);
                channelSEQCounter_PAR[mtxCh] = channelSEQCounter_ARP[mtxCh] = -channelNoteDelayCounter[mtxCh];

                if (!channelNoteDelayCounter[mtxCh] && !channelNoteRetriggerCounter[mtxCh]) // re-triggered from do_effects
                {
                    //if (instrumentIsMuted[_inst] == INST_MUTE) _key = NOTE_OFF;
                    //channelSEQCounter_PAR[mtxCh] = channelSEQCounter_ARP[mtxCh] = 0; // restart SEQ counters on new note
                    PlayNote((u8)_key, mtxCh, channelNoteTriggerType[mtxCh]);
                }
                /*else
                {
                    channelSEQCounter_PAR[mtxCh] = channelSEQCounter_ARP[mtxCh] = -channelNoteDelayCounter[mtxCh];
                }*/
            }
            else //if (channelCurrentRowNote[mtxCh] == NOTE_EMPTY) // no note
            {
                //channelPreviousNote[mtxCh] = _key;
                //_key = NOTE_EMPTY; // empty row
                //seq_par(mtxCh); seq_arp(mtxCh); // prepare _key
                //PlayNote(30, mtxCh, channelNoteTriggerType[mtxCh]); //?
            }
        }
    }

    auto void do_effects(u8 mtxCh) {
        if (/*channelDoEffects[mtxCh] && */channelFlags[mtxCh] && channelPreviousNote[mtxCh] != NOTE_OFF)
        //if (channelFlags[mtxCh] && channelPreviousNote[mtxCh] != NOTE_OFF)
        {
            //if (pulseCounter) // only do at sub-pulses; 0th seq pulse handled in do_row to modify note before it's played
            //{
                seq_par(mtxCh); seq_arp(mtxCh);
            //}

            if (channelNoteRetrigger[mtxCh])
            {
                if (channelNoteRetriggerCounter[mtxCh] == channelNoteRetrigger[mtxCh])
                {
                    channelNoteRetriggerCounter[mtxCh] = 0;
                    //channelNoteDelayCounter[mtxCh] = 0; // disable delay
                    channelSEQCounter_PAR[mtxCh] = 0; channelSEQCounter_ARP[mtxCh] = 0;
                    PlayNote(channelPreviousNote[mtxCh], mtxCh, channelNoteTriggerType[mtxCh]);
                }
                channelNoteRetriggerCounter[mtxCh]++;
            }
            //! delay (not works with seq)
            else if (channelNoteDelayCounter[mtxCh])
            {
                /*if (channelNoteDelayCounter[mtxCh] == 1)
                {
                    channelNoteDelayCounter[mtxCh] = 0;
                    channelSEQCounter_PAR[mtxCh] = channelSEQCounter_ARP[mtxCh] = 0;
                    PlayNote(channelPreviousNote[mtxCh], mtxCh, channelNoteTriggerType[mtxCh]);
                } else channelNoteDelayCounter[mtxCh]--;*/

                channelNoteDelayCounter[mtxCh]--;
                if (!channelNoteDelayCounter[mtxCh])
                {
                    channelSEQCounter_PAR[mtxCh] = channelSEQCounter_ARP[mtxCh] = 0;
                    PlayNote(channelPreviousNote[mtxCh], mtxCh, channelNoteTriggerType[mtxCh]);
                }
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
                channelTremolo[mtxCh] = (u8)F16_toRoundedInt
                (
                    F16_mul(FIX16(channelTremoloDepth[mtxCh]), (cosFix16(channelTremoloPhase[mtxCh]) + FIX16(1)) >> 1)
                );

                channelTremoloPhase[mtxCh] += channelTremoloSpeed[mtxCh];
                if (channelTremoloPhase[mtxCh] > 1023) channelTremoloPhase[mtxCh] -= 1024;
                if (!pulseCounter && channelCurrentRowNote[mtxCh] != NOTE_EMPTY) channelTremoloPhase[mtxCh] = TREMOLO_PHASE;
                SetChannelVolume(mtxCh);
            }

            // pitch effects
            if (channelPitchSlideSpeed[mtxCh] || (channelVibratoDepth[mtxCh] && channelVibratoSpeed[mtxCh]) || channelAutoGlideTicksLeft[mtxCh])
            {
                // auto glide
                if (channelAutoGlideTicksLeft[mtxCh] && !channelNoteDelayCounter[mtxCh])
                {
                    channelAutoGlideTicksLeft[mtxCh]--;
                    if (channelAutoGlideTicksLeft[mtxCh])
                        channelModNotePitch[mtxCh] = (channelAutoGlideStartPitch[mtxCh] * channelAutoGlideTicksLeft[mtxCh]) / channelAutoGlide[mtxCh];
                    else channelModNotePitch[mtxCh] = 0;
                }

                // pitch slide
                //pitch_slide(mtxCh);
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

    ProcessMidiSync();

    if (bPlayback)
    {
        if (_bBeginPlay)
        {
            //ssf_led_on();
            SYS_disableInts();
            _bBeginPlay = FALSE;
            ReadMatrixRow();

            // set instruments
            for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
            {
                if (mtxCh < CHANNEL_PSG1) chInst[mtxCh] = tmpInst[channelPreviousInstrument[mtxCh]];
                else
                {
                    channelArpSeqID[mtxCh] = channelParSeqID[mtxCh] = channelPreviousInstrument[mtxCh];
                    channelArpSeqActive[mtxCh] = channelParSeqActive[mtxCh] = 1;
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
                        u16 _pattern = SRAM_ReadMatrix(_ch, rowM) & 0x3FF;
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
                            ApplyCommand_FM(_ch, channelPreviousInstrument[_ch],
                                            channelPreviousEffectType[_ch][_eff],
                                            channelPreviousEffectValue[_ch][_eff]);
                            ApplyCommand_PSG(channelPreviousEffectType[_ch][_eff], channelPreviousEffectValue[_ch][_eff]);
                        }
                    }
                } // channel
            }*/

            // bb: Mode, ResetB ResetA, EnableB EnableA, LoadB LoadA
            //YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, FM_CH3_Mode | 0b00000000);
            // set frame length
            maxPulse = (playingPatternRow & 1)? ppl_1 : ppl_2;

            DrawMatrixPlaybackCursor(FALSE);
            hIntCounter = 0;// hIntToSkip * !useExternalSync; // reset h-int counter
            bDoPulse = FALSE; // do not trigger external pulse if button was pressed before playback start

            pulseCounter = 0;
            matrixRowJumpTo = OXFF;
            patternRowJumpTo = OXFF;

            VDP_setTextPalette(PAL0); VDP_drawTextBG(BG_B, "PLAY", 29, 27); VDP_drawTextBG(BG_B, "PLAY", 69, 27);

            SYS_enableInts();
            VDP_setHInterrupt(!useExternalSync && midi_sync_get_mode() == MIDI_SYNC_OFF);
        }

        if (hIntCounter >= hIntToSkip || bDoPulse)
        {
            bDoPulse = FALSE;
            hIntCounter = 0;

            if (pulseCounter == 0) // row first pulse; prepare command, note, instrument, draw cursor
            {

                //BPM = ((getTimer(1, TRUE) * 240) / 18432); DrawBPM();

                // main slowdown!
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

                if (matrixRowJumpTo != OXFF && currentScreen == SCREEN_MATRIX) // is jump command is set while on matrix screen
                {
                    playingPatternRow = 0x20; // overflow to trigger next condition
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

                    //BPM = ((getTimer(1, TRUE) / 32 * 240) / 18432); DrawBPM();
                }

                if (currentScreen == SCREEN_PATTERN) DrawPatternPlaybackCursor();
            }

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

            pulseCounter++; // count row sub-pulses

            if (pulseCounter >= maxPulse)
            {
                maxPulse = (playingPatternRow & 1)? ppl_1 : ppl_2;
                pulseCounter = 0;
            }
        }
    }
    else if (!_bBeginPlay) // need to run only once at playback stopped
    {
        _bBeginPlay = TRUE;
        //ssf_led_off();

        SYS_disableInts();
        // bb: Mode, ResetB ResetA, EnableB EnableA, LoadB LoadA
        //YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_TIMERS, CH3_NORMAL | 0b00000000);
        StopAllSound();
        ClearPatternPlaybackCursor();
        DrawMatrixPlaybackCursor(TRUE);

        VDP_setTextPalette(PAL0); VDP_drawTextBG(BG_B, "    ", 29, 27); VDP_drawTextBG(BG_B, "    ", 69, 27);

        SYS_enableInts();
        VDP_setHInterrupt(FALSE);
    }
}

static void ReadMatrixRow()
{
    for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++)
    {
        u16 combined = SRAM_ReadMatrix(mtxCh, playingMatrixRow);
        channelPlayingPatternID[mtxCh] = combined & 0x3FF;
        u8 raw = (combined >> 10) & 0x3F;
        channelMatrixTranspose[mtxCh] = (s8)(raw >= 32 ? raw - 64 : raw);
    }
}

static s16 FindUnusedPattern()
{
    static bool used = FALSE;
    u8 x = selectedMatrixChannel * 3 + 1;
    u8 y = selectedMatrixScreenRow + 2;

    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), x, y);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[2] + GUI_BIGDOT), x+1, y);

    for (s16 pattern = 1; pattern <= PATTERN_LAST; pattern++) {
        used = FALSE;
        for (u8 mtxCh = 0; mtxCh < CHANNELS_TOTAL; mtxCh++) {
            for (u8 line = 0; line < MATRIX_ROWS; line++) { if ((SRAM_ReadMatrix(mtxCh, line) & 0x3FF) == pattern) { used = TRUE; break; } }
            if (used) break;
        }
        if (!used) return pattern;
    }
    return NULL;
}

static void SetBPM(u16 tempo)
{
    if (!tempo)
    {
        hIntToSkip = SRAMW_readWord(SRAM_TEMPO);
    }
    else
    {
        SRAMW_writeWord(SRAM_TEMPO, tempo); // store
        hIntToSkip = tempo;
    }

    u16 activeLines = IS_PAL_SYSTEM ? PAL_ACTIVE_LINES : NTSC_ACTIVE_LINES;
    u16 fps = IS_PAL_SYSTEM ? PAL_FPS : NTSC_FPS;
    u8 hIntPerFrame = activeLines / H_INT_CALLS_SKIP;
    u16 hIntRate = hIntPerFrame * fps;
    u8 ppb = (ppl_1 + ppl_2) * 2;
    u16 pb = hIntToSkip * ppb;

    u32 bpmX100 = (u32)60 * hIntRate * 100 / pb;
    BPM = bpmX100 / 100;
    fBPM = F32_div(FIX32(bpmX100 % 100), FIX32(100));

    //PPS = (((BPM * 1000) / 6) * ppb) / 10000; // pulse per second
    DrawInfo();
}

static void ProcessMidiSync()
{
    MidiSyncMode mode = midi_sync_get_mode();
    if (mode == MIDI_SYNC_OFF)
        return;

    MidiTransportCmd cmd = midi_sync_get_transport();
    switch (cmd) {
    case MIDI_TRANSPORT_START:
        if (!bPlayback) {
            playingPatternRow = 0;
            playingMatrixRow = selectedMatrixRow;
            bPlayback = TRUE;
        }
        break;
    case MIDI_TRANSPORT_STOP:
        if (bPlayback) {
            pulseCounter = 0;
            bPlayback = FALSE;
        }
        break;
    case MIDI_TRANSPORT_CONTINUE:
        if (!bPlayback) {
            bPlayback = TRUE;
        }
        break;
    default:
        break;
    }

    midi_rx_process();
}

static void ChangeMatrixValue(s16 mod, u8 externalSync)
{
    static s32 value = 0;

    if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE) // matrix
    {
        if (mod)
        {
            value = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
            if (!value && lastEnteredPattern) value = lastEnteredPattern;
            else
            {
                value += mod;
                if (value < 1) value = PATTERN_LAST; // last pattern
                else if (value > PATTERN_LAST) value = 1; // first pattern
            }
            SRAM_WritePatternID(selectedMatrixChannel, selectedMatrixRow, (u16)value); bRefreshScreen = TRUE; matrixRowToRefresh = selectedMatrixRow;
            lastEnteredPattern = value;
        }
        else
        {
            value = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
            if (value) lastEnteredPattern = (u16)value; // accidential delete
            SRAM_WriteMatrix(selectedMatrixChannel, selectedMatrixRow, 0); bRefreshScreen = TRUE; matrixRowToRefresh = selectedMatrixRow;
        }
    }
    else // tempo
    {
        if (mod) // direction button: adjust tempo, disable sync
        {
            value = SRAMW_readWord(SRAM_TEMPO) - mod;
            if (value < TICK_SKIP_MIN) value = TICK_SKIP_MIN;
            else if (value > TICK_SKIP_MAX) value = TICK_SKIP_MAX;
            useExternalSync = FALSE;
            midi_sync_set_mode(MIDI_SYNC_OFF);
            SetBPM((u16)value);
        }
        else if (externalSync) // C button: cycle sync mode
        {
            MidiSyncMode m = midi_sync_get_mode();
            if (useExternalSync) {
                useExternalSync = FALSE;
                midi_sync_set_mode(MIDI_SYNC_CLOCK);
            } else if (m == MIDI_SYNC_CLOCK) {
                midi_sync_set_mode(MIDI_SYNC_NOTE);
            } else if (m == MIDI_SYNC_NOTE) {
                midi_sync_set_mode(MIDI_SYNC_OFF);
            } else {
                useExternalSync = TRUE;
            }
            DrawInfo();
            return;
        }
    }
}

static void LoadPreset(u8 id, u8 preset)
{
    SRAM_ResetInstrumentToPreset(id, preset);
}

// gamepad interrupts handler
static void JoyEvent(u16 joy, u16 changed, u16 state)
{
    static u8 patternColumnShift = 0;
    static s8 inc = 0; // paste increment
    static u8 row = 0; // paste row to
    static s8 col = 0; // pattern color slot
    static s8 transpose = 0; // matrix slot transpose

    // selection transpose
    u8 _rangeStart = 0;
    u8 _rangeEnd = PATTERN_ROWS;

    u8 muted = 0;

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
        CommitSeqEditBuffer();

        selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
        if (selectedPatternID != 0x00) // -- pattern should not be editable
        {
            // Unpack pattern from SRAM event stream to RAM edit buffer
            SRAM_UnpackToBuffer(selectedPatternID);

            currentScreen = SCREEN_PATTERN;
            bInitScreen = TRUE;
            bRefreshScreen = TRUE;
            VDP_setHorizontalScroll(BG_A, -320);
            VDP_setHorizontalScroll(BG_B, -320);
        }
    }

    auto void switch_to_instrument_editor()
    {
        // Commit editing pattern back to SRAM
        if (patternEditID != 0xFFFF)
        {
            SRAM_CommitBuffer(patternEditID);
            patternEditID = 0xFFFF;
        }

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

        LoadSeqEditBuffer(selectedInstrumentID);
    }

    auto void switch_to_matrix_editor()
    {
        CommitSeqEditBuffer();

        // Commit editing pattern back to SRAM
        if (patternEditID != 0xFFFF)
        {
            SRAM_CommitBuffer(patternEditID);
            patternEditID = 0xFFFF;
        }

        selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
        currentScreen = SCREEN_MATRIX;
        bInitScreen = TRUE;
        bRefreshScreen = TRUE;
        VDP_setHorizontalScroll(BG_A, 0);
        VDP_setHorizontalScroll(BG_B, 0);
        matrixRowToRefresh = OXFFFF;
    }

    auto void set_range()
    {
        if (patternCopyRangeStart != NOTHING)
        {
            _rangeStart = patternCopyRangeStart;
            _rangeEnd = patternCopyRangeEnd;
        }
    }

    if (joy == JOY_1) // joy 2 is for midi sync
    {
        switch (state)
        {
        case BUTTON_START:
            if (bPlayback == FALSE) // play from beginning
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
            if (bPlayback == FALSE && !useExternalSync && midi_sync_get_mode() == MIDI_SYNC_OFF) // play from current line
            {
                pulseCounter = 0;
                if (selectedPatternColumn < PATTERN_COLUMNS) playingPatternRow = selectedPatternRow; // start from the current selected pattern line
                else playingPatternRow = selectedPatternRow + PATTEN_ROWS_PER_SIDE;
                playingMatrixRow = selectedMatrixRow; // actual line in array
                bPlayback = TRUE;
            }
            else
            {
                bDoPulse = (useExternalSync && midi_sync_get_mode() == MIDI_SYNC_OFF); // external sync from gamepad;
            }
            break;

        /*case BUTTON_B:
            bDoPulse = useExternalSync;
            break;*/
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
                    selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF; // select current pattern
                    if (selectedPatternID != NULL)
                    {
                        col = SRAM_ReadPatternColor(selectedPatternID)-1;
                        if (col < 0) col = GUI_PATTERN_COLORS_MAX;
                        SRAM_WritePatternColor(selectedPatternID, col);
                        RedrawMarks();
                    }
                    break;

                case BUTTON_RIGHT:
                    selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
                    if (selectedPatternID != NULL)
                    {
                        col = SRAM_ReadPatternColor(selectedPatternID)+1;
                        if (col > GUI_PATTERN_COLORS_MAX) col = 0;
                        SRAM_WritePatternColor(selectedPatternID, col);
                        RedrawMarks();
                    }
                    break;

                case BUTTON_UP: // matrix transpose
                    transpose = SRAM_ReadMatrixTranspose(selectedMatrixChannel, selectedMatrixRow);
                    if (transpose < 12)
                    {
                        transpose++; SRAM_WriteMatrixTranspose(selectedMatrixChannel, selectedMatrixRow, transpose);
                        RedrawMarks();
                    }
                    break;

                case BUTTON_DOWN:
                    transpose = SRAM_ReadMatrixTranspose(selectedMatrixChannel, selectedMatrixRow);
                    if (transpose > -12)
                    {
                        transpose--; SRAM_WriteMatrixTranspose(selectedMatrixChannel, selectedMatrixRow, transpose);
                        RedrawMarks();
                    }
                    break;

                case BUTTON_C: // clear
                    selectedPatternID = SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF;
                    if (selectedPatternID != NULL)
                    {
                        col = SRAM_ReadPatternColor(selectedPatternID);
                        if (col != 0)
                        {
                            SRAM_WritePatternColor(selectedPatternID, 0);
                        RedrawMarks();
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
                        RedrawMarks();
                    }
                    break;

                case BUTTON_LEFT:
                    if (currentPage > 0)
                    {
                        currentPage--;
                        bRefreshScreen = bInitScreen = TRUE;
                        matrixRowToRefresh = OXFFFF;
                        RedrawMarks();
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
                    if (!(SRAM_ReadMatrix(selectedMatrixChannel, selectedMatrixRow) & 0x3FF))
                    {
                        u16 value = FindUnusedPattern();
                        if (value)
                        {
                            SRAM_WritePatternID(selectedMatrixChannel, selectedMatrixRow, value);
                            bRefreshScreen = TRUE;
                            matrixRowToRefresh = selectedMatrixRow;
                            lastEnteredPattern = value;
                        }
                    }
                    break;
                case BUTTON_RIGHT: ChangeMatrixValue(1, FALSE);
                    break;
                case BUTTON_LEFT: ChangeMatrixValue(-1, FALSE);
                    break;
                case BUTTON_UP: ChangeMatrixValue(16, FALSE);
                    break;
                case BUTTON_DOWN: ChangeMatrixValue(-16, FALSE);
                    break;
                }
                break;

            case BUTTON_B:

                if (bPlayback) break; // no adding/removing rows during playback allowed

                switch (changed)
                {
                case BUTTON_UP: // delete selected matrix row
                    if (selectedMatrixScreenRow < MATRIX_ROWS_ONPAGE)
                    {
                        if (selectedMatrixRow == 0) break;
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
                        if (selectedMatrixRow == 0) break;
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
                ChangeMatrixValue(0, TRUE); // cycle sync mode
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
                PrintSelectedPositionInfo();
                break;

            case BUTTON_A:
                // A + D-Pad: change note; instrument +- 1|16;  effect type/value +- 1|16
                switch (changed)
                {
                // modify selected parameter
                case BUTTON_RIGHT:
                    ChangePatternParameter(1, 1);
                    PrintSelectedPositionInfo();
                    break;

                case BUTTON_LEFT:
                    ChangePatternParameter(-1, -1);
                    PrintSelectedPositionInfo();
                    break;

                case BUTTON_UP:
                    ChangePatternParameter(12, 16);
                    PrintSelectedPositionInfo();
                    break;

                case BUTTON_DOWN:
                    ChangePatternParameter(-12, -16);
                    PrintSelectedPositionInfo();
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
                        PrintSelectedPositionInfo();
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
                                }
                                break;

                            case DATA_FX4_VALUE: case (DATA_FX4_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX4_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX4_VALUE)); inc++;
                                }
                                break;

                            case DATA_FX5_TYPE: case (DATA_FX5_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX5_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX5_TYPE)); inc++;
                                }
                                break;

                            case DATA_FX5_VALUE: case (DATA_FX5_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX5_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX5_VALUE)); inc++;
                                }
                                break;

                            case DATA_FX6_TYPE: case (DATA_FX6_TYPE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX6_TYPE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX6_TYPE)); inc++;
                                }
                                break;

                            case DATA_FX6_VALUE: case (DATA_FX6_VALUE + PATTERN_COLUMNS):
                                row = selectedPatternRow + patternColumnShift + inc;
                                if (row <= PATTERN_ROW_LAST) {
                                    SRAM_WritePattern(selectedPatternID, row, DATA_FX6_VALUE, SRAM_ReadPattern(patternCopyFrom, cnt, DATA_FX6_VALUE)); inc++;
                                }
                                break;

                            }
                        }
                        inc = 0; bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                        PrintSelectedPositionInfo();
                    }
                    break;
                }
                break;
            // navigate line
            case BUTTON_LEFT: case BUTTON_RIGHT: case BUTTON_UP: case BUTTON_DOWN:
                NavigatePattern(state);
                PrintSelectedPositionInfo();
                break;

            case BUTTON_B:
                switch (changed)
                {
                // octave -
                case BUTTON_DOWN:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        set_range();
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            if (note < OCTAVE) return; // safe transpose
                        }
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            note -= OCTAVE;
                            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, note);
                        }
                        break;

                    /*case DATA_FX1_VALUE:
                    case DATA_FX2_VALUE:
                    case DATA_FX3_VALUE:
                    case DATA_FX4_VALUE:
                    case DATA_FX5_VALUE:
                    case DATA_FX6_VALUE:
                    case (DATA_FX1_VALUE + PATTERN_COLUMNS):
                    case (DATA_FX2_VALUE + PATTERN_COLUMNS):
                    case (DATA_FX3_VALUE + PATTERN_COLUMNS):
                    case (DATA_FX4_VALUE + PATTERN_COLUMNS):
                    case (DATA_FX5_VALUE + PATTERN_COLUMNS):
                    case (DATA_FX6_VALUE + PATTERN_COLUMNS):*/
                    // B+Down effect value linear interpolation
                    default:
                        u8 param = selectedPatternColumn;
                        if (param >= PATTERN_COLUMNS) param -= PATTERN_COLUMNS;

                        if (param >= DATA_FX1_VALUE && (param & 1))
                        {
                            u8 row = selectedPatternRow + patternColumnShift;
                            u8 curType = SRAM_ReadPattern(selectedPatternID, row, param - 1);
                            u8 effectiveType = curType;
                            if (effectiveType == 0)
                            {
                                for (s8 r = (s8)(row) - 1; r >= 0; r--)
                                {
                                    u8 t = SRAM_ReadPattern(selectedPatternID, (u8)r, param - 1);
                                    if (t) { effectiveType = t; break; }
                                }
                            }
                            u8 curValue = SRAM_ReadPattern(selectedPatternID, row, param);

                            if (curType || curValue)
                            {
                                u8 tgtRow = OXFF, tgtVal = 0;
                                for (u8 r = row + 1; r <= PATTERN_ROW_LAST; r++)
                                {
                                    u8 nxtType = SRAM_ReadPattern(selectedPatternID, r, param - 1);
                                    if (nxtType != effectiveType && nxtType != 0) break;
                                    u8 nxtVal = SRAM_ReadPattern(selectedPatternID, r, param);
                                    if (nxtVal) { tgtRow = r; tgtVal = nxtVal; break; }
                                }

                                if (tgtRow != OXFF && tgtRow > row + 1)
                                {
                                    u8 steps = tgtRow - row;
                                    for (u8 s = 1; s < steps; s++)
                                    {
                                        s16 n = (s16)curValue * (s16)(steps - s)
                                                  + (s16)tgtVal * (s16)s
                                                  + (s16)(steps >> 1);
                                        SRAM_WritePattern(selectedPatternID, row + s, param, (u8)(n / (s16)steps));
                                    }
                                }
                            }
                        }
                        break;
                    }
                    bRefreshScreen = TRUE; patternRowToRefresh = OXFF; bInitScreen = true;
                    break;
                // octave +
                case BUTTON_UP:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        set_range();
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            if (note > NOTE_MAX-OCTAVE) return; // safe transpose
                        }
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            note += OCTAVE;
                            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, note);
                        }
                        break;
                    }
                    bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                    break;
                // semitone -
                case BUTTON_LEFT:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        set_range();
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            if (note == 0) return; // safe transpose
                        }
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            note -= SEMITONE;
                            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, note);
                        }
                        break;
                    }
                    bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                    break;
                // semitone +
                case BUTTON_RIGHT:
                    switch (selectedPatternColumn)
                    {
                    case DATA_NOTE: case (DATA_NOTE + PATTERN_COLUMNS):
                        set_range();
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            if (note == NOTE_MAX) return; // safe transpose
                        }
                        for (u8 row = _rangeStart; row < _rangeEnd; row++)
                        {
                            u8 note = SRAM_ReadPattern(selectedPatternID, row, DATA_NOTE);
                            if (note == NOTE_OFF || note == NOTE_EMPTY) continue;
                            note += SEMITONE;
                            SRAM_WritePattern(selectedPatternID, row, DATA_NOTE, note);
                        }
                        break;
                    }
                    bRefreshScreen = TRUE; patternRowToRefresh = OXFF;
                    break;
                }
                break;

            // copy/paste
            case BUTTON_Z:
                switch (changed)
                {
                /*case BUTTON_RIGHT:  // Z + D-Pad: select pattern for editing [useless]
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
                    break;*/
                // selected to nothing
                case BUTTON_Z: // pressed
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
                }
                PrintSelectedPositionInfo();
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
                PrintSelectedPositionInfo();
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
                    if (selectedInstrumentID < INSTRUMENTS_LAST) { CommitSeqEditBuffer(); selectedInstrumentID++; LoadSeqEditBuffer(selectedInstrumentID); bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF; }
                    break;

                case BUTTON_LEFT:
                    if (selectedInstrumentID > 1) { CommitSeqEditBuffer(); selectedInstrumentID--; LoadSeqEditBuffer(selectedInstrumentID); bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF; }
                    break;

                case BUTTON_UP:
                    if (selectedInstrumentID < (INSTRUMENTS_LAST - 16)) { CommitSeqEditBuffer(); selectedInstrumentID += 16; LoadSeqEditBuffer(selectedInstrumentID); } else selectedInstrumentID = INSTRUMENTS_LAST;
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = OXFF;
                    break;

                case BUTTON_DOWN:
                    if (selectedInstrumentID > 16) { CommitSeqEditBuffer(); selectedInstrumentID -= 16; LoadSeqEditBuffer(selectedInstrumentID); } else selectedInstrumentID = 1;
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
                    ChangeInstrumentParameter(1, FALSE);
                    break;

                case BUTTON_LEFT:
                    ChangeInstrumentParameter(-1, FALSE);
                    break;

                case BUTTON_UP:
                    ChangeInstrumentParameter(8, FALSE);
                    break;

                case BUTTON_DOWN:
                    ChangeInstrumentParameter(-8, FALSE);
                    break;
                }
                break;

            case BUTTON_A | BUTTON_B:
                // (A + B) + D-Pad: change same parameters value
                switch (changed)
                {
                case BUTTON_RIGHT:
                    ChangeInstrumentParameter(1, TRUE);
                    break;

                case BUTTON_LEFT:
                    ChangeInstrumentParameter(-1, TRUE);
                    break;

                case BUTTON_UP:
                    ChangeInstrumentParameter(8, TRUE);
                    break;

                case BUTTON_DOWN:
                    ChangeInstrumentParameter(-8, TRUE);
                    break;
                }
                break;

            // navigate parameters
            case BUTTON_LEFT: case BUTTON_RIGHT: case BUTTON_UP: case BUTTON_DOWN:
                NavigateInstrument(state);
                break;
            // ARP and VOL step on/off
            case BUTTON_C:
                if (selectedInstrumentParameter == GUI_INST_PARAM_PARSEQ)
                {
                    SRAM_WriteSEQ_PAR(selectedInstrumentID, selectedInstrumentOperator, SEQ_SKIP);
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_PARSEQ;
                }
                else if (selectedInstrumentParameter == GUI_INST_PARAM_ARPSEQ)
                {
                    SRAM_WriteSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator, NOTE_EMPTY);
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_ARPSEQ;
                }
                break;

            case BUTTON_B:
                switch (selectedInstrumentParameter)
                {
                case GUI_INST_PARAM_PARSEQ:
                    SRAM_WriteSEQ_PAR(selectedInstrumentID, selectedInstrumentOperator, SEQ_VOL_MIN_ATT); // VOL
                    bRefreshScreen = TRUE; instrumentParameterToRefresh = GUI_INST_PARAM_PARSEQ;
                    break;
                case GUI_INST_PARAM_ARPSEQ:
                    SRAM_WriteSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator, ARP_BASE); // ARP
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
    }
}

// ------------------------------ PATTERN MATRIX
// ------------------------------ PATTERN EDITOR
void PrintSelectedPositionInfo()
{
    switch (selectedPatternColumn)
    {
        case DATA_NOTE:
            if (selectedMatrixChannel == CHANNEL_FM6_DAC && bDAC_enable)
                DisplaySampleName(
                                  GUI_PATTERN_SAMPLE_NAME_XPOS,
                                  GUI_PATTERN_SAMPLE_NAME_YPOS,
                                  SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_NOTE),
                                  activeSampleBank);
            break;
        case DATA_NOTE+PATTERN_COLUMNS:
            if (selectedMatrixChannel == CHANNEL_FM6_DAC && bDAC_enable)
                DisplaySampleName(
                                  GUI_PATTERN_SAMPLE_NAME_XPOS,
                                  GUI_PATTERN_SAMPLE_NAME_YPOS,
                                  SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_NOTE),
                                  activeSampleBank);
            break;
        case DATA_INSTRUMENT:
            PrintInstrumentInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_INSTRUMENT)); break;
        case DATA_INSTRUMENT+PATTERN_COLUMNS:
            PrintInstrumentInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_INSTRUMENT)); break;
        case DATA_FX1_TYPE: case DATA_FX1_VALUE:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_FX1_TYPE)); break;
        case DATA_FX2_TYPE: case DATA_FX2_VALUE:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_FX2_TYPE)); break;
        case DATA_FX3_TYPE: case DATA_FX3_VALUE:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_FX3_TYPE)); break;
        case DATA_FX1_TYPE+PATTERN_COLUMNS: case DATA_FX1_VALUE+PATTERN_COLUMNS:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_FX1_TYPE)); break;
        case DATA_FX2_TYPE+PATTERN_COLUMNS: case DATA_FX2_VALUE+PATTERN_COLUMNS:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_FX2_TYPE)); break;
        case DATA_FX3_TYPE+PATTERN_COLUMNS: case DATA_FX3_VALUE+PATTERN_COLUMNS:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_FX3_TYPE)); break;
        case DATA_FX4_TYPE: case DATA_FX4_VALUE:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_FX4_TYPE)); break;
        case DATA_FX5_TYPE: case DATA_FX5_VALUE:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_FX5_TYPE)); break;
        case DATA_FX6_TYPE: case DATA_FX6_VALUE:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow, DATA_FX6_TYPE)); break;
        case DATA_FX4_TYPE+PATTERN_COLUMNS: case DATA_FX4_VALUE+PATTERN_COLUMNS:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_FX4_TYPE)); break;
        case DATA_FX5_TYPE+PATTERN_COLUMNS: case DATA_FX5_VALUE+PATTERN_COLUMNS:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_FX5_TYPE)); break;
        case DATA_FX6_TYPE+PATTERN_COLUMNS: case DATA_FX6_VALUE+PATTERN_COLUMNS:
            PrintCommandInfo(SRAM_ReadPattern(selectedPatternID, selectedPatternRow+PATTEN_ROWS_PER_SIDE, DATA_FX6_TYPE)); break;
    }
}

void PrintCommandInfo(u8 id)
{
    if (!strcmp(infoCommands[id], STRING_EMPTY))
    {
        VDP_clearTextArea(GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y, 39, 2);
    }
    else
    {
        VDP_setTextPalette(PAL0); VDP_drawText(infoCommands[id], GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y);
        VDP_setTextPalette(PAL1); VDP_drawText(infoDescriptions[id], GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y + 1);
    }
}

void PrintInstrumentInfo(u8 id)
{
    if (!id) return;
    for (u8 i = 0; i < 8; i++)
    {
        VDP_setTileMapXY(BG_A,
            TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_ALPHABET[SRAM_ReadInstrument(id, INST_NAME_1 + i)]),
            GUI_INFO_PRINT_INST_X + i, GUI_INFO_PRINT_INST_Y);
    }
}

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
        /*for (u8 i = 0; i < 8; i++)
        {
            VDP_setTileMapXY(BG_A,
                TILE_ATTR_FULL(PAL0, 1, FALSE, FALSE, bgBaseTileIndex[1] + GUI_ALPHABET[SRAM_ReadInstrument(lastEnteredInstrumentID, INST_NAME_1 + i)]),
                GUI_INFO_PRINT_INST_X + i, GUI_INFO_PRINT_INST_Y);
        }*/
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
        /*if (!strcmp(infoCommands[lastEnteredEffect], STRING_EMPTY))
        {
            VDP_clearTextArea(GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y, 39, 2);
        }
        else
        {
            VDP_setTextPalette(PAL0); VDP_drawText(infoCommands[lastEnteredEffect], GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y);
            VDP_setTextPalette(PAL1); VDP_drawText(infoDescriptions[lastEnteredEffect], GUI_INFO_PRINT_X, GUI_INFO_PRINT_Y + 1);
        }*/
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


// ------------------------------ INSTRUMENT EDITOR
// apply single instrument parameter change to YM2612 hardware registers,
// writing only the register(s) that were edited to preserve pattern automation
static void ApplyInstChangeToYM(u8 instID, u8 param, u8 op, u8 changeAll)
{
    for (u8 mtxCh = 0; mtxCh < CHANNEL_PSG1; mtxCh++)
    {
        if (channelPreviousInstrument[mtxCh] != instID) continue;
        if (mtxCh == CHANNEL_FM3_OP3 || mtxCh == CHANNEL_FM3_OP2 || mtxCh == CHANNEL_FM3_OP1) continue;

        u8 ymCh = (mtxCh < CHANNEL_FM4) ? mtxCh : (mtxCh - CHANNEL_FM4);
        u16 port = (mtxCh <= CHANNEL_FM3_OP4) ? PORT_1 : PORT_2;

        switch (param)
        {
        case GUI_INST_PARAM_ALG:
            chInst[mtxCh].ALG = tmpInst[instID].ALG;
            chInst[mtxCh].FB_ALG = (chInst[mtxCh].FB << 3) | chInst[mtxCh].ALG;
            YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0 + ymCh, chInst[mtxCh].FB_ALG);
            SetChannelBaseVolume_FM(mtxCh);
            bWriteRegs = TRUE; SetChannelVolume(mtxCh);
            break;

        case GUI_INST_PARAM_FMS:
            chInst[mtxCh].FMS = tmpInst[instID].FMS;
            chInst[mtxCh].PAN_AMS_FMS = (chInst[mtxCh].PAN << 6) | (chInst[mtxCh].AMS << 4) | chInst[mtxCh].FMS;
            YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0 + ymCh, chInst[mtxCh].PAN_AMS_FMS);
            break;

        case GUI_INST_PARAM_AMS:
            chInst[mtxCh].AMS = tmpInst[instID].AMS;
            chInst[mtxCh].PAN_AMS_FMS = (chInst[mtxCh].PAN << 6) | (chInst[mtxCh].AMS << 4) | chInst[mtxCh].FMS;
            YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0 + ymCh, chInst[mtxCh].PAN_AMS_FMS);
            break;

        case GUI_INST_PARAM_PAN:
            chInst[mtxCh].PAN = tmpInst[instID].PAN;
            chInst[mtxCh].PAN_AMS_FMS = (chInst[mtxCh].PAN << 6) | (chInst[mtxCh].AMS << 4) | chInst[mtxCh].FMS;
            YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0 + ymCh, chInst[mtxCh].PAN_AMS_FMS);
            break;

        case GUI_INST_PARAM_FB:
            chInst[mtxCh].FB = tmpInst[instID].FB;
            chInst[mtxCh].FB_ALG = (chInst[mtxCh].FB << 3) | chInst[mtxCh].ALG;
            YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0 + ymCh, chInst[mtxCh].FB_ALG);
            break;

        case GUI_INST_PARAM_TL:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            u8 *ch = &chInst[mtxCh].TL1;
            u8 *tmp = &tmpInst[instID].TL1;
            for (u8 i = start; i < end; i++)
            {
                ch[i] = tmp[i];
                YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + (i << 2) + ymCh, ch[i]);
            }
            SetChannelBaseVolume_FM(mtxCh);
            bWriteRegs = TRUE; SetChannelVolume(mtxCh);
            break;
        }

        case GUI_INST_PARAM_RS:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pRS = &chInst[mtxCh].RS1 + i;
                u8 *pAR = &chInst[mtxCh].AR1 + i;
                u8 *pComb = &chInst[mtxCh].RS1_AR1 + i;
                *pRS = *(&tmpInst[instID].RS1 + i);
                *pComb = (*pRS << 6) | *pAR;
                YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_AR:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pAR = &chInst[mtxCh].AR1 + i;
                u8 *pComb = &chInst[mtxCh].RS1_AR1 + i;
                *pAR = *(&tmpInst[instID].AR1 + i);
                *pComb = (*(&chInst[mtxCh].RS1 + i) << 6) | *pAR;
                YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_MUL:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pMUL = &chInst[mtxCh].MUL1 + i;
                u8 *pComb = &chInst[mtxCh].DT1_MUL1 + i;
                *pMUL = *(&tmpInst[instID].MUL1 + i);
                *pComb = (*(&chInst[mtxCh].DT1 + i) << 4) | *pMUL;
                YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_DT:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pDT = &chInst[mtxCh].DT1 + i;
                u8 *pComb = &chInst[mtxCh].DT1_MUL1 + i;
                *pDT = *(&tmpInst[instID].DT1 + i);
                *pComb = (*pDT << 4) | *(&chInst[mtxCh].MUL1 + i);
                YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_D1R:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pD1R = &chInst[mtxCh].D1R1 + i;
                u8 *pComb = &chInst[mtxCh].AM1_D1R1 + i;
                *pD1R = *(&tmpInst[instID].D1R1 + i);
                *pComb = (*(&chInst[mtxCh].AM1 + i) << 7) | *pD1R;
                YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_AM:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pAM = &chInst[mtxCh].AM1 + i;
                u8 *pComb = &chInst[mtxCh].AM1_D1R1 + i;
                *pAM = *(&tmpInst[instID].AM1 + i);
                *pComb = (*pAM << 7) | *(&chInst[mtxCh].D1R1 + i);
                YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_D1L:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pD1L = &chInst[mtxCh].D1L1 + i;
                u8 *pComb = &chInst[mtxCh].D1L1_RR1 + i;
                *pD1L = *(&tmpInst[instID].D1L1 + i);
                *pComb = (*pD1L << 4) | *(&chInst[mtxCh].RR1 + i);
                YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_RR:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pRR = &chInst[mtxCh].RR1 + i;
                u8 *pComb = &chInst[mtxCh].D1L1_RR1 + i;
                *pRR = *(&tmpInst[instID].RR1 + i);
                *pComb = (*(&chInst[mtxCh].D1L1 + i) << 4) | *pRR;
                YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0 + (i << 2) + ymCh, *pComb);
            }
            break;
        }

        case GUI_INST_PARAM_D2R:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pD2R = &chInst[mtxCh].D2R1 + i;
                *pD2R = *(&tmpInst[instID].D2R1 + i);
                YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH0 + (i << 2) + ymCh, *pD2R);
            }
            break;
        }

        case GUI_INST_PARAM_SSGEG:
        {
            u8 start = changeAll ? 0 : op, end = changeAll ? 4 : (u8)(op + 1);
            for (u8 i = start; i < end; i++)
            {
                u8 *pSSG = &chInst[mtxCh].SSGEG1 + i;
                *pSSG = *(&tmpInst[instID].SSGEG1 + i);
                YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH0 + (i << 2) + ymCh, *pSSG);
            }
            break;
        }
        }
    }
}

static void ChangeInstrumentParameter(s8 modifier, u8 changeAll)
{
    static s16 value = 0;

    bRefreshScreen = TRUE;
    instrumentParameterToRefresh = selectedInstrumentParameter;

    switch (selectedInstrumentParameter)
    {
    case GUI_INST_PARAM_ALG:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_ALG) + modifier;
        if (value < 0) value = 7; else if (value > 7) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_ALG, value);
        break;
    case GUI_INST_PARAM_FMS:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_FMS) + modifier;
        if (value < 0) value = 7; else if (value > 7) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_FMS, value);
        break;
    case GUI_INST_PARAM_AMS:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_AMS) + modifier;
        if (value < 0) value = 3; else if (value > 3) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_AMS, value);
        break;
    case GUI_INST_PARAM_PAN:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_PAN) + modifier;
        if (value < 0) value = 3; else if (value > 3) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_PAN, value);
        break;
    case GUI_INST_PARAM_FB:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_FB) + modifier;
        if (value < 0) value = 7; else if (value > 7) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_FB, value);
        break;
    case GUI_INST_PARAM_TL:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_TL1 + i) + modifier;
                if (value < 0) value = 0x7F; else if (value > 0x7F) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_TL1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_TL1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 0x7F; else if (value > 0x7F) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_TL1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_RS:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_RS1 + i) + modifier;
                if (value < 0) value = 3; else if (value > 3) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_RS1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_RS1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 3; else if (value > 3) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_RS1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_MUL:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_MUL1 + i) + modifier;
                if (value < 0) value = 15; else if (value > 15) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_MUL1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_MUL1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 15; else if (value > 15) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_MUL1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_DT:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_DT1 + i) + modifier;
                if (value < 1) value = 7; else if (value > 7) value = 1;
                SRAM_WriteInstrument(selectedInstrumentID, INST_DT1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_DT1 + selectedInstrumentOperator) + modifier;
            if (value < 1) value = 7; else if (value > 7) value = 1;
            SRAM_WriteInstrument(selectedInstrumentID, INST_DT1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_AR:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_AR1 + i) + modifier;
                if (value < 0) value = 31; else if (value > 31) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_AR1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_AR1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 31; else if (value > 31) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_AR1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D1R:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_D1R1 + i) + modifier;
                if (value < 0) value = 31; else if (value > 31) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_D1R1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_D1R1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 31; else if (value > 31) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_D1R1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D1L:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_D1L1 + i) + modifier;
                if (value < 0) value = 15; else if (value > 15) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_D1L1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_D1L1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 15; else if (value > 15) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_D1L1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_D2R:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_D2R1 + i) + modifier;
                if (value < 0) value = 31; else if (value > 31) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_D2R1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_D2R1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 31; else if (value > 31) value = 0;
            SRAM_WriteInstrument(selectedInstrumentID, INST_D2R1 + selectedInstrumentOperator, value);
        }
        break;
    case GUI_INST_PARAM_RR:
        if (changeAll)
        {
            for (u8 i = 0; i < 4; i++)
            {
                value = SRAM_ReadInstrument(selectedInstrumentID, INST_RR1 + i) + modifier;
                if (value < 0) value = 15; else if (value > 15) value = 0;
                SRAM_WriteInstrument(selectedInstrumentID, INST_RR1 + i, value);
            }
        }
        else
        {
            value = SRAM_ReadInstrument(selectedInstrumentID, INST_RR1 + selectedInstrumentOperator) + modifier;
            if (value < 0) value = 15; else if (value > 15) value = 0;
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
            if (value < 7) value = 15; else if (value > 15) value = 7;
            SRAM_WriteInstrument(selectedInstrumentID, INST_SSGEG1 + selectedInstrumentOperator, value);
        }
        break;
    // non instrument (global)
    case GUI_INST_PARAM_LFO:
        value = SRAMW_readByte(SRAM_GLOBAL_LFO) + modifier;
        if (value < 7) value = 15; else if (value > 15) value = 7;
        SRAMW_writeByte(SRAM_GLOBAL_LFO, value); SetGlobalLFO(value);
        break;
    case GUI_INST_PARAM_PARSEQ:
        if (changeAll)
        {
            for (u8 i = 0; i < SEQ_STEPS; i++)
            {
                value = SRAM_ReadSEQ_PAR(selectedInstrumentID, i);
                if (value != SEQ_SKIP) value += modifier; else continue;
                if (value > 0x7F) value = 0; else if (value < 0) value = 0x7F;
                SRAM_WriteSEQ_PAR(selectedInstrumentID, i, value);
                seqParValue[selectedInstrumentID][i] = value;
            }
        }
        else
        {
            value = SRAM_ReadSEQ_PAR(selectedInstrumentID, selectedInstrumentOperator);
            value += modifier;
            if (value > 0x7F) value = 0; else if (value < 0) value = 0x7F;
            SRAM_WriteSEQ_PAR(selectedInstrumentID, selectedInstrumentOperator, value);
            seqParValue[selectedInstrumentID][selectedInstrumentOperator] = value;
        }
        break;
    case GUI_INST_PARAM_ARPSEQ:
        if (changeAll)
        {
            for (u8 i = 0; i < SEQ_STEPS; i++)
            {
                value = SRAM_ReadSEQ_ARP(selectedInstrumentID, i);
                if (value != NOTE_EMPTY) value += modifier; else continue;
                if (value < 76) value = 124; else if (value > 124) value = 76; // +- 24 semitones
                SRAM_WriteSEQ_ARP(selectedInstrumentID, i, value);
                seqArpValue[selectedInstrumentID][i] = value;
            }
        }
        else
        {
            value = SRAM_ReadSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator);
            if (value != NOTE_EMPTY)
            {
                value += modifier;
                if (value < 76) value = 124; else if (value > 124) value = 76; // +- 24 semitones
                SRAM_WriteSEQ_ARP(selectedInstrumentID, selectedInstrumentOperator, value);
                seqArpValue[selectedInstrumentID][selectedInstrumentOperator] = value;
            }
        }
        break;
    case GUI_INST_PARAM_NAME:
        value = SRAM_ReadInstrument(selectedInstrumentID, INST_NAME_1 + selectedInstrumentOperator) + modifier;
        if (value < 0) value = 37; else if (value > 37) value = 0;
        SRAM_WriteInstrument(selectedInstrumentID, INST_NAME_1 + selectedInstrumentOperator, value);
        break;
    case GUI_INST_PARAM_PCM_BANK:
        value = selectedSampleBank + modifier;
        if (value > SAMPLE_BANK_LAST) value = 0;
        else if (value < 0) value = SAMPLE_BANK_LAST;
        selectedSampleBank = value;
        DisplaySampleName(106, 10, selectedSampleNote, selectedSampleBank);
        break;
    case GUI_INST_PARAM_PCM_NOTE:
        value = selectedSampleNote + modifier;
        if (value > NOTE_MAX) value = 0;
        else if (value < 0) value = NOTE_MAX;
        selectedSampleNote = value;
        DisplaySampleName(106, 10, selectedSampleNote, selectedSampleBank);
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
    CacheInstrumentToRAM(selectedInstrumentID); // update RAM struct
    // apply values at edit - write only the changed register(s) to preserve pattern automation
    ApplyInstChangeToYM(selectedInstrumentID, selectedInstrumentParameter, selectedInstrumentOperator, changeAll);
}

u32 GetSampleStartAddress(u8 bank, u8 note)
{
    return GetSampleSettings(bank, note)->startOffset;
}

// only attenuate, not increase
static void SetChannelVolume(u8 mtxCh)
{
    static s16 vol[4] = {0,0,0,0};
    static u8 port = 0, ymCh = 0, psgCh = 0;

    auto void set_normal_channel_vol() {
        switch (chInst[mtxCh].ALG)
        {
        case 0: case 1: case 2: case 3:
            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
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
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegBatchBegin();
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
                YM2612_writeRegBatchEnd();
            }
            chInst[mtxCh].TL3 = (u8)vol[2];
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        case 5: case 6:
            vol[1] =
                channelSlotBaseLevel[mtxCh][1] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[1] > 0x7F) vol[1] = 0x7F;

            vol[2] =
                channelSlotBaseLevel[mtxCh][2] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegBatchBegin();
                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, (u8)vol[1]);
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
                YM2612_writeRegBatchEnd();
            }
            chInst[mtxCh].TL2 = (u8)vol[1];
            chInst[mtxCh].TL3 = (u8)vol[2];
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        case 7:
            vol[0] =
                channelSlotBaseLevel[mtxCh][0] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[0] > 0x7F) vol[0] = 0x7F;

            vol[1] =
                channelSlotBaseLevel[mtxCh][1] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[1] > 0x7F) vol[1] = 0x7F;

            vol[2] =
                channelSlotBaseLevel[mtxCh][2] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            vol[3] =
                channelSlotBaseLevel[mtxCh][3] +
                channelAttenuation[mtxCh] +
                channelVolumeAttenuation[mtxCh] +
                channelTremolo[mtxCh];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs)
            {
                YM2612_writeRegBatchBegin();
                YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + ymCh, (u8)vol[0]);
                YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, (u8)vol[1]);
                YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
                YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
                YM2612_writeRegBatchEnd();
            }
            chInst[mtxCh].TL1 = (u8)vol[0];
            chInst[mtxCh].TL2 = (u8)vol[1];
            chInst[mtxCh].TL3 = (u8)vol[2];
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;

        default: break;
        }
    }

    auto void set_special_channel_vol() {
        switch (mtxCh)
        {
        case CHANNEL_FM3_OP4:
            vol[3] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][3] +
                channelAttenuation[CHANNEL_FM3_OP4] +
                channelVolumeAttenuation[CHANNEL_FM3_OP4] +
                channelTremolo[CHANNEL_FM3_OP4];
            if (vol[3] > 0x7F) vol[3] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, (u8)vol[3]);
            chInst[mtxCh].TL4 = (u8)vol[3];
            break;
        case CHANNEL_FM3_OP3:
            vol[2] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][2] +
                channelAttenuation[CHANNEL_FM3_OP3] +
                channelVolumeAttenuation[CHANNEL_FM3_OP3] +
                channelTremolo[CHANNEL_FM3_OP3];
            if (vol[2] > 0x7F) vol[2] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, (u8)vol[2]);
            chInst[mtxCh].TL3 = (u8)vol[2];
            break;
        case CHANNEL_FM3_OP2:
            vol[1] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][1] +
                channelAttenuation[CHANNEL_FM3_OP2] +
                channelVolumeAttenuation[CHANNEL_FM3_OP2] +
                channelTremolo[CHANNEL_FM3_OP2];
            if (vol[1] > 0x7F) vol[1] = 0x7F;

            if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, (u8)vol[1]);
            chInst[mtxCh].TL2 = (u8)vol[1];
            break;
        case CHANNEL_FM3_OP1:
            vol[0] =
                channelSlotBaseLevel[CHANNEL_FM3_OP4][0] +
                channelAttenuation[CHANNEL_FM3_OP1] +
                channelVolumeAttenuation[CHANNEL_FM3_OP1] +
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
        if (FM_CH3_Mode == CH3_SPECIAL || FM_CH3_Mode == CH3_SPECIAL_CSM) { port = PORT_1; ymCh = 2; set_special_channel_vol(); }
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
            channelVolumeAttenuation[mtxCh] +
            channelTremolo[mtxCh]
        ) >> 3; // / 8;

        if (vol[0] > PSG_ENVELOPE_MIN) vol[0] = PSG_ENVELOPE_MIN;
        if (bPsgIsPlayingNote[psgCh]) PSG_setEnvelope(psgCh, (u8)vol[0]);
    break;
    }
    bWriteRegs = TRUE; // trigger
}

FORCE_INLINE static void RequestZ80()
{
    //if (!Z80_isBusTaken()) Z80_requestBus(TRUE);
    Z80_requestBus(TRUE);
}

FORCE_INLINE static void ReleaseZ80()
{
    YM2612_write(PORT_1, YM2612REG_DAC);
    Z80_releaseBus();

    /*if (Z80_isBusTaken())
    {
            YM2612_write(PORT_1, YM2612REG_DAC);
            Z80_releaseBus();
    }
    else
    {
        Z80_getAndRequestBus(TRUE);
        YM2612_write(PORT_1, YM2612REG_DAC);
        Z80_releaseBus();
    }*/
}

static void SetPitchPSG(u8 mtxCh, u8 note)
{
    static u8 key = 0;
    static s8 keyMod = 0;

    keyMod = note + channelModNotePitch[mtxCh] + channelModNoteVibrato[mtxCh];
    channelFinalPitch[mtxCh] += channelFineTune[mtxCh];
    if (channelFinalPitch[mtxCh] > 31) { keyMod++; channelFinalPitch[mtxCh] -= 32; }

    if (keyMod < PSG_LOWEST_NOTE) { key = PSG_LOWEST_NOTE;
        channelPitchSlideSpeed[mtxCh] = 0; }
    else if (keyMod > NOTE_MAX) { key = NOTE_MAX;
        channelPitchSlideSpeed[mtxCh] = 0; }
    else key = keyMod;

    if (channelFlags[mtxCh])
    {
        switch (mtxCh)
        {
        case CHANNEL_PSG1:
            SetChannelVolume(mtxCh);
            PSG_setTone(0, noteMicrotone_PSG[key][(u8)channelFinalPitch[mtxCh]]);
            break;
        case CHANNEL_PSG2:
            SetChannelVolume(mtxCh);
            PSG_setTone(1, noteMicrotone_PSG[key][(u8)channelFinalPitch[mtxCh]]);
            break;
        case CHANNEL_PSG3:
            switch (PSG_NoiseMode)
            {
            case PSG_TONAL_CH3_MUTED:
                PSG_setEnvelope(2, PSG_ENVELOPE_MIN); // mute PSG3 channel
                break;
            case PSG_TONAL_CH3_NOT_MUTED: case PSG_FIXED:
                SetChannelVolume(mtxCh);
                PSG_setTone(2, noteMicrotone_PSG[key][(u8)channelFinalPitch[mtxCh]]);
                break;
            }
            break;
        case CHANNEL_PSG4_NOISE:
            switch (PSG_NoiseMode)
            {
            case PSG_TONAL_CH3_MUTED:
                SetChannelVolume(mtxCh);
                PSG_setEnvelope(2, PSG_ENVELOPE_MIN); // mute PSG3 channel
                PSG_setTone(2, noteMicrotone_PSG[key][(u8)channelFinalPitch[mtxCh]]); // write tone to PSG3 to supply PSG4 tonal noise
                break;
            case PSG_TONAL_CH3_NOT_MUTED:
                SetChannelVolume(mtxCh);
                PSG_setTone(2, noteMicrotone_PSG[key][(u8)channelFinalPitch[mtxCh]]); // write tone to PSG3 to supply PSG4 tonal noise
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
static void SetPitchFM(u8 mtxCh, u8 note)
{
    static u8 part1 = 0, part2 = 0;
    static s8 key = 0;

    auto void set_key(u8 n)
    {
        key = n + channelModNotePitch[mtxCh] + channelModNoteVibrato[mtxCh];
    }

    // CSM
    if (FM_CH3_Mode == CH3_SPECIAL_CSM)
    {
        switch (mtxCh)
        {
            case CHANNEL_FM3_OP4: set_key(FM_CH3_OpFreq[0]); break;
            case CHANNEL_FM3_OP3: set_key(FM_CH3_OpFreq[1]); break;
            case CHANNEL_FM3_OP2: set_key(FM_CH3_OpFreq[2]); break;
            case CHANNEL_FM3_OP1: set_key(FM_CH3_OpFreq[3]); break;
            default: set_key(note); break;
        }
    }
    // Normal or Special
    else set_key(note);

    channelFinalPitch[mtxCh] += channelFineTune[mtxCh]; // 31 max
    if (channelFinalPitch[mtxCh] > 31) { key++; channelFinalPitch[mtxCh] -= MICROTONE_STEPS; }

    if ((key > -1) && (key < NOTES))
    {
        part1 = ((noteOctave[(u8)key]) << 3) | (noteMicrotone_YM[noteFreqID[(u8)key]][(u8)channelFinalPitch[mtxCh]] >> 8);
        part2 = 0b0000000011111111 & noteMicrotone_YM[noteFreqID[(u8)key]][(u8)channelFinalPitch[mtxCh]];

        YM2612_writeRegBatchBegin();
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
                //YM2612_writeRegZ80(PORT_1, YM2612REG_CH3_FREQ_MSB, CH3_SPECIAL_CSM | 0b00001111);
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
            else if (FM_CH3_Mode == CH3_SPECIAL_CSM)
            {
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP3_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP3_LSB, part2);
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
            else if (FM_CH3_Mode == CH3_SPECIAL_CSM)
            {
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP2_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP2_LSB, part2);
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
            else if (FM_CH3_Mode == CH3_SPECIAL_CSM)
            {
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP1_MSB, part1);
                YM2612_writeRegZ80(PORT_1, YM2612REG_CH3SP_FREQ_OP1_LSB, part2);
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
                switch (Z80_getLoadedDriver())
                {
                case Z80_DRIVER_PCM:
                    if (!FM_CH6_DAC_Pan)
                    {
                        SND_PCM_startPlay(sampleStart[activeSampleBank][note],
                                        sampleLength[activeSampleBank][note],
                                        sampleRate[activeSampleBank][note],
                                        samplePan[activeSampleBank][note],
                                        sampleLoop[activeSampleBank][note]);
                    }
                    else
                    {
                        SND_PCM_startPlay(sampleStart[activeSampleBank][note],
                                      sampleLength[activeSampleBank][note],
                                      sampleRate[activeSampleBank][note],
                                      FM_CH6_DAC_Pan,
                                      sampleLoop[activeSampleBank][note]);
                    }
                    break;

                case Z80_DRIVER_PCM4:
                        SND_PCM4_startPlay(
                                        sampleStart[activeSampleBank][note],
                                        sampleLength[activeSampleBank][note],
                                        SOUND_PCM_CH_AUTO,
                                        sampleLoop[activeSampleBank][note]);
                    break;

                case Z80_DRIVER_DPCM2:
                        SND_DPCM2_startPlay(
                                        sampleStart[activeSampleBank][note],
                                        sampleLength[activeSampleBank][note],
                                        SOUND_PCM_CH_AUTO,
                                        sampleLoop[activeSampleBank][note]);
                                        //sampleRate[activeSampleBank][note],
                                        //samplePan[activeSampleBank][note]);
                    break;
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
        YM2612_writeRegBatchEnd();
    }
    else
    {
        channelPitchSlideSpeed[mtxCh]= 0;
    }
}

static void PlayNoteOff(u8 mtxCh)
{
        if (FM_CH3_Mode == CH3_SPECIAL_CSM && mtxCh == CHANNEL_FM3_OP4) FM_CH3_Mode = CH3_SPECIAL_CSM_OFF;
        StopChannelSound(mtxCh);
        StopEffects(mtxCh);
}

static void PlayNote(u8 note, u8 mtxCh, u8 glide) // FALSE == retrigger, TRUE = set pitch only
{
    if (mtxCh < CHANNEL_PSG1) // FM
    {
        // S1>S3>S2>S4 for common registers and S4>S3>S1>S2 for CH3 frequencies
        if (!glide && !(mtxCh == CHANNEL_FM6_DAC && bDAC_enable))
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

    channelAutoGlide[mtxCh] = 0;
    channelAutoGlideTicksLeft[mtxCh] = 0;
    channelAutoGlideStartPitch[mtxCh] = 0;

    channelPreviousNote[mtxCh] = NOTE_OFF;

    channelSEQCounter_ARP[mtxCh] = 0;
    channelSEQCounter_PAR[mtxCh] = 0;
}

// stopping sound on matrix channel
static void StopChannelSound(u8 mtxCh)
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
        switch (Z80_getLoadedDriver())
        {
        case Z80_DRIVER_PCM:
            if (SND_PCM_isPlaying()) SND_PCM_stopPlay();
            break;
        case Z80_DRIVER_PCM4:
            if (SND_PCM4_isPlaying(SOUND_PCM_CH1_MSK)) SND_PCM4_stopPlay(SOUND_PCM_CH1);
            if (SND_PCM4_isPlaying(SOUND_PCM_CH2_MSK)) SND_PCM4_stopPlay(SOUND_PCM_CH2);
            if (SND_PCM4_isPlaying(SOUND_PCM_CH3_MSK)) SND_PCM4_stopPlay(SOUND_PCM_CH3);
            if (SND_PCM4_isPlaying(SOUND_PCM_CH4_MSK)) SND_PCM4_stopPlay(SOUND_PCM_CH4);
            break;
        case Z80_DRIVER_DPCM2:
            if (SND_DPCM2_isPlaying(SOUND_PCM_CH1_MSK)) SND_DPCM2_stopPlay(SOUND_PCM_CH1);
            if (SND_DPCM2_isPlaying(SOUND_PCM_CH2_MSK)) SND_DPCM2_stopPlay(SOUND_PCM_CH2);
            break;
        }
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
    for (u8 mtxCh = CHANNEL_FM1; mtxCh < CHANNELS_TOTAL; mtxCh++)
    {
        StopChannelSound(mtxCh);
        StopEffects(mtxCh);


        // only at playback stop, so note OFF is not affected
        //channelFineTune[mtxCh] = 0;
        channelVolumeChangeSpeed[mtxCh] = 0;

        channelVolumeAttenuation[mtxCh] = SEQ_VOL_MIN_ATT;
        channelCurrentRowNote[mtxCh] = channelPreviousNote[mtxCh] = NOTE_OFF;
        /*if (mtxCh < CHANNEL_PSG1) channelPreviousNote[mtxCh] = NOTE_EMPTY;
        else channelPreviousNote[mtxCh] = NOTE_OFF;*/

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
    // global LFO frequency (0..7) 3.98 5.56 6.02 6.37 6.88 9.63 48.1 72.2
    // 0000 - unused, 0 - enable, 000 - frequency
    // 8< - disable; 9..15 enable and set frequency
    YM2612_writeRegZ80(PORT_1, YM2612REG_GLOBAL_LFO, freq);
}

// cache instrument
static void CacheInstrumentToRAM(u8 id)
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
static void SetChannelBaseVolume_FM(u8 mtxCh)
{
    auto void set_normal_slots()
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
static void WriteYM2612(u8 mtxCh)
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
    default: break;
    }

    switch (ymCh)
    {
    case 0:
        YM2612_writeRegBatchBegin();
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
        YM2612_writeRegBatchEnd();
        break;
    case 1:
        YM2612_writeRegBatchBegin();
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
        YM2612_writeRegBatchEnd();
        break;
    case 2:
        YM2612_writeRegBatchBegin();
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
        YM2612_writeRegBatchEnd();
        break;
    default: break;
    }
}

static void ApplyCommand_Common(u8 mtxCh, u8 fxParam, u8 fxValue)
{
    switch (fxParam)
    {
    // H-INT callback skip
    case 0x0F:
        if ((fxValue < 0xE2 || (IS_PAL_SYSTEM && fxValue < 0xF2)) && fxValue > 0) H_INT_CALLS_SKIP = fxValue;
        else H_INT_CALLS_SKIP = 32;
        VDP_setHIntCounter(H_INT_CALLS_SKIP-1);
        SetBPM(NULL);
        break;
    // SRAM_TEMPO
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

    // CHANNEL FINETUNE
    case 0x18:
        if (fxValue < 32) channelFineTune[mtxCh] = fxValue;
        break;

     // CHANNEL SEQ SPEED
    case 0x19:
        channelSeqSkipStep[mtxCh] = fxValue;
        break;

    // CHANNEL ARP SPEED
    case 0x1A:
        channelArpSkipStep[mtxCh] = fxValue;
        break;

    // CHANNEL NOTE TRIGGER TYPE
    case 0x1E:
        channelNoteTriggerType[mtxCh] = fxValue;
        break;

    // CHANNEL ARP TRIGGER TYPE
    case 0x2E:
        channelArpSeqTriggerType[mtxCh] = fxValue;
        break;

    // ARP SEQUENCE MODE
    case 0x2F:
        if (!fxValue) channelArpSeqPlayMODE[mtxCh] = 0; else channelArpSeqPlayMODE[mtxCh] = 1;
        break;

    // ARP SEQUENCE
    case 0x30:
        channelArpSeqID[mtxCh] = fxValue;
        channelArpSeqActive[mtxCh] = 1;
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

    // CHANNEL AUTO GLIDE
    case 0x38:
        channelAutoGlide[mtxCh] = fxValue;
        if (!fxValue)
        {
            channelAutoGlideTicksLeft[mtxCh] = 0;
            channelAutoGlideStartPitch[mtxCh] = 0;
        }
        break;

    // VOLUME SEQUENCE MODE
    case 0x3F:
        if (!fxValue) channelParSeqPlayMODE[mtxCh] = 0; else channelParSeqPlayMODE[mtxCh] = 1;
        break;

    // VOLUME SEQUENCE
    case 0x40:
        channelParSeqID[mtxCh] = fxValue;
        if (!fxValue) channelVolumeAttenuation[mtxCh] = 0;
        channelParSeqActive[mtxCh] = 1;
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
        //channelNoteRetriggerCounter[mtxCh] = fxValue;
        channelNoteRetriggerCounter[mtxCh] = 0;
        channelNoteDelayCounter[mtxCh] = 0; // disable delay
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
        channelNoteDelayCounter[mtxCh] = fxValue + 1;
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

static void ApplyCommand_DAC(u8 fxParam, u8 fxValue)
{
    auto void dac_play(u8 channel, u8 channel_mask, u8 bank)
    {
        if (Z80_getLoadedDriver() == Z80_DRIVER_PCM4)
        {
            if (fxValue < NOTES)
            {
                SND_PCM4_startPlay(
                    sampleStart[bank][fxValue],
                    sampleLength[bank][fxValue],
                    channel,
                    sampleLoop[bank][fxValue]);
            }
            else if (SND_PCM4_isPlaying(channel_mask) && fxValue == NOTE_OFF) SND_PCM4_stopPlay(channel);
        }
        else if (Z80_getLoadedDriver() == Z80_DRIVER_DPCM2)
        {
            if (fxValue < NOTES)
            {
                SND_DPCM2_startPlay(
                    sampleStart[bank][fxValue],
                    sampleLength[bank][fxValue],
                    channel,
                    sampleLoop[bank][fxValue]);
            }
            else if (SND_DPCM2_isPlaying(channel_mask) && fxValue == NOTE_OFF) SND_DPCM2_stopPlay(channel);
        }
    }

    auto void load_dac_driver(u16 driver)
    {
        if (Z80_getLoadedDriver() != driver)
        {
            SYS_disableInts();
            Z80_loadDriver(driver, TRUE);
            // to trigger FM instruments rewrite on playback (fixing muted sound)
            for (u8 mtxCh = CHANNEL_FM1; mtxCh < CHANNEL_PSG1; mtxCh++)
                channelPreviousInstrument[mtxCh] = NULL;
            SYS_enableInts();
        }
        SetGlobalLFO(SRAMW_readByte(SRAM_GLOBAL_LFO));
    }

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
    /*case 0x0E:
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
        break;*/

    // PCM SAMPLE BANK SET
    case 0x16:
        if (fxValue < 4) activeSampleBank = fxValue;
        break;

    // MSU MD CD audio PLAY ONCE
    /*case 0x20:
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
        break;*/

    // MSU MD CD audio PLAY LOOP
    /*case 0x21:
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
        break;*/

    // MSU MD CD audio SEEK TIME EMULATION
    /*case 0x22:
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
        break;*/

    // Smooth PCM pan
    //case 0x2C:
        /*When Bit 5 of YM Register $2C is set to 1, Panning gets affected by the L/R part of the L/R/AMS/FMS reg of these channels:

        move.b #$2C, ($A04000)
        move.b #$20, ($A04001) ; Set bit 5 (this is the relevant bit starting from 0)

        $B4 in Bank 1 of the YM2612 for Channel FM1
        $B5 in Bank 1 of the YM2612 for Channel FM2
        $B6 in Bank 1 of the YM2612 for Channel FM3
        $B4 in Bank 2 of the YM2612 for Channel FM4
        $B6 in Bank 2 of the YM2612 for Channel FM6

        9 bit DAC
        2A represents D8 to D1
        D0 is located in bit 3 of register 2C.

        */
        //YM2612_writeRegZ80(0, 0x2C, 0x20);
        //break;
    // SWITCH PCM DRIVER
    case 0x70:
        switch (fxValue)
        {
        case 0:
            load_dac_driver(Z80_DRIVER_PCM);
            break;
        case 1:
            load_dac_driver(Z80_DRIVER_PCM4);
            break;
        case 2:
            load_dac_driver(Z80_DRIVER_DPCM2);
            break;
        default:
            break;
        }
        break;
    // CH1 PLAY
    case 0x71: dac_play(SOUND_PCM_CH1, SOUND_PCM_CH1_MSK, 0); break;
    case 0x75: dac_play(SOUND_PCM_CH1, SOUND_PCM_CH1_MSK, 1); break;
    case 0x79: dac_play(SOUND_PCM_CH1, SOUND_PCM_CH1_MSK, 2); break;
    case 0x7D: dac_play(SOUND_PCM_CH1, SOUND_PCM_CH1_MSK, 3); break;

    // CH2 PLAY
    case 0x72: dac_play(SOUND_PCM_CH2, SOUND_PCM_CH2_MSK, 0); break;
    case 0x76: dac_play(SOUND_PCM_CH2, SOUND_PCM_CH2_MSK, 1); break;
    case 0x7A: dac_play(SOUND_PCM_CH2, SOUND_PCM_CH2_MSK, 2); break;
    case 0x7E: dac_play(SOUND_PCM_CH2, SOUND_PCM_CH2_MSK, 3); break;

        break;
    // CH3 PLAY
    case 0x73: dac_play(SOUND_PCM_CH3, SOUND_PCM_CH3_MSK, 0); break;
    case 0x77: dac_play(SOUND_PCM_CH3, SOUND_PCM_CH3_MSK, 1); break;
    case 0x7B: dac_play(SOUND_PCM_CH3, SOUND_PCM_CH3_MSK, 2); break;
    case 0x7F: dac_play(SOUND_PCM_CH3, SOUND_PCM_CH3_MSK, 3); break;

        break;
    // CH4 PLAY
    case 0x74: dac_play(SOUND_PCM_CH4, SOUND_PCM_CH4_MSK, 0); break;
    case 0x78: dac_play(SOUND_PCM_CH4, SOUND_PCM_CH4_MSK, 1); break;
    case 0x7C: dac_play(SOUND_PCM_CH4, SOUND_PCM_CH4_MSK, 2); break;
    case 0x80: dac_play(SOUND_PCM_CH4, SOUND_PCM_CH4_MSK, 3); break;

    // PCM4 VOLUME
    case 0x81:
        if (fxValue < 0x10)
        {
            SND_PCM4_setVolume(SOUND_PCM_CH1, fxValue);
            SND_PCM4_setVolume(SOUND_PCM_CH2, fxValue);
            SND_PCM4_setVolume(SOUND_PCM_CH3, fxValue);
            SND_PCM4_setVolume(SOUND_PCM_CH4, fxValue);
        }
        else if (fxValue < 0x50)
        {
            SND_PCM4_setVolume((fxValue >> 4)-1, fxValue & 0x0F);
        }
    break;

    default: break;
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

    default: break;
    }
}

static void ApplyCommand_FM3_SP(u8 mtxCh, u8 fxParam, u8 fxValue)
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
        if (FM_CH3_Mode != CH3_SPECIAL_CSM) return;
        if (fxValue >= 0x09 && fxValue <= NOTE_MAX)
        {
            switch (mtxCh)
            {
            case CHANNEL_FM3_OP4: FM_CH3_OpFreq[0] = fxValue; break;
            case CHANNEL_FM3_OP3: FM_CH3_OpFreq[1] = fxValue; SetPitchFM(mtxCh, 0); break;
            case CHANNEL_FM3_OP2: FM_CH3_OpFreq[2] = fxValue; SetPitchFM(mtxCh, 0); break;
            case CHANNEL_FM3_OP1: FM_CH3_OpFreq[3] = fxValue; SetPitchFM(mtxCh, 0); break;
            }
        }
        break;

    default: break;
    }
}

static void ApplyCommand_FM(u8 mtxCh, u8 id, u8 fxParam, u8 fxValue)
{
    static u16 port = 0;    // chip port (0..1)
    static u8 ymCh = 0;       // chip channel (0..2)

    // TL; 0 - unused, 000 0000 - TL (0..127) high to low ~0.75db step
    auto void write_tl1() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_TL_CH0 + ymCh, chInst[mtxCh].TL1); }
    auto void write_tl2() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_TL_CH0 + ymCh, chInst[mtxCh].TL2); }
    auto void write_tl3() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_TL_CH0 + ymCh, chInst[mtxCh].TL3); }
    auto void write_tl4() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_TL_CH0 + ymCh, chInst[mtxCh].TL4); }

    // RS, AR
    // 2b - RS (0..3), 1b - unused, 5b - AR (0..31)
    auto void write_rs1_ar1()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_RS_AR_CH0 + ymCh, chInst[mtxCh].RS1_AR1);
    }
    auto void write_rs2_ar2()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_RS_AR_CH0 + ymCh, chInst[mtxCh].RS2_AR2);
    }
    auto void write_rs3_ar3()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_RS_AR_CH0 + ymCh, chInst[mtxCh].RS3_AR3);
    }
    auto void write_rs4_ar4()
    {
        CalculateCombined(mtxCh, COMB_RS_AR_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_RS_AR_CH0 + ymCh, chInst[mtxCh].RS4_AR4);
    }

    // DT, MUL (FM channels 0, 1, 2)
    // 1b - unused, 3b - DT1, 4b - MUL; DT1 = 1..-4+..8, MUL = 0..15
    auto void write_dt1_mul1()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT1_MUL1);
    }
    auto void write_dt2_mul2()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT2_MUL2);
    }
    auto void write_dt3_mul3()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT3_MUL3);

    }
    auto void write_dt4_mul4()
    {
        CalculateCombined(mtxCh, COMB_DT_MUL_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_DT_MUL_CH0 + ymCh, chInst[mtxCh].DT4_MUL4);
    }

    // FB, ALG
    // 2b - unused, 3b (0..7) - FB, 3b - ALG (0..7)
    auto void write_fb_alg()
    {
        CalculateCombined(mtxCh, COMB_FB_ALG);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_FB_ALG_CH0 + ymCh, chInst[mtxCh].FB_ALG);
    }

    // PAN, AMS, FMS
    // 2b - PAN (1..3), 3b - AMS (0..7), 1b - unused, 2b - FMS (0..3)
    auto void write_pan_ams_fms()
    {
        CalculateCombined(mtxCh, COMB_PAN_AMS_FMS);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_PAN_AMS_FMS_CH0 + ymCh, chInst[mtxCh].PAN_AMS_FMS);
    }

    // AM, D1R
    // 1b - AM (0 or 1), 2b - unused, 5b - D1R (0..31)
    auto void write_am1_d1r1()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM1_D1R1);
    }
    auto void write_am2_d1r2()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM2_D1R2);
    }
    auto void write_am3_d1r3()
    {
        CalculateCombined(mtxCh, COMB_AM_D1R_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_AM_D1R_CH0 + ymCh, chInst[mtxCh].AM3_D1R3);
    }
    auto void write_am4_d1r4()
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
    auto void write_ssgeg1() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG1); }
    auto void write_ssgeg2() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG2); }
    auto void write_ssgeg3() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG3); }
    auto void write_ssgeg4() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_SSGEG_CH0 + ymCh, chInst[mtxCh].SSGEG4); }

    // D1L, RR
    // 4b - D1L (0..15), 4b - RR (0..15)
    auto void write_d1l1_rr1()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_1);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L1_RR1);
    }
    auto void write_d1l2_rr2()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_2);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L2_RR2);
    }
    auto void write_d1l3_rr3()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_3);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L3_RR3);
    }
    auto void write_d1l4_rr4()
    {
        CalculateCombined(mtxCh, COMB_D1L_RR_4);
        if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_D1L_RR_CH0 + ymCh, chInst[mtxCh].D1L4_RR4);
    }

    // D2R
    // 3b - unused, 5b - D2R (0..31)
    auto void write_d2r1() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP1_D2R_CH0 + ymCh, chInst[mtxCh].D2R1); }
    auto void write_d2r2() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP2_D2R_CH0 + ymCh, chInst[mtxCh].D2R2); }
    auto void write_d2r3() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP3_D2R_CH0 + ymCh, chInst[mtxCh].D2R3); }
    auto void write_d2r4() { if (bWriteRegs) YM2612_writeRegZ80(port, YM2612REG_OP4_D2R_CH0 + ymCh, chInst[mtxCh].D2R4); }

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
            chInst[mtxCh].RS1 = tmpInst[id].RS1; chInst[mtxCh].RS2 = tmpInst[id].RS2;
            chInst[mtxCh].RS3 = tmpInst[id].RS3; chInst[mtxCh].RS4 = tmpInst[id].RS4;
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

    // PARAMETER SEQUENCE TYPE
    case 0x3E:
        channelParSeqTYPE[mtxCh] = fxValue;
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
        if (fxValue > 0x07) chInst[mtxCh].ALG = tmpInst[id].ALG;
        else chInst[mtxCh].ALG = fxValue;
        write_fb_alg();
        break;

    // OP1 FEEDBACK
    case 0x0B:
        if (fxValue > 0x07) chInst[mtxCh].FB = tmpInst[id].FB;
        else chInst[mtxCh].FB = fxValue;
        write_fb_alg();
        break;

    // AMS = amplitude modulation sensitivity/scale
    case 0x0C:
        if (fxValue > 0x03) chInst[mtxCh].AMS = tmpInst[id].AMS;
        else chInst[mtxCh].AMS = fxValue;
        write_pan_ams_fms();
        break;

    // FMS = frequency modulation sensitivity/scale (actually PMS  = phase modulation sensitivity/scale)
    case 0x0D:
        if (fxValue > 0x07) chInst[mtxCh].FMS = tmpInst[id].FMS;
        else chInst[mtxCh].FMS = fxValue;
        write_pan_ams_fms();
        break;

    // PAN
    case 0x0E:
        switch (fxValue)
        {
        case 0x00: chInst[mtxCh].PAN = tmpInst[id].PAN; if (mtxCh == CHANNEL_FM6_DAC) FM_CH6_DAC_Pan = NULL; break;
        case 0x10: chInst[mtxCh].PAN = 2; if (mtxCh == CHANNEL_FM6_DAC) FM_CH6_DAC_Pan = SOUND_PAN_LEFT; break;
        case 0x01: chInst[mtxCh].PAN = 1; if (mtxCh == CHANNEL_FM6_DAC) FM_CH6_DAC_Pan = SOUND_PAN_RIGHT; break;
        case 0x11: chInst[mtxCh].PAN = 3; if (mtxCh == CHANNEL_FM6_DAC) FM_CH6_DAC_Pan = SOUND_PAN_CENTER; break;
        default: chInst[mtxCh].PAN = 0; FM_CH6_DAC_Pan = NULL; break;
        }
        write_pan_ams_fms();
        break;

    default: break;
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

// ============================================================
// RecalcAllAddrs: scan SRAM instrument + sequencer compact
// blocks to rebuild instDataAddr[], seqDataAddr[], and the
// cached block boundary pointers.
// ============================================================
void RecalcAllAddrs()
{
    // --- Instrument block ---
    u16 modCount = SRAMW_readWord(INST_MOD_COUNT_ADDR);
    u32 ptr = INST_COMPACT_START;

    for (u16 i = 0; i < INSTRUMENTS_TOTAL; i++)
        instDataAddr[i] = 0;

    for (u16 i = 0; i < modCount; i++)
    {
        u8 id = SRAMW_readByte(ptr);
        instDataAddr[id] = (u16)ptr;
        ptr += INST_RECORD_SIZE;
    }
    instBlockEnd = ptr;

    // --- Sequencer block ---
    u32 seqBase = instBlockEnd;
    u16 seqModCount = SRAMW_readWord(seqBase);
    ptr = seqBase + 2 + INSTRUMENTS_TOTAL;   // skip modCount + lookup table

    for (u16 i = 0; i < INSTRUMENTS_TOTAL; i++)
        seqDataAddr[i] = 0;

    for (u16 i = 0; i < seqModCount; i++)
    {
        u8 id = SRAMW_readByte(ptr);
        seqDataAddr[id] = (u16)ptr;
        ptr += SEQ_RECORD_SIZE;
    }
    seqBlockEnd = ptr;
    patternRegionBase = seqBlockEnd;

    // Update SRAM usage
    u32 _rs = SRAMW_readWord(patternRegionBase + 4);
    sramUsed = patternRegionBase + _rs;
    DrawInfo();
}

// ============================================================
// Read a single instrument parameter.
// Param 0..48: FM data (ROM preset if unmodified, else SRAM).
// Param 49..56: name bytes (0 if unmodified, else SRAM).
// ============================================================
u8 SRAM_ReadInstrument(u8 id, u16 param)
{
    u8 lookup = SRAMW_readByte(INST_LOOKUP_TABLE_ADDR + id);
    if (lookup != INST_SENTINEL_MODIFIED)
    {
        // Unmodified — ROM preset for data, empty for name
        if (param < INST_DATA_SIZE)
            return ((const u8*)M_BANK_0[lookup])[paramToPresetByte[param]];
        return 0;
    }
    // Modified — read from compact record
    if (instDataAddr[id] != 0)
        return SRAMW_readByte(instDataAddr[id] + 1 + param);
    return 0;
}

// Forward declarations
void assimilateInstrument(u8 id);
void assimilateSeq(u8 id);
void ResetSeqToDefault(u8 id);

// ============================================================
// Write a single instrument parameter.
// First write auto-converts (assimilates) the instrument.
// ============================================================
void SRAM_WriteInstrument(u8 id, u16 param, u8 data)
{
    assimilateInstrument(id);
    SRAMW_writeByte(instDataAddr[id] + 1 + param, data);
}

// ============================================================
// Convert a ROM-preset instrument to SRAM mode.
// Shifts the tail (sequencers + patterns) forward by
// INST_RECORD_SIZE, writes a new compact record with all 49
// data bytes from ROM + 8 zero name bytes.
// ============================================================
void assimilateInstrument(u8 id)
{
    u8 lookup = SRAMW_readByte(INST_LOOKUP_TABLE_ADDR + id);
    if (lookup == INST_SENTINEL_MODIFIED) return;

    u16 modCount = SRAMW_readWord(INST_MOD_COUNT_ADDR);
    u32 recordAddr = INST_COMPACT_START + modCount * INST_RECORD_SIZE;

    // Shift tail (sequencers + patterns) forward to make room
    u32 regionSize = SRAMW_readWord(patternRegionBase + 4);
    u32 tailEnd = patternRegionBase + regionSize;
    u32 tailLen = tailEnd - instBlockEnd;

    for (s32 i = (s32)tailLen - 1; i >= 0; i--)
        SRAMW_writeByte(instBlockEnd + INST_RECORD_SIZE + i, SRAMW_readByte(instBlockEnd + i));

    // Write compact record (1 ID + 49 data + 8 name = 58 bytes)
    SRAMW_writeByte(recordAddr, id);
    for (u8 p = 0; p < INST_DATA_SIZE; p++)
        SRAMW_writeByte(recordAddr + 1 + p, ((const u8*)M_BANK_0[lookup])[paramToPresetByte[p]]);
    for (u8 p = 0; p < INST_NAME_SIZE; p++)
        SRAMW_writeByte(recordAddr + 1 + INST_DATA_SIZE + p, 0);

    SRAMW_writeByte(INST_LOOKUP_TABLE_ADDR + id, INST_SENTINEL_MODIFIED);
    SRAMW_writeWord(INST_MOD_COUNT_ADDR, modCount + 1);
    RecalcAllAddrs();
}

// ============================================================
// Convert instrument back to ROM-preset mode.
// Removes its compact record, shifts tail (sequencers +
// patterns) backward.
// ============================================================
void SRAM_ResetInstrumentToPreset(u8 id, u8 preset)
{
    if (SRAMW_readByte(INST_LOOKUP_TABLE_ADDR + id) == INST_SENTINEL_MODIFIED && instDataAddr[id] != 0)
    {
        u16 oldModCount = SRAMW_readWord(INST_MOD_COUNT_ADDR);
        u32 delAddr = instDataAddr[id];

        // Shift remaining instrument compact records backward
        u32 tailStart = delAddr + INST_RECORD_SIZE;
        u32 tailEnd = INST_COMPACT_START + oldModCount * INST_RECORD_SIZE;
        for (u32 i = 0; i < tailEnd - tailStart; i++)
            SRAMW_writeByte(delAddr + i, SRAMW_readByte(tailStart + i));

        // Shift sequencers + patterns backward to fill the gap
        u32 regionSize = SRAMW_readWord(patternRegionBase + 4);
        u32 dataTailEnd = patternRegionBase + regionSize;
        u32 dataTailLen = dataTailEnd - instBlockEnd;
        for (u32 i = 0; i < dataTailLen; i++)
            SRAMW_writeByte(instBlockEnd - INST_RECORD_SIZE + i, SRAMW_readByte(instBlockEnd + i));

        SRAMW_writeWord(INST_MOD_COUNT_ADDR, oldModCount - 1);
    }

    SRAMW_writeByte(INST_LOOKUP_TABLE_ADDR + id, preset);
    instDataAddr[id] = 0;
    RecalcAllAddrs();
}

// ============================================================
// SEQ functions with transparent edit-buffer support.
// When seqEditID matches the requested instrument, reads/writes
// go to the RAM buffer instead of SRAM.
// ============================================================
u8 SRAM_ReadSEQ_PAR(u8 id, u8 step)
{
    if (id == seqEditID && seqEditID != 0xFFFF)
        return seqEditBuffer[step];
    u8 lookup = SRAMW_readByte(instBlockEnd + 2 + id);
    if (lookup == SEQ_SENTINEL_MODIFIED && seqDataAddr[id] != 0)
        return SRAMW_readByte(seqDataAddr[id] + 1 + step);
    return SEQ_SKIP;
}

void SRAM_WriteSEQ_PAR(u8 id, u8 step, u8 data)
{
    if (id == seqEditID && seqEditID != 0xFFFF)
    {
        seqEditBuffer[step] = data;
        return;
    }
    // Direct write when not being context-edited (e.g. pattern commands)
    assimilateSeq(id);
    SRAMW_writeByte(seqDataAddr[id] + 1 + step, data);
}

u8 SRAM_ReadSEQ_ARP(u8 id, u8 step)
{
    if (id == seqEditID && seqEditID != 0xFFFF)
        return seqEditBuffer[SEQ_STEPS + step];
    u8 lookup = SRAMW_readByte(instBlockEnd + 2 + id);
    if (lookup == SEQ_SENTINEL_MODIFIED && seqDataAddr[id] != 0)
        return SRAMW_readByte(seqDataAddr[id] + 1 + SEQ_STEPS + step);
    return NOTE_EMPTY;
}

void SRAM_WriteSEQ_ARP(u8 id, u8 step, u8 data)
{
    if (id == seqEditID && seqEditID != 0xFFFF)
    {
        seqEditBuffer[SEQ_STEPS + step] = data;
        return;
    }
    assimilateSeq(id);
    SRAMW_writeByte(seqDataAddr[id] + 1 + SEQ_STEPS + step, data);
}

// ============================================================
// Create a compact sequencer record for an instrument.
// Shifts the pattern tail forward by SEQ_RECORD_SIZE.
// ============================================================
void assimilateSeq(u8 id)
{
    u8 lookup = SRAMW_readByte(instBlockEnd + 2 + id);
    if (lookup == SEQ_SENTINEL_MODIFIED) return;

    u16 modCount = SRAMW_readWord(instBlockEnd);  // SEQ_MOD_COUNT_ADDR
    u32 recordAddr = instBlockEnd + 2 + INSTRUMENTS_TOTAL + modCount * SEQ_RECORD_SIZE;

    // Shift pattern region forward
    u32 regionSize = SRAMW_readWord(patternRegionBase + 4);
    u32 tailEnd = patternRegionBase + regionSize;
    u32 tailLen = tailEnd - seqBlockEnd;
    for (s32 i = (s32)tailLen - 1; i >= 0; i--)
        SRAMW_writeByte(seqBlockEnd + SEQ_RECORD_SIZE + i, SRAMW_readByte(seqBlockEnd + i));

    // Write record (1 ID + 32 VOL + 32 ARP = 65 bytes), defaults
    SRAMW_writeByte(recordAddr, id);
    for (u8 s = 0; s < SEQ_STEPS; s++)
    {
        SRAMW_writeByte(recordAddr + 1 + s, SEQ_SKIP);
        SRAMW_writeByte(recordAddr + 1 + SEQ_STEPS + s, NOTE_EMPTY);
    }

    SRAMW_writeByte(instBlockEnd + 2 + id, SEQ_SENTINEL_MODIFIED);
    SRAMW_writeWord(instBlockEnd, modCount + 1);
    RecalcAllAddrs();
}

// ============================================================
// Remove a sequencer compact record (all steps are default).
// Shifts the pattern tail backward by SEQ_RECORD_SIZE.
// ============================================================
void ResetSeqToDefault(u8 id)
{
    if (SRAMW_readByte(instBlockEnd + 2 + id) == SEQ_SENTINEL_MODIFIED && seqDataAddr[id] != 0)
    {
        u16 oldModCount = SRAMW_readWord(instBlockEnd);
        u32 delAddr = seqDataAddr[id];
        u32 tailStart = delAddr + SEQ_RECORD_SIZE;
        u32 tailEnd = instBlockEnd + 2 + INSTRUMENTS_TOTAL + oldModCount * SEQ_RECORD_SIZE;

        // Shift remaining SEQ compact records backward
        for (u32 i = 0; i < tailEnd - tailStart; i++)
            SRAMW_writeByte(delAddr + i, SRAMW_readByte(tailStart + i));

        // Shift pattern region backward
        u32 regionSize = SRAMW_readWord(patternRegionBase + 4);
        u32 dataTailEnd = patternRegionBase + regionSize;
        u32 dataTailLen = dataTailEnd - seqBlockEnd;
        for (u32 i = 0; i < dataTailLen; i++)
            SRAMW_writeByte(seqBlockEnd - SEQ_RECORD_SIZE + i, SRAMW_readByte(seqBlockEnd + i));

        SRAMW_writeWord(instBlockEnd, oldModCount - 1);
    }
    SRAMW_writeByte(instBlockEnd + 2 + id, 0);
    seqDataAddr[id] = 0;
    RecalcAllAddrs();
}

// ============================================================
// Commit the SEQ edit buffer to SRAM.
// If entirely default, removes the compact record (compaction).
// ============================================================
void CommitSeqEditBuffer()
{
    if (seqEditID == 0xFFFF) return;

    bool allDefault = TRUE;
    for (u8 s = 0; s < SEQ_STEPS; s++)
    {
        if (seqEditBuffer[s] != SEQ_SKIP)         { allDefault = FALSE; break; }
        if (seqEditBuffer[SEQ_STEPS + s] != NOTE_EMPTY) { allDefault = FALSE; break; }
    }

    if (allDefault)
    {
        ResetSeqToDefault(seqEditID);
    }
    else
    {
        assimilateSeq(seqEditID);
        u32 addr = seqDataAddr[seqEditID];
        for (u8 s = 0; s < SEQ_STEPS; s++)
        {
            SRAMW_writeByte(addr + 1 + s, seqEditBuffer[s]);
            SRAMW_writeByte(addr + 1 + SEQ_STEPS + s, seqEditBuffer[SEQ_STEPS + s]);
        }
    }
    seqEditID = 0xFFFF;
}

// ============================================================
// Load the SEQ edit buffer for a given instrument.
// ============================================================
void LoadSeqEditBuffer(u8 id)
{
    for (u8 s = 0; s < SEQ_STEPS; s++)
    {
        seqEditBuffer[s]            = SRAM_ReadSEQ_PAR(id, s);
        seqEditBuffer[SEQ_STEPS + s] = SRAM_ReadSEQ_ARP(id, s);
    }
    seqEditID = id;
}

// pattern - new event-based format
// Read from event-encoded SRAM (raw read, no edit buffer)
u8 SRAM_ReadPatternFromEvents(u16 id, u8 line, u8 param)
{
    u32 offset = patternRegionBase + patternOffset[id] * 2u;
    u32 evtBase;
    u16 numEvents;
    u8 tick, dataHi, dataLo, targetTick;

    if (patternOffset[id] == 0)
        return (param == DATA_NOTE) ? NOTE_EMPTY : 0;

    numEvents = SRAMW_readWord(offset + 2);
    evtBase = offset + 4;

    targetTick = line * EVT_COUNT + COL_TO_EVT(param);

    for (u16 i = 0; i < numEvents; i++)
    {
        tick = SRAMW_readByte(evtBase + i * 3);
        if (tick == targetTick)
        {
            dataHi = SRAMW_readByte(evtBase + i * 3 + 1);
            dataLo = SRAMW_readByte(evtBase + i * 3 + 2);
            return (param & 1) ? dataLo : dataHi;
        }
        if (tick > targetTick) break;
    }
    return (param == DATA_NOTE) ? NOTE_EMPTY : 0;
}

// Read pattern: if editing pattern, read from editBuffer; else from SRAM events
u8 SRAM_ReadPattern(u16 id, u8 line, u8 param)
{
    if (id == patternEditID && patternEditID != 0xFFFF)
        return editBuffer[line * PATTERN_COLUMNS + param];
    return SRAM_ReadPatternFromEvents(id, line, param);
}

// Write pattern: if editing pattern, write to editBuffer only
// Committed to SRAM on pattern editor exit
void SRAM_WritePattern(u16 id, u8 line, u8 param, u8 data)
{
    if (id == patternEditID && patternEditID != 0xFFFF)
        editBuffer[line * PATTERN_COLUMNS + param] = data;
}

// Fill a 14-byte row buffer from event-encoded pattern (for playback)
void SRAM_ReadRowToBuffer(u16 id, u8 line, u8* buf)
{
    if (id == patternEditID && patternEditID != 0xFFFF)
    {
        u16 base = line * PATTERN_COLUMNS;
        for (u16 i = 0; i < PATTERN_COLUMNS; i++)
            buf[i] = editBuffer[base + i];
        return;
    }

    u16 numEvents;
    u8 tick, evtPos, dataHi, dataLo;
    u32 offset, evtBase;

    for (u16 i = 0; i < PATTERN_COLUMNS; i++)
        buf[i] = (i == DATA_NOTE) ? NOTE_EMPTY : 0;

    if (patternOffset[id] == 0) return;
    offset = patternRegionBase + patternOffset[id] * 2u;

    numEvents = SRAMW_readWord(offset + 2);
    evtBase = offset + 4;

    for (u16 i = 0; i < numEvents; i++)
    {
        tick = SRAMW_readByte(evtBase + i * 3);
        if (tick / EVT_COUNT > line) break;
        if (tick / EVT_COUNT < line) continue;

        evtPos = tick % EVT_COUNT;
        dataHi = SRAMW_readByte(evtBase + i * 3 + 1);
        dataLo = SRAMW_readByte(evtBase + i * 3 + 2);

        if (evtPos == EVT_NOTEINST)
        {
            buf[DATA_NOTE] = dataHi;
            buf[DATA_INSTRUMENT] = dataLo;
        }
        else
        {
            buf[EVT_TO_COL_TYPE(evtPos)] = dataHi;
            buf[EVT_TO_COL_VALUE(evtPos)] = dataLo;
        }
    }
}

// Unpack entire pattern from SRAM events to editBuffer
void SRAM_UnpackToBuffer(u16 id)
{
    u16 i, numEvents;
    u8 tick, evtPos, dataHi, dataLo;
    u32 offset, evtBase;

    if (!editBuffer)
    {
        editBuffer = MEM_alloc(PATTERN_SIZE);
        if (!editBuffer) { patternEditID = 0xFFFF; return; }
    }
    for (i = 0; i < PATTERN_SIZE; i++)
        editBuffer[i] = 0;
    for (i = 0; i < PATTERN_ROWS; i++)
        editBuffer[i * PATTERN_COLUMNS + DATA_NOTE] = NOTE_EMPTY;
    patternEditID = id;

    if (patternOffset[id] == 0)
    {
        for (i = 0; i < PATTERN_ROWS; i++)
            editBuffer[i * PATTERN_COLUMNS + DATA_NOTE] = NOTE_EMPTY;
        return;
    }
    offset = patternRegionBase + patternOffset[id] * 2u;
    numEvents = SRAMW_readWord(offset + 2);
    evtBase = offset + 4;

    for (i = 0; i < numEvents; i++)
    {
        tick = SRAMW_readByte(evtBase + i * 3);
        evtPos = tick % EVT_COUNT;
        dataHi = SRAMW_readByte(evtBase + i * 3 + 1);
        dataLo = SRAMW_readByte(evtBase + i * 3 + 2);

        if (evtPos == EVT_NOTEINST)
        {
            editBuffer[(tick / EVT_COUNT) * PATTERN_COLUMNS + DATA_NOTE] = dataHi;
            editBuffer[(tick / EVT_COUNT) * PATTERN_COLUMNS + DATA_INSTRUMENT] = dataLo;
        }
        else
        {
            editBuffer[(tick / EVT_COUNT) * PATTERN_COLUMNS + EVT_TO_COL_TYPE(evtPos)] = dataHi;
            editBuffer[(tick / EVT_COUNT) * PATTERN_COLUMNS + EVT_TO_COL_VALUE(evtPos)] = dataLo;
        }
    }
}

// Pack a 14-byte row into output buffer and return number of events written
static u16 packRow(u8* rowData, u8 row, u8* out, u16 outIdx)
{
    u8 note = rowData[DATA_NOTE];
    u8 inst = rowData[DATA_INSTRUMENT];
    u16 count = 0;

    if (note <= NOTE_MAX || note == NOTE_OFF || inst)
    {
        out[outIdx] = row * EVT_COUNT + EVT_NOTEINST;
        out[outIdx + 1] = note;
        out[outIdx + 2] = inst;
        outIdx += 3; count++;
    }
    return count;
}

static u16 packFX(u8* rowData, u8 row, u8 fxIdx, u8* out, u16 outIdx)
{
    u8 typeCol = DATA_FX1_TYPE + fxIdx * 2;
    u8 valCol = DATA_FX1_VALUE + fxIdx * 2;
    u8 type = rowData[typeCol];
    u8 val = rowData[valCol];

    if (type || val)
    {
        out[outIdx] = row * EVT_COUNT + (EVT_FX1 + fxIdx);
        out[outIdx + 1] = type;
        out[outIdx + 2] = val;
        outIdx += 3;
        return 1;
    }
    return 0;
}

// Write packed block to SRAM at specified offset.
// Color is packed into upper 6 bits of blockId.
// numEvents is passed, data is in buffer at events[0..numEvents*3-1]
// This is an internal helper used by SRAM_CommitBuffer
static void writeBlockToSRAM(u32 sramOffset, u16 id, u16 numEvents, u8* events, u8 color)
{
    u16 dataBytes = numEvents * 3;
    SRAMW_writeWord(sramOffset, id | ((u16)color << 10));
    SRAMW_writeWord(sramOffset + 2, numEvents);
    for (u16 i = 0; i < dataBytes; i++)
        SRAMW_writeByte(sramOffset + 4 + i, events[i]);
    if (dataBytes & 1)
        SRAMW_writeByte(sramOffset + 4 + dataBytes, 0);
}

// Pack editBuffer and write/update pattern block in SRAM.
// Shift subsequent blocks in SRAM to accommodate size change.
void SRAM_CommitBuffer(u16 id)
{
    // Temp buffer for packed events (max 148 events * 3 bytes = 444, plus 4 header = 448)
    // We know packed never exceeds 448 bytes (threshold for raw format)
    u8 packed[448];
    u16 numEvents = 0, oldNumEvents;
    u32 regionBase = patternRegionBase;
    u32 regionSize, newOffset, tailStart, tailLen, oldBlockSize, newBlockSize;
    s32 delta;
    u32 absOffset = regionBase + patternOffset[id] * 2u;

    // Pack the editBuffer
    for (u8 row = 0; row < PATTERN_ROWS; row++)
    {
        u8* rowData = &editBuffer[row * PATTERN_COLUMNS];
        numEvents += packRow(rowData, row, packed, numEvents * 3);
        for (u8 fx = 0; fx < EFFECTS_TOTAL; fx++)
            numEvents += packFX(rowData, row, fx, packed, numEvents * 3);
    }

    newBlockSize = 4 + numEvents * 3 + (numEvents & 1);

    // Read region size
    regionSize = (u32)SRAMW_readWord(regionBase + 4);

    if (patternOffset[id] != 0)
    {
        // Update existing pattern
        oldNumEvents = SRAMW_readWord(absOffset + 2);
        oldBlockSize = 4 + oldNumEvents * 3 + (oldNumEvents & 1);
        delta = newBlockSize - oldBlockSize;

        if (delta > 0)
        {
            // Growing - shift tail forward, copy BACKWARD
            tailStart = absOffset + oldBlockSize;
            tailLen = (regionBase + regionSize) - tailStart;
            for (s32 i = (s32)tailLen - 1; i >= 0; i--)
                SRAMW_writeByte(tailStart + delta + i, SRAMW_readByte(tailStart + i));
        }
        else if (delta < 0)
        {
            // Shrinking - shift tail backward, copy FORWARD
            tailStart = absOffset + oldBlockSize;
            tailLen = (regionBase + regionSize) - tailStart;
            for (u32 i = 0; i < tailLen; i++)
                SRAMW_writeByte(tailStart + delta + i, SRAMW_readByte(tailStart + i));
        }

        // Write new block, preserving existing color
        u8 color = (SRAMW_readWord(absOffset) >> 10) & 0x3F;
        writeBlockToSRAM(absOffset, id, numEvents, packed, color);

        // Update region size
        regionSize += delta;
        SRAMW_writeWord(regionBase + 4, (u16)regionSize);

        // Update offset table in RAM for patterns after this one
        for (u16 pid = 0; pid <= PATTERN_LAST; pid++)
        {
            if (patternOffset[pid] != 0 && regionBase + patternOffset[pid] * 2u > absOffset)
            {
                u32 newRelBytes = patternOffset[pid] * 2u + delta;
                patternOffset[pid] = (u16)(newRelBytes / 2);
            }
        }
    }
    else
    {
        // New pattern - append at end with color 0
        newOffset = regionBase + regionSize;
        writeBlockToSRAM(newOffset, id, numEvents, packed, 0);
        patternOffset[id] = (u16)((newOffset - regionBase) / 2);
        regionSize += newBlockSize;
        SRAMW_writeWord(regionBase + 4, (u16)regionSize);
    }
    sramUsed = regionBase + regionSize;
    DrawInfo();
    if (editBuffer) { MEM_free(editBuffer); editBuffer = NULL; }
}

// Scan the pattern region and build the offset table in RAM
void SRAM_ScanPatternRegion()
{
    u32 regionBase = patternRegionBase;
    u32 regionSize;
    u32 ptr;

    for (u16 i = 0; i <= PATTERN_LAST; i++)
        patternOffset[i] = 0;

    if (SRAMW_readWord(regionBase) != PATTERN_MAGIC)
        return;

    regionSize = SRAMW_readWord(regionBase + 4);

    ptr = regionBase + 6;
    while (ptr < regionBase + regionSize)
    {
        u16 raw = SRAMW_readWord(ptr);
        u16 blockId = raw & 0x3FF;
        u16 blockNumEvents = SRAMW_readWord(ptr + 2);
        if (blockId <= PATTERN_LAST)
            patternOffset[blockId] = (u16)((ptr - regionBase) / 2);
        ptr += 4 + blockNumEvents * 3 + (blockNumEvents & 1);
    }
}

// Pattern color is packed into upper 6 bits of the pattern block header's blockId
u8 SRAM_ReadPatternColor(u16 id) {
    if (patternOffset[id] == 0) return 0;
    u32 offset = patternRegionBase + patternOffset[id] * 2u;
    return (SRAMW_readWord(offset) >> 10) & 0x3F;
}
void SRAM_WritePatternColor(u16 id, u8 color) {
    if (patternOffset[id] == 0) return;
    u32 offset = patternRegionBase + patternOffset[id] * 2u;
    u16 raw = SRAMW_readWord(offset);
    SRAMW_writeWord(offset, (raw & 0x3FF) | ((u16)(color & 0x3F) << 10));
}

u16 SRAM_ReadMatrix(u8 channel, u8 line) { return SRAMW_readWord((u32)SRAM_PATTERN_MATRIX + ((channel * MATRIX_ROWS) + line) * 2); }
void SRAM_WriteMatrix(u8 channel, u8 line, u16 data) { SRAMW_writeWord((u32)SRAM_PATTERN_MATRIX + ((channel * MATRIX_ROWS) + line) * 2, data); }

// Transpose is packed into upper 6 bits of combined pattern matrix cell.
// Bits 0-9: pattern ID, Bits 10-15: transpose (signed 6-bit, -32..+31).
static u8 transposeUnpack(u16 combined) {
    u8 raw = (combined >> 10) & 0x3F;
    return (raw >= 32) ? raw - 64 : raw;
}
static u16 transposePack(s8 transpose) {
    return (u16)(transpose & 0x3F) << 10;
}

s8 SRAM_ReadMatrixTranspose(u8 channel, u8 line) { return (s8)transposeUnpack(SRAM_ReadMatrix(channel, line)); }
void SRAM_WriteMatrixTranspose(u8 channel, u8 line, s8 transpose) {
    u16 combined = SRAM_ReadMatrix(channel, line);
    SRAM_WriteMatrix(channel, line, (combined & 0x3FF) | transposePack(transpose));
}
void SRAM_WritePatternID(u8 channel, u8 line, u16 patternID) {
    u16 combined = SRAM_ReadMatrix(channel, line);
    SRAM_WriteMatrix(channel, line, (combined & 0xFC00) | (patternID & 0x3FF));
}

// MUTE_CHANNEL is RAM-only (channelFlags[])
u8 SRAM_ReadMatrixChannelEnabled(u8 channel) { return channelFlags[channel]; }
void SRAM_WriteMatrixChannelEnabled(u8 channel, u8 state) { channelFlags[channel] = state; }

// other
static u8 ym2612Z80BatchDepth = 0;

void YM2612_writeRegZ80(u16 part, u8 reg, u8 data)
{
    if (!ym2612Z80BatchDepth) RequestZ80();
    YM2612_writeReg(part, reg, data);
    if (!ym2612Z80BatchDepth) ReleaseZ80();
}

void YM2612_writeRegBatchBegin()
{
    RequestZ80();
    ym2612Z80BatchDepth++;
}

void YM2612_writeRegBatchEnd()
{
    ym2612Z80BatchDepth--;
    if (!ym2612Z80BatchDepth) ReleaseZ80();
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

    VDP_init();
    VDP_setDMAEnabled(TRUE);
    VDP_setHInterrupt(TRUE);
    VDP_setHIntCounter(H_INT_CALLS_SKIP-1);
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
    PAL_setPaletteColors(0, &palette_gui, DMA);
    PAL_setPaletteColors(16, &palette_1, DMA);
    PAL_setPaletteColors(32, &palette_2, DMA);
    PAL_setPaletteColors(48, &palette_3, DMA);

    // Double digit font 00(--)..FF
    u16 ind;
    ind = TILE_USER_INDEX;
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
    // Color dots
    bgBaseTileIndex[4] = ind;
    VDP_loadTileSet(&tileset_colordots, ind, DMA);
    ind += tileset_colordots.numTile;

    VDP_setTextPalette(PAL0);

    SRAM_enable();

    PSG_reset();
    YM2612_reset();
    Z80_init();

    Z80_loadDriver(Z80_DRIVER_PCM, TRUE);
    //Z80_loadDriver(Z80_DRIVER_DPCM2, TRUE);
    //Z80_loadDriver(Z80_DRIVER_PCM4, TRUE);
    //Z80_loadDriver(Z80_DRIVER_XGM, TRUE);
    //Z80_loadCustomDriver((u8*)0x61E, (u16)0x1B36); // dualpcm_drv in symbols.txt, bin file size

    Z80_setForceDelayDMA(TRUE);
    //Z80_enableBusProtection(); // for XGM

    JOY_setSupport(PORT_1, JOY_SUPPORT_6BTN);
    JOY_setSupport(PORT_2, JOY_SUPPORT_6BTN);
    JOY_setEventHandler(JoyEvent);

    comm_midi_init();
    midi_sync_init();

    //ReColorsAndTranspose(); // need SRAM

    // Check if valid save file exists by reading header string (SRAM[0..5]) directly
    for (u8 i = 0; i < 6; i++) str[i] = SRAM_readByte_Odd(i);

    if (strcmp(str, MDT_HEADER) != 0)
    {
        // No valid save — fresh init everything
        VDP_setTextPalette(PAL0); VDP_drawText("GENERATING MODULE DATA", 3, 3);

        // --- Block 1: Static data (fixed addresses) ---
        SetBPM(DEFAULT_TEMPO);
        SRAMW_writeByte(SRAM_GLOBAL_LFO, 7);

        // --- Block 2: Instruments ---
        SRAMW_writeWord(INST_MOD_COUNT_ADDR, 0);
        for (u16 inst = 0; inst <= INSTRUMENTS_LAST; inst++)
            SRAMW_writeByte(INST_LOOKUP_TABLE_ADDR + inst, 0);

        // No name block or SEQ data — defaults are implicit

        instBlockEnd = INST_COMPACT_START;  // modCount=0, no compact records

        // --- Block 3: Sequencers ---
        SRAMW_writeWord(instBlockEnd, 0);  // seqModCount = 0
        for (u16 inst = 0; inst <= INSTRUMENTS_LAST; inst++)
            SRAMW_writeByte(instBlockEnd + 2 + inst, 0);

        seqBlockEnd = instBlockEnd + 2 + INSTRUMENTS_TOTAL;  // = instBlockEnd + 258
        patternRegionBase = seqBlockEnd;

        // --- Block 4: Patterns ---
        SRAMW_writeWord(patternRegionBase, PATTERN_MAGIC);
        SRAMW_writeWord(patternRegionBase + 2, PATTERN_FORMAT_VERSION);
        SRAMW_writeWord(patternRegionBase + 4, 6);

        // Matrix (static data continued)
        for (u8 channel = CHANNEL_FM1; channel < CHANNELS_TOTAL; channel++)
        {
            channelFlags[channel] = TRUE;
            VDP_fillTileMapRect(BG_B, NULL, (channel * 3) + 1, 1, 2, 1);
            for (u8 row = 0; row < MATRIX_ROWS; row++)
                SRAM_WriteMatrix(channel, row, NULL);
        }

        RecalcAllAddrs();
        FileWriteHeader();
    }
    else
    {
        RecalcAllAddrs();
        SetBPM(NULL);
    }

    // Build pattern offset table from SRAM
    SRAM_ScanPatternRegion();

    // init chEventIdx
    for (u8 i = 0; i < CHANNELS_TOTAL; i++)
        chEventIdx[i] = 0;

    // init
    for (u8 mtxCh = CHANNEL_FM1; mtxCh < CHANNELS_TOTAL; mtxCh++)
    {
        channelPreviousInstrument[mtxCh] =
        channelPreviousEffectType[mtxCh][0] =
        channelPreviousEffectType[mtxCh][1] =
        channelPreviousEffectType[mtxCh][2] =
        channelArpSeqID[mtxCh] =
        channelArpSeqActive[mtxCh] =
        channelVibratoMode[mtxCh] =
        channelParSeqID[mtxCh] =
        channelParSeqActive[mtxCh] =
        channelNoteCut[mtxCh] = 0;

        channelPreviousNote[mtxCh] = NOTE_OFF;

        channelTremoloSpeedMult[mtxCh] = 0x20;
        channelVibratoSpeedMult[mtxCh] = 0x08;
        channelVibratoDepthMult[mtxCh] = 0x02;

        // MUTE_CHANNEL is RAM-only — channelFlags[] stays at its
        // initialized default (TRUE) on fresh start
        DrawMute(mtxCh);
    }

    sampleBankSize = sizeof(sample_bank_1);
    SetGlobalLFO(SRAMW_readByte(SRAM_GLOBAL_LFO));

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
        CacheInstrumentToRAM(id);
        instrumentIsMuted[id] = INST_PLAY;

        for (u8 step = 0; step <= SEQ_STEP_LAST; step++)
        {
            seqParValue[id][step] = SRAM_ReadSEQ_PAR(id, step);
            seqArpValue[id][step] = SRAM_ReadSEQ_ARP(id, step);
        }
    }

    for (u8 bank = 0; bank < 4; bank++)
    {
        for (u8 note = 0; note < NOTES; note++)
        {
            const struct SampleSettings* s = GetSampleSettings(bank, note);
            sampleStart[bank][note] = sample_bank_1 + s->startOffset;
            sampleLength[bank][note] = s->endOffset - s->startOffset;
            sampleRate[bank][note] = s->rate;
            samplePan[bank][note] = s->pan;
            sampleLoop[bank][note] = s->looped;
        }
    }

    DrawStaticGUI();
    VDP_waitDMACompletion();

    SYS_enableInts();

    /*
    Vertical interrupt (V-INT): level 6
    Horizontal interrupt (H-INT): level 4
    External interrupt (EX-INT): level 2
    */

    SYS_setHIntCallback(*hIntCallback);
    SYS_setVIntCallback(*vIntCallback);
    SYS_setInterruptMaskLevel(1);
}

void FileWriteHeader()
{
    for (u8 i = 0; i < 6; i++)
    {
        SRAM_writeByte_Odd(i, MDT_HEADER[i]); // write file version
        SRAM_writeByte(i, 0); // clean spaces
    }
}

void ForceResetVariables()
{
    ForceResetGUI();
    playingMatrixRow=
    selectedMatrixScreenRow=
    selectedMatrixRow=
    currentPage=
    selectedPatternRow=
    selectedPatternColumn=
    selectedPatternID=
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
    selectedMatrixChannel=0;

    lastEnteredEffect=
    lastEnteredEffectValue=
    lastEnteredInstrumentID=
    lastEnteredPattern=
    bRefreshScreen=
    bDAC_enable=
    bWriteRegs=
    updateCursor=
    selectedInstrumentID=1;

    H_INT_CALLS_SKIP = 32;

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
        channelArpSeqActive[ch]=
        channelParSeqID[ch]=
        channelParSeqActive[ch]=
        channelCurrentRowNote[ch]=
        channelSEQCounter_PAR[ch]=
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
        channelVolumeAttenuation[ch]=
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
        channelArpSeqTriggerType[ch]=
        channelNoteTriggerType[ch]=
        channelSeqSkipStep[ch]=
        channelArpSkipStep[ch]=
        channelNoteRetriggerCounter[ch]=0;

        channelFlags[ch]=
        channelArpSeqPlayMODE[ch]=
        channelParSeqPlayMODE[ch]=1;
        channelParSeqTYPE[ch]=0x41;

        for (u8 ef=0; ef<EFFECTS_TOTAL; ef++)
        {
            channelPreviousEffectType[ch][ef]=0;
        }

        for (u8 row=0; row<PATTERN_ROWS; row++)
        {
            channelRowShift[ch][row]=0;
        }
    }
}
