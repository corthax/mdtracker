

#include "sys.h"

u16 msu_drv();

vu16 *mcd_cmd = (vu16 *) 0xA12010;
vu32 *mcd_arg = (vu32 *) 0xA12012;
vu8 *mcd_cmd_ck = (vu8 *) 0xA1201F;
vu8 *mcd_stat = (vu8 *) 0xA12020;

int main() {

    u8 loop = 1;
    u8 track = 1;
    u16 play_cmd;
    u16 resp;
    u8 seek_delay_off = 0;
    u8 loop_offset = 0;

    sysInit();
    gSetColor(0x00, 0x000);
    gSetColor(0x0f, 0xfff);

    gCleanPlan();
    gConsPrint("           MSU-MD SAMPLE DEMO           ");
    gConsPrint("----------------------------------------");
    gConsPrint("");

    gConsPrint("load driver...");
    resp = msu_drv();
    if (resp) {
        gAppendString("err");
        gConsPrint("MCD hardware not detected");
        while (1);
    }
    gAppendString("ok");
    gConsPrint("init driver..");
    while (*mcd_stat != 1);
    gAppendString(".");
    while (*mcd_stat == 1); //wait till sub cpu finis initialization
    gAppendString("ok");


    while (1) {

        gSetXY(0, 6);
        gConsPrint("track:        ");
        gAppendNum(track);
        gAppendString(" ");
        gConsPrint("loop:         ");
        gAppendString(loop ? "ON " : "OFF");
        gConsPrint("seek delay:   ");
        gAppendString(seek_delay_off ? "OFF" : "ON ");
        gConsPrint("loop offset:  ");
        gAppendString(loop_offset == 0 ? "OFF" : "ON ");

        gSetXY(0, G_SCREEN_H - 7);
        
        gConsPrint("SWITCH TRACK (LEFT/RIGHT)");
        gConsPrint("CHANGE LOOP MODE (START)");
        gConsPrint("PALY(A) PAUSE(B) RESUME(C)");
        gConsPrint("SEEK DELAY (UP)");
        gConsPrint("LOOP OFFSET (DOWN)");
        u16 joy = sysJoyWait();

        play_cmd = loop ? (loop_offset ? 0x1A00 : 0x1200) : 0x1100;

        if (joy == JOY_L && track > 1) {
            track--;
        }

        if (joy == JOY_R && track < 99) {
            track++;
        }

        if (joy == JOY_A) {
            *mcd_cmd = play_cmd | track;
            *mcd_arg = 12 * 75; //loop offset (12 sec in this sample). Used only for cmd 0x1A00
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }

        if (joy == JOY_B) {
            *mcd_cmd = 0x1300 | 75; //one sec fading
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }

        if (joy == JOY_C) {
            *mcd_cmd = 0x1400;
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }

        if (joy == JOY_STA) {
            loop ^= 1;
        }

        if (joy == JOY_U) {

            seek_delay_off ^= 1; //0=delay on (default), 1=delay off

            *mcd_cmd = 0x1600 | (seek_delay_off & 1);
            *mcd_cmd_ck = *mcd_cmd_ck + 1;
        }

        if (joy == JOY_D) {
            loop_offset ^= 1;
        }

    }

    return 0;
}
