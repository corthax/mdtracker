/* 
 * File:   sub.h
 * Author: igor
 *
 * Created on June 30, 2020, 7:50 PM
 */

#ifndef SUB_H
#define	SUB_H


#define u8      unsigned char
#define u16     unsigned short
#define u32     unsigned long

#define vu8      volatile unsigned char
#define vu16     volatile unsigned short
#define vu32     volatile unsigned long

#define LEDS_R          0x01
#define LEDS_G          0x02
#define RST_RES0        0x01

#define CDD_CTRL_DTS    0x01
#define CDD_CTRL_DRS    0x02
#define CDD_CTRL_HOCK   0x04


#define CDD_CMD_NOP     0x00
#define CDD_CMD_STOP    0x01
#define CDD_CMD_TOC     0x02
#define CDD_CMD_PLAY    0x03
#define CDD_CMD_SEEK    0x04
#define CDD_CMD_PAUSE   0x06
#define CDD_CMD_RESUME  0x07

#define TOC_ABS_POS     0x000000
#define TOC_REL_POS     0x010000
#define TOC_TRACK       0x020000
#define TOC_CDLEN       0x030000
#define TOC_TRNUM       0x040000
#define TOC_TRADDR      0x050000

#define MAIN_CMD        0x00
#define MAIN_ARG        0x01
#define MAIN_ARG32      0x02
#define MAIN_CMD_CK     0x0F

#define SUB_STAT        0x00

#define MAIN_CMD_PLAY   0x11    //paly and stop int the end
#define MAIN_CMD_PLAYLP 0x12    //paly and loop
#define MAIN_CMD_PAUSE  0x13    //pause
#define MAIN_CMD_RESUME 0x14    //resume
#define MAIN_CMD_VOL    0x15    //cdda volume
#define MAIN_CMD_NOSEEK 0x16    //turn off seek tile emulation. 0=emu-on(default), 1=emu-off
#define MAIN_CMD_PLAYOF 0x1A    //play cdda track and loop from specified sector offset

#define SUB_STAT_READY  0x00
#define SUB_STAT_INIT   0x01
#define SUB_STAT_BUSY   0x02

#define VOL_MAX         0x4000

typedef struct {
    vu8 LEDS;
    vu8 RST;
    vu8 MEM_WP;
    vu8 MEMMOD;
    vu8 CDC_MOD;
    vu8 CDC_RADDR;
    vu8 unused0;
    vu8 CDC_RDATA;
    vu16 CDC_HDATA;
    vu16 CDC_DADDR;
    vu16 SWATCH;
    vu8 COMF_MAIN;
    vu8 COMF_SUB;
    vu8 CMD_MAIN[16];
    vu8 CMD_SUB[16];
    vu8 unused1;
    vu8 TIMER;
    vu8 unused2;
    vu8 IEMASK;
    vu16 CD_FADER;
    vu16 CDD_CTRL;
    vu16 CDD_STAT[5];
    vu16 CDD_CMD[5];
} McdRegs;

typedef struct {
    u8 cmd;
    u8 u0;
    u8 arg[6];
    u8 u1;
    u8 crc;
} CddCmd;

typedef struct {
    u16 vol;
    u32 cdd_cmd_arg;
    u8 cdd_cmd_code;
    vu8 cdd_cmd_exec;
    vu8 irq_ctr;
    u8 track;
    u8 loop_mode;
    u32 loop_offset;
} McdState;

typedef struct {
    u32 tracks_num;
    u32 total_len;
    u32 track_addr[100 + 1];
} Toc;

#endif	/* SUB_H */

