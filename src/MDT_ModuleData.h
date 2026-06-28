//#include "MDT_Version.h"

#ifndef MDT_MODULEDATA_H_INCLUDED
#define MDT_MODULEDATA_H_INCLUDED

// New pattern format
#define PATTERN_MAGIC            0x4D44
#define PATTERN_FORMAT_VERSION   2
#define EVT_NOTEINST            0
#define EVT_FX1                 1
#define EVT_FX2                 2
#define EVT_FX3                 3
#define EVT_FX4                 4
#define EVT_FX5                 5
#define EVT_FX6                 6
#define EVT_COUNT               7
#define EVT_RESERVED            7

//{ Instrument data param IDs (used with SRAM_ReadInstrument/SRAM_WriteInstrument)
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

// Compact instrument data contains fields 0-48 (INST_ALG through INST_SSGEG4)
#define INST_DATA_SIZE      49
#define INST_NAME_SIZE       8
#define INST_NAME_OFFSET    49   // name starts at param 49
#define INST_NAME_1         INST_NAME_OFFSET
#define INST_NAME_8         (INST_NAME_OFFSET + 7)

#define INST_RECORD_SIZE     58  // 1 byte id + 49 data bytes + 8 name bytes
#define INST_SENTINEL_MODIFIED 0xFF
#define SEQ_RECORD_SIZE      65  // 1 byte id + 32 VOL bytes + 32 ARP bytes
#define SEQ_SENTINEL_MODIFIED 0xFF

#define SEQ_STEP_LAST       31
#define PATTERN_COLUMNS     14
#ifndef PATTERN_SIZE
#define PATTERN_SIZE        448
#endif

// ========================================================
// SRAM Layout (absolute byte offsets from SRAM start)
// ========================================================
// Block 1: Static data (fixed addresses, never shifts)
#define STATIC_BASE             12    // after 6-byte header + 6 bytes padding (avoid _Odd overlap)

#define SRAM_GLOBAL_LFO         (STATIC_BASE + 0u)       // 3 bytes
#define SRAM_PATTERN_MATRIX     (STATIC_BASE + 3u)       // 6500 bytes (13ch * 250row * 2, combined patternID + transpose)
#define SRAM_TEMPO              (STATIC_BASE + 6503u)    // 2 bytes
#define STATIC_END              (STATIC_BASE + 6505u)    // = 0x1969

// Block 2: Instruments (at STATIC_END, expandable via compact records)
#define INST_BLOCK_BASE           STATIC_END              // 0x1969 = 6517
#define INST_MOD_COUNT_ADDR       INST_BLOCK_BASE         // word
#define INST_LOOKUP_TABLE_ADDR    (INST_BLOCK_BASE + 2)   // 256 bytes
#define INST_COMPACT_START        (INST_BLOCK_BASE + 258) // = 0x1A77 = 6775

// instBlockEnd = INST_COMPACT_START + modCount * INST_RECORD_SIZE

// Block 3: Sequencers (at instBlockEnd, expandable via compact records)
// SEQ_MOD_COUNT_ADDR      = instBlockEnd (word)
// SEQ_LOOKUP_TABLE_ADDR   = instBlockEnd + 2 (256 bytes)
// SEQ_COMPACT_START       = instBlockEnd + 258
// seqBlockEnd = instBlockEnd + 258 + modCount * SEQ_RECORD_SIZE

// Block 4: Patterns (at seqBlockEnd = patternRegionBase)
// PATTERN_REGION_BASE = patternRegionBase (global variable)

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

// Column to event position map (for DATA_NOTE, column 0 → event pos EVT_NOTEINST)
// Note: column 0 (DATA_NOTE) and column 1 (DATA_INSTRUMENT) share event pos 0
// FX type/value pairs share one event position each
#define COL_TO_EVT(col) (((col) <= DATA_INSTRUMENT) ? EVT_NOTEINST : (EVT_FX1 + (((col) - DATA_FX1_TYPE) >> 1)))
#define EVT_TO_COL_TYPE(evt) ((evt) == EVT_NOTEINST ? DATA_NOTE : (DATA_FX1_TYPE + ((evt) - EVT_FX1) * 2))
#define EVT_TO_COL_VALUE(evt) ((evt) == EVT_NOTEINST ? DATA_INSTRUMENT : (DATA_FX1_VALUE + ((evt) - EVT_FX1) * 2))
//}

#endif // MDT_MODULEDATA_H_INCLUDED
