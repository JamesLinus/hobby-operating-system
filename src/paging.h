#ifndef PAGING_H
#define PAGING_H

#include "pmm.h"
#include "vmm.h"
#include "kio.h"

#define PTE_PRESENT 0x01
#define PTE_WRITE 0x10

extern void* gdt_entry_start;
extern void* gdt_entry_end;

extern void* idt_entry_start;
extern void* idt_entry_end;

extern const void kernel_end;
	
extern void* kernel_stack;

bool paging_enabled;

typedef struct context {
	unsigned int* pagedir;
} context;

context* kernel_context;

static void* PD_START = (void*)0x3fcff000;

void map_range(char* str, void* start,void* end);
void init_paging();
void activate_context(context* ctx);
context* create_context();
int map_page(context* ctx, void* virt, void* phys);

#endif
