#ifndef MDT_SRAM_H_INCLUDED
#define MDT_SRAM_H_INCLUDED

void SRAMW_writeWord(u32 offset, u16 val);
void SRAMW_writeByte(u32 offset, u8 val);
u16 SRAMW_readWord(u32 offset);
u8 SRAMW_readByte(u32 offset);

// SRAM
static u8 ReadInstrumentSRAM(u8 id, u16 param);
static void WriteInstrumentSRAM(u8 id, u16 param, u8 data);

static u8 ReadPatternSRAM(u16 id, u8 line, u8 param);
static void WritePatternSRAM(u16 id, u8 line, u8 param, u8 data);

static u16 ReadMatrixSRAM(u8 channel, u8 line);
static void WriteMatrixSRAM(u8 channel, u8 line, u16 data);

static u8 ReadPatternColorSRAM(u16 id);
static void WritePatternColorSRAM(u16 id, u8 color);

#endif // MDT_SRAM_H_INCLUDED
