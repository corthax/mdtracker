
#include "segalib.h"



#define GFX_DATA_PORT           0xC00000
#define GFX_CTRL_PORT           0xC00004
#define GFX_DATA_PORT16 *((volatile u16 *)GFX_DATA_PORT)
#define GFX_CTRL_PORT16 *((volatile u16 *)GFX_CTRL_PORT)
#define GFX_CTRL_PORT32 *((volatile u32 *)GFX_CTRL_PORT)

#define GFX_WRITE_VRAM_ADDR(adr)    ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define GFX_READ_VRAM_ADDR(adr)     ((0x0000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define GFX_WRITE_CRAM_ADDR(adr)    ((0xC000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x00)
#define GFX_WRITE_VSRAM_ADDR(adr)   ((0x4000 + ((adr) & 0x3FFF)) << 16) + (((adr) >> 14) | 0x10)

#define FONT_ADDR       1024
#define FONT_ADDR       1024
#define TILE_MEM_END    0xB000
#define WPLAN           (TILE_MEM_END + 0x0000)
#define HSCRL           (TILE_MEM_END + 0x0800)
#define SLIST           (TILE_MEM_END + 0x0C00)
#define APLAN           (TILE_MEM_END + 0x1000)
#define BPLAN           (TILE_MEM_END + 0x3000)

#define JOY_DATA *((volatile u8 *) 0xa10003)
#define JOY_CONTROL *((volatile u8 *) 0xa10009)

#define VDP_VBLANK_FLAG (1 << 3)

#define Z80_HALT_PORT       0xA11100
#define Z80_RESET_PORT      0xA11200

#define Z80_RAM             0xA00000
#define Z80_YM2612          0xA04000
#define Z80_BANK_REGISTER   0xA06000
#define Z80_PSG             0xA07F11

#define Z80_BUSREQ_ON *((volatile u16 *) Z80_HALT_PORT) = 0x0100
#define Z80_BUSREQ_OFF *((volatile u16 *) Z80_HALT_PORT) = 0x0000
#define Z80_IS_BUSREQ_OFF (*((volatile u16 *) Z80_HALT_PORT) & 0x0100)

#define Z80_RESET_ON *((volatile u16 *) Z80_RESET_PORT) = 0
#define Z80_RESET_OFF *((volatile u16 *) Z80_RESET_PORT) = 0x0100


#define VDP_SET_REG(reg, val)(GFX_CTRL_PORT16 =  0x8000 | (reg << 8) | val)

/*
void gSetVdpReg(u8 reg, u16 val) {
    GFX_CTRL_PORT16 = 0x8000 | (reg << 8) | val;
}*/

void gSetPaletteColor(u16 palnum, u16 colornum, u16 val) {

    u16 addr;
    addr = (palnum * 32) + (colornum * 2);
    GFX_CTRL_PORT32 = GFX_WRITE_CRAM_ADDR(addr);
    GFX_DATA_PORT16 = val;
}

void gSetVdpReg(u8 reg, u16 val) {
    GFX_CTRL_PORT16 = 0x8000 | (reg << 8) | val;
}

void gWriteVram(u16 *src, u16 dst, u16 len) {

    len >>= 1;
    GFX_CTRL_PORT32 = GFX_WRITE_VRAM_ADDR(dst);
    while (len--)GFX_DATA_PORT16 = *src++;
}
extern u16 font_base[];

u16 g_plan;
u16 g_pal;

void gInit() {

    gSetVdpReg(15, 0x02); //autoinc
    gSetVdpReg(16, 0x11); //plan size 64x64

    gSetVdpReg(0, 0x06);
    gSetVdpReg(1, 0x44);

    gSetPaletteColor(0, 0, 0x000);
    gSetPaletteColor(0, 15, 0x555);
    gSetPaletteColor(1, 15, 0xfff);

    gWriteVram(font_base, FONT_ADDR, 4096);
    g_plan = APLAN;
    g_pal = 0;

}

void gVsync() {

    u16 vdp_state;
    vdp_state = VDP_VBLANK_FLAG;
    while (vdp_state & VDP_VBLANK_FLAG) vdp_state = GFX_CTRL_PORT16;
    while (!(vdp_state & VDP_VBLANK_FLAG)) vdp_state = GFX_CTRL_PORT16;
}

void gDrawString(u8 *str, u8 x, u8 y) {

    u16 dst = g_plan + x * 2 + y * G_PLAN_W * 2;
    GFX_CTRL_PORT32 = GFX_WRITE_VRAM_ADDR(dst);
    gAppendString(str);
}

void gAppendChar(u8 val) {
    GFX_DATA_PORT16 = val | g_pal;
}

void gAppendString(u8 *str) {

    while (*str != 0)GFX_DATA_PORT16 = *str++ | g_pal;
}

void gAppendHex8(u8 num) {

    u8 val;
    u8 buff[3];
    buff[2] = 0;

    val = num >> 4;
    if (val > 9)val += 7;
    buff[0] = val + '0';
    val = num & 0x0f;
    if (val > 9)val += 7;
    buff[1] = val + '0';

    gAppendString(buff);
}

void gAppendHex16(u16 num) {
    gAppendHex8(num >> 8);
    gAppendHex8(num & 0xff);
}

void gAppendHex32(u32 num) {
    gAppendHex16(num >> 16);
    gAppendHex16(num & 0xffff);
}

void gSetPal(u8 pal) {

    g_pal = pal << 13;
}

void gCleanScreen() {

    u16 i;
    GFX_CTRL_PORT32 = GFX_WRITE_VRAM_ADDR(APLAN);
    for (i = 0; i < G_PLAN_W * 28; i++)GFX_DATA_PORT16 = 0;
    GFX_CTRL_PORT32 = GFX_WRITE_VRAM_ADDR(BPLAN);
    for (i = 0; i < G_PLAN_W * 28; i++)GFX_DATA_PORT16 = 0;
}
