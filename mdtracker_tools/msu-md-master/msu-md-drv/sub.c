
/* 
 * File:   sub.c
 * Author: Igor
 *
 * Created on Jun 22, 2020, 12:03 AM
 */

#include "sub.h"

void memSet(void *dst, u8 val, u16 len);
u32 msfToDec(u32 val);
u32 decToMsf(u32 val);
u8 bcdToDec(u8 val);
u8 decToBcd(u8 val);
void cddCmdExec(u8 cmd, u32 arg);
void cddCmdParse(u8 cmd, u32 arg);
void cddInit();
u32 cddReadToc(u32 arg);
void loopCtrl();
void fade(u8 mute_time);


McdState mstate;
CddCmd cdd_cmd;
CddCmd cdd_sta;
Toc toc;
McdRegs *mcd = (McdRegs *) 0xFF8000; //MCD Gate array registers

void msu_drv() {


    vu8 cmd_ck;
    mcd->CMD_SUB[SUB_STAT] = SUB_STAT_INIT;
    cddInit();
    mcd->LEDS = LEDS_G;

    cmd_ck = mcd->CMD_MAIN[MAIN_CMD_CK];

    while (1) {

        //wait for next cmd
        mcd->CMD_SUB[SUB_STAT] = SUB_STAT_READY;
        while (mcd->CMD_MAIN[MAIN_CMD_CK] == cmd_ck);
        cmd_ck = mcd->CMD_MAIN[MAIN_CMD_CK];
        mcd->CMD_SUB[SUB_STAT] = SUB_STAT_BUSY;

        //volume control
        if (mcd->CMD_MAIN[MAIN_CMD] == MAIN_CMD_VOL) {
            mstate.vol = (mcd->CMD_MAIN[MAIN_ARG] * 16) << 2;
            mcd->CD_FADER = mstate.vol;
            continue;
        }

        //playback control
        u8 cmd = mcd->CMD_MAIN[MAIN_CMD];

        switch (cmd) {

            case MAIN_CMD_PLAY:
            case MAIN_CMD_PLAYLP:
            case MAIN_CMD_PLAYOF:
                mstate.track = (mcd->CMD_MAIN[MAIN_ARG] - 1);
                mstate.loop_offset = toc.track_addr[mstate.track]; //MSF in BCD format (Minute,Seconds,Frames)
                cddCmdExec(CDD_CMD_PLAY, mstate.loop_offset);
                mcd->CD_FADER = mstate.vol;
                mstate.loop_mode = cmd;

                if (cmd == MAIN_CMD_PLAYOF) {
                    u32 offset = *(vu32 *) (&mcd->CMD_MAIN[MAIN_ARG32]); //number of sectors (decimal number)
                    mstate.loop_mode = MAIN_CMD_PLAYLP;
                    offset += msfToDec(mstate.loop_offset);
                    mstate.loop_offset = decToMsf(offset);
                }

                break;

            case MAIN_CMD_PAUSE:
                fade(mcd->CMD_MAIN[MAIN_ARG]);
                mcd->CD_FADER = 0;
                cddCmdExec(CDD_CMD_PAUSE, 0);
                break;

            case MAIN_CMD_RESUME:
                cddCmdExec(CDD_CMD_RESUME, 0);
                mcd->CD_FADER = mstate.vol;
                break;

            case MAIN_CMD_NOSEEK:
                cddCmdExec(CDD_CMD_NOP, 0x2A5500 | mcd->CMD_MAIN[MAIN_ARG]);
                break;
        }

    }
}

void fade(u8 mute_time) {

    if (mute_time == 0)return;


    u32 fade_vol = (u32) (mstate.vol >> 2) * 256;
    u32 fade_step = fade_vol / mute_time;
    vu8 ctr;

    while (fade_vol >= fade_step) {

        mcd->CD_FADER = (fade_vol / 256) << 2;
        fade_vol -= fade_step;
        ctr = mstate.irq_ctr;
        while (ctr == mstate.irq_ctr);
    }

}

void memSet(void *dst, u8 val, u16 len) {

    u8 *ptr = (u8 *) dst;
    while (len--)*ptr++ = val;
}

u32 msfToDec(u32 val) {

    u8 min = bcdToDec(val >> 16);
    u8 sec = bcdToDec(val >> 8);
    u8 fra = bcdToDec(val);

    return ((u32) min * 60 * 75) + ((u16) sec * 75) + fra;
}

u32 decToMsf(u32 val) {

    u8 min = decToBcd(val / 75 / 60);
    u8 sec = decToBcd(val / 75 % 60);
    u8 fra = decToBcd(val % 75);

    return ((u32) min << 16) | ((u16) sec << 8) | fra;
}

u8 bcdToDec(u8 val) {

    return (val >> 4) * 10 + (val & 15);
}

u8 decToBcd(u8 val) {

    if (val > 99)val %= 99;
    return (val / 10 << 4) | val % 10;
}

//****************************************************************************** cdd control

void cddCmdExec(u8 cmd, u32 arg) {

    while (mstate.cdd_cmd_exec); //wait if previos command isn't sent yet
    mstate.cdd_cmd_code = cmd;
    mstate.cdd_cmd_arg = arg;
    mstate.cdd_cmd_exec = 1;
}

