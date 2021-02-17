
#define s8      char
#define s16     short
#define s32     long

#define u8      unsigned char
#define u16     unsigned short
#define u32     unsigned long
#define u64     unsigned long long

#define G_SCREEN_W 40
#define G_SCREEN_H 28
#define G_PLAN_W 64

void gInit();
void gVsync();


void gDrawString(u8 *str, u8 x, u8 y);
void gAppendChar(u8 val);
void gAppendString(u8 *str);
void gAppendHex8(u8 num);
void gAppendHex16(u16 num);
void gAppendHex32(u32 num);
void gSetPal(u8 pal);
void gCleanScreen();
