#include "vmm.h"

#define THRESHOLD 100
#define EMPTY 0
#define USED 1

unsigned int getMemPages(){ 
	return memPages;
}

void init_vmm(){
	kernel_context->pagedir = 0x400000;
	map_page(kernel_context,heap,heap);
	heap->addr = (unsigned long)heap+sizeof(node);
	heap->size = 0x1000-sizeof(node);
	heap->flags = EMPTY;
	heap->next = NULL;
	heap->prev = NULL;
}

void* malloc(unsigned int size){
	node* current = heap;
	//kprintf(">> looking for %d bytes\n",size);
	do {
		//kprintf("block[%x]",current);
		//kprintf(" size: %d\n",current->size);
		if (current->size>=size){
			//kprintf(" | => enough, checking ");
			if (current->flags == EMPTY){
				//kprintf(" | using\n");
				//need to make block smaller
				//kprintf("found block with %d, wanting %d ",current->size,size);
				if (current->size-size > sizeof(node)){
					unsigned int sz = current->size;
					struct node* n = ((void*)current)+sizeof(node)+size;
					//kprintf("making current=%x / %x + %d new at %x\n",current,current->addr,sizeof(node)+size,n);
					n->flags = EMPTY;
					n->prev = current;
					n->next = current->next;
					current->next = n;
					n->size = current->size-size-sizeof(node);
					current->size = size;
					n->addr = (unsigned long)n+sizeof(node);
					n->flags = EMPTY;
					current->flags = USED;
					//kprintf("giving out %x\n",current->addr);
				}
				return current->addr;
			} //else kprintf("\n");
		}
		//else kprintf("\n");
		if (current->next == NULL) break;
		else current = current->next;
	}while (current != NULL);
	unsigned int nr = size/(0x1000-sizeof(node));
	if (nr*(0x1000-sizeof(node)) < size) nr++;
	void* i1= pmm_alloc(); nr--; memPages++;
	map_page(kernel_context,i1,i1);
	//adding new node to the end
	struct node* n = i1;
	current->next = n;
	n->prev = current;
	n->next =NULL;
	n->size = 0x1000-sizeof(node);
	n->addr = (unsigned long)i1+sizeof(node);
	n->flags = EMPTY;		
	//sizing it up!
	for(unsigned int i = 0;i<nr;i++){
		void* i1= pmm_alloc(); memPages++;
		map_page(kernel_context,i1,i1);
		n->size += 0x1000;		
	}
	return malloc(size);
}

void free(void* addr){
}
