#ifndef MDT_PCM_H_INCLUDED
#define MDT_PCM_H_INCLUDED

#define SAMPLE_DATA_SIZE    8   // 3 bytes start, 3 bytes end, 1 byte loop, 1 byte rate
//#define DAC_DATA_START       0x00027700 // sample_bank_1
#define DAC_DATA_END        0x0020FA00 // see symbols.txt for sgdk_logo_palette_data

/**
 *  \brief
 *      Read PCM region data.
 *
 *  \param bank
 *      Sample bank.
 *  \param note
 *      Note used for playback.
 *  \param byteNum
 *      Data to read, possible values are:<br>
 *      - 0, 1, 2: region start bytes.<br>
 *      - 3, 4, 5: region end bytes.<br>
 *      - 6: loop.<br>
 */
static u32 ReadSampleRegionSRAM(u8 bank, u8 note, u8 byteNum);

/**
 *  \brief
 *      Write PCM region data.
 *
 *  \param bank
 *      Sample bank.
 *  \param note
 *      Note used for playback.
 *  \param byteNum
 *      Data to write, possible values are:<br>
 *      - 0, 1, 2: region start bytes.<br>
 *      - 3, 4, 5: region end bytes.<br>
 *      - 6: loop.<br>
 *  \param data
 *      Value to write.
 */
static void WriteSampleRegionSRAM(u8 bank, u8 note, u8 byteNum, u8 data);

#endif // MDT_PCM_H_INCLUDED
