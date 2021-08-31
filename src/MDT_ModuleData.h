#ifndef MDT_MODULEDATA_H_INCLUDED
#define MDT_MODULEDATA_H_INCLUDED

//{ Instrument data SRAM offsets
#define INST_ALG 0 // 1 byte ..
#define INST_FMS 1
#define INST_AMS 2
#define INST_PAN 3
#define INST_FB 4

#define INST_TL1 5
#define INST_TL2 6
#define INST_TL3 7
#define INST_TL4 8

#define INST_RS1 9
#define INST_RS2 10
#define INST_RS3 11
#define INST_RS4 12

#define INST_MUL1 13
#define INST_MUL2 14
#define INST_MUL3 15
#define INST_MUL4 16

#define INST_DT1 17
#define INST_DT2 18
#define INST_DT3 19
#define INST_DT4 20

#define INST_AR1 21
#define INST_AR2 22
#define INST_AR3 23
#define INST_AR4 24

#define INST_D1R1 25
#define INST_D1R2 26
#define INST_D1R3 27
#define INST_D1R4 28

#define INST_D1L1 29
#define INST_D1L2 30
#define INST_D1L3 31
#define INST_D1L4 32

#define INST_D2R1 33
#define INST_D2R2 34
#define INST_D2R3 35
#define INST_D2R4 36

#define INST_RR1 37
#define INST_RR2 38
#define INST_RR3 39
#define INST_RR4 40

#define INST_AM1 41
#define INST_AM2 42
#define INST_AM3 43
#define INST_AM4 44

#define INST_SSGEG1 45
#define INST_SSGEG2 46
#define INST_SSGEG3 47
#define INST_SSGEG4 48

#define SEQ_STEP_LAST    31
/*#define INST_VOL_TICK_01 49
#define INST_VOL_TICK_16 64
#define INST_ARP_TICK_01 65
#define INST_ARP_TICK_16 80*/

#define INST_NAME_1 81
#define INST_NAME_8 88
//}
// sizes (byte):
// pattern data size
// 32 * 14 * 896 = 401408
// 32 * 14 * 512 = 229376
// 32 *  8 * 896 = 229376

// 32 * 10 * 512 = 163840; 199463; (62169 free) 704 patterns max
// 32 * 12 * 512 = 196608; 205231; (56913 free) 660 patterns max
// 32 *  8 * 768 = 196608
// pattern color
// 896

// ------------------------
// instruments size
// 256 * 89 = 22784

// matrix size
// 13 * 250 * 2 = 6500

// matrix channels mute
// 13

// matrix transpose
// 13 * 250 = 3250

// sample data size
// 4 * 96 * 8 = 3072
// ------------------------

// tempo/checker
// 4

// 35623

// 256K = 262144
// 512K = 524288

#define PATTERN_COLUMNS     14
#define PATTERN_SIZE        448 // 32 (pattern rows) * PATTERN_COLUMNS bytes

//{ SRAM data blocks (BYTESWAPPED!!!)
#define INSTRUMENT_DATA     0x00002 // 89 * 256 bytes
#define GLOBAL_LFO          0x05902 // INSTRUMENT_DATA +  5900h; 1 byte
#define FILE_CHECKER        0x05903 // DEAD. To check if SRAM file exists; 2 bytes
#define PATTERN_MATRIX      0x05905 // MAX_MATRIX_ROWS * 13 * 2 bytes
#define TEMPO               0x07269 // PATTERN_MATRIX + 1964h; 2 bytes
#define SAMPLE_DATA         0x0726B // 4 * 96 * 7 bytes (3byte start + 3byte end + 1byte loop)
#define PATTERN_DATA        0x07CEB // SAMPLE_DATA + A80h;
#define PATTERN_COLOR       0x6A06B // PATTERN_DATA + PATTERN_SIZE * (MAX_PATTERN + 1);
#define MATRIX_TRANSPOSE    0x6A3EC // matrix slot transpose (250*13)
#define MUTE_CHANNEL        0x6B09E // store disabled matrix channels (13)
#define SAMPLE_PAN          0x6B0AB // default sample pan (4 * 96)
#define SEQ_VOL_START       0x6B230 // 32 steps vol seq start
#define SEQ_ARP_START       0x6D230 // 32 steps arp seq start
//}

#define SAMPLE_START_1      0
#define SAMPLE_START_2      1
#define SAMPLE_START_3      2
#define SAMPLE_END_1        3
#define SAMPLE_END_2        4
#define SAMPLE_END_3        5
#define SAMPLE_LOOP         6
#define SAMPLE_RATE         7

//{ Pattern data 14 * 32 bytes
#define DATA_NOTE           0
#define DATA_INSTRUMENT     1
#define DATA_FX1_TYPE       2
#define DATA_FX1_VALUE      3
#define DATA_FX2_TYPE       4
#define DATA_FX2_VALUE      5
#define DATA_FX3_TYPE       6
#define DATA_FX3_VALUE      7
#define DATA_FX4_TYPE       8
#define DATA_FX4_VALUE      9
#define DATA_FX5_TYPE       10
#define DATA_FX5_VALUE      11
#define DATA_FX6_TYPE       12
#define DATA_FX6_VALUE      13
//}

#endif // MDT_MODULEDATA_H_INCLUDED
