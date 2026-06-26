#include <genesis.h>
#include <sram.h>
#include "MDT_Version.h"
#include "MDT_SRAM.h"

//! change SRAM_BASE to 0x380000

//! offset must be > 1

#if MDT_VERSION == MDT_VERSION_PRO_BLASTEM

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

u8 SRAM_readByte_Odd(u32 offset)
{
    return *(vu8*)(SRAM_BASE + (offset * 2));
}

void SRAM_writeByte_Odd(u32 offset, u8 val)
{
    *(vu8*)(SRAM_BASE + (offset * 2)) = val;
}

#elif MDT_VERSION == MDT_VERSION_EDMDV3

u8 SRAMW_readByte(u32 offset)
{
    return *(vu8*)(SRAM_BASE + (offset * 2) + 1);
}

void SRAMW_writeByte(u32 offset, u8 val)
{
    *(vu8*)(SRAM_BASE + (offset * 2) + 1) = val;
}

u8 SRAM_readByte_Odd(u32 offset)
{
    return *(vu8*)(SRAM_BASE + (offset * 2) + 1);
}

void SRAM_writeByte_Odd(u32 offset, u8 val)
{
    *(vu8*)(SRAM_BASE + (offset * 2) + 1) = val;
}

#endif

u8 SRAM_readByte_Even(u32 offset)
{
    return *(vu8*)(SRAM_BASE + (offset * 2));
}

void SRAM_writeByte_Even(u32 offset, u8 val)
{
    *(vu8*)(SRAM_BASE + (offset * 2)) = val;
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
