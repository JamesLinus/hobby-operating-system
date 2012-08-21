#include "tasks.h"

unsigned int* dec(unsigned int* tPtr){
	tPtr-=1;
	return tPtr;
}

void init_tasks(){
	AllTasks = 0;
	currentPID=0;
	init_pit();
}

void switch_to_user_mode() {
     __asm__ volatile("  \
     mov $0x23, %ax; \
     mov %ax, %ds; \
     mov %ax, %es; \
     mov %ax, %fs; \
     mov %ax, %gs; \
                   \
     mov %esp, %eax; \
     pushl $0x23; \
     pushl %eax; \
     pushf; \
     pushl $0x1B; \
     push $1f; \
     iret; \
   1: \
     ");
} 

unsigned int task_scheduler (unsigned int stack){
	if (AllTasks>0){
		//kprintf("stack: %d %x\n",stack,stack);
		Tasks[currentPID].stack = stack;
		//if (currentPID==2) while(1==1){}
		//kprintf("\n(%d/",currentPID);
		do {
			currentPID++;
			if (currentPID > AllTasks)
				currentPID = 0;
		} while (Tasks[currentPID].status != TASK_READY);
		//currentPID=1-currentPID;
		//kprintf("%d)",currentPID);
		return Tasks[currentPID].stack;
	} else kprintf("NO TASKING YET\n");
//	return 0x0;
}

void task_create (unsigned char priority, unsigned long* address,bool cli){
	
	unsigned int* stack;
	Tasks[AllTasks].stack = (unsigned int*)malloc(0x1000)+1024;
	
	stack = Tasks[AllTasks].stack;
	
	*--stack = 0x202;       // EFLAGS
	*--stack = 0x08;        // CS
	*--stack = (unsigned int) address;       // EIP

	// pusha
	*--stack = 0;           // EDI
	*--stack = 0;           // ESI
	*--stack = 0;           // EBP
	*--stack = 0;           // NULL
	*--stack = 0;           // EBX
	*--stack = 0;           // EDX
	*--stack = 0;           // ECX
	*--stack = 0;           // EAX

	// data segments
	*--stack = 0x10;        // DS
	*--stack = 0x10;        // ES
	*--stack = 0x10;        // FS
	*--stack = 0x10;        // GS

 
    Tasks[AllTasks].status = TASK_READY;
	Tasks[AllTasks].stack = (unsigned int)stack;
	Tasks[AllTasks].PID = AllTasks;
	
	AllTasks++;
}