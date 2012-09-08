#ifndef COMMON_H
#define COMMON_H

typedef enum {false=0,true} bool;

#include "vmm.h"

static unsigned char *videoram = (unsigned char *) 0xb8000;
static char command[1024];
static int command_offset;


typedef char *va_list;
#define _INTSIZEOF(n)    ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(ap, v)  (ap = (va_list) &v + _INTSIZEOF(v))
#define va_arg(ap, t)    (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap)       (ap = (va_list) 0)

#define NULL 0

void outb(unsigned short port, unsigned char value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);
void skprintf(char* str, char* x, ...);
void memset(void* addr, int value, unsigned long length);

#endif
