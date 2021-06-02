#include <genesis.h>
#include "MDT_Presets.h"

// based on https://github.com/rhargreaves/mega-drive-midi-interface

static const Preset_FM M_BANK_0_INST_0_GRANDPIANO = { 2, 0, 3, 0, 0,
1, 4, 26, 1, 7, 0, 7, 4, 1, 39, 0,
2, 7, 31, 3, 23, 0, 9, 15, 1, 4, 0,
4, 6, 24, 1, 9, 0, 6, 9, 7, 36, 0,
1, 3, 27, 2, 4, 0, 10, 4, 6, 2, 0 };

static const Preset_FM M_BANK_0_INST_1_BRIGHTPIANO = { 5, 7, 3, 0, 0,
4, 2, 27, 1, 9, 0, 11, 5, 6, 33, 0,
1, 2, 27, 1, 5, 1, 10, 5, 6, 8, 0,
4, 5, 27, 1, 9, 0, 7, 9, 7, 18, 0,
6, 5, 27, 1, 9, 0, 3, 8, 7, 9, 0 };

static const Preset_FM M_BANK_0_INST_2_ELECTRICPIANO_ANIMATICS = { 4, 0, 3, 0, 0,
1, 7, 24, 1, 5, 0, 6, 6, 4, 33, 0,
1, 4, 25, 2, 5, 0, 1, 4, 5, 6, 0,
2, 4, 22, 1, 7, 0, 6, 6, 4, 26, 0,
1, 3, 25, 2, 4, 1, 6, 4, 5, 9, 0 };

static const Preset_FM M_BANK_0_INST_3_HONKYTONK_SONICMODDED = { 5, 1, 3, 0, 0,
2, 3, 31, 0, 10, 0, 15, 5, 7, 35, 0,
4, 4, 25, 0, 7, 0, 13, 0, 7, 7, 0,
2, 4, 25, 0, 6, 0, 2, 6, 7, 14, 0,
1, 3, 25, 0, 9, 0, 2, 0, 7, 16, 0 };

static const Preset_FM M_BANK_0_INST_4_RHODESPIANO = { 4, 6, 3, 0, 0,
12, 4, 31, 1, 7, 0, 11, 0, 8, 57, 0,
1, 4, 31, 2, 4, 1, 1, 4, 8, 9, 0,
1, 4, 22, 2, 5, 0, 11, 0, 8, 34, 0,
1, 4, 31, 2, 4, 1, 6, 4, 8, 9, 0 };

static const Preset_FM M_BANK_0_INST_5_CHORUSPIANOTINITOON = { 7, 6, 3, 0, 0,
10, 7, 31, 0, 10, 0, 15, 7, 6, 30, 0,
2, 3, 31, 0, 13, 0, 15, 0, 8, 12, 0,
1, 5, 25, 1, 5, 0, 15, 2, 6, 10, 0,
1, 1, 25, 0, 10, 0, 15, 2, 6, 20, 0 };

static const Preset_FM M_BANK_0_INST_6_HARPSICHORD_ADAMS = { 1, 7, 3, 0, 0,
6, 1, 31, 2, 0, 1, 0, 1, 5, 38, 0,
10, 3, 31, 3, 0, 0, 0, 1, 6, 33, 0,
0, 5, 31, 3, 0, 1, 0, 0, 2, 35, 0,
0, 7, 31, 0, 6, 0, 4, 6, 7, 6, 0 };

static const Preset_FM M_BANK_0_INST_7_CLAVINET = { 1, 7, 3, 0, 0,
1, 1, 31, 2, 0, 1, 0, 1, 6, 28, 0,
1, 3, 31, 2, 0, 0, 0, 1, 7, 33, 0,
1, 5, 31, 3, 0, 1, 0, 0, 2, 30, 0,
1, 7, 31, 0, 6, 0, 4, 6, 7, 6, 0 };

static const Preset_FM M_BANK_0_INST_8_CELESTA = { 0, 0, 3, 3, 0,
3, 1, 19, 1, 1, 0, 4, 27, 0, 8, 0,
1, 7, 27, 1, 7, 0, 0, 26, 6, 17, 0,
11, 3, 29, 0, 15, 0, 15, 31, 8, 34, 0,
1, 7, 31, 0, 0, 0, 0, 9, 6, 11, 0 };

static const Preset_FM M_BANK_0_INST_9_GLOCKENSPIEL = { 4, 1, 3, 3, 0,
7, 1, 31, 0, 7, 0, 15, 0, 2, 30, 0,
1, 7, 31, 2, 7, 0, 15, 0, 3, 0, 0,
5, 3, 31, 0, 11, 0, 15, 0, 6, 51, 0,
1, 7, 31, 2, 7, 0, 15, 0, 3, 15, 0 };

static const Preset_FM M_BANK_0_INST_10_MUSICBOX = { 4, 1, 3, 3, 0,
7, 1, 15, 0, 14, 0, 15, 0, 7, 30, 0,
1, 7, 31, 0, 12, 0, 15, 0, 6, 0, 0,
10, 3, 15, 0, 14, 0, 15, 0, 5, 51, 0,
0, 7, 31, 0, 12, 0, 15, 0, 6, 15, 0 };

static const Preset_FM M_BANK_0_INST_11_VIBRAPHONE_NINEKO = { 6, 7, 3, 2, 0,
8, 3, 25, 1, 15, 0, 10, 6, 6, 40, 0,
1, 4, 25, 1, 6, 1, 10, 5, 6, 4, 0,
10, 7, 31, 1, 16, 0, 6, 6, 6, 35, 0,
4, 3, 28, 2, 10, 0, 6, 5, 5, 4, 0 };

static const Preset_FM M_BANK_0_INST_12_MARIMBA_ALADDIN = { 4, 0, 3, 0, 0,
5, 4, 31, 0, 19, 1, 7, 7, 14, 27, 0,
1, 3, 30, 0, 15, 0, 15, 7, 7, 0, 0,
2, 2, 31, 2, 18, 1, 8, 4, 2, 40, 0,
1, 4, 31, 0, 15, 0, 15, 2, 7, 11, 0 };

