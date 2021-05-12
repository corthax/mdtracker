#ifndef MDT_SRAM_H_INCLUDED
#define MDT_SRAM_H_INCLUDED

void SRAMW_writeWord(u32 offset, u16 val);
void SRAMW_writeByte(u32 offset, u8 val);
u16 SRAMW_readWord(u32 offset);
u8 SRAMW_readByte(u32 offset);

// SRAM
static u8 SRAM_ReadInstrument(u8 id, u16 param);
void SRAM_WriteInstrument(u8 id, u16 param, u8 data);

static u8 SRAM_ReadPattern(u16 id, u8 line, u8 param);
void SRAM_WritePattern(u16 id, u8 line, u8 param, u8 data);

static u16 SRAM_ReadMatrix(u8 channel, u8 line);
void SRAM_WriteMatrix(u8 channel, u8 line, u16 data);

u8 SRAM_ReadPatternColor(u16 id);
void SRAM_WritePatternColor(u16 id, u8 color);

static s8 SRAM_ReadMatrixTranspose(u8 channel, u8 line);
void SRAM_WriteMatrixTranspose(u8 channel, u8 line, s8 transpose);

static inline u8 SRAM_ReadMatrixChannelMuted(u8 channel);
void SRAM_WriteMatrixChannelMuted(u8 channel, u8 state);

#endif // MDT_SRAM_H_INCLUDED
