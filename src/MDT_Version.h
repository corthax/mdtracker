#ifndef MDT_VERSION_H_INCLUDED
#define MDT_VERSION_H_INCLUDED

/*
0 = Mega Everdrive Pro / BlastEm (512KB SRAM, 16-bit linear access)
1 = EDMD-v3 (64KB SRAM, 8-bit odd-byte access only)
*/

#define MDT_VERSION_MEDPRO  0 // 512k
#define MDT_VERSION_EDMDV3  1 // 32k

#define MDT_VERSION 1 // REBUILD if changed!

#if MDT_VERSION == MDT_VERSION_MEDPRO
#define SRAM_LIMIT 0x7FFFF       // 512KB linear access
#elif MDT_VERSION == MDT_VERSION_EDMDV3
#define SRAM_LIMIT 0x7FFF        // 32KB usable (64KB / 2 for odd-byte addressing)
#endif

extern const char *MDT_HEADER;

#endif
