#include "hdd.h"

void check_partitions(int val){
	outb(val+6,0xA0);
	for(int i = 0;i<1000;i++) {}
	int x = inb(val+7);
	if (x & 0x40) { 
		vfs_init(0xA0,val);		
	}
	
	outb(val+6,0xB0);
	for(int i = 0;i<1000;i++) {}
	int x2 = inb(val+7);
	if (x2 & 0x40) { 
		vfs_init(0xB0,val);		
	}
}

void init_hdd(){
	outb(0x1F3,0x88);
	if (inb(0x1F3)==0x88){
		check_partitions(0x1F0);
	}
		
	outb(0x173,0x88);
	if (inb(0x173)==0x88){
		check_partitions(0x170);
	}
}

void read_sector_lba28(unsigned short drive, unsigned int base, unsigned char sectorCount, unsigned int lba, unsigned char* buffer){
	while((inb(base+7) & 0x80) == 0x80);

	outb(base+6,0x40 | (drive << 4) | ((lba >> 24) & 0x0F));
	outb(base+1,0x00);
	outb(base+2,sectorCount);
	outb(base+3,(unsigned char)lba & 0xFF);
	outb(base+4,(unsigned char)(lba>>8) & 0xFF);
	outb(base+5,(unsigned char)(lba>>16) & 0xFF);
	outb(base+7,0x20);
	short actualSectors = sectorCount;
	while((inb(base+7) & 0x80) == 0x80);	
	while((inb(base+7) & 0x0F) != 8);			
	for(int i = 0;i<actualSectors*256;i++){
		unsigned short tmpword = inw(base);
		buffer[i*2] = (unsigned char)tmpword;
		buffer[i*2+1] = (unsigned char)(tmpword>>8);
	}
}


char* read_sector_lba48(){
	unsigned int addr;
	unsigned int drive;
	/*
	outb(0x1F1,0x00);
	outb(0x1F1,0x00);
	outb(0x1F2,0x00);
	outb(0x1F2,0x01);
	outb(0x1F3,(unsigned char)(addr >> 24));
	outb(0x1F4,(unsigned char)(addr >> 32));
	outb(0x1F4,(unsigned char)(addr >> 8));
	outb(0x1F5,(unsigned char)(addr >> 40));
	outb(0x1F5,(unsigned char)(addr >> 16));
	outb(0x1F6,0x40 | (drive << 4));
	outb(0x1F7,0x24);
	while(!inb(0x1F7) & 0x08){}   //wait to be ready!
	unsigned char buffer[512];
	for(int i = 0;i<256;i++){
		unsigned short tmpword = inw(0x1F0);
		buffer[i*2] = (unsigned char)tmpword;
		buffer[i*2+1] = (unsigned char)(tmpword>>8);
	}
	return buffer;*/ 
	
}