#ifndef MDT_SRAM_H_INCLUDED
#define MDT_SRAM_H_INCLUDED

void SRAMW_writeWord(u32 offset, u16 val);
void SRAMW_writeByte(u32 offset, u8 val);
u16 SRAMW_readWord(u32 offset);
u8 SRAMW_readByte(u32 offset);

// SRAM
static u8 ReadInstrumentSRAM(u8 id, u16 param);
void WriteInstrumentSRAM(u8 id, u16 param, u8 data);

static u8 ReadPatternSRAM(u16 id, u8 line, u8 param);
void WritePatternSRAM(u16 id, u8 line, u8 param, u8 data);

static u16 ReadMatrixSRAM(u8 channel, u8 line);
void WriteMatrixSRAM(u8 channel, u8 line, u16 data);

u8 ReadPatternColorSRAM(u16 id);
void WritePatternColorSRAM(u16 id, u8 color);

static s8 ReadMatrixTransposeSRAM(u8 channel, u8 line);
void WriteMatrixTransposeSRAM(u8 channel, u8 line, s8 transpose);

static inline u8 ReadMatrixChannelMutedSRAM(u8 channel);
void WriteMatrixChannelMutedSRAM(u8 channel, u8 state);

//static inline u8 ReadPatternCommandMutedSRAM(u8 id);
//void WritePatternCommandMutedSRAM(u8 id, u8 state);

#endif // MDT_SRAM_H_INCLUDED