static const Preset_FM M_BANK_0_INST_13_XYLAPHONE_ALADDIN = { 0, 0, 3, 0, 0,
0, 7, 0, 1, 29, 1, 9, 31, 15, 0, 0,
0, 7, 31, 1, 0, 1, 0, 24, 15, 0, 0,
3, 6, 29, 1, 15, 0, 15, 31, 15, 31, 0,
0, 7, 31, 0, 0, 0, 0, 12, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_14_TUBULARBELLS_ANIMATICS = { 4, 5, 3, 0, 0,
7, 7, 22, 0, 5, 0, 15, 30, 4, 30, 0,
2, 3, 24, 0, 3, 0, 12, 31, 4, 8, 0,
7, 3, 22, 0, 6, 0, 8, 30, 4, 30, 0,
2, 7, 19, 0, 2, 0, 9, 31, 4, 8, 0 };

static const Preset_FM M_BANK_0_INST_15_DULCIMER = { 6, 2, 3, 0, 0,
3, 4, 15, 1, 11, 0, 15, 6, 5, 23, 0,
1, 4, 24, 2, 8, 0, 15, 0, 5, 3, 0,
10, 4, 19, 1, 14, 0, 4, 6, 6, 44, 0,
1, 4, 22, 2, 8, 0, 15, 0, 3, 9, 0 };

static const Preset_FM M_BANK_0_INST_16_HAMMOND_NINEKO = { 7, 4, 3, 0, 0,
4, 1, 28, 1, 0, 0, 15, 4, 10, 17, 0,
0, 3, 28, 1, 0, 0, 15, 27, 8, 23, 0,
1, 5, 28, 1, 0, 0, 15, 4, 8, 15, 0,
2, 6, 28, 1, 0, 0, 15, 4, 10, 23, 0 };

static const Preset_FM M_BANK_0_INST_17_PERCUSIVEORGAIN_ACTION52 = { 4, 0, 3, 3, 2,
14, 4, 27, 0, 23, 0, 11, 11, 8, 21, 0,
6, 4, 20, 0, 9, 0, 2, 0, 8, 7, 0,
1, 1, 21, 0, 28, 0, 2, 5, 15, 17, 0,
1, 4, 23, 0, 1, 0, 10, 31, 15, 11, 0 };

static const Preset_FM M_BANK_0_INST_18_ROCKORGAN = { 4, 4, 3, 1, 4,
3, 1, 20, 1, 12, 1, 2, 0, 10, 25, 0,
1, 4, 20, 2, 0, 0, 0, 0, 10, 7, 0,
1, 4, 20, 2, 0, 1, 0, 0, 10, 24, 0,
2, 5, 20, 2, 0, 0, 0, 0, 10, 10, 0 };

static const Preset_FM M_BANK_0_INST_19_CHURCHORGAN = { 6, 2, 3, 0, 0,
3, 1, 20, 2, 10, 0, 2, 0, 3, 27, 0,
1, 4, 14, 0, 10, 0, 1, 0, 6, 9, 0,
4, 4, 19, 0, 10, 0, 2, 0, 9, 6, 0,
0, 4, 12, 0, 10, 0, 1, 0, 6, 4, 0 };

static const Preset_FM M_BANK_0_INST_20_REEDORGAN = { 4, 6, 3, 1, 0,
1, 1, 24, 2, 16, 0, 1, 0, 7, 25, 0,
1, 4, 16, 0, 0, 0, 0, 0, 8, 7, 0,
1, 4, 25, 0, 0, 0, 0, 0, 6, 21, 0,
2, 4, 13, 2, 0, 1, 0, 0, 8, 7, 0 };

static const Preset_FM M_BANK_0_INST_21_ACCORDEON = { 2, 6, 3, 0, 2,
1, 3, 21, 0, 5, 0, 1, 0, 7, 30, 0,
7, 3, 18, 0, 8, 0, 2, 0, 8, 21, 0,
2, 7, 14, 0, 9, 0, 2, 3, 9, 52, 0,
2, 4, 13, 2, 22, 0, 1, 0, 9, 0, 0 };

static const Preset_FM M_BANK_0_INST_22_HARMONICA_SONIC = { 0, 7, 3, 0, 0,
10, 3, 20, 3, 5, 0, 9, 0, 9, 45, 0,
10, 4, 20, 0, 8, 0, 0, 0, 9, 40, 0,
1, 1, 16, 1, 2, 0, 0, 0, 9, 39, 0,
2, 4, 16, 0, 8, 1, 1, 0, 10, 0, 0 };

static const Preset_FM M_BANK_0_INST_23_TANGO_ACCORDION = { 4, 6, 3, 1, 0,
3, 1, 15, 0, 4, 0, 1, 0, 10, 25, 0,
1, 4, 13, 2, 4, 0, 2, 0, 10, 7, 0,
3, 4, 14, 0, 4, 0, 1, 0, 10, 26, 0,
2, 5, 14, 2, 4, 0, 2, 0, 10, 8, 0 };

static const Preset_FM M_BANK_0_INST_24_NYLONGUITARHELLROLL = { 0, 3, 3, 0, 0,
5, 4, 31, 0, 18, 0, 2, 0, 7, 48, 0,
0, 3, 31, 0, 10, 0, 2, 4, 7, 19, 0,
1, 3, 31, 0, 14, 0, 2, 4, 7, 45, 0,
0, 4, 31, 1, 10, 0, 2, 3, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_25_STEELGUITARHELLROLL = { 0, 3, 3, 0, 0,
5, 4, 31, 0, 18, 0, 2, 0, 7, 32, 0,
0, 3, 31, 0, 10, 0, 2, 4, 7, 19, 0,
1, 3, 31, 0, 14, 0, 2, 4, 7, 45, 0,
0, 4, 31, 1, 10, 0, 2, 3, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_26_JAZZGUITAR = { 0, 5, 3, 0, 0,
2, 4, 31, 1, 4, 0, 15, 0, 7, 30, 0,
3, 3, 31, 1, 4, 0, 15, 0, 7, 43, 0,
1, 3, 31, 2, 4, 0, 15, 0, 7, 42, 0,
1, 4, 31, 0, 4, 0, 15, 0, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_27_CLEANGUITAR_COLUMNS_III = { 0, 7, 3, 0, 0,
8, 5, 31, 1, 7, 0, 15, 0, 6, 39, 0,
3, 3, 31, 1, 10, 0, 15, 0, 9, 29, 0,
3, 5, 31, 1, 4, 0, 15, 0, 8, 34, 0,
1, 3, 31, 1, 31, 0, 1, 4, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_28_MUTEDGUITAR_ALADDIN = { 2, 3, 3, 0, 4,
3, 3, 20, 1, 23, 0, 8, 9, 10, 28, 0,
4, 4, 24, 1, 23, 0, 11, 1, 11, 11, 0,
1, 3, 27, 1, 27, 0, 8, 7, 11, 5, 0,
1, 4, 20, 0, 18, 0, 0, 4, 12, 0, 0 };

static const Preset_FM M_BANK_0_INST_29_OVERDRIVE_OPL3 = { 0, 0, 3, 0, 4,
3, 4, 31, 2, 12, 0, 8, 0, 12, 36, 0,
2, 4, 18, 2, 1, 0, 13, 0, 10, 25, 0,
1, 2, 19, 1, 1, 0, 11, 0, 8, 32, 0,
2, 4, 17, 2, 1, 0, 4, 0, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_30_DISTORTION_OPL3 = { 0, 0, 3, 0, 4,
3, 4, 31, 2, 12, 0, 8, 0, 12, 36, 0,
2, 4, 18, 2, 1, 0, 15, 0, 7, 25, 0,
1, 2, 19, 1, 2, 0, 11, 0, 8, 26, 0,
2, 4, 17, 2, 1, 0, 4, 0, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_31_GUITAR_HARMONICSHELLFIRE = { 3, 6, 3, 0, 5,
10, 4, 16, 2, 6, 0, 15, 8, 12, 28, 0,
8, 7, 21, 2, 6, 0, 8, 7, 1, 13, 0,
2, 3, 31, 2, 3, 0, 15, 0, 6, 20, 0,
1, 1, 16, 0, 2, 0, 15, 15, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_32_ACOUSTICBASS_SONIC = { 2, 7, 3, 0, 0,
0, 2, 30, 0, 10, 0, 10, 5, 4, 33, 0,
3, 2, 31, 0, 10, 0, 8, 7, 5, 37, 0,
0, 6, 31, 0, 8, 0, 9, 10, 6, 40, 0,
1, 4, 31, 0, 7, 0, 7, 7, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_33_FINGERBASS_SONIC = { 0, 1, 3, 0, 0,
9, 4, 31, 0, 18, 0, 2, 0, 15, 37, 0,
0, 7, 31, 0, 14, 0, 2, 4, 15, 48, 0,
0, 3, 31, 1, 10, 0, 2, 4, 15, 19, 0,
0, 4, 31, 1, 10, 0, 2, 3, 15, 0, 0 };

static const Preset_FM M_BANK_0_INST_34_PICKBASS = { 0, 1, 3, 0, 0,
9, 4, 31, 0, 21, 0, 8, 0, 15, 30, 0,
0, 7, 31, 0, 14, 0, 3, 4, 15, 49, 0,
0, 3, 31, 1, 13, 0, 2, 4, 15, 14, 0,
0, 4, 31, 1, 10, 0, 2, 3, 15, 0, 0 };

static const Preset_FM M_BANK_0_INST_35_FRETLESSBASSALADDIN = { 6, 0, 3, 0, 4,
0, 4, 26, 0, 9, 0, 0, 7, 11, 29, 0,
0, 4, 13, 1, 3, 0, 6, 0, 10, 5, 0,
0, 5, 18, 2, 11, 0, 14, 19, 10, 0, 0,
0, 6, 10, 1, 12, 0, 15, 0, 10, 9, 0 };

static const Preset_FM M_BANK_0_INST_36_SLAPBASS1 = { 0, 4, 3, 0, 0,
15, 3, 31, 2, 14, 0, 2, 8, 7, 19, 0,
3, 4, 31, 2, 15, 0, 2, 8, 7, 56, 0,
0, 4, 31, 1, 13, 0, 2, 8, 7, 24, 0,
1, 4, 31, 1, 7, 0, 1, 8, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_37_SLAPBASS2 = { 0, 4, 3, 0, 0,
15, 3, 31, 2, 14, 0, 2, 6, 7, 14, 0,
3, 4, 31, 2, 15, 0, 2, 8, 7, 56, 0,
0, 4, 31, 1, 13, 0, 2, 8, 7, 24, 0,
1, 4, 31, 1, 7, 0, 1, 8, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_38_SYNBASS1_DYNABROS = { 5, 7, 3, 0, 0,
4, 4, 21, 1, 30, 0, 0, 13, 11, 23, 0,
4, 1, 25, 1, 30, 0, 0, 10, 10, 0, 0,
2, 4, 20, 1, 30, 0, 0, 9, 9, 5, 0,
3, 3, 20, 1, 30, 0, 8, 5, 9, 24, 0 };

static const Preset_FM M_BANK_0_INST_39_SYNBASS2_NINEKO = { 0, 4, 3, 0, 0,
6, 3, 31, 3, 7, 0, 2, 7, 0, 25, 0,
5, 3, 31, 3, 6, 0, 1, 6, 6, 55, 0,
0, 3, 31, 2, 9, 0, 1, 6, 7, 19, 0,
1, 3, 31, 2, 6, 0, 15, 8, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_40_VIOLIN = { 2, 7, 3, 2, 3,
1, 3, 15, 0, 4, 0, 1, 0, 5, 30, 0,
7, 3, 13, 0, 16, 0, 0, 0, 4, 31, 0,
0, 7, 12, 0, 9, 0, 0, 3, 7, 49, 0,
2, 4, 11, 2, 10, 0, 1, 0, 5, 3, 0 };

static const Preset_FM M_BANK_0_INST_41_VIOLA = { 2, 7, 3, 2, 3,
1, 3, 14, 0, 4, 0, 1, 0, 5, 30, 0,
4, 3, 14, 0, 16, 0, 0, 0, 4, 30, 0,
0, 7, 11, 0, 9, 0, 0, 3, 7, 61, 0,
2, 4, 12, 2, 10, 0, 1, 0, 5, 3, 0 };

static const Preset_FM M_BANK_0_INST_42_CELLO = { 2, 7, 3, 2, 3,
1, 3, 16, 0, 4, 0, 1, 0, 5, 40, 0,
5, 3, 14, 1, 16, 0, 0, 0, 4, 30, 0,
0, 7, 14, 0, 9, 0, 0, 3, 8, 47, 0,
2, 4, 14, 2, 10, 0, 1, 0, 6, 3, 0 };

static const Preset_FM M_BANK_0_INST_43_CONTROLBASS = { 2, 7, 3, 2, 3,
1, 3, 16, 0, 4, 0, 1, 0, 5, 43, 0,
4, 3, 14, 0, 16, 0, 0, 0, 4, 44, 0,
0, 7, 14, 0, 9, 0, 0, 3, 7, 31, 0,
2, 4, 14, 2, 10, 0, 1, 0, 5, 0, 0 };

static const Preset_FM M_BANK_0_INST_44_TREMOLOSTRINGS = { 4, 7, 3, 2, 5,
1, 3, 16, 3, 1, 1, 1, 0, 3, 21, 0,
1, 4, 15, 0, 10, 0, 2, 0, 7, 6, 0,
1, 4, 13, 2, 11, 0, 4, 0, 4, 19, 0,
1, 4, 14, 0, 10, 0, 1, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_45_PIZZIKATOSTRINGS = { 5, 6, 3, 0, 0,
4, 1, 19, 3, 19, 0, 10, 11, 7, 0, 0,
1, 1, 16, 0, 19, 0, 15, 23, 7, 8, 0,
2, 1, 19, 1, 15, 0, 15, 24, 7, 5, 0,
1, 1, 18, 1, 14, 0, 12, 14, 7, 12, 0 };

static const Preset_FM M_BANK_0_INST_46_ORCHESTRALHARP = { 4, 0, 3, 0, 0,
2, 4, 31, 0, 10, 0, 1, 0, 4, 27, 0,
1, 1, 31, 0, 14, 0, 6, 5, 4, 0, 0,
1, 5, 31, 1, 10, 0, 7, 2, 4, 127, 0,
0, 5, 14, 1, 11, 0, 15, 2, 4, 127, 0 };

static const Preset_FM M_BANK_0_INST_47_TIMPANY = { 4, 1, 3, 0, 0,
0, 4, 20, 1, 15, 0, 15, 0, 7, 10, 0,
1, 4, 31, 0, 11, 0, 15, 0, 6, 0, 0,
1, 4, 31, 0, 14, 0, 15, 0, 10, 0, 0,
2, 4, 15, 0, 13, 0, 15, 0, 14, 71, 0 };

static const Preset_FM M_BANK_0_INST_48_STRINGESSEMBLE1 = { 4, 7, 3, 1, 3,
1, 3, 14, 3, 11, 0, 0, 0, 4, 29, 0,
5, 4, 14, 0, 10, 1, 2, 0, 8, 6, 0,
1, 4, 14, 2, 10, 1, 1, 0, 5, 28, 0,
1, 4, 15, 0, 10, 0, 1, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_49_STRINGESSEMBLE2 = { 4, 7, 3, 1, 1,
1, 3, 11, 3, 11, 0, 0, 0, 1, 29, 0,
1, 4, 10, 0, 8, 0, 2, 0, 6, 6, 0,
1, 4, 11, 2, 10, 1, 1, 0, 1, 28, 0,
1, 4, 10, 0, 8, 0, 1, 0, 6, 3, 0 };

static const Preset_FM M_BANK_0_INST_50_SYNSTRINGS_1 = { 4, 6, 3, 0, 0,
1, 1, 26, 0, 2, 1, 6, 0, 2, 21, 0,
1, 4, 10, 0, 0, 0, 0, 0, 6, 24, 0,
1, 3, 25, 0, 1, 1, 11, 0, 3, 28, 0,
1, 4, 10, 0, 0, 0, 0, 0, 5, 2, 0 };

static const Preset_FM M_BANK_0_INST_51_SYNSTRING2_ALADDIN = { 2, 6, 3, 1, 5,
0, 3, 8, 0, 4, 0, 0, 0, 5, 35, 0,
4, 3, 8, 2, 3, 0, 10, 0, 4, 52, 0,
0, 4, 15, 0, 1, 0, 7, 3, 3, 32, 0,
0, 4, 8, 1, 10, 1, 0, 0, 5, 0, 0 };

static const Preset_FM M_BANK_0_INST_52_CHOIR_AAHS = { 4, 0, 3, 0, 4,
0, 3, 17, 1, 0, 1, 5, 0, 1, 42, 0,
0, 3, 13, 1, 0, 1, 0, 0, 5, 8, 0,
0, 7, 13, 2, 0, 0, 5, 0, 1, 30, 0,
0, 7, 11, 2, 0, 1, 0, 0, 5, 8, 0 };

static const Preset_FM M_BANK_0_INST_53_VOICE = { 4, 6, 3, 0, 0,
5, 7, 20, 0, 10, 0, 1, 3, 7, 0, 0,
3, 4, 21, 1, 10, 0, 6, 2, 7, 37, 0,
1, 3, 19, 2, 13, 0, 2, 5, 8, 14, 0,
1, 4, 13, 1, 4, 1, 1, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_54_SYNTHVOICE = { 0, 4, 3, 0, 4,
10, 3, 12, 1, 0, 1, 5, 0, 1, 76, 0,
7, 3, 14, 1, 0, 1, 0, 0, 5, 51, 0,
0, 7, 12, 2, 0, 0, 5, 0, 1, 46, 0,
0, 7, 11, 2, 0, 1, 0, 0, 5, 0, 0 };

static const Preset_FM M_BANK_0_INST_55_ORCHESTRAHIT = { 0, 7, 3, 0, 0,
0, 4, 16, 1, 13, 0, 15, 0, 5, 19, 0,
1, 4, 22, 0, 9, 0, 15, 0, 3, 17, 0,
1, 4, 18, 0, 5, 0, 15, 0, 2, 21, 0,
2, 4, 18, 1, 10, 0, 15, 0, 5, 0, 0 };

static const Preset_FM M_BANK_0_INST_56_TRUMPET_SONIC = { 5, 7, 3, 0, 0,
2, 4, 16, 2, 14, 0, 1, 1, 4, 27, 0,
4, 4, 16, 0, 11, 0, 15, 0, 13, 10, 0,
2, 4, 19, 0, 14, 0, 1, 0, 9, 21, 0,
2, 4, 20, 0, 13, 0, 0, 0, 14, 9, 0 };

static const Preset_FM M_BANK_0_INST_57_TROMBONE_ALADDIN = { 5, 5, 3, 2, 3,
0, 4, 12, 2, 13, 0, 2, 1, 10, 26, 0,
0, 4, 23, 2, 7, 1, 1, 2, 10, 9, 0,
0, 4, 10, 2, 8, 0, 2, 0, 10, 18, 0,
1, 5, 23, 2, 7, 1, 1, 0, 10, 3, 0 };

static const Preset_FM M_BANK_0_INST_58_TUBA_ALADDIN = { 5, 7, 3, 1, 2,
0, 4, 14, 2, 6, 0, 6, 1, 6, 27, 0,
0, 4, 14, 2, 7, 1, 1, 2, 5, 4, 0,
1, 4, 13, 2, 13, 0, 2, 10, 5, 3, 0,
1, 5, 17, 2, 7, 1, 1, 0, 6, 5, 0 };

static const Preset_FM M_BANK_0_INST_59_MUTEDTRUMPET_SONIC = { 5, 7, 3, 0, 0,
2, 1, 15, 2, 14, 0, 1, 1, 3, 21, 0,
2, 4, 13, 0, 11, 0, 15, 0, 13, 31, 0,
2, 4, 15, 0, 14, 0, 1, 0, 8, 74, 0,
2, 4, 15, 0, 13, 0, 0, 0, 14, 0, 0 };

static const Preset_FM M_BANK_0_INST_60_FRENCHHORN = { 4, 7, 3, 0, 0,
1, 4, 11, 2, 4, 0, 3, 0, 7, 34, 0,
1, 4, 20, 2, 0, 0, 0, 0, 6, 0, 0,
0, 4, 0, 0, 0, 0, 0, 0, 0, 127, 0,
0, 4, 0, 0, 0, 0, 0, 0, 0, 127, 0 };

static const Preset_FM M_BANK_0_INST_61_BRASSSECTION_GREENDOG = { 5, 6, 3, 1, 2,
1, 2, 14, 2, 0, 0, 4, 1, 7, 26, 0,
1, 3, 27, 2, 5, 0, 3, 2, 6, 0, 0,
0, 2, 21, 2, 0, 0, 1, 2, 7, 9, 0,
4, 1, 20, 2, 0, 1, 0, 2, 8, 6, 0 };

static const Preset_FM M_BANK_0_INST_62_SYNBRASS1 = { 2, 6, 3, 0, 2,
1, 4, 22, 0, 6, 0, 15, 0, 6, 20, 0,
1, 3, 31, 0, 0, 0, 0, 0, 13, 28, 0,
1, 7, 31, 0, 0, 0, 0, 0, 12, 42, 0,
1, 4, 19, 0, 0, 0, 0, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_63_SYNBRASS2 = { 2, 7, 3, 2, 3,
1, 3, 17, 0, 4, 0, 1, 0, 9, 30, 0,
7, 3, 17, 0, 16, 0, 0, 0, 9, 31, 0,
2, 7, 17, 0, 9, 0, 0, 3, 9, 55, 0,
2, 4, 17, 2, 10, 0, 1, 0, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_64_SOPRANOSAX = { 5, 6, 3, 1, 2,
1, 2, 17, 1, 0, 0, 4, 1, 8, 32, 0,
2, 3, 12, 1, 5, 0, 3, 2, 7, 13, 0,
0, 2, 13, 1, 0, 0, 1, 2, 8, 14, 0,
2, 1, 14, 2, 0, 1, 0, 2, 9, 0, 0 };

static const Preset_FM M_BANK_0_INST_65_ALTOSAX = { 5, 6, 3, 1, 2,
1, 2, 14, 2, 0, 0, 4, 1, 8, 26, 0,
1, 3, 12, 2, 5, 0, 3, 2, 7, 5, 0,
0, 2, 19, 1, 0, 0, 1, 2, 8, 11, 0,
4, 1, 14, 2, 0, 1, 0, 2, 8, 4, 0 };

static const Preset_FM M_BANK_0_INST_66_TENORSAX = { 5, 6, 3, 1, 2,
1, 2, 15, 2, 0, 0, 4, 1, 8, 30, 0,
1, 3, 11, 2, 5, 0, 3, 2, 6, 5, 0,
0, 2, 18, 1, 0, 0, 1, 2, 7, 10, 0,
2, 1, 13, 2, 0, 1, 0, 2, 8, 4, 0 };

static const Preset_FM M_BANK_0_INST_67_BARITONESAX = { 5, 6, 3, 1, 2,
1, 2, 14, 2, 0, 0, 4, 1, 8, 23, 0,
1, 3, 14, 2, 5, 0, 3, 2, 6, 5, 0,
0, 2, 17, 2, 0, 0, 1, 2, 7, 10, 0,
3, 1, 17, 2, 0, 1, 0, 2, 8, 4, 0 };

static const Preset_FM M_BANK_0_INST_68_OBOE_ALADDIN = { 0, 7, 3, 1, 2,
1, 4, 24, 0, 23, 1, 0, 0, 10, 39, 0,
1, 4, 16, 0, 31, 1, 0, 0, 10, 30, 0,
1, 4, 22, 0, 31, 0, 0, 0, 10, 40, 0,
4, 4, 24, 0, 31, 0, 0, 0, 10, 0, 0 };

static const Preset_FM M_BANK_0_INST_69_ENGLISHHORN = { 0, 7, 3, 1, 2,
1, 4, 24, 0, 23, 1, 0, 0, 10, 38, 0,
1, 4, 16, 0, 31, 1, 0, 0, 10, 23, 0,
1, 4, 22, 0, 31, 0, 0, 0, 10, 43, 0,
2, 4, 16, 0, 31, 0, 0, 0, 8, 5, 0 };

static const Preset_FM M_BANK_0_INST_70_BASSOON_ALADDIN = { 0, 7, 3, 1, 2,
0, 4, 3, 0, 23, 1, 0, 0, 12, 39, 0,
1, 4, 15, 0, 31, 1, 0, 0, 12, 35, 0,
0, 4, 31, 0, 31, 0, 0, 0, 12, 36, 0,
2, 4, 16, 0, 31, 0, 0, 0, 12, 0, 0 };

static const Preset_FM M_BANK_0_INST_71_CLARINET_ALADDIN = { 4, 1, 3, 2, 3,
2, 1, 13, 2, 0, 0, 0, 1, 10, 30, 0,
1, 7, 18, 0, 0, 0, 0, 0, 10, 4, 0,
2, 7, 15, 1, 11, 1, 8, 29, 10, 18, 0,
1, 1, 19, 0, 15, 1, 2, 6, 10, 12, 0 };

static const Preset_FM M_BANK_0_INST_72_PICCOLO_ALADDIN = { 7, 2, 3, 3, 2,
2, 3, 10, 3, 0, 0, 0, 0, 8, 9, 0,
2, 7, 10, 2, 8, 0, 7, 0, 8, 6, 0,
2, 3, 12, 2, 12, 1, 12, 10, 8, 17, 0,
2, 1, 10, 3, 13, 1, 7, 0, 8, 11, 0 };

static const Preset_FM M_BANK_0_INST_73_FLUTE = { 4, 6, 3, 1, 0,
5, 7, 20, 0, 10, 0, 1, 3, 5, 0, 0,
3, 4, 22, 1, 12, 0, 6, 2, 8, 49, 0,
1, 3, 19, 2, 13, 0, 4, 0, 5, 22, 0,
1, 4, 14, 1, 4, 1, 1, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_74_RECORDER = { 3, 7, 3, 0, 0,
1, 7, 20, 1, 14, 0, 6, 3, 5, 8, 0,
2, 4, 20, 1, 16, 0, 5, 2, 5, 55, 0,
2, 3, 13, 2, 4, 0, 2, 0, 5, 36, 0,
2, 4, 14, 1, 4, 1, 0, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_75_PANFLUTE = { 4, 6, 3, 0, 0,
5, 7, 19, 0, 10, 0, 1, 0, 5, 0, 0,
3, 4, 22, 1, 12, 0, 1, 0, 5, 49, 0,
2, 3, 18, 2, 13, 0, 4, 0, 5, 19, 0,
1, 4, 14, 1, 4, 1, 1, 0, 8, 0, 0 };

static const Preset_FM M_BANK_0_INST_76_BOTTLEBLOW = { 4, 7, 3, 0, 0,
5, 7, 18, 0, 10, 0, 1, 0, 6, 0, 0,
3, 4, 25, 1, 13, 0, 3, 0, 6, 41, 0,
2, 3, 12, 2, 13, 1, 5, 0, 6, 24, 0,
1, 4, 12, 1, 4, 1, 2, 0, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_77_SHAKUHACHI_ALISIA_DRAGOON = { 4, 5, 3, 0, 0,
4, 3, 18, 0, 6, 0, 2, 2, 7, 32, 0,
2, 7, 15, 0, 9, 0, 7, 0, 8, 6, 0,
4, 5, 18, 0, 6, 0, 2, 2, 6, 35, 0,
2, 3, 14, 0, 10, 1, 2, 0, 9, 11, 0 };

static const Preset_FM M_BANK_0_INST_78_WHISTLE_ALADDIN = { 7, 2, 3, 1, 3,
1, 3, 18, 0, 0, 1, 0, 0, 8, 8, 0,
1, 4, 15, 2, 2, 0, 7, 0, 8, 8, 0,
1, 3, 0, 2, 0, 0, 4, 0, 8, 127, 0,
2, 1, 0, 3, 0, 0, 0, 0, 8, 127, 0 };

static const Preset_FM M_BANK_0_INST_79_OCARINA = { 4, 6, 3, 0, 0,
5, 7, 20, 0, 10, 0, 1, 3, 5, 0, 0,
3, 4, 22, 1, 12, 0, 6, 2, 8, 49, 0,
2, 3, 20, 2, 13, 0, 4, 0, 5, 41, 0,
1, 4, 14, 1, 4, 1, 1, 0, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_80_LEADSQUARE = { 4, 7, 3, 1, 0,
2, 3, 31, 1, 0, 1, 8, 1, 11, 24, 0,
1, 7, 28, 0, 0, 0, 1, 0, 10, 9, 0,
2, 4, 26, 0, 0, 1, 2, 0, 10, 26, 0,
1, 4, 27, 1, 0, 1, 0, 0, 15, 13, 0 };

static const Preset_FM M_BANK_0_INST_81_LEADSAW = { 4, 7, 3, 0, 0,
1, 7, 31, 0, 0, 0, 0, 0, 15, 28, 0,
2, 7, 31, 0, 0, 0, 0, 0, 15, 4, 0,
1, 3, 31, 0, 0, 0, 0, 0, 15, 28, 0,
2, 3, 31, 0, 0, 0, 0, 0, 15, 4, 0 };

static const Preset_FM M_BANK_0_INST_82_LEADCALLIOPE = { 4, 7, 3, 0, 5,
5, 7, 19, 0, 10, 1, 1, 0, 5, 0, 0,
3, 2, 25, 2, 13, 0, 13, 0, 5, 41, 0,
2, 3, 13, 2, 13, 0, 5, 0, 5, 24, 0,
1, 6, 14, 1, 4, 1, 2, 0, 11, 5, 0 };

static const Preset_FM M_BANK_0_INST_83_LEAD4CHIF_SONIC = { 2, 7, 3, 0, 0,
1, 4, 14, 2, 14, 0, 5, 0, 15, 20, 0,
7, 4, 15, 2, 14, 0, 5, 8, 15, 43, 0,
1, 4, 14, 2, 14, 0, 11, 0, 15, 23, 0,
1, 4, 20, 1, 3, 0, 11, 8, 15, 4, 0 };

static const Preset_FM M_BANK_0_INST_84_LEAD5CHAR_NINEKO = { 0, 7, 3, 0, 0,
2, 7, 17, 3, 1, 0, 15, 0, 15, 30, 0,
3, 1, 18, 1, 7, 0, 15, 0, 15, 30, 0,
1, 7, 20, 0, 1, 0, 15, 0, 15, 30, 0,
1, 1, 20, 0, 1, 0, 15, 0, 15, 4, 0 };

static const Preset_FM M_BANK_0_INST_85_LEADVOICE = { 4, 1, 3, 2, 3,
2, 1, 15, 2, 0, 0, 0, 1, 10, 30, 0,
1, 7, 21, 0, 0, 0, 0, 0, 10, 4, 0,
2, 7, 16, 1, 11, 1, 8, 29, 10, 18, 0,
1, 1, 19, 0, 15, 1, 2, 6, 10, 12, 0 };

static const Preset_FM M_BANK_0_INST_86_LEAD7FIFTS_ANIMATICS = { 4, 7, 3, 0, 0,
4, 3, 27, 1, 5, 0, 0, 3, 7, 24, 0,
4, 7, 27, 1, 5, 0, 0, 3, 7, 0, 0,
3, 7, 24, 1, 0, 0, 0, 3, 6, 14, 0,
3, 3, 22, 1, 0, 0, 0, 3, 6, 12, 0 };

static const Preset_FM M_BANK_0_INST_87_LEAD8BASS_GEMS_ANALOG_SOLO = { 4, 0, 3, 0, 1,
1, 4, 19, 1, 13, 1, 6, 9, 9, 17, 0,
2, 7, 30, 0, 5, 1, 4, 0, 8, 6, 0,
1, 4, 1, 2, 19, 1, 0, 2, 8, 29, 0,
1, 6, 31, 0, 7, 0, 0, 0, 8, 4, 0 };

static const Preset_FM M_BANK_0_INST_88_PAD1NEWAGE = { 4, 1, 3, 0, 0,
7, 4, 15, 0, 11, 0, 7, 6, 6, 25, 0,
8, 4, 31, 0, 7, 0, 15, 0, 5, 22, 0,
1, 3, 9, 2, 2, 0, 2, 0, 3, 33, 0,
1, 4, 14, 0, 5, 0, 1, 0, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_89_PAD_WARM_PORTED_FROM_DMX = { 7, 0, 3, 0, 0,
1, 4, 7, 3, 0, 0, 0, 0, 3, 16, 0,
1, 4, 5, 0, 0, 0, 0, 0, 6, 16, 0,
1, 3, 7, 1, 0, 0, 0, 0, 4, 16, 0,
1, 3, 5, 0, 0, 0, 0, 0, 6, 16, 0 };

static const Preset_FM M_BANK_0_INST_90_POLYSYNTH = { 4, 7, 3, 0, 0,
1, 3, 18, 1, 8, 0, 1, 4, 6, 26, 0,
2, 5, 17, 1, 0, 0, 0, 0, 6, 3, 0,
0, 5, 21, 1, 8, 0, 1, 4, 5, 22, 0,
0, 3, 20, 1, 0, 0, 0, 0, 5, 5, 0 };

static const Preset_FM M_BANK_0_INST_91_PADCHOIR = { 5, 0, 3, 0, 4,
0, 4, 14, 1, 0, 1, 5, 0, 2, 34, 0,
0, 4, 12, 1, 0, 1, 0, 0, 5, 12, 0,
0, 4, 10, 2, 0, 0, 5, 0, 3, 17, 0,
0, 4, 9, 2, 0, 1, 0, 0, 4, 14, 0 };

static const Preset_FM M_BANK_0_INST_92_BOWED = { 4, 3, 3, 0, 0,
6, 4, 11, 3, 3, 0, 6, 1, 1, 26, 0,
1, 4, 9, 2, 6, 0, 7, 2, 3, 6, 0,
5, 4, 10, 0, 9, 0, 15, 0, 3, 35, 0,
2, 4, 10, 0, 6, 0, 15, 0, 4, 29, 0 };

static const Preset_FM M_BANK_0_INST_93_METALLIC = { 4, 7, 3, 0, 5,
1, 3, 11, 3, 11, 0, 0, 0, 1, 28, 0,
9, 4, 7, 0, 3, 0, 10, 0, 5, 14, 0,
1, 4, 11, 2, 3, 1, 1, 0, 1, 22, 0,
2, 4, 7, 0, 1, 0, 14, 0, 5, 11, 0 };

static const Preset_FM M_BANK_0_INST_94_PADHALO = { 4, 5, 3, 0, 4,
1, 2, 3, 1, 4, 0, 5, 0, 5, 12, 0,
1, 4, 9, 0, 7, 1, 3, 0, 6, 3, 0,
1, 7, 3, 1, 4, 0, 5, 1, 4, 0, 0,
1, 4, 10, 0, 7, 1, 3, 0, 6, 22, 0 };

static const Preset_FM M_BANK_0_INST_95_SWEEP = { 4, 5, 3, 0, 5,
2, 2, 6, 0, 6, 0, 5, 0, 4, 22, 0,
1, 4, 15, 0, 7, 1, 3, 0, 6, 3, 0,
5, 7, 4, 0, 8, 0, 5, 5, 4, 24, 0,
2, 4, 9, 0, 7, 1, 3, 0, 6, 19, 0 };

static const Preset_FM M_BANK_0_INST_96_FX_RAIN = { 6, 7, 3, 2, 2,
6, 4, 31, 0, 18, 0, 3, 0, 7, 20, 0,
2, 4, 31, 0, 18, 1, 2, 13, 5, 21, 0,
2, 3, 12, 0, 0, 1, 0, 0, 5, 17, 0,
1, 7, 12, 0, 0, 1, 0, 0, 5, 17, 0 };

static const Preset_FM M_BANK_0_INST_97_FXSNDTRACK_THUNDERFORCE_IV = { 4, 5, 3, 0, 0,
3, 3, 6, 0, 6, 0, 15, 4, 4, 30, 0,
3, 5, 17, 0, 3, 0, 2, 0, 4, 9, 0,
2, 5, 5, 0, 6, 0, 15, 4, 5, 30, 0,
2, 3, 10, 0, 3, 0, 2, 0, 5, 9, 0 };

static const Preset_FM M_BANK_0_INST_98_FXCRYSTAL_ALISIA_DRAGOON = { 4, 7, 3, 0, 0,
5, 4, 31, 0, 13, 0, 7, 4, 6, 55, 0,
2, 7, 31, 0, 8, 0, 6, 7, 8, 12, 0,
4, 4, 26, 0, 19, 0, 2, 7, 6, 43, 0,
2, 3, 24, 0, 8, 0, 2, 7, 5, 15, 0 };

static const Preset_FM M_BANK_0_INST_99_ATMOSPHERE_ALISIA_DRAGOON = { 4, 6, 3, 1, 3,
2, 7, 22, 2, 1, 0, 5, 0, 5, 23, 0,
1, 3, 31, 2, 4, 1, 4, 0, 5, 5, 0,
1, 1, 22, 0, 4, 0, 0, 5, 6, 49, 0,
1, 5, 13, 0, 9, 1, 5, 6, 6, 16, 0 };

static const Preset_FM M_BANK_0_INST_100_BRIGHTNESS = { 3, 0, 3, 0, 0,
2, 3, 18, 2, 1, 1, 5, 4, 1, 40, 0,
3, 5, 31, 1, 0, 0, 0, 0, 6, 43, 0,
5, 5, 20, 1, 8, 0, 1, 4, 4, 39, 0,
2, 3, 31, 1, 0, 0, 0, 0, 5, 6, 0 };

static const Preset_FM M_BANK_0_INST_101_GOBLINS = { 4, 5, 3, 0, 4,
1, 2, 4, 0, 6, 0, 5, 0, 3, 18, 0,
1, 4, 3, 1, 7, 1, 3, 0, 3, 19, 0,
1, 7, 4, 0, 1, 1, 0, 1, 3, 0, 0,
1, 4, 5, 0, 7, 0, 3, 0, 3, 22, 0 };

static const Preset_FM M_BANK_0_INST_102_ECHOS = { 4, 2, 3, 0, 4,
2, 3, 16, 1, 0, 1, 5, 0, 5, 41, 0,
2, 4, 16, 1, 0, 1, 0, 0, 5, 12, 0,
1, 4, 11, 2, 0, 0, 5, 0, 5, 45, 0,
1, 7, 20, 2, 0, 1, 0, 0, 5, 16, 0 };

static const Preset_FM M_BANK_0_INST_103_SCIFI = { 0, 3, 3, 0, 4,
1, 3, 20, 2, 2, 0, 2, 0, 3, 34, 0,
8, 6, 31, 2, 2, 0, 2, 0, 3, 36, 0,
3, 2, 31, 2, 3, 0, 4, 0, 3, 34, 0,
1, 4, 19, 0, 1, 1, 2, 0, 5, 8, 0 };

static const Preset_FM M_BANK_0_INST_104_SITAR = { 4, 0, 3, 0, 0,
2, 4, 31, 1, 7, 0, 4, 5, 4, 47, 0,
7, 4, 19, 2, 3, 0, 15, 0, 6, 25, 0,
2, 4, 18, 2, 3, 0, 15, 0, 3, 8, 0,
7, 4, 31, 0, 5, 0, 15, 0, 6, 14, 0 };

static const Preset_FM M_BANK_0_INST_105_BANJO = { 2, 6, 3, 0, 0,
6, 3, 15, 1, 20, 0, 1, 8, 9, 25, 0,
1, 2, 31, 1, 15, 0, 1, 23, 8, 69, 0,
2, 5, 28, 1, 9, 0, 1, 1, 0, 23, 0,
2, 3, 19, 0, 25, 0, 1, 9, 7, 0, 0 };

static const Preset_FM M_BANK_0_INST_106_SHAMISEN = { 0, 7, 3, 0, 0,
5, 4, 31, 2, 14, 0, 15, 0, 8, 27, 0,
2, 4, 31, 2, 14, 0, 14, 0, 2, 34, 0,
1, 4, 31, 1, 5, 0, 14, 0, 4, 24, 0,
6, 4, 31, 1, 7, 0, 15, 0, 5, 10, 0 };

static const Preset_FM M_BANK_0_INST_107_KOTO = { 4, 4, 3, 0, 3,
3, 5, 27, 2, 8, 0, 3, 4, 3, 23, 0,
1, 4, 24, 1, 22, 0, 2, 4, 3, 7, 0,
3, 1, 27, 2, 8, 1, 3, 8, 2, 28, 0,
3, 4, 24, 0, 22, 0, 1, 8, 5, 14, 0 };

static const Preset_FM M_BANK_0_INST_108_KALIMBA = { 6, 6, 3, 0, 0,
4, 7, 20, 1, 22, 1, 7, 21, 7, 23, 0,
1, 7, 21, 0, 0, 1, 0, 10, 6, 7, 0,
4, 6, 30, 1, 13, 0, 5, 7, 5, 21, 0,
4, 7, 21, 0, 12, 0, 4, 9, 6, 19, 0 };

static const Preset_FM M_BANK_0_INST_109_BAGPIPE = { 0, 3, 3, 1, 0,
1, 4, 16, 2, 16, 0, 3, 0, 6, 11, 0,
4, 4, 12, 0, 0, 0, 0, 0, 13, 39, 0,
0, 4, 17, 0, 0, 0, 3, 0, 5, 28, 0,
2, 4, 12, 2, 0, 0, 0, 0, 13, 8, 0 };

static const Preset_FM M_BANK_0_INST_110_FIDDLE = { 2, 7, 3, 2, 3,
1, 3, 14, 0, 4, 0, 1, 0, 5, 29, 0,
7, 3, 12, 0, 16, 0, 0, 0, 4, 31, 0,
0, 7, 11, 0, 9, 0, 0, 3, 7, 55, 0,
2, 4, 10, 2, 10, 0, 1, 0, 5, 3, 0 };

static const Preset_FM M_BANK_0_INST_111_SHANAI_ALADDIN = { 0, 7, 3, 1, 2,
1, 4, 24, 0, 23, 1, 0, 0, 10, 39, 0,
1, 4, 16, 0, 31, 1, 0, 0, 10, 30, 0,
1, 4, 22, 0, 31, 0, 0, 0, 10, 40, 0,
4, 4, 24, 0, 31, 0, 0, 0, 10, 0, 0 };

static const Preset_FM M_BANK_0_INST_112_TINKLEBELL = { 7, 0, 3, 0, 0,
3, 3, 20, 3, 4, 0, 0, 4, 3, 25, 0,
2, 7, 22, 3, 3, 0, 5, 8, 3, 16, 0,
2, 3, 20, 0, 5, 0, 0, 6, 3, 17, 0,
3, 7, 22, 2, 7, 0, 11, 7, 3, 11, 0 };

static const Preset_FM M_BANK_0_INST_113_AGOGO = { 4, 7, 3, 0, 0,
6, 4, 31, 0, 31, 0, 6, 14, 8, 0, 0,
14, 4, 31, 2, 14, 0, 10, 14, 7, 14, 0,
11, 4, 31, 0, 31, 0, 4, 14, 8, 0, 0,
14, 4, 31, 2, 31, 0, 0, 15, 7, 14, 0 };

static const Preset_FM M_BANK_0_INST_114_STEELDRUMS = { 4, 1, 3, 3, 0,
12, 1, 13, 0, 19, 0, 5, 0, 7, 65, 0,
1, 7, 26, 0, 12, 0, 15, 0, 6, 12, 0,
5, 3, 13, 0, 16, 0, 8, 7, 7, 34, 0,
1, 7, 26, 0, 14, 0, 15, 0, 6, 12, 0 };

static const Preset_FM M_BANK_0_INST_115_WOODBLOCK = { 4, 7, 3, 0, 0,
7, 1, 31, 0, 22, 0, 12, 22, 8, 20, 0,
5, 1, 31, 0, 18, 0, 11, 28, 8, 0, 0,
14, 1, 31, 3, 25, 0, 14, 12, 8, 10, 0,
9, 1, 31, 3, 20, 0, 1, 13, 8, 5, 0 };

static const Preset_FM M_BANK_0_INST_116_TAIKO_DRUM = { 6, 3, 3, 0, 0,
0, 4, 31, 2, 18, 0, 0, 18, 8, 6, 0,
1, 4, 31, 2, 16, 0, 0, 16, 11, 4, 0,
0, 4, 31, 0, 16, 0, 0, 16, 7, 12, 0,
0, 4, 31, 0, 8, 0, 9, 12, 5, 3, 0 };

static const Preset_FM M_BANK_0_INST_117_MELODIC_TOM = { 1, 6, 3, 0, 0,
2, 4, 31, 0, 0, 0, 0, 13, 0, 16, 0,
0, 4, 31, 0, 2, 0, 0, 18, 0, 2, 0,
0, 4, 30, 0, 0, 0, 0, 20, 0, 35, 0,
0, 4, 30, 0, 0, 0, 0, 14, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_118_SYNDRUM_ALISIA_DRAGOON = { 3, 7, 3, 0, 0,
1, 4, 29, 0, 18, 0, 5, 0, 6, 6, 0,
1, 4, 31, 0, 26, 0, 2, 22, 6, 8, 0,
2, 4, 31, 1, 27, 0, 5, 8, 6, 13, 0,
1, 4, 31, 0, 14, 0, 12, 17, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_119_REVERSE_CYMBELL = { 4, 7, 3, 1, 0,
15, 7, 29, 0, 1, 0, 8, 0, 0, 0, 0,
3, 1, 4, 1, 13, 1, 0, 22, 9, 9, 0,
15, 7, 31, 0, 0, 0, 1, 0, 4, 0, 0,
0, 4, 4, 2, 27, 0, 10, 6, 6, 29, 0 };

static const Preset_FM M_BANK_0_INST_120_GUITAR_FRET_NOISE = { 4, 7, 3, 1, 0,
6, 7, 27, 0, 1, 0, 8, 0, 0, 14, 0,
3, 1, 14, 1, 7, 1, 0, 14, 9, 17, 0,
3, 7, 31, 0, 0, 0, 1, 0, 4, 33, 0,
4, 4, 12, 2, 14, 0, 10, 9, 6, 32, 0 };

static const Preset_FM M_BANK_0_INST_121_BREATH_NOISE = { 4, 7, 3, 0, 0,
4, 3, 31, 0, 0, 0, 2, 0, 2, 0, 0,
2, 7, 12, 0, 12, 0, 11, 7, 7, 11, 0,
4, 5, 31, 0, 0, 0, 2, 0, 2, 8, 0,
2, 3, 12, 0, 9, 1, 7, 9, 6, 24, 0 };

static const Preset_FM M_BANK_0_INST_122_SEASHORE = { 5, 7, 3, 0, 0,
3, 3, 31, 0, 0, 0, 15, 0, 0, 0, 0,
1, 4, 6, 0, 8, 0, 15, 0, 4, 26, 0,
1, 4, 6, 0, 8, 0, 15, 0, 4, 22, 0,
1, 4, 7, 0, 8, 0, 15, 0, 4, 28, 0 };

static const Preset_FM M_BANK_0_INST_123_BIRDTWEET = { 7, 3, 3, 0, 0,
7, 4, 6, 3, 16, 0, 0, 14, 7, 25, 8,
8, 4, 8, 3, 14, 0, 5, 16, 6, 16, 8,
5, 1, 15, 0, 13, 0, 0, 14, 10, 17, 0,
6, 4, 11, 2, 14, 0, 11, 16, 6, 0, 0 };

static const Preset_FM M_BANK_0_INST_124_TELEPHONE = { 4, 7, 3, 0, 0,
11, 3, 31, 0, 2, 0, 15, 0, 8, 32, 0,
13, 3, 16, 1, 17, 0, 15, 0, 5, 0, 14,
0, 4, 31, 0, 2, 0, 15, 0, 8, 50, 0,
6, 4, 22, 0, 19, 0, 15, 0, 6, 0, 14 };

static const Preset_FM M_BANK_0_INST_125_HELICOPTER = { 4, 6, 3, 3, 0,
0, 3, 5, 1, 1, 0, 0, 0, 0, 0, 0,
2, 4, 7, 0, 2, 1, 1, 0, 5, 20, 0,
0, 4, 5, 0, 13, 1, 0, 0, 9, 127, 0,
0, 4, 7, 0, 15, 1, 0, 0, 4, 127, 0 };

static const Preset_FM M_BANK_0_INST_126_APPLAUSE = { 5, 7, 3, 0, 0,
3, 3, 31, 0, 0, 0, 15, 0, 0, 4, 0,
1, 1, 6, 0, 7, 0, 0, 0, 8, 23, 0,
1, 4, 6, 0, 13, 0, 0, 0, 9, 22, 0,
1, 4, 6, 0, 15, 0, 0, 0, 4, 21, 0 };

static const Preset_FM M_BANK_0_INST_127_GUNSHOT = { 5, 7, 3, 0, 5,
3, 3, 31, 0, 3, 0, 15, 17, 5, 4, 0,
1, 4, 31, 0, 15, 0, 15, 31, 8, 0, 0,
1, 4, 31, 0, 20, 0, 15, 20, 8, 0, 0,
1, 4, 31, 0, 15, 0, 15, 16, 11, 0, 0 };

// for blastem
void InitPresets()
{
    M_BANK_0[0] = &M_BANK_0_INST_0_GRANDPIANO;
    M_BANK_0[1] = &M_BANK_0_INST_1_BRIGHTPIANO;
    M_BANK_0[2] = &M_BANK_0_INST_2_ELECTRICPIANO_ANIMATICS;
    M_BANK_0[3] = &M_BANK_0_INST_3_HONKYTONK_SONICMODDED;
    M_BANK_0[4] = &M_BANK_0_INST_4_RHODESPIANO;
    M_BANK_0[5] = &M_BANK_0_INST_5_CHORUSPIANOTINITOON;
    M_BANK_0[6] = &M_BANK_0_INST_6_HARPSICHORD_ADAMS;
    M_BANK_0[7] = &M_BANK_0_INST_7_CLAVINET;
    M_BANK_0[8] = &M_BANK_0_INST_8_CELESTA;
    M_BANK_0[9] = &M_BANK_0_INST_9_GLOCKENSPIEL;
    M_BANK_0[10] = &M_BANK_0_INST_10_MUSICBOX;
    M_BANK_0[11] = &M_BANK_0_INST_11_VIBRAPHONE_NINEKO;
    M_BANK_0[12] = &M_BANK_0_INST_12_MARIMBA_ALADDIN;
    M_BANK_0[13] = &M_BANK_0_INST_13_XYLAPHONE_ALADDIN;
    M_BANK_0[14] = &M_BANK_0_INST_14_TUBULARBELLS_ANIMATICS;
    M_BANK_0[15] = &M_BANK_0_INST_15_DULCIMER;
    M_BANK_0[16] = &M_BANK_0_INST_16_HAMMOND_NINEKO;
    M_BANK_0[17] = &M_BANK_0_INST_17_PERCUSIVEORGAIN_ACTION52;
    M_BANK_0[18] = &M_BANK_0_INST_18_ROCKORGAN;
    M_BANK_0[19] = &M_BANK_0_INST_19_CHURCHORGAN;
    M_BANK_0[20] = &M_BANK_0_INST_20_REEDORGAN;
    M_BANK_0[21] = &M_BANK_0_INST_21_ACCORDEON;
    M_BANK_0[22] = &M_BANK_0_INST_22_HARMONICA_SONIC;
    M_BANK_0[23] = &M_BANK_0_INST_23_TANGO_ACCORDION;
    M_BANK_0[24] = &M_BANK_0_INST_24_NYLONGUITARHELLROLL;
    M_BANK_0[25] = &M_BANK_0_INST_25_STEELGUITARHELLROLL;
    M_BANK_0[26] = &M_BANK_0_INST_26_JAZZGUITAR;
    M_BANK_0[27] = &M_BANK_0_INST_27_CLEANGUITAR_COLUMNS_III;
    M_BANK_0[28] = &M_BANK_0_INST_28_MUTEDGUITAR_ALADDIN;
    M_BANK_0[29] = &M_BANK_0_INST_29_OVERDRIVE_OPL3;
    M_BANK_0[30] = &M_BANK_0_INST_30_DISTORTION_OPL3;
    M_BANK_0[31] = &M_BANK_0_INST_31_GUITAR_HARMONICSHELLFIRE;
    M_BANK_0[32] = &M_BANK_0_INST_32_ACOUSTICBASS_SONIC;
    M_BANK_0[33] = &M_BANK_0_INST_33_FINGERBASS_SONIC;
    M_BANK_0[34] = &M_BANK_0_INST_34_PICKBASS;
    M_BANK_0[35] = &M_BANK_0_INST_35_FRETLESSBASSALADDIN;
    M_BANK_0[36] = &M_BANK_0_INST_36_SLAPBASS1;
    M_BANK_0[37] = &M_BANK_0_INST_37_SLAPBASS2;
    M_BANK_0[38] = &M_BANK_0_INST_38_SYNBASS1_DYNABROS;
    M_BANK_0[39] = &M_BANK_0_INST_39_SYNBASS2_NINEKO;
    M_BANK_0[40] = &M_BANK_0_INST_40_VIOLIN;
    M_BANK_0[41] = &M_BANK_0_INST_41_VIOLA;
    M_BANK_0[42] = &M_BANK_0_INST_42_CELLO;
    M_BANK_0[43] = &M_BANK_0_INST_43_CONTROLBASS;
    M_BANK_0[44] = &M_BANK_0_INST_44_TREMOLOSTRINGS;
    M_BANK_0[45] = &M_BANK_0_INST_45_PIZZIKATOSTRINGS;
    M_BANK_0[46] = &M_BANK_0_INST_46_ORCHESTRALHARP;
    M_BANK_0[47] = &M_BANK_0_INST_47_TIMPANY;
    M_BANK_0[48] = &M_BANK_0_INST_48_STRINGESSEMBLE1;
    M_BANK_0[49] = &M_BANK_0_INST_49_STRINGESSEMBLE2;
    M_BANK_0[50] = &M_BANK_0_INST_50_SYNSTRINGS_1;
    M_BANK_0[51] = &M_BANK_0_INST_51_SYNSTRING2_ALADDIN;
    M_BANK_0[52] = &M_BANK_0_INST_52_CHOIR_AAHS;
    M_BANK_0[53] = &M_BANK_0_INST_53_VOICE;
    M_BANK_0[54] = &M_BANK_0_INST_54_SYNTHVOICE;
    M_BANK_0[55] = &M_BANK_0_INST_55_ORCHESTRAHIT;
    M_BANK_0[56] = &M_BANK_0_INST_56_TRUMPET_SONIC;
    M_BANK_0[57] = &M_BANK_0_INST_57_TROMBONE_ALADDIN;
    M_BANK_0[58] = &M_BANK_0_INST_58_TUBA_ALADDIN;
    M_BANK_0[59] = &M_BANK_0_INST_59_MUTEDTRUMPET_SONIC;
    M_BANK_0[60] = &M_BANK_0_INST_60_FRENCHHORN;
    M_BANK_0[61] = &M_BANK_0_INST_61_BRASSSECTION_GREENDOG;
    M_BANK_0[62] = &M_BANK_0_INST_62_SYNBRASS1;
    M_BANK_0[63] = &M_BANK_0_INST_63_SYNBRASS2;
    M_BANK_0[64] = &M_BANK_0_INST_64_SOPRANOSAX;
    M_BANK_0[65] = &M_BANK_0_INST_65_ALTOSAX;
    M_BANK_0[66] = &M_BANK_0_INST_66_TENORSAX;
    M_BANK_0[67] = &M_BANK_0_INST_67_BARITONESAX;
    M_BANK_0[68] = &M_BANK_0_INST_68_OBOE_ALADDIN;
    M_BANK_0[69] = &M_BANK_0_INST_69_ENGLISHHORN;
    M_BANK_0[70] = &M_BANK_0_INST_70_BASSOON_ALADDIN;
    M_BANK_0[71] = &M_BANK_0_INST_71_CLARINET_ALADDIN;
    M_BANK_0[72] = &M_BANK_0_INST_72_PICCOLO_ALADDIN;
    M_BANK_0[73] = &M_BANK_0_INST_73_FLUTE;
    M_BANK_0[74] = &M_BANK_0_INST_74_RECORDER;
    M_BANK_0[75] = &M_BANK_0_INST_75_PANFLUTE;
    M_BANK_0[76] = &M_BANK_0_INST_76_BOTTLEBLOW;
    M_BANK_0[77] = &M_BANK_0_INST_77_SHAKUHACHI_ALISIA_DRAGOON;
    M_BANK_0[78] = &M_BANK_0_INST_78_WHISTLE_ALADDIN;
    M_BANK_0[79] = &M_BANK_0_INST_79_OCARINA;
    M_BANK_0[80] = &M_BANK_0_INST_80_LEADSQUARE;
    M_BANK_0[81] = &M_BANK_0_INST_81_LEADSAW;
    M_BANK_0[82] = &M_BANK_0_INST_82_LEADCALLIOPE;
    M_BANK_0[83] = &M_BANK_0_INST_83_LEAD4CHIF_SONIC;
    M_BANK_0[84] = &M_BANK_0_INST_84_LEAD5CHAR_NINEKO;
    M_BANK_0[85] = &M_BANK_0_INST_85_LEADVOICE;
    M_BANK_0[86] = &M_BANK_0_INST_86_LEAD7FIFTS_ANIMATICS;
    M_BANK_0[87] = &M_BANK_0_INST_87_LEAD8BASS_GEMS_ANALOG_SOLO;
    M_BANK_0[88] = &M_BANK_0_INST_88_PAD1NEWAGE;
    M_BANK_0[89] = &M_BANK_0_INST_89_PAD_WARM_PORTED_FROM_DMX;
    M_BANK_0[90] = &M_BANK_0_INST_90_POLYSYNTH;
    M_BANK_0[91] = &M_BANK_0_INST_91_PADCHOIR;
    M_BANK_0[92] = &M_BANK_0_INST_92_BOWED;
    M_BANK_0[93] = &M_BANK_0_INST_93_METALLIC;
    M_BANK_0[94] = &M_BANK_0_INST_94_PADHALO;
    M_BANK_0[95] = &M_BANK_0_INST_95_SWEEP;
    M_BANK_0[96] = &M_BANK_0_INST_96_FX_RAIN;
    M_BANK_0[97] = &M_BANK_0_INST_97_FXSNDTRACK_THUNDERFORCE_IV;
    M_BANK_0[98] = &M_BANK_0_INST_98_FXCRYSTAL_ALISIA_DRAGOON;
    M_BANK_0[99] = &M_BANK_0_INST_99_ATMOSPHERE_ALISIA_DRAGOON;
    M_BANK_0[100] = &M_BANK_0_INST_100_BRIGHTNESS;
    M_BANK_0[101] = &M_BANK_0_INST_101_GOBLINS;
    M_BANK_0[102] = &M_BANK_0_INST_102_ECHOS;
    M_BANK_0[103] = &M_BANK_0_INST_103_SCIFI;
    M_BANK_0[104] = &M_BANK_0_INST_104_SITAR;
    M_BANK_0[105] = &M_BANK_0_INST_105_BANJO;
    M_BANK_0[106] = &M_BANK_0_INST_106_SHAMISEN;
    M_BANK_0[107] = &M_BANK_0_INST_107_KOTO;
    M_BANK_0[108] = &M_BANK_0_INST_108_KALIMBA;
    M_BANK_0[109] = &M_BANK_0_INST_109_BAGPIPE;
    M_BANK_0[110] = &M_BANK_0_INST_110_FIDDLE;
    M_BANK_0[111] = &M_BANK_0_INST_111_SHANAI_ALADDIN;
    M_BANK_0[112] = &M_BANK_0_INST_112_TINKLEBELL;
    M_BANK_0[113] = &M_BANK_0_INST_113_AGOGO;
    M_BANK_0[114] = &M_BANK_0_INST_114_STEELDRUMS;
    M_BANK_0[115] = &M_BANK_0_INST_115_WOODBLOCK;
    M_BANK_0[116] = &M_BANK_0_INST_116_TAIKO_DRUM;
    M_BANK_0[117] = &M_BANK_0_INST_117_MELODIC_TOM;
    M_BANK_0[118] = &M_BANK_0_INST_118_SYNDRUM_ALISIA_DRAGOON;
    M_BANK_0[119] = &M_BANK_0_INST_119_REVERSE_CYMBELL;
    M_BANK_0[120] = &M_BANK_0_INST_120_GUITAR_FRET_NOISE;
    M_BANK_0[121] = &M_BANK_0_INST_121_BREATH_NOISE;
    M_BANK_0[122] = &M_BANK_0_INST_122_SEASHORE;
    M_BANK_0[123] = &M_BANK_0_INST_123_BIRDTWEET;
    M_BANK_0[124] = &M_BANK_0_INST_124_TELEPHONE;
    M_BANK_0[125] = &M_BANK_0_INST_125_HELICOPTER;
    M_BANK_0[126] = &M_BANK_0_INST_126_APPLAUSE;
    M_BANK_0[127] = &M_BANK_0_INST_127_GUNSHOT;

    //{ presets
    presetName[0x00] = "PIANO GRAND  ";
    presetName[0x01] = "PIANO BRIGHT ";
    presetName[0x02] = "PIANO ELECTRO";
    presetName[0x03] = "HONKYTONK    ";
    presetName[0x04] = "PIANO RHODES ";
    presetName[0x05] = "PIANO CHORUS ";
    presetName[0x06] = "HARPICHORD   ";
    presetName[0x07] = "CLAVINET     ";
    presetName[0x08] = "CELESTA      ";
    presetName[0x09] = "GLOCKENSPIEL ";
    presetName[0x0A] = "MUSIC BOX    ";
    presetName[0x0B] = "VIBRAPHONE   ";
    presetName[0x0C] = "MARIMBA      ";
    presetName[0x0D] = "XYLAPHONE    ";
    presetName[0x0E] = "TUBULAR BELLS";
    presetName[0x0F] = "DULCIMER     ";
    presetName[0x10] = "ORGAN HAMMOND";
    presetName[0x11] = "ORGAN PERC.  ";
    presetName[0x12] = "ORGAN ROCK   ";
    presetName[0x13] = "ORGAN CHURCH ";
    presetName[0x14] = "ORGAN REED   ";
    presetName[0x15] = "ACCORDEON    ";
    presetName[0x16] = "HARMONICA    ";
    presetName[0x17] = "TANGO ACCORD.";
    presetName[0x18] = "GUITAR NYLON ";
    presetName[0x19] = "GUITAR STEEL ";
    presetName[0x1A] = "GUITAR JAZZ  ";
    presetName[0x1B] = "GUITAR CLEAN ";
    presetName[0x1C] = "GUITAR MUTED ";
    presetName[0x1D] = "GUITAR OVERD.";
    presetName[0x1E] = "GUITAR DIST. ";
    presetName[0x1F] = "GUITAR HARM. ";
    presetName[0x20] = "BASS ACOUSTIC";
    presetName[0x21] = "BASS FINGER  ";
    presetName[0x22] = "BASS PICK    ";
    presetName[0x23] = "BASS FRETLESS";
    presetName[0x24] = "BASS SLAP 1  ";
    presetName[0x25] = "BASS SLAP 2  ";
    presetName[0x26] = "BASS SYNTH 1 ";
    presetName[0x27] = "BASS SYNTH 2 ";
    presetName[0x28] = "VIOLIN       ";
    presetName[0x29] = "VIOLA        ";
    presetName[0x2A] = "CELLO        ";
    presetName[0x2B] = "CONTRABASS   ";
    presetName[0x2C] = "STRINGS TREM.";
    presetName[0x2D] = "STRINGS PIZZ.";
    presetName[0x2E] = "HARP         ";
    presetName[0x2F] = "TIMPANY      ";
    presetName[0x30] = "STRINGS ENS.1";
    presetName[0x31] = "STRINGS ENS.2";
    presetName[0x32] = "STRINGS SYN.1";
    presetName[0x33] = "STRINGS SYN.2";
    presetName[0x34] = "CHOIR AAHS   ";
    presetName[0x35] = "VOICE        ";
    presetName[0x36] = "VOICE SYNTH  ";
    presetName[0x37] = "ORCH. HIT    ";
    presetName[0x38] = "TRUMPET      ";
    presetName[0x39] = "TROMBONE     ";
    presetName[0x3A] = "TUBA         ";
    presetName[0x3B] = "TRUMPET MUTED";
    presetName[0x3C] = "FRENCH HORN  ";
    presetName[0x3D] = "BRASS        ";
    presetName[0x3E] = "BRASS SYN.1  ";
    presetName[0x3F] = "BRASS SYN.2  ";
    presetName[0x40] = "SAX. SOPRANO ";
    presetName[0x41] = "SAX. ALTO    ";
    presetName[0x42] = "SAX. TENOR   ";
    presetName[0x43] = "SAX. BARITONE";
    presetName[0x44] = "OBOE         ";
    presetName[0x45] = "ENGLISH HORN ";
    presetName[0x46] = "BASSOON      ";
    presetName[0x47] = "CLARINET     ";
    presetName[0x48] = "PICCOLO      ";
    presetName[0x49] = "FLUTE        ";
    presetName[0x4A] = "RECORDER     ";
    presetName[0x4B] = "PANFLUTE     ";
    presetName[0x4C] = "BOTTLE BLOW  ";
    presetName[0x4D] = "SHAKUHACHI   ";
    presetName[0x4E] = "WHISTLE      ";
    presetName[0x4F] = "OCARINA      ";
    presetName[0x50] = "LEAD SQUARE  ";
    presetName[0x51] = "LEAD SAW     ";
    presetName[0x52] = "LEAD CALLIOPE";
    presetName[0x53] = "LEAD CHIF    ";
    presetName[0x54] = "LEAD CHAR    ";
    presetName[0x55] = "LEAD VOICE   ";
    presetName[0x56] = "LEAD FIFTHS  ";
    presetName[0x57] = "LEAD BASS    ";
    presetName[0x58] = "PAD NEWAGE   ";
    presetName[0x59] = "PAD WARM     ";
    presetName[0x5A] = "PAD POLYSYNTH";
    presetName[0x5B] = "PAD CHOIR    ";
    presetName[0x5C] = "PAD BOWED    ";
    presetName[0x5D] = "PAD METALLIC ";
    presetName[0x5E] = "PAD HALO     ";
    presetName[0x5F] = "PAD SWEEP    ";
    presetName[0x60] = "FX RAIN      ";
    presetName[0x61] = "FX SOUNDTRACK";
    presetName[0x62] = "FX CRYSTAL   ";
    presetName[0x63] = "FX ATMOSPHERE";
    presetName[0x64] = "FX BRIGHTNESS";
    presetName[0x65] = "FX GOBLINS   ";
    presetName[0x66] = "FX ECHOS     ";
    presetName[0x67] = "FX SCIFI     ";
    presetName[0x68] = "SITAR        ";
    presetName[0x69] = "BANJO        ";
    presetName[0x6A] = "SHAMISEN     ";
    presetName[0x6B] = "KOTO         ";
    presetName[0x6C] = "KALIMBA      ";
    presetName[0x6D] = "BAGPIPE      ";
    presetName[0x6E] = "FIDDLE       ";
    presetName[0x6F] = "SHANAI       ";
    presetName[0x70] = "TINKLEBELL   ";
    presetName[0x71] = "AGOGO        ";
    presetName[0x72] = "STEEL DRUMS  ";
    presetName[0x73] = "WOODBLOCK    ";
    presetName[0x74] = "TAIKO DRUM   ";
    presetName[0x75] = "MELODIC TOM  ";
    presetName[0x76] = "SYNTH DRUM   ";
    presetName[0x77] = "REV. CYMBELL ";
    presetName[0x78] = "FRET NOISE   ";
    presetName[0x79] = "BREATH NOISE ";
    presetName[0x7A] = "SEASHORE     ";
    presetName[0x7B] = "BIRDTWEET    ";
    presetName[0x7C] = "TELEPHONE    ";
    presetName[0x7D] = "HELICOPTER   ";
    presetName[0x7E] = "APPLAUSE     ";
    presetName[0x7F] = "GUNSHOT      ";
    //}
}

const Preset_FM* M_BANK_0[128] =
{
    &M_BANK_0_INST_0_GRANDPIANO,
    &M_BANK_0_INST_1_BRIGHTPIANO,
    &M_BANK_0_INST_2_ELECTRICPIANO_ANIMATICS,
    &M_BANK_0_INST_3_HONKYTONK_SONICMODDED,
    &M_BANK_0_INST_4_RHODESPIANO,
    &M_BANK_0_INST_5_CHORUSPIANOTINITOON,
    &M_BANK_0_INST_6_HARPSICHORD_ADAMS,
    &M_BANK_0_INST_7_CLAVINET,
    &M_BANK_0_INST_8_CELESTA,
    &M_BANK_0_INST_9_GLOCKENSPIEL,
    &M_BANK_0_INST_10_MUSICBOX,
    &M_BANK_0_INST_11_VIBRAPHONE_NINEKO,
    &M_BANK_0_INST_12_MARIMBA_ALADDIN,
    &M_BANK_0_INST_13_XYLAPHONE_ALADDIN,
    &M_BANK_0_INST_14_TUBULARBELLS_ANIMATICS,
    &M_BANK_0_INST_15_DULCIMER,
    &M_BANK_0_INST_16_HAMMOND_NINEKO,
    &M_BANK_0_INST_17_PERCUSIVEORGAIN_ACTION52,
    &M_BANK_0_INST_18_ROCKORGAN,
    &M_BANK_0_INST_19_CHURCHORGAN,
    &M_BANK_0_INST_20_REEDORGAN,
    &M_BANK_0_INST_21_ACCORDEON,
    &M_BANK_0_INST_22_HARMONICA_SONIC,
    &M_BANK_0_INST_23_TANGO_ACCORDION,
    &M_BANK_0_INST_24_NYLONGUITARHELLROLL,
    &M_BANK_0_INST_25_STEELGUITARHELLROLL,
    &M_BANK_0_INST_26_JAZZGUITAR,
    &M_BANK_0_INST_27_CLEANGUITAR_COLUMNS_III,
    &M_BANK_0_INST_28_MUTEDGUITAR_ALADDIN,
    &M_BANK_0_INST_29_OVERDRIVE_OPL3,
    &M_BANK_0_INST_30_DISTORTION_OPL3,
    &M_BANK_0_INST_31_GUITAR_HARMONICSHELLFIRE,
    &M_BANK_0_INST_32_ACOUSTICBASS_SONIC,
    &M_BANK_0_INST_33_FINGERBASS_SONIC,
    &M_BANK_0_INST_34_PICKBASS,
    &M_BANK_0_INST_35_FRETLESSBASSALADDIN,
    &M_BANK_0_INST_36_SLAPBASS1,
    &M_BANK_0_INST_37_SLAPBASS2,
    &M_BANK_0_INST_38_SYNBASS1_DYNABROS,
    &M_BANK_0_INST_39_SYNBASS2_NINEKO,
    &M_BANK_0_INST_40_VIOLIN,
    &M_BANK_0_INST_41_VIOLA,
    &M_BANK_0_INST_42_CELLO,
    &M_BANK_0_INST_43_CONTROLBASS,
    &M_BANK_0_INST_44_TREMOLOSTRINGS,
    &M_BANK_0_INST_45_PIZZIKATOSTRINGS,
    &M_BANK_0_INST_46_ORCHESTRALHARP,
    &M_BANK_0_INST_47_TIMPANY,
    &M_BANK_0_INST_48_STRINGESSEMBLE1,
    &M_BANK_0_INST_49_STRINGESSEMBLE2,
    &M_BANK_0_INST_50_SYNSTRINGS_1,
    &M_BANK_0_INST_51_SYNSTRING2_ALADDIN,
    &M_BANK_0_INST_52_CHOIR_AAHS,
    &M_BANK_0_INST_53_VOICE,
    &M_BANK_0_INST_54_SYNTHVOICE,
    &M_BANK_0_INST_55_ORCHESTRAHIT,
    &M_BANK_0_INST_56_TRUMPET_SONIC,
    &M_BANK_0_INST_57_TROMBONE_ALADDIN,
    &M_BANK_0_INST_58_TUBA_ALADDIN,
    &M_BANK_0_INST_59_MUTEDTRUMPET_SONIC,
    &M_BANK_0_INST_60_FRENCHHORN,
    &M_BANK_0_INST_61_BRASSSECTION_GREENDOG,
    &M_BANK_0_INST_62_SYNBRASS1,
    &M_BANK_0_INST_63_SYNBRASS2,
    &M_BANK_0_INST_64_SOPRANOSAX,
    &M_BANK_0_INST_65_ALTOSAX,
    &M_BANK_0_INST_66_TENORSAX,
    &M_BANK_0_INST_67_BARITONESAX,
    &M_BANK_0_INST_68_OBOE_ALADDIN,
    &M_BANK_0_INST_69_ENGLISHHORN,
    &M_BANK_0_INST_70_BASSOON_ALADDIN,
    &M_BANK_0_INST_71_CLARINET_ALADDIN,
    &M_BANK_0_INST_72_PICCOLO_ALADDIN,
    &M_BANK_0_INST_73_FLUTE,
    &M_BANK_0_INST_74_RECORDER,
    &M_BANK_0_INST_75_PANFLUTE,
    &M_BANK_0_INST_76_BOTTLEBLOW,
    &M_BANK_0_INST_77_SHAKUHACHI_ALISIA_DRAGOON,
    &M_BANK_0_INST_78_WHISTLE_ALADDIN,
    &M_BANK_0_INST_79_OCARINA,
    &M_BANK_0_INST_80_LEADSQUARE,
    &M_BANK_0_INST_81_LEADSAW,
    &M_BANK_0_INST_82_LEADCALLIOPE,
    &M_BANK_0_INST_83_LEAD4CHIF_SONIC,
    &M_BANK_0_INST_84_LEAD5CHAR_NINEKO,
    &M_BANK_0_INST_85_LEADVOICE,
    &M_BANK_0_INST_86_LEAD7FIFTS_ANIMATICS,
    &M_BANK_0_INST_87_LEAD8BASS_GEMS_ANALOG_SOLO,
    &M_BANK_0_INST_88_PAD1NEWAGE,
    &M_BANK_0_INST_89_PAD_WARM_PORTED_FROM_DMX,
    &M_BANK_0_INST_90_POLYSYNTH,
    &M_BANK_0_INST_91_PADCHOIR,
    &M_BANK_0_INST_92_BOWED,
    &M_BANK_0_INST_93_METALLIC,
    &M_BANK_0_INST_94_PADHALO,
    &M_BANK_0_INST_95_SWEEP,
    &M_BANK_0_INST_96_FX_RAIN,
    &M_BANK_0_INST_97_FXSNDTRACK_THUNDERFORCE_IV,
    &M_BANK_0_INST_98_FXCRYSTAL_ALISIA_DRAGOON,
    &M_BANK_0_INST_99_ATMOSPHERE_ALISIA_DRAGOON,
    &M_BANK_0_INST_100_BRIGHTNESS,
    &M_BANK_0_INST_101_GOBLINS,
    &M_BANK_0_INST_102_ECHOS,
    &M_BANK_0_INST_103_SCIFI,
    &M_BANK_0_INST_104_SITAR,
    &M_BANK_0_INST_105_BANJO,
    &M_BANK_0_INST_106_SHAMISEN,
    &M_BANK_0_INST_107_KOTO,
    &M_BANK_0_INST_108_KALIMBA,
    &M_BANK_0_INST_109_BAGPIPE,
    &M_BANK_0_INST_110_FIDDLE,
    &M_BANK_0_INST_111_SHANAI_ALADDIN,
    &M_BANK_0_INST_112_TINKLEBELL,
    &M_BANK_0_INST_113_AGOGO,
    &M_BANK_0_INST_114_STEELDRUMS,
    &M_BANK_0_INST_115_WOODBLOCK,
    &M_BANK_0_INST_116_TAIKO_DRUM,
    &M_BANK_0_INST_117_MELODIC_TOM,
    &M_BANK_0_INST_118_SYNDRUM_ALISIA_DRAGOON,
    &M_BANK_0_INST_119_REVERSE_CYMBELL,
    &M_BANK_0_INST_120_GUITAR_FRET_NOISE,
    &M_BANK_0_INST_121_BREATH_NOISE,
    &M_BANK_0_INST_122_SEASHORE,
    &M_BANK_0_INST_123_BIRDTWEET,
    &M_BANK_0_INST_124_TELEPHONE,
    &M_BANK_0_INST_125_HELICOPTER,
    &M_BANK_0_INST_126_APPLAUSE,
    &M_BANK_0_INST_127_GUNSHOT
};

/*static const PercussionPreset P_BANK_0_INST_0
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_1
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_2
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_3
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_4
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_5
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_6
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_7
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_8
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_9
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_10
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_11
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_12
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_13
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_14
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_15
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_16
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_17
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_18
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_19
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_20
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_21
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_22
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_23
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_24
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_25
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_26
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_27
    = { { 4, 1, 3, 0, 0, 0, 0,
  0, 0, 18, 0, 0, 0, 0, 29, 12, 0, 0,
  1, 0, 31, 1, 0, 0, 0, 16, 9, 0, 0,
  0, 0, 18, 0, 0, 0, 0, 29, 12, 0, 0,
  1, 0, 31, 1, 0, 0, 0, 16, 9, 1, 0 } },
    30 };

static const PercussionPreset P_BANK_0_INST_28
    = { { 5, 7, 3, 0, 5, 0, 0,
  3, 3, 31, 3, 6, 0, 15, 16, 3, 4, 0,
  1, 0, 31, 2, 16, 0, 15, 31, 8, 0, 0,
  1, 0, 31, 3, 18, 0, 15, 14, 9, 0, 0,
  1, 0, 31, 2, 16, 0, 7, 14, 6, 0, 0 } },
    49 };

static const PercussionPreset P_BANK_0_INST_29
    = { { 4, 7, 3, 3, 7, 0, 0,
  0, 0, 31, 0, 11, 0, 0, 10, 6, 8, 0,
  0, 0, 15, 0, 14, 1, 1, 16, 8, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    59 };

static const PercussionPreset P_BANK_0_INST_30_CASTANETS
    = { { 4, 3, 3, 0, 0, 0, 0,
  9, 0, 31, 0, 11, 0, 15, 0, 15, 23, 0,
  4, 0, 31, 2, 20, 0, 15, 0, 15, 13, 0,
  1, 0, 31, 0, 19, 0, 15, 0, 15, 15, 0,
  2, 0, 31, 2, 20, 0, 15, 0, 15, 13, 0 } },
    62 };

static const PercussionPreset P_BANK_0_INST_31_STICKS
    = { { 0, 3, 3, 0, 0, 0, 0,
  2, 0, 31, 0, 20, 0, 15, 0, 15, 10, 0,
  2, 0, 31, 2, 20, 0, 15, 0, 15, 42, 0,
  2, 0, 31, 0, 18, 0, 15, 0, 15, 23, 0,
  1, 0, 31, 0, 21, 0, 15, 0, 15, 0, 0 } },
    54 };

static const PercussionPreset P_BANK_0_INST_32_SQUARE_CLICK
    = { { 7, 3, 3, 0, 0, 0, 0,
  1, 0, 31, 0, 23, 0, 15, 0, 15, 39, 0,
  1, 0, 31, 2, 23, 0, 15, 0, 15, 39, 0,
  1, 0, 31, 0, 22, 0, 15, 0, 15, 0, 0,
  1, 0, 18, 0, 24, 0, 15, 0, 15, 0, 0 } },
    75 };

static const PercussionPreset P_BANK_0_INST_33_METRONOMECLICK
    = { { 0, 3, 3, 0, 0, 0, 0,
  2, 0, 31, 0, 18, 0, 15, 0, 15, 6, 0,
  2, 0, 31, 2, 22, 0, 15, 0, 15, 33, 0,
  1, 0, 31, 0, 19, 0, 15, 0, 15, 5, 0,
  1, 0, 31, 0, 21, 0, 15, 0, 15, 0, 0 } },
    43 };

static const PercussionPreset P_BANK_0_INST_34
    = { { 5, 7, 3, 0, 5, 0, 0,
  3, 3, 31, 3, 6, 0, 15, 16, 3, 4, 0,
  1, 0, 31, 2, 16, 0, 15, 31, 8, 0, 0,
  1, 0, 31, 3, 18, 0, 15, 14, 9, 0, 0,
  1, 0, 31, 2, 16, 0, 7, 15, 8, 0, 0 } },
    46 };

static const PercussionPreset P_BANK_0_INST_35_BASSDRUM_
    = { { 2, 4, 3, 0, 0, 0, 0,
  0, 0, 27, 0, 0, 0, 0, 20, 7, 0, 0,
  6, 0, 0, 0, 15, 0, 15, 12, 6, 127, 0,
  5, 0, 0, 0, 11, 0, 15, 15, 5, 127, 0,
  1, 0, 31, 1, 0, 0, 0, 14, 7, 0, 0 } },
    35 };

static const PercussionPreset P_BANK_0_INST_36_BASSDRUM_
    = { { 2, 3, 3, 0, 0, 0, 0,
  0, 0, 26, 0, 0, 0, 0, 20, 7, 0, 0,
  6, 0, 0, 0, 15, 0, 15, 13, 6, 127, 0,
  5, 0, 0, 0, 11, 0, 15, 15, 4, 127, 0,
  1, 0, 31, 1, 0, 0, 0, 12, 7, 0, 0 } },
    35 };

static const PercussionPreset P_BANK_0_INST_37_STICKSIDE
    = { { 0, 3, 3, 0, 0, 0, 0,
  2, 0, 31, 0, 19, 0, 15, 0, 15, 21, 0,
  2, 0, 31, 2, 20, 0, 15, 0, 15, 33, 0,
  1, 0, 31, 0, 19, 0, 15, 0, 15, 14, 0,
  1, 0, 31, 0, 20, 0, 15, 0, 15, 0, 0 } },
    44 };

static const PercussionPreset P_BANK_0_INST_38_ACUSTICSNARE_ACTION52
    = { { 5, 6, 3, 0, 5, 0, 0,
  3, 3, 31, 3, 6, 0, 15, 16, 3, 4, 0,
  1, 0, 31, 2, 16, 0, 15, 31, 8, 0, 0,
  1, 0, 31, 3, 18, 0, 15, 14, 9, 0, 0,
  1, 0, 31, 2, 16, 0, 7, 13, 6, 0, 0 } },
    39 };

static const PercussionPreset P_BANK_0_INST_39_HANDCLAP_OPL3
    = { { 4, 7, 3, 0, 0, 0, 0,
  15, 0, 31, 2, 0, 0, 0, 0, 5, 13, 0,
  15, 0, 31, 0, 17, 0, 2, 15, 7, 27, 0,
  0, 0, 31, 0, 20, 0, 0, 16, 8, 3, 0,
  1, 0, 31, 0, 3, 0, 0, 18, 9, 0, 0 } },
    58 };

static const PercussionPreset P_BANK_0_INST_40_ACUSTICSNARE_ACTION52
    = { { 5, 6, 3, 0, 5, 0, 0,
  3, 3, 31, 3, 5, 0, 15, 15, 3, 4, 0,
  1, 0, 31, 2, 16, 0, 15, 31, 7, 0, 0,
  1, 0, 31, 3, 20, 0, 15, 14, 9, 0, 0,
  1, 0, 31, 2, 16, 0, 7, 12, 6, 0, 0 } },
    39 };

static const PercussionPreset P_BANK_0_INST_41_LOW_FLOOR_TOM
    = { { 5, 7, 3, 0, 0, 0, 0,
  1, 3, 30, 0, 26, 0, 15, 3, 6, 21, 0,
  0, 7, 30, 0, 17, 0, 15, 3, 6, 0, 0,
  3, 1, 30, 1, 12, 0, 15, 1, 6, 3, 0,
  0, 1, 30, 0, 17, 0, 15, 3, 6, 3, 0 } },
    20 };

static const PercussionPreset P_BANK_0_INST_42_CLOSED_HAT
    = { { 2, 7, 3, 3, 3, 0, 0,
  15, 3, 31, 0, 14, 1, 8, 13, 4, 25, 0,
  15, 7, 31, 0, 14, 1, 9, 13, 4, 20, 0,
  3, 3, 31, 1, 10, 0, 6, 8, 4, 0, 0,
  1, 7, 31, 3, 9, 0, 2, 9, 5, 0, 0 } },
    70 };

static const PercussionPreset P_BANK_0_INST_43_HIGH_FLOOR_TOM
    = { { 5, 7, 3, 0, 0, 0, 0,
  1, 3, 30, 0, 26, 0, 15, 3, 6, 22, 0,
  0, 7, 30, 0, 17, 0, 15, 14, 6, 0, 0,
  4, 1, 30, 1, 12, 0, 15, 1, 6, 3, 0,
  0, 1, 30, 0, 17, 0, 15, 9, 6, 3, 0 } },
    20 };

static const PercussionPreset P_BANK_0_INST_44_PEDALHIHAT
    = { { 0, 7, 3, 0, 0, 0, 0,
  12, 3, 7, 1, 10, 0, 11, 28, 4, 0, 0,
  10, 7, 3, 3, 10, 1, 11, 16, 7, 0, 0,
  3, 1, 31, 3, 0, 0, 0, 0, 4, 0, 0,
  5, 7, 4, 3, 13, 0, 5, 29, 9, 7, 0 } },
    100 };

static const PercussionPreset P_BANK_0_INST_45_LOW_TOM
    = { { 5, 7, 3, 0, 0, 0, 0,
  1, 3, 30, 0, 26, 0, 15, 3, 6, 22, 0,
  0, 7, 30, 0, 17, 0, 15, 3, 6, 0, 0,
  4, 1, 30, 1, 12, 0, 15, 1, 6, 3, 0,
  0, 1, 30, 0, 17, 0, 15, 3, 6, 3, 0 } },
    24 };

static const PercussionPreset P_BANK_0_INST_46_OPENHIHAT
    = { { 4, 5, 3, 0, 0, 0, 0,
  9, 0, 31, 0, 14, 0, 1, 0, 0, 0, 0,
  2, 0, 31, 0, 23, 0, 4, 8, 4, 8, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    69 };

static const PercussionPreset P_BANK_0_INST_47_LOWMED_TOM
    = { { 5, 7, 3, 0, 0, 0, 0,
  0, 3, 30, 0, 26, 0, 15, 3, 6, 22, 0,
  0, 7, 30, 0, 17, 0, 15, 3, 6, 0, 0,
  4, 1, 30, 1, 12, 0, 15, 1, 6, 3, 0,
  0, 1, 30, 0, 17, 0, 15, 3, 6, 3, 0 } },
    29 };

static const PercussionPreset P_BANK_0_INST_48_HIGHMED_TOM
    = { { 5, 7, 3, 0, 0, 0, 0,
  0, 3, 30, 0, 24, 0, 15, 19, 6, 22, 0,
  0, 7, 30, 0, 17, 0, 15, 3, 6, 0, 0,
  4, 1, 30, 1, 12, 0, 15, 1, 6, 3, 0,
  0, 1, 30, 0, 17, 0, 15, 3, 6, 3, 0 } },
    33 };

static const PercussionPreset P_BANK_0_INST_49_CRASH_ANIMATICS
    = { { 4, 7, 3, 1, 0, 0, 0,
  15, 7, 31, 0, 11, 0, 4, 0, 0, 0, 0,
  3, 1, 31, 0, 11, 1, 15, 0, 6, 0, 0,
  1, 7, 31, 0, 22, 0, 3, 7, 4, 0, 0,
  7, 0, 31, 2, 27, 0, 10, 7, 6, 0, 0 } },
    68 };

static const PercussionPreset P_BANK_0_INST_50_HIGH_TOM_
    = { { 5, 7, 3, 0, 0, 0, 0,
  1, 3, 30, 0, 26, 0, 15, 3, 6, 22, 0,
  0, 7, 30, 0, 17, 0, 15, 3, 6, 0, 0,
  4, 1, 30, 1, 12, 0, 15, 1, 6, 3, 0,
  0, 1, 30, 0, 17, 0, 15, 3, 6, 3, 0 } },
    36 };

static const PercussionPreset P_BANK_0_INST_51_RIDECYMBELL1
    = { { 4, 5, 3, 0, 0, 0, 0,
  12, 0, 31, 0, 13, 0, 1, 0, 0, 0, 0,
  3, 0, 24, 0, 21, 0, 4, 7, 4, 12, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    76 };

static const PercussionPreset P_BANK_0_INST_52_CHINESE_CRASH_ANIMATICS
    = { { 4, 5, 3, 0, 0, 0, 0,
  15, 7, 31, 0, 7, 0, 5, 0, 2, 0, 0,
  3, 1, 31, 0, 12, 0, 8, 4, 4, 0, 0,
  1, 7, 31, 0, 21, 0, 3, 6, 3, 0, 0,
  7, 0, 31, 2, 25, 0, 10, 6, 3, 0, 0 } },
    65 };

static const PercussionPreset P_BANK_0_INST_53_RIDE_BELL
    = { { 4, 3, 3, 0, 0, 0, 0,
  13, 0, 31, 0, 7, 0, 14, 7, 1, 0, 0,
  13, 0, 31, 0, 10, 0, 14, 18, 5, 17, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    76 };

static const PercussionPreset P_BANK_0_INST_54_TAMBORINE
    = { { 1, 7, 3, 3, 5, 0, 0,
  2, 2, 31, 1, 5, 0, 8, 2, 1, 6, 0,
  4, 0, 31, 2, 5, 0, 5, 2, 3, 1, 0,
  5, 7, 22, 2, 15, 0, 10, 12, 1, 0, 0,
  14, 3, 18, 2, 13, 0, 10, 13, 6, 8, 0 } },
    61 };

static const PercussionPreset P_BANK_0_INST_55_SPLASH_CRASH_ANIMATICS
    = { { 4, 5, 3, 0, 0, 0, 0,
  15, 7, 31, 0, 5, 0, 5, 6, 0, 0, 0,
  3, 1, 31, 0, 11, 0, 8, 4, 5, 0, 0,
  1, 7, 31, 0, 21, 0, 3, 6, 3, 0, 0,
  7, 0, 31, 2, 26, 0, 10, 6, 5, 0, 0 } },
    84 };

static const PercussionPreset P_BANK_0_INST_56_COWBELL
    = { { 4, 0, 3, 0, 0, 0, 0,
  2, 0, 31, 0, 18, 0, 13, 17, 8, 16, 0,
  7, 0, 31, 0, 16, 0, 7, 16, 5, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    64 };

static const PercussionPreset P_BANK_0_INST_57_CRASH_2_ANIMATICS
    = { { 4, 5, 3, 0, 0, 0, 0,
  15, 7, 31, 0, 0, 0, 5, 0, 0, 0, 0,
  3, 1, 31, 0, 10, 0, 8, 4, 4, 0, 0,
  1, 7, 31, 0, 20, 0, 3, 6, 3, 0, 0,
  7, 0, 31, 2, 25, 0, 10, 6, 4, 0, 0 } },
    70 };

static const PercussionPreset P_BANK_0_INST_58_VIBESLAP
    = { { 4, 6, 3, 0, 0, 0, 0,
  0, 0, 31, 0, 0, 0, 0, 0, 0, 8, 0,
  0, 0, 31, 0, 10, 0, 15, 0, 5, 0, 0,
  0, 0, 31, 0, 5, 0, 0, 0, 0, 0, 0,
  0, 0, 31, 0, 10, 0, 15, 0, 5, 21, 0 } },
    28 };

static const PercussionPreset P_BANK_0_INST_59_RIDECYMBELL2
    = { { 4, 5, 3, 0, 0, 0, 0,
  12, 0, 31, 0, 12, 0, 1, 0, 0, 0, 0,
  3, 0, 24, 0, 20, 0, 4, 8, 4, 11, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    81 };

static const PercussionPreset P_BANK_0_INST_60_HIBO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 28, 2, 15, 0, 15, 0, 9, 5, 0,
  0, 0, 28, 2, 20, 0, 15, 0, 9, 0, 0,
  2, 7, 23, 0, 18, 0, 15, 10, 9, 0, 0,
  3, 3, 18, 0, 18, 0, 15, 10, 9, 4, 0 } },
    65 };

static const PercussionPreset P_BANK_0_INST_61_LOBO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 28, 2, 15, 0, 15, 0, 9, 5, 0,
  0, 0, 28, 2, 20, 0, 15, 0, 9, 0, 0,
  2, 7, 23, 0, 18, 0, 15, 10, 9, 0, 0,
  3, 3, 18, 0, 18, 0, 15, 10, 9, 4, 0 } },
    60 };

static const PercussionPreset P_BANK_0_INST_62_MUHICO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 31, 2, 15, 0, 15, 0, 9, 7, 0,
  1, 0, 31, 2, 18, 0, 15, 0, 9, 0, 0,
  2, 7, 30, 0, 18, 0, 15, 10, 9, 0, 0,
  2, 1, 18, 0, 22, 0, 15, 10, 10, 0, 0 } },
    44 };

static const PercussionPreset P_BANK_0_INST_63_OPHICO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 28, 2, 15, 0, 15, 0, 9, 5, 0,
  0, 0, 28, 2, 23, 0, 15, 0, 10, 0, 0,
  1, 7, 23, 0, 20, 0, 15, 12, 11, 0, 0,
  1, 1, 18, 0, 16, 0, 15, 12, 8, 0, 0 } },
    62 };

static const PercussionPreset P_BANK_0_INST_64_LOWCO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 28, 2, 15, 0, 15, 0, 9, 5, 0,
  0, 0, 28, 2, 22, 0, 15, 0, 10, 0, 0,
  2, 7, 23, 0, 20, 0, 15, 10, 9, 13, 0,
  1, 3, 18, 0, 15, 0, 15, 10, 8, 6, 0 } },
    56 };

static const PercussionPreset P_BANK_0_INST_65_HIGHTIMBALE
    = { { 2, 5, 3, 0, 5, 0, 0,
  3, 1, 31, 2, 25, 0, 12, 8, 6, 6, 0,
  6, 3, 31, 1, 25, 0, 10, 6, 6, 5, 0,
  0, 0, 31, 1, 19, 0, 11, 5, 12, 13, 0,
  2, 0, 31, 1, 15, 0, 8, 17, 6, 0, 0 } },
    67 };

static const PercussionPreset P_BANK_0_INST_66_LOWTIMBALE
    = { { 2, 5, 3, 0, 5, 0, 0,
  3, 1, 31, 2, 25, 0, 12, 8, 10, 6, 0,
  6, 3, 31, 1, 25, 0, 10, 6, 12, 5, 0,
  0, 0, 31, 1, 19, 0, 11, 5, 12, 10, 0,
  2, 0, 31, 1, 15, 0, 8, 17, 12, 0, 0 } },
    60 };

static const PercussionPreset P_BANK_0_INST_67_HIAGOGO
    = { { 4, 7, 3, 0, 0, 0, 0,
  3, 0, 31, 2, 16, 0, 8, 16, 6, 13, 0,
  12, 0, 31, 0, 12, 0, 10, 12, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    58 };

static const PercussionPreset P_BANK_0_INST_68_LOAGOGO
    = { { 4, 7, 3, 0, 0, 0, 0,
  3, 0, 31, 2, 16, 0, 8, 16, 6, 13, 0,
  12, 0, 31, 0, 12, 0, 10, 12, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    53 };

static const PercussionPreset P_BANK_0_INST_69_CABASA
    = { { 1, 6, 3, 3, 5, 0, 0,
  15, 2, 31, 1, 5, 0, 8, 2, 1, 6, 0,
  15, 0, 31, 2, 5, 0, 5, 2, 3, 1, 0,
  13, 7, 25, 2, 5, 0, 7, 2, 1, 2, 0,
  3, 3, 13, 2, 17, 0, 10, 11, 9, 0, 0 } },
    67 };

static const PercussionPreset P_BANK_0_INST_70_MARACAS
    = { { 4, 7, 3, 0, 0, 0, 0,
  15, 0, 24, 1, 14, 0, 4, 24, 9, 0, 0,
  2, 0, 14, 2, 22, 0, 8, 25, 13, 0, 0,
  15, 0, 24, 3, 16, 0, 4, 18, 9, 0, 0,
  15, 0, 18, 0, 18, 0, 6, 18, 9, 31, 0 } },
    71 };

static const PercussionPreset P_BANK_0_INST_71_SHORTWHISTLE
    = { { 4, 0, 3, 0, 0, 0, 0,
  0, 0, 18, 0, 10, 0, 0, 0, 11, 37, 0,
  14, 0, 18, 0, 11, 0, 2, 22, 11, 0, 0,
  0, 0, 0, 0, 0, 0, 15, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 15, 0, 0, 127, 0 } },
    55 };

static const PercussionPreset P_BANK_0_INST_72_LONGWHISTLE
    = { { 4, 0, 3, 0, 0, 0, 0,
  0, 0, 18, 0, 10, 0, 0, 0, 11, 37, 0,
  14, 0, 18, 0, 8, 0, 2, 22, 11, 0, 0,
  0, 0, 0, 0, 0, 0, 15, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 15, 0, 0, 127, 0 } },
    51 };

static const PercussionPreset P_BANK_0_INST_73_SHORT_GUIRO
    = { { 4, 7, 3, 0, 0, 0, 0,
  0, 0, 31, 0, 11, 0, 0, 10, 6, 0, 0,
  0, 0, 15, 0, 14, 0, 1, 16, 8, 2, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    62 };

static const PercussionPreset P_BANK_0_INST_74_LONG_GUIRO
    = { { 4, 7, 3, 0, 0, 0, 0,
  0, 0, 31, 0, 9, 0, 0, 9, 6, 0, 0,
  0, 0, 12, 0, 14, 0, 1, 16, 8, 7, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    62 };

static const PercussionPreset P_BANK_0_INST_75_WOODBLOCK
    = { { 4, 7, 3, 0, 0, 0, 0,
  7, 1, 31, 0, 22, 0, 12, 22, 8, 20, 0,
  5, 1, 31, 0, 18, 0, 11, 28, 8, 0, 0,
  14, 1, 31, 3, 25, 0, 14, 12, 8, 10, 0,
  9, 1, 31, 3, 20, 0, 1, 13, 8, 5, 0 } },
    69 };

static const PercussionPreset P_BANK_0_INST_76_WOODBLOCK
    = { { 4, 7, 3, 0, 0, 0, 0,
  7, 1, 31, 0, 22, 0, 12, 22, 8, 20, 0,
  5, 1, 31, 0, 18, 0, 11, 28, 8, 0, 0,
  14, 1, 31, 3, 25, 0, 14, 12, 8, 10, 0,
  9, 1, 31, 3, 20, 0, 1, 13, 8, 6, 0 } },
    59 };

static const PercussionPreset P_BANK_0_INST_77_WOODBLOCK
    = { { 4, 7, 3, 0, 0, 0, 0,
  7, 1, 31, 0, 22, 0, 12, 22, 8, 20, 0,
  5, 1, 31, 0, 18, 0, 11, 28, 8, 0, 0,
  14, 1, 31, 3, 25, 0, 14, 12, 8, 10, 0,
  9, 1, 31, 3, 20, 0, 1, 13, 8, 6, 0 } },
    53 };

static const PercussionPreset P_BANK_0_INST_78_MUTE_CUIKA
    = { { 4, 7, 3, 0, 0, 0, 0,
  2, 7, 12, 0, 15, 0, 0, 15, 7, 39, 0,
  8, 7, 14, 1, 6, 0, 0, 17, 6, 0, 0,
  1, 0, 12, 0, 15, 0, 0, 15, 7, 27, 0,
  8, 0, 14, 2, 6, 0, 0, 17, 6, 0, 0 } },
    45 };

static const PercussionPreset P_BANK_0_INST_79_OPEN_CUIKA
    = { { 4, 7, 3, 0, 0, 0, 0,
  1, 0, 12, 0, 15, 0, 0, 15, 7, 38, 0,
  4, 0, 14, 0, 6, 0, 0, 17, 6, 0, 0,
  1, 0, 12, 0, 0, 0, 0, 0, 7, 30, 0,
  1, 0, 14, 0, 15, 0, 11, 14, 6, 23, 0 } },
    39 };

static const PercussionPreset P_BANK_0_INST_80_MUTETRIANGLE
    = { { 4, 4, 3, 0, 0, 0, 0,
  3, 0, 31, 0, 0, 0, 8, 0, 0, 26, 0,
  2, 0, 31, 0, 15, 0, 8, 13, 11, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    99 };

static const PercussionPreset P_BANK_0_INST_81_OPENTRIANGLE
    = { { 4, 4, 3, 0, 0, 0, 0,
  3, 0, 31, 0, 0, 0, 8, 0, 0, 26, 0,
  2, 0, 31, 0, 8, 0, 8, 8, 5, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    99 };

static const PercussionPreset P_BANK_0_INST_82_SHAKER
    = { { 4, 7, 3, 0, 0, 0, 0,
  15, 0, 24, 1, 14, 0, 4, 24, 9, 0, 0,
  2, 0, 12, 2, 22, 0, 8, 25, 13, 14, 0,
  15, 0, 24, 3, 16, 0, 4, 18, 9, 0, 0,
  15, 0, 18, 0, 18, 0, 6, 18, 9, 18, 0 } },
    71 };

static const PercussionPreset P_BANK_0_INST_83_JINGLEBELLS
    = { { 4, 7, 3, 0, 0, 0, 0,
  5, 0, 31, 0, 4, 0, 15, 15, 4, 24, 0,
  4, 0, 17, 1, 11, 0, 10, 7, 6, 9, 0,
  5, 0, 31, 0, 9, 0, 15, 16, 0, 28, 0,
  4, 0, 17, 1, 11, 0, 10, 7, 5, 14, 0 } },
    79 };

static const PercussionPreset P_BANK_0_INST_84_BELLTREE
    = { { 4, 5, 3, 0, 0, 0, 0,
  5, 0, 20, 0, 13, 0, 1, 0, 0, 9, 0,
  9, 0, 20, 0, 21, 0, 4, 8, 4, 0, 0,
  5, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0,
  8, 0, 0, 0, 0, 0, 0, 0, 0, 127, 0 } },
    83 };

static const PercussionPreset P_BANK_0_INST_85_CASTANETS
    = { { 4, 3, 3, 0, 0, 0, 0,
  9, 0, 31, 0, 11, 0, 15, 0, 15, 23, 0,
  4, 0, 31, 2, 20, 0, 15, 0, 15, 13, 0,
  1, 0, 31, 0, 19, 0, 15, 0, 15, 15, 0,
  2, 0, 31, 2, 20, 0, 15, 0, 15, 13, 0 } },
    62 };

static const PercussionPreset P_BANK_0_INST_86_MUTESURDO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 28, 2, 15, 0, 15, 0, 15, 5, 0,
  0, 0, 19, 3, 20, 0, 15, 0, 15, 0, 0,
  1, 7, 16, 0, 18, 0, 15, 10, 15, 0, 0,
  1, 3, 14, 0, 14, 0, 15, 10, 7, 0, 0 } },
    48 };

static const PercussionPreset P_BANK_0_INST_87_OPENSURDO
    = { { 6, 6, 3, 0, 0, 0, 0,
  1, 0, 28, 2, 15, 0, 15, 0, 15, 5, 0,
  0, 0, 19, 3, 20, 0, 15, 0, 15, 0, 0,
  1, 7, 16, 0, 18, 0, 15, 10, 15, 0, 0,
  1, 3, 14, 0, 11, 0, 15, 8, 5, 0, 0 } },
    41 };

static const PercussionPreset P_BANK_0_INST_88
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_89
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_90
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_91
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_92
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_93
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_94
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_95
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_96
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_97
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_98
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_99
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_100
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_101
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_102
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_103
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_104
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_105
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_106
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_107
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_108
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_109
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_110
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_111
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_112
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_113
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_114
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_115
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_116
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_117
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_118
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_119
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_120
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_121
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_122
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_123
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_124
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_125
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_126
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

static const PercussionPreset P_BANK_0_INST_127
    = { { 0, 0, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    0 };

const PercussionPreset* P_BANK_0[] = { &P_BANK_0_INST_0, &P_BANK_0_INST_1,
    &P_BANK_0_INST_2, &P_BANK_0_INST_3, &P_BANK_0_INST_4, &P_BANK_0_INST_5,
    &P_BANK_0_INST_6, &P_BANK_0_INST_7, &P_BANK_0_INST_8, &P_BANK_0_INST_9,
    &P_BANK_0_INST_10, &P_BANK_0_INST_11, &P_BANK_0_INST_12, &P_BANK_0_INST_13,
    &P_BANK_0_INST_14, &P_BANK_0_INST_15, &P_BANK_0_INST_16, &P_BANK_0_INST_17,
    &P_BANK_0_INST_18, &P_BANK_0_INST_19, &P_BANK_0_INST_20, &P_BANK_0_INST_21,
    &P_BANK_0_INST_22, &P_BANK_0_INST_23, &P_BANK_0_INST_24, &P_BANK_0_INST_25,
    &P_BANK_0_INST_26, &P_BANK_0_INST_27, &P_BANK_0_INST_28, &P_BANK_0_INST_29,
    &P_BANK_0_INST_30_CASTANETS, &P_BANK_0_INST_31_STICKS,
    &P_BANK_0_INST_32_SQUARE_CLICK, &P_BANK_0_INST_33_METRONOMECLICK,
    &P_BANK_0_INST_34, &P_BANK_0_INST_35_BASSDRUM_, &P_BANK_0_INST_36_BASSDRUM_,
    &P_BANK_0_INST_37_STICKSIDE, &P_BANK_0_INST_38_ACUSTICSNARE_ACTION52,
    &P_BANK_0_INST_39_HANDCLAP_OPL3, &P_BANK_0_INST_40_ACUSTICSNARE_ACTION52,
    &P_BANK_0_INST_41_LOW_FLOOR_TOM, &P_BANK_0_INST_42_CLOSED_HAT,
    &P_BANK_0_INST_43_HIGH_FLOOR_TOM, &P_BANK_0_INST_44_PEDALHIHAT,
    &P_BANK_0_INST_45_LOW_TOM, &P_BANK_0_INST_46_OPENHIHAT,
    &P_BANK_0_INST_47_LOWMED_TOM, &P_BANK_0_INST_48_HIGHMED_TOM,
    &P_BANK_0_INST_49_CRASH_ANIMATICS, &P_BANK_0_INST_50_HIGH_TOM_,
    &P_BANK_0_INST_51_RIDECYMBELL1, &P_BANK_0_INST_52_CHINESE_CRASH_ANIMATICS,
    &P_BANK_0_INST_53_RIDE_BELL, &P_BANK_0_INST_54_TAMBORINE,
    &P_BANK_0_INST_55_SPLASH_CRASH_ANIMATICS, &P_BANK_0_INST_56_COWBELL,
    &P_BANK_0_INST_57_CRASH_2_ANIMATICS, &P_BANK_0_INST_58_VIBESLAP,
    &P_BANK_0_INST_59_RIDECYMBELL2, &P_BANK_0_INST_60_HIBO,
    &P_BANK_0_INST_61_LOBO, &P_BANK_0_INST_62_MUHICO, &P_BANK_0_INST_63_OPHICO,
    &P_BANK_0_INST_64_LOWCO, &P_BANK_0_INST_65_HIGHTIMBALE,
    &P_BANK_0_INST_66_LOWTIMBALE, &P_BANK_0_INST_67_HIAGOGO,
    &P_BANK_0_INST_68_LOAGOGO, &P_BANK_0_INST_69_CABASA,
    &P_BANK_0_INST_70_MARACAS, &P_BANK_0_INST_71_SHORTWHISTLE,
    &P_BANK_0_INST_72_LONGWHISTLE, &P_BANK_0_INST_73_SHORT_GUIRO,
    &P_BANK_0_INST_74_LONG_GUIRO, &P_BANK_0_INST_75_WOODBLOCK,
    &P_BANK_0_INST_76_WOODBLOCK, &P_BANK_0_INST_77_WOODBLOCK,
    &P_BANK_0_INST_78_MUTE_CUIKA, &P_BANK_0_INST_79_OPEN_CUIKA,
    &P_BANK_0_INST_80_MUTETRIANGLE, &P_BANK_0_INST_81_OPENTRIANGLE,
    &P_BANK_0_INST_82_SHAKER, &P_BANK_0_INST_83_JINGLEBELLS,
    &P_BANK_0_INST_84_BELLTREE, &P_BANK_0_INST_85_CASTANETS,
    &P_BANK_0_INST_86_MUTESURDO, &P_BANK_0_INST_87_OPENSURDO, &P_BANK_0_INST_88,
    &P_BANK_0_INST_89, &P_BANK_0_INST_90, &P_BANK_0_INST_91, &P_BANK_0_INST_92,
    &P_BANK_0_INST_93, &P_BANK_0_INST_94, &P_BANK_0_INST_95, &P_BANK_0_INST_96,
    &P_BANK_0_INST_97, &P_BANK_0_INST_98, &P_BANK_0_INST_99, &P_BANK_0_INST_100,
    &P_BANK_0_INST_101, &P_BANK_0_INST_102, &P_BANK_0_INST_103,
    &P_BANK_0_INST_104, &P_BANK_0_INST_105, &P_BANK_0_INST_106,
    &P_BANK_0_INST_107, &P_BANK_0_INST_108, &P_BANK_0_INST_109,
    &P_BANK_0_INST_110, &P_BANK_0_INST_111, &P_BANK_0_INST_112,
    &P_BANK_0_INST_113, &P_BANK_0_INST_114, &P_BANK_0_INST_115,
    &P_BANK_0_INST_116, &P_BANK_0_INST_117, &P_BANK_0_INST_118,
    &P_BANK_0_INST_119, &P_BANK_0_INST_120, &P_BANK_0_INST_121,
    &P_BANK_0_INST_122, &P_BANK_0_INST_123, &P_BANK_0_INST_124,
    &P_BANK_0_INST_125, &P_BANK_0_INST_126, &P_BANK_0_INST_127 };*/
