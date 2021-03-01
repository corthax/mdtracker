#include <genesis.h>
#include <sram.h>

#define SRAM_BASE   0x240000

u8 SRAMW_readByte(u32 offset)
{
    if (offset & 1) offset -= 2;
    return *(vu8*)(SRAM_BASE + offset);
}

void SRAMW_writeByte(u32 offset, u8 val)
{
    if (offset & 1) offset -= 2;
    *(vu8*)(SRAM_BASE + offset) = val;
}

u16 SRAMW_readWord(u32 offset)
{
    return ((u16)SRAMW_readByte(offset) << 8) | SRAMW_readByte(offset+1);
}

void SRAMW_writeWord(u32 offset, u16 val)
{
    SRAMW_writeByte(offset, (u8)(val >> 8));
    SRAMW_writeByte(offset+1, (u8)(val & 255));
}
