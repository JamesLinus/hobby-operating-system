#include "common.h"

void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %1, %0" : : "Nd" (port), "a" (value));
}

unsigned char inb(unsigned short port) {
   unsigned char ret;
   __asm__ volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

unsigned short inw(unsigned short port) {
   unsigned short ret;
   __asm__ volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}

void skprintf(char* str, char* x, ...){
	int i = 0;
	while (x[i]!='\0') i++;
	str = malloc(sizeof(char)*i);
	i=0;
	while (x[i]!='\0')
	str[i]=x[i++];
}

void memset(void* addr, int value, unsigned long length){
	char *d = (char*)addr;
	while(length--){
		*d++=value;
	}
}