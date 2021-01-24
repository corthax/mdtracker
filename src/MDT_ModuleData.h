#ifndef MDT_MODULEDATA_H_INCLUDED
#define MDT_MODULEDATA_H_INCLUDED

#define MD_TRACKER_VERSION 2

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

#define INST_VOL_TICK_01 49
#define INST_VOL_TICK_02 50
#define INST_VOL_TICK_03 51
#define INST_VOL_TICK_04 52
#define INST_VOL_TICK_05 53
#define INST_VOL_TICK_06 54
#define INST_VOL_TICK_07 55
#define INST_VOL_TICK_08 56
#define INST_VOL_TICK_09 57
#define INST_VOL_TICK_10 58
#define INST_VOL_TICK_11 59
#define INST_VOL_TICK_12 60
#define INST_VOL_TICK_13 61
#define INST_VOL_TICK_14 62
#define INST_VOL_TICK_15 63
#define INST_VOL_TICK_16 64

#define INST_ARP_TICK_01 65
#define INST_ARP_TICK_02 66
#define INST_ARP_TICK_03 67
#define INST_ARP_TICK_04 68
#define INST_ARP_TICK_05 69
#define INST_ARP_TICK_06 70
#define INST_ARP_TICK_07 71
#define INST_ARP_TICK_08 72
#define INST_ARP_TICK_09 73
#define INST_ARP_TICK_10 74
#define INST_ARP_TICK_11 75
#define INST_ARP_TICK_12 76
#define INST_ARP_TICK_13 77
#define INST_ARP_TICK_14 78
#define INST_ARP_TICK_15 79
#define INST_ARP_TICK_16 80

#define INST_NAME_1 81
#define INST_NAME_2 82
#define INST_NAME_3 83
#define INST_NAME_4 84
#define INST_NAME_5 85
#define INST_NAME_6 86
#define INST_NAME_7 87
#define INST_NAME_8 88
//}

//{ SRAM data blocks (BYTESWAPPED!!!)
#define INSTRUMENT_DATA     0x0002 // 89 * 256 bytes
#define GLOBAL_LFO          0x5902 // INSTRUMENT_DATA +  5900h; 1 byte
#define DEAD_INSTRUMENT     0x5903 // DEAD. To check if SRAM file exists; 2 bytes
#define PATTERN_MATRIX      0x5905 // MAX_MATRIX_ROWS * 13 * 2 bytes
#define TEMPO               0x7269 // PATTERN_MATRIX + 1964h; 2 bytes
#define SAMPLE_DATA         0x726B // 4 * 96 * 7 bytes (3byte start + 3byte end + 1byte loop)
#define PATTERN_DATA        0x7CEB // SAMPLE_DATA + A80h;
//}

//{ Pattern data
#define DATA_NOTE           0
#define DATA_INSTRUMENT     1
#define DATA_FX1_TYPE       2
#define DATA_FX1_VALUE      3
#define DATA_FX2_TYPE       4
#define DATA_FX2_VALUE      5
#define DATA_FX3_TYPE       6
#define DATA_FX3_VALUE      7

#if (MD_TRACKER_VERSION == 5)
    #define DATA_FX4_TYPE       8
    #define DATA_FX4_VALUE      9
    #define DATA_FX5_TYPE       10
    #define DATA_FX5_VALUE      11
    #define DATA_FX6_TYPE       12
    #define DATA_FX6_VALUE      13
#endif
//}

#if (MD_TRACKER_VERSION == 5)   // ~80Kb sram free
    #define PATTERN_COLUMNS     14
    #define PATTERN_SIZE        448 // 32 (pattern rows) * PATTERN_COLUMNS bytes
#elif (MD_TRACKER_VERSION == 2) // 216 bytes sram free
    #define PATTERN_COLUMNS     8
    #define PATTERN_SIZE        256
#endif

#endif // MDT_MODULEDATA_H_INCLUDED
