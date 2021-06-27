#include <types.h>
#include "../MDT_Version.h"

__attribute__((externally_visible))
const struct
{
    char console[16];               /* Console Name (16)*/
    char copyright[16];             /* Copyright Information (16) */
    char title_local[48];           /* Domestic Name (48) */
    char title_int[48];             /* Overseas Name (48) */
    char serial[14];                /* Serial Number (2, 12) */
    u16 checksum;                   /* Checksum (2) */
    char IOSupport[16];             /* I/O Support (16) */
    u32 rom_start;                  /* ROM Start Address (4) */
    u32 rom_end;                    /* ROM End Address (4) */
    u32 ram_start;                  /* Start of Backup RAM (4) */
    u32 ram_end;                    /* End of Backup RAM (4) */
    char sram_sig[2];               /* "RA" for save ram (2) */
    u16 sram_type;                  /* 0xF820 for save ram on odd bytes (2); 0xE020 - word;  0xF020 - even*/
    u32 sram_start;                 /* SRAM start address - normally 0x200001 (4) */
    u32 sram_end;                   /* SRAM end address - start + 2*sram_size (4) */
    char modem_support[12];         /* Modem Support (24) */
    char notes[40];                 /* Memo (40) */
    char region[16];                /* Country Support (16) */
} rom_header = {
//    "SEGA MEGADRIVE  "	Mega Drive
//    "SEGA GENESIS    "	Mega Drive
//    "SEGA 32X        "	Mega Drive + 32X
//    "SEGA EVERDRIVE  "	Mega Drive (Everdrive extensions)
//    "SEGA EVERDRIVEXX"
//          where XX reflects mapper number (MED PRO)
//          01:SMD, 02:32X, 03:10M, 05:SSF, 06:SMS, 07:SVP(Virtua Racing), 08:CD, 09:PIE(Pier Solar), 11:SMD+CD, 12:EDAPP(NES)
//    "SEGA SSF        "	Mega Drive (Mega Everdrive extensions) [for mega everdrive only]
//    "SEGA MEGAWIFI   "	Mega Drive (Mega Wifi extensions)
//    "SEGA PICO       "	SEGA Pico console for kids
//    "SEGA TERA68K    "	Tera Drive (boot from 68000 side)
//    "SEGA TERA286    "	Tera Drive (boot from x86 side)
#if (MDT_VERSION == 0)
    "SEGA SSF        ", // Mega Everdrive Pro
#elif (MDT_VERSION == 1)
    "SEGA SSF        ", // BlastEm
#else
    "SEGA GENESIS    ", // PicoDrive
#endif
    "(C)Corthax 2021 ",
#if (MDT_VERSION == 0)
    "MD.Tracker (1.0b) [MEGA EVERDRIVE PRO]          ",
#elif (MDT_VERSION == 1)
    "MD.Tracker (1.0b) [BLASTEM]                     ",
#else
    "MD.Tracker (1.0b) [PICODRIVE]                   ",
#endif
    "MD.Tracker                                      ",
//    "GM"	Game
//    "AI"	Educational Aid
//    "OS"	Boot ROM (TMSS)
//    "BR"	Boot ROM (Sega CD)
    "AI 00000000-00",
    0x0000,
//"J"	3-button controller
//"6"	6-button controller
//"0"	Master System controller
//"A"	Analog joystick
//"4"	Multitap
//"G"	Lightgun
//"L"	Activator
//"M"	Mouse
//"B"	Trackball
//"T"	Tablet
//"V"	Paddle
//"K"	Keyboard or keypad
//"R"	RS-232
//"P"	Printer
//"C"	CD-ROM (Sega CD)
//"F"	Floppy drive
//"D"	Download?
    "6               ",
    0x00000000, // ROM start
    0x0037FFFF, // ROM end (2MB) 1FFFFF
    0x00FF0000, // BRAM start
    0x00FFFFFF, // BRAM end
    "RA",       // uses SRAM
//      Saves:
//A0	No	    16-bit
//B0	No	    8-bit (even addresses)
//B8	No	    8-bit (odd addresses)
//E0	Yes	    16-bit
//F0	Yes	    8-bit (even addresses)
//F8	Yes	    8-bit (odd addresses)
//"RA", 0xE840; EEPROM type
    0xE020,     // 16 bit SRAM mode
    0x00380000, // SRAM start 200000
    0x003FFFFF, // SRAM end 27FFFF
    "            ",                             // modem
    "MUSIC TRACKER                           ", // memo
    "JUE             "                          // country
};
/*$1B0 - 'R'
$1B1 - 'A'
$1B2 :
___ D7 always 1,
___ D6 - 1 = RAM don't destroy after power off,
___ D5 - always 1,
___ D4:D3 - 00 = word sized RAM (D0-D15), 01 = not used, 10 = even byte RAM (D8-D15), 11 = odd byte RAM (D0-D7),
___ D2:D0 - always 000.
$1B3 - ' ' (0x20)
$1B4 - dword, start address of RAM
$1B8 - dword, end address of RAM
*/
