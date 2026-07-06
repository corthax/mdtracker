#ifndef MDT_VERSION_H_INCLUDED
#define MDT_VERSION_H_INCLUDED

/*
0 = Mega Everdrive Pro / BlastEm (512KB SRAM, 16-bit linear access)
1 = EDMD-v3 (64KB SRAM, 8-bit odd-byte access only)
*/

#define MDT_VERSION_MEDPRO  0
#define MDT_VERSION_EDMDV3  1

#define MDT_VERSION 1 // REBUILD if changed!

extern const char *MDT_HEADER;

#endif
