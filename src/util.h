#ifndef UTIL_H
#define UTIL_H

#include "common.h"
#include "idt.h"
#include "pmm.h"
#include "kio.h"

typedef struct {
	char c;
	int code;
} KbKey;

static unsigned char kbde[128] = {
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', 'ß', '´', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'ü', '+', '\n',	
    0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö',	'ä','\'',
	0, '<', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',  0,	
	'*',
    0,	/* Alt */
	' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
	'-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
	'+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
static unsigned int ramAmount;

//void memset(void* ptr, unsigned char off, unsigned long long length);
char* trim(char* str);
void memcpy(char *dest, const char *src, unsigned int len);
int strlen(char* str);
unsigned int pow(unsigned int base, unsigned int pot);
unsigned int hex2dec(unsigned int hex);
void interpret_command(char* str);
void keyboardHandler(bool released, KbKey key);
bool strcmp(char* str1,char* str2);
bool strStart(char* str1,char* str2);
char scancodeToChar(int scancode);
int scancodeToCode(int scancode);
unsigned int amountOfRam();

#endif
