#include "MDT_Info.h"

void InitInfo()
{
    //{ commands
    infoCommands[0x00] = "";
    infoCommands[0x01] = "FM TOTAL LEVEL [OP1]            ";
    infoCommands[0x02] = "FM TOTAL LEVEL [OP2]            ";
    infoCommands[0x03] = "FM TOTAL LEVEL [OP3]            ";
    infoCommands[0x04] = "FM TOTAL LEVEL [OP4]            ";
    infoCommands[0x05] = "FM ADSR RATE SCALE              ";
    infoCommands[0x06] = "FM FREQUENCY MULTIPLIER         ";
    infoCommands[0x07] = "FM DETUNE                       ";
    infoCommands[0x08] = "FM AMPLITUDE MODULATION         ";
    infoCommands[0x09] = "FM SSG-EG                       ";
    infoCommands[0x0A] = "FM ALGORITHM                    ";
    infoCommands[0x0B] = "FM [OP1] FEEDBACK               ";
    infoCommands[0x0C] = "FM AMPLITUDE MODULATION SCALE   ";
    infoCommands[0x0D] = "FM FREQUENCY MODULATION SCALE   ";
    infoCommands[0x0E] = "FM/DAC PAN                      ";
    infoCommands[0x0F] = "";
    infoCommands[0x10] = "GLOBAL LFO                      ";
    infoCommands[0x11] = "FM CH.6                         ";
    infoCommands[0x12] = "FM CH.3                         ";
    infoCommands[0x13] = "TEMPO                           ";
    infoCommands[0x14] = "PATTERN ROW RESOLUTION (PULSES) ";
    infoCommands[0x15] = "PSG CH.4 NOISE MODE             ";
    infoCommands[0x16] = "PCM BANK                        ";
    infoCommands[0x17] = "";
    infoCommands[0x18] = "";
    infoCommands[0x19] = "";
    infoCommands[0x1A] = "";
    infoCommands[0x1B] = "";
    infoCommands[0x1C] = "";
    infoCommands[0x1D] = "";
    infoCommands[0x1E] = "";
    infoCommands[0x1F] = "";
    infoCommands[0x20] = "MSU MD: CD AUDIO - PLAY ONCE    ";
    infoCommands[0x21] = "MSU MD: CD AUDIO - PLAY LOOP    ";
    infoCommands[0x22] = "MSU MD: CD AUDIO - SEEK TIME    ";
    infoCommands[0x23] = "";
    infoCommands[0x24] = "";
    infoCommands[0x25] = "";
    infoCommands[0x26] = "";
    infoCommands[0x27] = "";
    infoCommands[0x28] = "";
    infoCommands[0x29] = "";
    infoCommands[0x2A] = "";
    infoCommands[0x2B] = "";
    infoCommands[0x2C] = "";
    infoCommands[0x2D] = "";
    infoCommands[0x2E] = "";
    infoCommands[0x2F] = "ARP SEQUENCE MACRO MODE         ";
    infoCommands[0x30] = "ARP SEQUENCE MACRO              ";
    infoCommands[0x31] = "PITCH SLIDE UP                  ";
    infoCommands[0x32] = "PITCH SLIDE DOWN                ";
    infoCommands[0x33] = "VIBRATO                         ";
    infoCommands[0x34] = "VIBRATO SPEED                   ";
    infoCommands[0x35] = "VIBRATO DEPTH                   ";
    infoCommands[0x36] = "VIBRATO MODE                    ";
    infoCommands[0x37] = "PITCH SLIDE SKIP PULSES         ";
    infoCommands[0x38] = "";
    infoCommands[0x39] = "";
    infoCommands[0x3A] = "";
    infoCommands[0x3B] = "";
    infoCommands[0x3C] = "";
    infoCommands[0x3D] = "";
    infoCommands[0x3E] = "";
    infoCommands[0x3F] = "VOL SEQUENCE MACRO MODE         ";
    infoCommands[0x40] = "VOL SEQUENCE MACRO              ";
    infoCommands[0x41] = "CHANNEL VOLUME ATTENUATION      ";
    infoCommands[0x42] = "TREMOLO                         ";
    infoCommands[0x43] = "TREMOLO SPEED                   ";
    infoCommands[0x44] = "VOLUME SLIDE DOWN               ";
    infoCommands[0x45] = "VOLUME SLIDE UP                 ";
    infoCommands[0x46] = "";
    infoCommands[0x47] = "";
    infoCommands[0x48] = "";
    infoCommands[0x49] = "";
    infoCommands[0x4A] = "";
    infoCommands[0x4B] = "";
    infoCommands[0x4C] = "";
    infoCommands[0x4D] = "";
    infoCommands[0x4E] = "";
    infoCommands[0x4F] = "";
    infoCommands[0x50] = "NOTE CUT                        ";
    infoCommands[0x51] = "NOTE RETRIGER                   ";
    infoCommands[0x52] = "JUMP TO MATRIX ROW              ";
    infoCommands[0x53] = "JUMP TO NEXT PATTERN            ";
    infoCommands[0x54] = "NOTE DELAY                      ";
    infoCommands[0x55] = "CSM BASE NOTE                   ";
    infoCommands[0x56] = "NOTE AUTO CUT BEFORE NEXT NOTE  ";
    infoCommands[0x57] = "";
    infoCommands[0x58] = "";
    infoCommands[0x59] = "";
    infoCommands[0x5A] = "";
    infoCommands[0x5B] = "";
    infoCommands[0x5C] = "";
    infoCommands[0x5D] = "";
    infoCommands[0x5E] = "";
    infoCommands[0x5F] = "";
    infoCommands[0x60] = "PATTERN SIZE (GLOBAL)           ";
    infoCommands[0x61] = "CH. PATTERN ROW SHIFT (LEFT)    ";
    infoCommands[0x62] = "CH. PATTERN ROW SHIFT (RIGHT)   ";
    infoCommands[0x63] = "";
    infoCommands[0x64] = "";
    infoCommands[0x65] = "";
    infoCommands[0x66] = "";
    infoCommands[0x67] = "";
    infoCommands[0x68] = "";
    infoCommands[0x69] = "";
    infoCommands[0x6A] = "";
    infoCommands[0x6B] = "";
    infoCommands[0x6C] = "";
    infoCommands[0x6D] = "";
    infoCommands[0x6E] = "";
    infoCommands[0x6F] = "";
    infoCommands[0x70] = "";
    infoCommands[0x71] = "";
    infoCommands[0x72] = "";
    infoCommands[0x73] = "";
    infoCommands[0x74] = "";
    infoCommands[0x75] = "";
    infoCommands[0x76] = "";
    infoCommands[0x77] = "";
    infoCommands[0x78] = "";
    infoCommands[0x79] = "";
    infoCommands[0x7A] = "";
    infoCommands[0x7B] = "";
    infoCommands[0x7C] = "";
    infoCommands[0x7D] = "";
    infoCommands[0x7E] = "";
    infoCommands[0x7F] = "";
    infoCommands[0x80] = "";
    infoCommands[0x81] = "";
    infoCommands[0x82] = "";
    infoCommands[0x83] = "";
    infoCommands[0x84] = "";
    infoCommands[0x85] = "";
    infoCommands[0x86] = "";
    infoCommands[0x87] = "";
    infoCommands[0x88] = "";
    infoCommands[0x89] = "";
    infoCommands[0x8A] = "";
    infoCommands[0x8B] = "";
    infoCommands[0x8C] = "";
    infoCommands[0x8D] = "";
    infoCommands[0x8E] = "";
    infoCommands[0x8F] = "";
    infoCommands[0x90] = "";
    infoCommands[0x91] = "";
    infoCommands[0x92] = "";
    infoCommands[0x93] = "";
    infoCommands[0x94] = "";
    infoCommands[0x95] = "";
    infoCommands[0x96] = "";
    infoCommands[0x97] = "";
    infoCommands[0x98] = "";
    infoCommands[0x99] = "";
    infoCommands[0x9A] = "";
    infoCommands[0x9B] = "";
    infoCommands[0x9C] = "";
    infoCommands[0x9D] = "";
    infoCommands[0x9E] = "";
    infoCommands[0x9F] = "";
    infoCommands[0xA0] = "";
    infoCommands[0xA1] = "FM ADSR ATTACK RATE [OP1]       ";
    infoCommands[0xA2] = "FM ADSR ATTACK RATE [OP2]       ";
    infoCommands[0xA3] = "FM ADSR ATTACK RATE [OP3]       ";
    infoCommands[0xA4] = "FM ADSR ATTACK RATE [OP4]       ";
    infoCommands[0xA5] = "";
    infoCommands[0xA6] = "";
    infoCommands[0xA7] = "";
    infoCommands[0xA8] = "";
    infoCommands[0xA9] = "";
    infoCommands[0xAA] = "";
    infoCommands[0xAB] = "";
    infoCommands[0xAC] = "";
    infoCommands[0xAD] = "";
    infoCommands[0xAE] = "";
    infoCommands[0xAF] = "";
    infoCommands[0xB0] = "";
    infoCommands[0xB1] = "FM ADSR DECAY RATE 1 [OP1]      ";
    infoCommands[0xB2] = "FM ADSR DECAY RATE 1 [OP2]      ";
    infoCommands[0xB3] = "FM ADSR DECAY RATE 1 [OP3]      ";
    infoCommands[0xB4] = "FM ADSR DECAY RATE 1 [OP4]      ";
    infoCommands[0xB5] = "";
    infoCommands[0xB6] = "";
    infoCommands[0xB7] = "";
    infoCommands[0xB8] = "";
    infoCommands[0xB9] = "";
    infoCommands[0xBA] = "";
    infoCommands[0xBB] = "";
    infoCommands[0xBC] = "";
    infoCommands[0xBD] = "";
    infoCommands[0xBE] = "";
    infoCommands[0xBF] = "";
    infoCommands[0xC0] = "";
    infoCommands[0xC1] = "FM ADSR SUSTAIN [OP1]           ";
    infoCommands[0xC2] = "FM ADSR SUSTAIN [OP2]           ";
    infoCommands[0xC3] = "FM ADSR SUSTAIN [OP3]           ";
    infoCommands[0xC4] = "FM ADSR SUSTAIN [OP4]           ";
    infoCommands[0xC5] = "";
    infoCommands[0xC6] = "";
    infoCommands[0xC7] = "";
    infoCommands[0xC8] = "";
    infoCommands[0xC9] = "";
    infoCommands[0xCA] = "";
    infoCommands[0xCB] = "";
    infoCommands[0xCC] = "";
    infoCommands[0xCD] = "";
    infoCommands[0xCE] = "";
    infoCommands[0xCF] = "";
    infoCommands[0xD0] = "";
    infoCommands[0xD1] = "FM ADSR DECAY RATE 2 [OP1]      ";
    infoCommands[0xD2] = "FM ADSR DECAY RATE 2 [OP2]      ";
    infoCommands[0xD3] = "FM ADSR DECAY RATE 2 [OP3]      ";
    infoCommands[0xD4] = "FM ADSR DECAY RATE 2 [OP4]      ";
    infoCommands[0xD5] = "";
    infoCommands[0xD6] = "";
    infoCommands[0xD7] = "";
    infoCommands[0xD8] = "";
    infoCommands[0xD9] = "";
    infoCommands[0xDA] = "";
    infoCommands[0xDB] = "";
    infoCommands[0xDC] = "";
    infoCommands[0xDD] = "";
    infoCommands[0xDE] = "";
    infoCommands[0xDF] = "";
    infoCommands[0xE0] = "";
    infoCommands[0xE1] = "FM ADSR RELEASE [OP1]           ";
    infoCommands[0xE2] = "FM ADSR RELEASE [OP2]           ";
    infoCommands[0xE3] = "FM ADSR RELEASE [OP3]           ";
    infoCommands[0xE4] = "FM ADSR RELEASE [OP4]           ";
    infoCommands[0xE5] = "";
    infoCommands[0xE6] = "";
    infoCommands[0xE7] = "";
    infoCommands[0xE8] = "";
    infoCommands[0xE9] = "";
    infoCommands[0xEA] = "";
    infoCommands[0xEB] = "";
    infoCommands[0xEC] = "";
    infoCommands[0xED] = "";
    infoCommands[0xEE] = "";
    infoCommands[0xEF] = "";
    infoCommands[0xF0] = "";
    infoCommands[0xF1] = "";
    infoCommands[0xF2] = "";
    infoCommands[0xF3] = "";
    infoCommands[0xF4] = "";
    infoCommands[0xF5] = "";
    infoCommands[0xF6] = "";
    infoCommands[0xF7] = "";
    infoCommands[0xF8] = "";
    infoCommands[0xF9] = "";
    infoCommands[0xFA] = "";
    infoCommands[0xFB] = "";
    infoCommands[0xFC] = "";
    infoCommands[0xFD] = "";
    infoCommands[0xFE] = "";
    infoCommands[0xFF] = "";
    //}

    //{ descriptions
    infoDescriptions[0x00] = "";
    infoDescriptions[0x01] = "00..7F; >7F=RESET                    ";
    infoDescriptions[0x02] = "00..7F; >7F=RESET                    ";
    infoDescriptions[0x03] = "00..7F; >7F=RESET                    ";
    infoDescriptions[0x04] = "00..7F; >7F=RESET                    ";
    infoDescriptions[0x05] = "X[OP] 1..4,5; Y 0..3; 0X,--=RESET    ";
    infoDescriptions[0x06] = "X[OP] 1..4,5; Y 0..F; 0X,--=RESET    ";
    infoDescriptions[0x07] = "X[OP] 1..4,5; Y 0..7; 0X,--=RESET    ";
    infoDescriptions[0x08] = "X[OP] 1..4,5; Y 0..1; 0X,--=RESET    ";
    infoDescriptions[0x09] = "X[OP] 1..4,5; Y 0..8; 0X,--=RESET    ";
    infoDescriptions[0x0A] = "00..07; >07=RESET                    ";
    infoDescriptions[0x0B] = "00..07; >07=RESET                    ";
    infoDescriptions[0x0C] = "00..07; >07=RESET                    ";
    infoDescriptions[0x0D] = "00..03; >03=RESET                    ";
    infoDescriptions[0x0E] = "11=C, 10=L, 01=R, FF=MUTE, --=DEFAULT";
    infoDescriptions[0x0F] = "";
    infoDescriptions[0x10] = "01..08, --=RESET                     ";
    infoDescriptions[0x11] = "01=DAC ON, --=DAC OFF                ";
    infoDescriptions[0x12] = "01=SPECIAL, 02=CSM, --=NORMAL        ";
    infoDescriptions[0x13] = "[H-INT SKIPS] 01..FF, --=RESET       ";
    infoDescriptions[0x14] = "X[ODD] 1..F; Y[EVEN] 1..F            ";
    infoDescriptions[0x15] = "X: 1=P, 2=W; Y: 0,1=TONAL 2,3,4=FIXED";
    infoDescriptions[0x16] = "00..03                               ";
    infoDescriptions[0x17] = "";
    infoDescriptions[0x18] = "";
    infoDescriptions[0x19] = "";
    infoDescriptions[0x1A] = "";
    infoDescriptions[0x1B] = "";
    infoDescriptions[0x1C] = "";
    infoDescriptions[0x1D] = "";
    infoDescriptions[0x1E] = "";
    infoDescriptions[0x1F] = "";
    infoDescriptions[0x20] = "01..63 TRACK NUMBER; --=PAUSE        ";
    infoDescriptions[0x21] = "01..63 TRACK NUMBER; --=PAUSE        ";
    infoDescriptions[0x22] = "01..FF=ENABLE; --=DISABLE            ";
    infoDescriptions[0x23] = "";
    infoDescriptions[0x24] = "";
    infoDescriptions[0x25] = "";
    infoDescriptions[0x26] = "";
    infoDescriptions[0x27] = "";
    infoDescriptions[0x28] = "";
    infoDescriptions[0x29] = "";
    infoDescriptions[0x2A] = "";
    infoDescriptions[0x2B] = "";
    infoDescriptions[0x2C] = "";
    infoDescriptions[0x2D] = "";
    infoDescriptions[0x2E] = "";
    infoDescriptions[0x2F] = "--=LOOP, 01=ONCE                     ";
    infoDescriptions[0x30] = "[XX] INSTRUMENT; --=DISABLE          ";
    infoDescriptions[0x31] = "01..7F, --=OFF, FF=RESET, FE=HOLD    ";
    infoDescriptions[0x32] = "01..7F, --=OFF, FF=RESET, FE=HOLD    ";
    infoDescriptions[0x33] = "X=SPEED, Y=DEPTH, --=OFF             ";
    infoDescriptions[0x34] = "01..1A, --=RESET [DEFAULT 8]         ";
    infoDescriptions[0x35] = "--=RESET [DEFAULT 2]                 ";
    infoDescriptions[0x36] = "01=UP, 02=DOWN, --=UP AND DOWN       ";
    infoDescriptions[0x37] = "[XX] PULSES TO SKIP; --=NOTHING      ";
    infoDescriptions[0x38] = "";
    infoDescriptions[0x39] = "";
    infoDescriptions[0x3A] = "";
    infoDescriptions[0x3B] = "";
    infoDescriptions[0x3C] = "";
    infoDescriptions[0x3D] = "";
    infoDescriptions[0x3E] = "";
    infoDescriptions[0x3F] = "--=LOOP, 01=ONCE                     ";
    infoDescriptions[0x40] = "--=DISABLE                           ";
    infoDescriptions[0x41] = "[FM] 01..7F; [PSG] / 8; --=DEFAULT   ";
    infoDescriptions[0x42] = "X=SPEED, Y=DEPTH, --=OFF             ";
    infoDescriptions[0x43] = "--=RESET [DEFAULT 20]                ";
    infoDescriptions[0x44] = "X=SPEED, Y=AMOUNT, --=STOP           ";
    infoDescriptions[0x45] = "X=SPEED, Y=AMOUNT, --=STOP           ";
    infoDescriptions[0x46] = "";
    infoDescriptions[0x47] = "";
    infoDescriptions[0x48] = "";
    infoDescriptions[0x49] = "";
    infoDescriptions[0x4A] = "";
    infoDescriptions[0x4B] = "";
    infoDescriptions[0x4C] = "";
    infoDescriptions[0x4D] = "";
    infoDescriptions[0x4E] = "";
    infoDescriptions[0x4F] = "";
    infoDescriptions[0x50] = "AFTER [XX] PULSES                    ";
    infoDescriptions[0x51] = "AFTER [XX] PULSES                    ";
    infoDescriptions[0x52] = "01..FA MATRIX ROW TO JUMP, --=NEXT   ";
    infoDescriptions[0x53] = "01..1F PATTERN ROW TO JUMP, --=FIRST ";
    infoDescriptions[0x54] = "BY [XX] PULSES, --=NOTHING           ";
    infoDescriptions[0x55] = "09..5F 'FILTER' EFFECT, --=NOTHING   ";
    infoDescriptions[0x56] = "01..0F TICKS<PPL, --=DISABLE         ";
    infoDescriptions[0x57] = "";
    infoDescriptions[0x58] = "";
    infoDescriptions[0x59] = "";
    infoDescriptions[0x5A] = "";
    infoDescriptions[0x5B] = "";
    infoDescriptions[0x5C] = "";
    infoDescriptions[0x5D] = "";
    infoDescriptions[0x5E] = "";
    infoDescriptions[0x5F] = "";
    infoDescriptions[0x60] = "01..1F, --=NOTHING                   ";
    infoDescriptions[0x61] = "X: ROW; Y: SHIFT;                    ";
    infoDescriptions[0x62] = "X: ROW; Y: SHIFT;                    ";
    infoDescriptions[0x63] = "";
    infoDescriptions[0x64] = "";
    infoDescriptions[0x65] = "";
    infoDescriptions[0x66] = "";
    infoDescriptions[0x67] = "";
    infoDescriptions[0x68] = "";
    infoDescriptions[0x69] = "";
    infoDescriptions[0x6A] = "";
    infoDescriptions[0x6B] = "";
    infoDescriptions[0x6C] = "";
    infoDescriptions[0x6D] = "";
    infoDescriptions[0x6E] = "";
    infoDescriptions[0x6F] = "";
    infoDescriptions[0x70] = "";
    infoDescriptions[0x71] = "";
    infoDescriptions[0x72] = "";
    infoDescriptions[0x73] = "";
    infoDescriptions[0x74] = "";
    infoDescriptions[0x75] = "";
    infoDescriptions[0x76] = "";
    infoDescriptions[0x77] = "";
    infoDescriptions[0x78] = "";
    infoDescriptions[0x79] = "";
    infoDescriptions[0x7A] = "";
    infoDescriptions[0x7B] = "";
    infoDescriptions[0x7C] = "";
    infoDescriptions[0x7D] = "";
    infoDescriptions[0x7E] = "";
    infoDescriptions[0x7F] = "";
    infoDescriptions[0x80] = "";
    infoDescriptions[0x81] = "";
    infoDescriptions[0x82] = "";
    infoDescriptions[0x83] = "";
    infoDescriptions[0x84] = "";
    infoDescriptions[0x85] = "";
    infoDescriptions[0x86] = "";
    infoDescriptions[0x87] = "";
    infoDescriptions[0x88] = "";
    infoDescriptions[0x89] = "";
    infoDescriptions[0x8A] = "";
    infoDescriptions[0x8B] = "";
    infoDescriptions[0x8C] = "";
    infoDescriptions[0x8D] = "";
    infoDescriptions[0x8E] = "";
    infoDescriptions[0x8F] = "";
    infoDescriptions[0x90] = "";
    infoDescriptions[0x91] = "";
    infoDescriptions[0x92] = "";
    infoDescriptions[0x93] = "";
    infoDescriptions[0x94] = "";
    infoDescriptions[0x95] = "";
    infoDescriptions[0x96] = "";
    infoDescriptions[0x97] = "";
    infoDescriptions[0x98] = "";
    infoDescriptions[0x99] = "";
    infoDescriptions[0x9A] = "";
    infoDescriptions[0x9B] = "";
    infoDescriptions[0x9C] = "";
    infoDescriptions[0x9D] = "";
    infoDescriptions[0x9E] = "";
    infoDescriptions[0x9F] = "";
    infoDescriptions[0xA0] = "";
    infoDescriptions[0xA1] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xA2] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xA3] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xA4] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xA5] = "";
    infoDescriptions[0xA6] = "";
    infoDescriptions[0xA7] = "";
    infoDescriptions[0xA8] = "";
    infoDescriptions[0xA9] = "";
    infoDescriptions[0xAA] = "";
    infoDescriptions[0xAB] = "";
    infoDescriptions[0xAC] = "";
    infoDescriptions[0xAD] = "";
    infoDescriptions[0xAE] = "";
    infoDescriptions[0xAF] = "";
    infoDescriptions[0xB0] = "";
    infoDescriptions[0xB1] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xB2] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xB3] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xB4] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xB5] = "";
    infoDescriptions[0xB6] = "";
    infoDescriptions[0xB7] = "";
    infoDescriptions[0xB8] = "";
    infoDescriptions[0xB9] = "";
    infoDescriptions[0xBA] = "";
    infoDescriptions[0xBB] = "";
    infoDescriptions[0xBC] = "";
    infoDescriptions[0xBD] = "";
    infoDescriptions[0xBE] = "";
    infoDescriptions[0xBF] = "";
    infoDescriptions[0xC0] = "";
    infoDescriptions[0xC1] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xC2] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xC3] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xC4] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xC5] = "";
    infoDescriptions[0xC6] = "";
    infoDescriptions[0xC7] = "";
    infoDescriptions[0xC8] = "";
    infoDescriptions[0xC9] = "";
    infoDescriptions[0xCA] = "";
    infoDescriptions[0xCB] = "";
    infoDescriptions[0xCC] = "";
    infoDescriptions[0xCD] = "";
    infoDescriptions[0xCE] = "";
    infoDescriptions[0xCF] = "";
    infoDescriptions[0xD0] = "";
    infoDescriptions[0xD1] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xD2] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xD3] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xD4] = "00..1F, >1F=RESET                    ";
    infoDescriptions[0xD5] = "";
    infoDescriptions[0xD6] = "";
    infoDescriptions[0xD7] = "";
    infoDescriptions[0xD8] = "";
    infoDescriptions[0xD9] = "";
    infoDescriptions[0xDA] = "";
    infoDescriptions[0xDB] = "";
    infoDescriptions[0xDC] = "";
    infoDescriptions[0xDD] = "";
    infoDescriptions[0xDE] = "";
    infoDescriptions[0xDF] = "";
    infoDescriptions[0xE0] = "";
    infoDescriptions[0xE1] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xE2] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xE3] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xE4] = "00..0F, >0F=RESET                    ";
    infoDescriptions[0xE5] = "";
    infoDescriptions[0xE6] = "";
    infoDescriptions[0xE7] = "";
    infoDescriptions[0xE8] = "";
    infoDescriptions[0xE9] = "";
    infoDescriptions[0xEA] = "";
    infoDescriptions[0xEB] = "";
    infoDescriptions[0xEC] = "";
    infoDescriptions[0xED] = "";
    infoDescriptions[0xEE] = "";
    infoDescriptions[0xEF] = "";
    infoDescriptions[0xF0] = "";
    infoDescriptions[0xF1] = "";
    infoDescriptions[0xF2] = "";
    infoDescriptions[0xF3] = "";
    infoDescriptions[0xF4] = "";
    infoDescriptions[0xF5] = "";
    infoDescriptions[0xF6] = "";
    infoDescriptions[0xF7] = "";
    infoDescriptions[0xF8] = "";
    infoDescriptions[0xF9] = "";
    infoDescriptions[0xFA] = "";
    infoDescriptions[0xFB] = "";
    infoDescriptions[0xFC] = "";
    infoDescriptions[0xFD] = "";
    infoDescriptions[0xFE] = "";
    infoDescriptions[0xFF] = "";
    //}
}