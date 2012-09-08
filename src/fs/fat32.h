#ifndef FAT32_H
#define FAT32_H

#include "vfs.h"

typedef struct fat32_dir {
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
} fat32_dir;

typedef struct fat12_16{
	unsigned char number;
	unsigned char reserved_1;
	unsigned char boot;
	unsigned int fsID;
	unsigned char fsName[11];
	unsigned char fsType[8];
}__attribute__((packed)) fat_12_16;

typedef struct fat32 {
	unsigned long FATsize_F32;      //count of sectors occupied by one FAT
	unsigned short  extFlags;
	unsigned short  FSversion;        //0x0000 (defines version 0.0)
	unsigned long rootCluster;      //first cluster of root directory (=2)
	unsigned short  FSinfo;           //sector number of FSinfo structure (=1)
	unsigned short  BackupBootSector;
	unsigned char reserved[12];
	unsigned char driveNumber;
	unsigned char reserved1;
	unsigned char bootSignature;
	unsigned long volumeID;
	unsigned char volumeLabel[11];   //"NO NAME "
	unsigned char fileSystemType[8]; //"FAT32"
	unsigned char bootData[420];
	unsigned short  bootEndSignature;  //0xaa55
}__attribute__((packed)) fat32;

typedef struct bpb {
	unsigned char jumpBoot[3];     //default: 0x009000EB
	unsigned char OEMName[8];
	unsigned short bytesPerSector;   //default: 512
	unsigned char sectorPerCluster;
	unsigned short  reservedSectorCount;
	unsigned char numberofFATs;
	unsigned short  rootEntryCount;
	unsigned short  totalSectors_F16; //must be 0 for FAT32
	unsigned char mediaType;
	unsigned short  FATsize_F16;      //must be 0 for FAT32
	unsigned short  sectorsPerTrack;
	unsigned short  numberofHeads;
	unsigned long hiddenSectors;
	unsigned long totalSectors_F32;
	unsigned char extended[54];
}__attribute__((packed)) bpb;


typedef struct fat32_inode {
	unsigned int drive;
	unsigned int addr;
	unsigned int offset;
	unsigned int sector;
	unsigned int cluster;
} fat32_inode;

unsigned int getFirstSector(int i, unsigned int clusterNumber);
fat32_inode *rInode,*rootInode;
fs_node* fat32_init(unsigned int drive, unsigned int addr, unsigned int offset);

#endif