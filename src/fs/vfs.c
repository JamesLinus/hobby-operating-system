#include "vfs.h"



//void vfs_mount_at(int drive,

void vfs_init_tree(){
	root = (mountpoint*)malloc(sizeof(mountpoint));
	root->next = NULL;
	skprintf(root->name,"...");
	root->fsID = 0x00;
}

void vfs_init_fs(unsigned int partitionID,unsigned int fsID,unsigned int drive,unsigned int addr,unsigned int offset){
	switch (fsID){
		case 0x0C:	//fat32
		case 0x0B:	
					kprintf("found fat32 on drive %d, partition %d\n",drive==0xA0?1:2,partitionID);
					fat32_init(drive,addr,offset);	
					break;
	}	
}

void vfs_init(unsigned int drive, unsigned int addr){
	unsigned char sector[513];
	read_sector_lba28(drive, addr, 1,0, &sector);
	unsigned int sysID1 = sector[0x01BE + 0x04];
	unsigned int sysID2 = sector[0x01CE + 0x04];
	unsigned int sysID3 = sector[0x01DE + 0x04];
	unsigned int sysID4 = sector[0x01EE + 0x04];
	
	unsigned int lba1 = hex2dec(sector[0x01BE + 0x08]*1000+sector[0x01BE + 0x09]*100+sector[0x01BE + 0x10]*10);
	unsigned int lba2 = sector[0x01CE + 0x08];
	unsigned int lba3 = sector[0x01DE + 0x08];
	unsigned int lba4 = sector[0x01EE + 0x08];
	
	vfs_init_fs(1,sysID1,drive,addr,lba1);
	vfs_init_fs(2,sysID2,drive,addr,lba2);
	vfs_init_fs(3,sysID3,drive,addr,lba3);
	vfs_init_fs(4,sysID4,drive,addr,lba4);
}

void vfs_readDir(char* name){
	//fat32_readDir(name);
}