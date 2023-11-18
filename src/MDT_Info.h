#ifndef MDT_INFO_H_INCLUDED
#define MDT_INFO_H_INCLUDED

const char *infoCommands[256];
const char *infoDescriptions[256];

void InitInfo();
static void PrintSelectedPositionInfo();
static void PrintCommandInfo(u8 id);
static void PrintInstrumentInfo(u8 id);

#endif // MDT_INFO_H_INCLUDED
