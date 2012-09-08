#include "main.h"

unsigned int initial_esp;
 
void idleLoop(){
	while (1){
		__asm__ volatile("hlt;");
	}
}
 
void loopA(){
	while(1){ 
		kprintf("A");
		for(int i = 0;i<500000;i++){}
	}
}
void loopB(){
	while(1){ 
		kprintf("B");
		for(int i = 0;i<500000;i++){}
	}
}
void loopC(){
	while(1){ 
		kprintf("C");
		for(int i = 0;i<500000;i++){}
	}
}

void kmain(multiboot_info_t* mbd ) {
	kinit();
	command_offset = 0;
	kclear();
	extern void* kernel_stack;
	//unsigned long ram = InitAmountOfRam(mbd);
	//kprintf("[ RAM: %l KB / %l MB ]\n",ram,ram/1024);
	__asm__ volatile("cli");
	
	init_gdt();	//	kStatus("GDT","OK",true);

	init_idt();	//	kStatus("IDT","OK",true);
	
	init_pmm(mbd);//	kStatus("PMM","OK",true);
	
	/*unsigned int* addr1 = pmm_alloc();
	kprintf("pmm_alloc %x\n",addr1);
	pmm_test();
	
	unsigned int* addr2 = pmm_alloc();
	kprintf("pmm_alloc %x\n",addr2);
	pmm_test();
	
	kprintf("pmm_free 2\n");
	pmm_free(addr2);
	pmm_test();
		
	unsigned int* addr3 = pmm_alloc();
	kprintf("pmm_alloc %x\n",addr3);
	pmm_test();

	kprintf("pmm_free 1\n");
	pmm_free(addr1);
	pmm_test();
		
	unsigned int* addr4 = pmm_alloc();
	kprintf("pmm_alloc %x\n",addr4);
	
	kprintf("----------------\n");*/
	init_paging();	//	kStatus("Paging","OK",true);	
	
	init_vmm();	//	kStatus("VMM","OK",true);
	
	init_tasks();	//	kStatus("Tasks","OK",true);
	
	task_create(255,(unsigned long*)idleLoop,false);
	//task_create(255,(unsigned long*)loopA,false);
	
	init_hdd();		kStatus("HDD","OK",true);
	
	kprint('>');
	__asm__ volatile("sti");
	while(1) {__asm__ volatile("hlt"); }
	/*
	//
	int* time = getTime();
	int* date = getDate();
	kprintf("\n\n\n");
	kprintf("curent time: %02d:%02d:%02d\n",time[0],time[1],time[2]);
	kprintf("curent date: %02d.%02d.%04d\n",date[0],date[1],date[2]);	

	kprint('>');
	kprintf("Kernel Mode ...\n");
	switch_to_user_mode();
	//kprintf("aaaa ...\n");
	
	int a; 
	int b = 1;
	__asm__ volatile("int $0x80");
	__asm__ volatile("int $0x81");
	__asm__ volatile("int $0x82");
	//__asm__ volatile("int $0x80" : "=a" (a) : "0" (b)); 
	
	
	while(1) {__asm__ volatile("hlt"); }
//	__asm__ volatile("mov $1,%eax; int $0x80");*/
}
