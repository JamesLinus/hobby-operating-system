#ifndef VFS_H
#define VFS_H

#include "fat32.h"
#include "../hdd.h"

typedef struct dir {
	unsigned char fileName[8];
	unsigned char ext[3];
	unsigned char attr;
	unsigned char reserved1;
	unsigned char creationTimeSecs[5];
	unsigned short lastAccessedDate;
	unsigned short firstClusterNumber;
	unsigned char lastModification[4];
	unsigned short lowFirstCluster;
	unsigned int fileSize;
} dir;

typedef struct mountpoint {
	char name[256];
	int fsID;
	struct mountpoint* next;
} mountpoint;

mountpoint* root;

void vfs_init(unsigned int drive,unsigned int addr);

#endif