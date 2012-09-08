#include "vfs.h"

unsigned int read_fs(fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer){
	if (node->read != 0)
		return node->read(node,offset,size,buffer);
	return 0;
}

unsigned int write_fs(fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer){
	if (node->write != 0)
		return node->write(node,offset,size,buffer);
	return 0;
}

void open_fs(fs_node *node, unsigned char read, unsigned char write){
	if (node->open != 0)
		node->open(node);
}

void close_fs(fs_node *node){
	if (node->close != 0)
		node->close(node);
}

struct dirent *readdir_fs(fs_node *node, unsigned int index){
	if ((node->flags&0x7) == FS_DIRECTORY && node->readdir != 0) 
		return node->readdir(node,index);
	return 0;
}

fs_node *finddir_fs(fs_node *node, char *name){
	if ((node->flags&0x7) == FS_DIRECTORY && node->finddir != 0) 
		return node->finddir(node,name);
	return 0;
} 

fs_node* vfs_init_fs(unsigned int partitionID, unsigned int fsID, unsigned int drive, unsigned int addr, unsigned int offset){
	switch (fsID){
		case 0x0C:
		case 0x0B: //FAT32
				return fat32_init(drive,addr,offset);
				break;
		default: kprintf("unknown fs=%x drive=%x addr=%x partitionID=%d offset=%d\n",fsID,drive,addr,partitionID,offset);
				break;
	}
	return NULL;
}


void vfs_init(unsigned int drive,unsigned int addr){
	unsigned char sector[513];
	kprintf("reading drive=%d at addr=%x\n",drive,addr);
	read_sector_lba28(drive, addr, 1,0, &sector);
	unsigned int fsID1 = sector[0x01BE + 0x04];
	unsigned int fsID2 = sector[0x01CE + 0x04];
	unsigned int fsID3 = sector[0x01DE + 0x04];
	unsigned int fsID4 = sector[0x01EE + 0x04];
	
	unsigned int boot1 = sector[0x01BE];
	unsigned int boot2 = sector[0x01CE];
	unsigned int boot3 = sector[0x01DE];
	unsigned int boot4 = sector[0x01EE];
	
	unsigned int offset1 = hex2dec(sector[0x01BE + 0x08]*1000+sector[0x01BE + 0x09]*100+sector[0x01BE + 0x10]*10);
	unsigned int offset2 = sector[0x01CE + 0x08];
	unsigned int offset3 = sector[0x01DE + 0x08];
	unsigned int offset4 = sector[0x01EE + 0x08];
	
	if (boot1==0x80)
		vfs_init_fs(1,fsID1,drive,addr,offset1);
/*	if (boot2==0x80)
		vfs_init_fs(2,sysID2,drive,addr,offset2);
	if (boot3==0x80)
		vfs_init_fs(3,sysID3,drive,addr,offset3);
	if (boot4==0x80)
		vfs_init_fs(4,sysID4,drive,addr,offset4);*/
}