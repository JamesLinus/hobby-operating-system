#ifndef VMM_H
#define VMM_H

#include "pmm.h"
#include "kio.h"
#include "paging.h"

unsigned int memPages;

typedef struct node{
	struct node* next;
	struct node* prev;
	void* addr;
	unsigned int size;
	unsigned char flags;
} node;

node* heap;

void init_vmm();
void* malloc(unsigned int size);
void free(void* addr);
unsigned int getMemPages();

#endif
