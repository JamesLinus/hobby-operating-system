#ifndef KIO_H
#define KIO_H

#include "common.h"
#include "util.h"


#define Black 0x00
#define Blue 0x01
#define Green 	0x02
#define Cyan 0x03
#define Red 	0x04
#define Magenta 0x05
#define Brown 0x06
#define Gray 0x07
#define White 0x0F
#define Yellow 0x0E
#define LightMagenta 0x0D
#define LightRed 0x0C
#define LightCyan 0x0B
#define LightGreen 0x0A
#define LightBlue 0x09
#define DarkGray 0x08

static char color;
static int line_x = 0;
static int line_y = 0;

static bool debugging = false;

bool kIsDebug();
void kDebug(bool debug);
void kStatus(char* name, char* status, bool good);
void kColorReset();
void kinit();
void kColor(int background, int foreground);
void update_cursor();
int amountOfSpecialChars(char* str);
void kclear();
void scroll();
void kprint(char c);
void kprintInt(char filler, int amount, int x);
void kprintHex(char filler, int amount, int num);
void kprintHexShort(char filler, int amount,unsigned int num);
void kprintFloat(char filler, int amount, float x);
void kprintf(char* str,...);
void kBackspace();

#endif
