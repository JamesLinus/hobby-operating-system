#include "fat32.h"

bpb hdd[4];
unsigned int fatOffset;

unsigned int getFirstSector(int i, unsigned int clusterNumber) {
	fat32* f = (fat32*)(&hdd[i].extended);
	unsigned int firstDataSector = fatOffset + hdd[i].hiddenSectors + hdd[i].reservedSectorCount + (hdd[i].numberofFATs * f->FATsize_F32);
	return (((clusterNumber - 2) * hdd[i].sectorPerCluster) + firstDataSector);
}

void fat32_readAsDir(unsigned int driveNum,unsigned int drive, unsigned int addr, unsigned int sectorNum, unsigned int level){
	char fileName[256];
	char sector[513];
	int seq = -1;
	read_sector_lba28(drive,addr,1,sectorNum,(unsigned char*)&sector);
	for(int i = 0;i<hdd[driveNum].bytesPerSector;i+=32){
		dir *d = (dir*)&sector[i];
		if (d->fileName[0]==0xE5 || d->fileName[0]==0x05) { 
			//skip
		}
		else if (d->fileName[0]==0x00) { 
			i = hdd[driveNum].bytesPerSector; //end
		}
		else {
			if (d->attr == 0x0F){
				if (seq == -1){
					seq = d->fileName[0]-0x40;
					fileName[seq*13]='\0';
				}
				seq--;
				int nr = 0;
				for(int j = 1;j<=9;j+=2)
					fileName[seq*13+(nr++)] = (&sector[i])[j];
				
				for(int j = 0;j<=10;j+=2)
					fileName[seq*13+(nr++)] = (&sector[i])[j+0x0E];
				
				fileName[seq*13+(nr++)] = (&sector[i])[0x1C];
				fileName[seq*13+(nr++)] = (&sector[i])[2+0x1C];
			}
			else{
				if (d->attr == 0x10){
					if (fileName[0] != '\0'){
						for(int x = 0;x<level;x++) kprintf("\t");
						kprintf("|> %s\n",fileName);
						unsigned int newSector =getFirstSector(driveNum,d->firstClusterNumber*10+d->lowFirstCluster);
						fat32_readAsDir(driveNum,drive,addr,newSector,level+1);
					}
				}
				else {
					for(int x = 0;x<level;x++) kprintf("\t");
					kprintf("#  %s [%d Byte]\n",fileName,d->fileSize);
				}
				seq = -1;
			}
		}
	}
}

void fat32_init(unsigned int drive,unsigned int addr, unsigned int offset){
	int driveNum = 0;
	fatOffset = offset;
	read_sector_lba28(drive,addr,sizeof(hdd[driveNum]),offset,(unsigned char*)&hdd[driveNum]);
	fat32* f = (fat32*)(&hdd[driveNum].extended);
	unsigned int firstSector = getFirstSector(0,f->rootCluster);
	//unsigned char* sector = (unsigned char*)malloc(hdd[driveNum].bytesPerSector);
	for(int s = 0;s<hdd[driveNum].sectorPerCluster;s++){
		fat32_readAsDir(driveNum,drive,addr,firstSector+s,0);
	}
}