void cddInit() {

    u16 i;
    u16 track_num;

    mcd->RST = 0; //reset cd drive
    mcd->LEDS = LEDS_G;
    memSet(&mstate, 0, sizeof (McdState));
    memSet(&toc, 0, sizeof (Toc));
    mstate.vol = VOL_MAX;
    while ((mcd->RST & RST_RES0) == 0); //wait for cd drive reset end

    mcd->IEMASK = (1 << 4); //turn on cdd irq
    mcd->CDD_CTRL = CDD_CTRL_HOCK; //enable host clock for communications with cdd


    //cd bios starts like that, so i just doing the same
    for (i = 0; i < 16; i++)cddCmdExec(CDD_CMD_NOP, 0);
    cddCmdExec(CDD_CMD_TOC, TOC_TRNUM);
    for (i = 0; i < 48; i++)cddCmdExec(CDD_CMD_NOP, 0);
    cddReadToc(TOC_TRNUM); //may be need timeout and retry here

    //get number of tracks
    toc.tracks_num = bcdToDec((cdd_sta.arg[2] << 4) | cdd_sta.arg[3]);


    //get tracks address list (in MSF format)
    track_num = 0x0100; //track number
    for (i = 0; i < toc.tracks_num; i++) {

        toc.track_addr[i] = cddReadToc(TOC_TRADDR | track_num);
        track_num = (track_num & 0xF00) == 0x900 ? track_num + 0x700 : track_num + 0x100;
    }

    toc.total_len = cddReadToc(TOC_CDLEN); //total cd len
    toc.track_addr[i] = toc.total_len; //disk total len matches to end of last track
    cddCmdExec(CDD_CMD_TOC, TOC_ABS_POS);

}

u32 cddReadToc(u32 arg) {//read disk table of content

    u16 i;
    u32 val = 0;
    u8 cmd = arg >> 16;

    cddCmdExec(CDD_CMD_TOC, arg);
    cddCmdExec(CDD_CMD_NOP, 0);

    do {
        cddCmdExec(CDD_CMD_NOP, 0);
    } while (cdd_sta.u0 != cmd);

    for (i = 0; i < 6; i++) {
        val <<= 4;
        val |= cdd_sta.arg[i];
    }

    return val;
}

void cddCmdParse(u8 cmd, u32 arg) {//prepare cdd cmd and calc checksum 

    u16 i;
    u8 *ptr8 = (u8 *) & cdd_cmd;

    cdd_cmd.cmd = cmd;
    cdd_cmd.u0 = 0;
    cdd_cmd.u1 = 0;
    cdd_cmd.arg[0] = (arg >> 20) & 15;
    cdd_cmd.arg[1] = (arg >> 16) & 15;
    cdd_cmd.arg[2] = (arg >> 12) & 15;
    cdd_cmd.arg[3] = (arg >> 8) & 15;
    cdd_cmd.arg[4] = (arg >> 4) & 15;
    cdd_cmd.arg[5] = (arg >> 0) & 15;

    cdd_cmd.crc = 0;
    for (i = 0; i < 9; i++) {

        cdd_cmd.crc += ptr8[i];
    }

    cdd_cmd.crc ^= 0x0f;
    cdd_cmd.crc &= 0x0f;
}


//****************************************************************************** cdd irq

//cdd irq comes when cdd ready for exchange. 
//CDD_STAT valid only for a while after IRQ
//CDD_CMD should be filled shortly after IRQ

void irq4() {

    u16 *staus = (u16 *) & cdd_sta;
    u16 *cmd = (u16 *) & cdd_cmd;

    while ((mcd->CDD_CTRL & CDD_CTRL_DRS) != 0);

    staus[0] = mcd->CDD_STAT[0];
    staus[1] = mcd->CDD_STAT[1];
    staus[2] = mcd->CDD_STAT[2];
    staus[3] = mcd->CDD_STAT[3];
    staus[4] = mcd->CDD_STAT[4];

    if (mstate.cdd_cmd_exec) {
        cddCmdParse(mstate.cdd_cmd_code, mstate.cdd_cmd_arg);
        mstate.cdd_cmd_exec = 0;
    } else {
        cddCmdParse(CDD_CMD_NOP, 0);
    }

    //cmdCrc();
    mcd->CDD_CMD[0] = cmd[0];
    mcd->CDD_CMD[1] = cmd[1];
    mcd->CDD_CMD[2] = cmd[2];
    mcd->CDD_CMD[3] = cmd[3];
    mcd->CDD_CMD[4] = cmd[4];

    if (cdd_sta.cmd == 1) {
        mcd->LEDS |= LEDS_R;
    } else {
        mcd->LEDS &= ~LEDS_R;
    }

    loopCtrl();

    mstate.irq_ctr++;


}

void loopCtrl() {


    u16 i;
    u32 cur_addr = 0;
    u32 track_end;

    if (cdd_sta.cmd != 1 || cdd_sta.u0 != TOC_ABS_POS >> 16) {
        return;
    }

    //get current disk address. in MSF format
    for (i = 0; i < 6; i++) {
        cur_addr <<= 4;
        cur_addr |= cdd_sta.arg[i];
    }

    track_end = toc.track_addr[mstate.track + 1];
    track_end -= 2;

    if (cur_addr >= track_end && cdd_cmd.cmd == CDD_CMD_NOP) {

        if (mstate.loop_mode == MAIN_CMD_PLAYLP) {
            cddCmdExec(CDD_CMD_PLAY, mstate.loop_offset);
        } else {
            cddCmdExec(CDD_CMD_PAUSE, 0);
        }
    }

}
