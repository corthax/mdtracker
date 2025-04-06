#include <genesis.h>
#include <sram.h>
#include "MDT_Version.h"
#include "MDT_SRAM.h"

//! change SRAM_BASE to 0x380000

//! offset must be > 1

u8 SRAMW_readByte(u32 offset)
{
    #if (MDT_VERSION == 0 || MDT_VERSION == 1 || MDT_VERSION == 2)

    if (offset & 1) offset -= 2;
    return *(vu8*)(SRAM_BASE + offset);

    #elif (MDT_VERSION == 3)

    //return SRAM_readByte(offset);
    return SRAM_readByte_Odd(offset);

    /*if (offset & 1) offset -= 2;
    return *(vu8*)(SRAM_BASE + offset);*/

    #endif
}

// default is even
u8 SRAM_readByte_Odd(u32 offset)
{
    return *(vu8*)(SRAM_BASE + (offset * 2));
}

void SRAM_writeByte_Odd(u32 offset, u8 val)
{
    *(vu8*)(SRAM_BASE + (offset * 2)) = val;
}
// ---

void SRAMW_writeByte(u32 offset, u8 val)
{
    #if (MDT_VERSION == 0 || MDT_VERSION == 1 || MDT_VERSION == 2)

    if (offset & 1) offset -= 2;
    *(vu8*)(SRAM_BASE + offset) = val;

    #elif (MDT_VERSION == 3)

    /*if (offset & 1) offset -= 2;
    *(vu8*)(SRAM_BASE + offset) = val;*/

    //SRAM_writeByte(offset, val);
    SRAM_writeByte_Odd(offset, val);

    #endif
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
