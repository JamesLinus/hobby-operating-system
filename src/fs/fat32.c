#include "fat32.h"

#define BytePerSector 512
bpb hdd[4];
unsigned int fatOffset;

unsigned int nextCluster(struct fs_node* node, unsigned int cluster){
	fat32_inode* fsNode = ((fat32_inode*)node->fs_inode);
	unsigned char sector[513];
	unsigned int fat_sector = (cluster * sizeof(unsigned int)) / 512;
	unsigned int fat_offset = (cluster * sizeof(unsigned int)) % 512;
	unsigned int value;
 
	unsigned int x = fat_sector+fatOffset+hdd[0].reservedSectorCount;
	read_sector_lba28(fsNode->drive,fsNode->addr,1,x,&sector);
	unsigned char *buf = &sector;
	
	value = *((unsigned int*)&buf[fat_offset]);
	
	if (value > 0xffffff5) {
		value |= 0xf0000000;
	}
 
	return value;
 }

unsigned int read_fat32(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer){
	fat32_inode* fsNode = ((fat32_inode*)node->fs_inode);
	fat32* f = (fat32*)(&hdd[0].extended);
	unsigned int sectorOffset = offset/512;										//nth sector in cluster (0-7)
	unsigned int insideSectorOffset = offset%512;								//position inside sector
	unsigned int clusterOffset = offset/(hdd[0].sectorPerCluster*512);			//nth cluster in files cluster chain
	
	if ( node->length < size )
		size = node->length; /* avoid that underflow. */ 
	if ( node->length < offset+size){
		if (offset > node->length) return 0;
		else size = node->length-offset;
	}
	if (size<=0) return 0;
	
	unsigned int nextC = fsNode->cluster;
	//kprintf("Reading from off=%d... cluster=%d sector=%d insideSector=%d size=%d\n",offset,clusterOffset,sectorOffset,insideSectorOffset,size);
	while (clusterOffset>0){
		nextC = nextCluster(node,nextC);
		if (nextC == -1){
			buffer = NULL;
			return 0;
		}
		clusterOffset--;
	}
	unsigned int sectorNr = getFirstSector(0,nextC);
	char sector[512];
	read_sector_lba28(fsNode->drive,fsNode->addr,1,sectorNr+sectorOffset,(unsigned char*)&sector);
	unsigned int i = insideSectorOffset;
	unsigned int x = 0;
	while ((i<512) && (i-insideSectorOffset)<size){
		buffer[x++] = sector[i++];
	}
	//kprintf("");
	return size;
}

unsigned int write_fat32(struct fs_node* node, unsigned int offset, unsigned int size, unsigned char* buffer){
	return 0;
}

void open_fat32(struct fs_node* node){
}

void close_fat32(struct fs_node* node){
}

struct dirent* readdir_fat32(struct fs_node* node,unsigned int index){
	char sector[513];
	char fileName[128];
	fat32_inode* fsNode = (fat32_inode*)(node->fs_inode);
	read_sector_lba28(fsNode->drive,fsNode->addr,1,fsNode->sector,(unsigned char*)&sector);
	unsigned int seq = -1;
	unsigned int idx = 0;
	bool isLFN = false;
	for(int i = 0;i<BytePerSector;i+=32){
		fat32_dir *fd = (fat32_dir*)&sector[i];
		if (fd->fileName[0]==0xE5 || fd->fileName[0]==0x00) { 
			continue;
		}
		else if (fd->fileName[0]==0x00) { 
			i = BytePerSector; //end
			break;
		}
		else {
			if (fd->attr == 0x0F){
				if (seq == -1){
					seq = fd->fileName[0]-0x40;
					fileName[seq*13]='\0';
					isLFN = true;
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
				if (!isLFN){
					//kprintf("not lfn!\n");
					int x;
					for(x = 7;x>=0;x--){
						if (fd->fileName[x]!=' ') break;
					}
					x++;
					memcpy(fileName,fd->fileName,x);
					if (fd->attr!=0x10){
						fileName[x] = '.';
						fileName[x+1]=fd->fileName[8];
						fileName[x+2]=fd->fileName[9];
						fileName[x+3]=fd->fileName[10];
						fileName[x+4]='\0';
					} 
					else  fileName[x] = '\0';
					isLFN = false;
				}
				if (fd->attr == 0x10){ 	//dir
					if (idx == index && fileName[0] != '\0'){
						//kprintf("[DIR %x %s]\n",fd->attr,fileName);
						dirent *d = (dirent*)malloc(sizeof(dirent));
						memcpy(d->name,fileName,128);
						d->ino = 0;
						return d;
					}
					idx++;
				}
				else {
					if (idx == index && fileName[0] != '\0'){
						//kprintf("[FILE %x %s]\n",fd->attr,fileName);
						dirent *d = (dirent*)malloc(sizeof(dirent));
						memcpy(d->name,fileName,128);
						d->ino = 0;
						return d;
					}
					idx++;
				}
				seq = -1;
			}
		}
	}
	return NULL;
}

unsigned int getFirstSector(int i, unsigned int clusterNumber) {
	/*fat32* f = (fat32*)(&hdd[i].extended);
	bpb* b = (bpb*)&hdd[i];
	unsigned int sector = b->reservedSectorCount+b->numberofFATs*b->FATsize_F16+b->rootEntryCount*sizeof(fat32_dir)/b->bytesPerSector;
	unsigned int newSector = sector+(clusterNumber-2)*b->sectorPerCluster*b->bytesPerSector;
	kprintf("getting %d sector %d\n",newSector,clusterNumber);
	return newSector;*/
	fat32* f = (fat32*)(&hdd[i].extended);
	unsigned int firstDataSector = fatOffset + hdd[i].hiddenSectors + hdd[i].reservedSectorCount + (hdd[i].numberofFATs * f->FATsize_F32);
	unsigned int newSector = (((clusterNumber - 2) * hdd[i].sectorPerCluster) + firstDataSector);
	return newSector;
}

struct fs_node* finddir_fat32(struct fs_node* node,char *name){
	char sector[513];
	char fileName[128];
	fat32_inode* fsNode = ((fat32_inode*)node->fs_inode);
	read_sector_lba28(fsNode->drive,fsNode->addr,1,fsNode->sector,(unsigned char*)&sector);
	//vread_sector_lba28(fsNode->drive,fsNode->addr,1,fsNode->sector,(unsigned char*)&sector);
	unsigned int seq = -1;
	bool isLFN = false;
	for(int i = 0;i<BytePerSector;i+=32){
		fat32_dir *fd = (fat32_dir*)&sector[i];
		if (fd->fileName[0]==0xE5 || fd->fileName[0]==0x05) { 
			continue;
		}
		else if (fd->fileName[0]==0x00) { 
			i = BytePerSector; //end
			break;
		}
		else {
			if (fd->attr == 0x0F){
				if (seq == -1){
					seq = fd->fileName[0]-0x40;
					fileName[seq*13]='\0';
					isLFN = true;
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
				if (!isLFN){
					//kprintf("not lfn!\n");
					int x;
					for(x = 7;x>=0;x--){
						if (fd->fileName[x]!=' ') break;
					}
					x++;
					memcpy(fileName,fd->fileName,x);
					if (fd->attr!=0x10){
						fileName[x] = '.';
						fileName[x+1]=fd->fileName[8];
						fileName[x+2]=fd->fileName[9];
						fileName[x+3]=fd->fileName[10];
						fileName[x+4]='\0';
					} 
					else  fileName[x] = '\0';
					isLFN = false;
				}
				if (fd->attr == 0x10){
					if (fileName[0] != '\0'){
						if (strcmp(fileName,name)){
							unsigned int newSector = getFirstSector(0,fd->firstClusterNumber*10+fd->lowFirstCluster);
							
							fs_node* new_node = (fs_node*)malloc(sizeof(fs_node));
							new_node->mask = new_node->uid = new_node->gid = new_node->inode = new_node->length = 0;
							new_node->flags = FS_DIRECTORY;
							new_node->read = 0;
							new_node->write = 0;
							new_node->open = 0;
							new_node->close = 0;
							new_node->readdir = &readdir_fat32;
							new_node->finddir = &finddir_fat32;
							new_node->ptr = 0;
							new_node->impl = 0;
							memcpy(new_node->name,fileName,128);
							new_node->fs_inode = (fat32_inode*)malloc(sizeof(fat32_inode));
							unsigned int offset = ((fat32_inode*)node->fs_inode)->offset;
							unsigned int addr = ((fat32_inode*)node->fs_inode)->addr;
							unsigned int drive = ((fat32_inode*)node->fs_inode)->drive;
							((fat32_inode*)new_node->fs_inode)->offset = offset;
							((fat32_inode*)new_node->fs_inode)->addr = addr;
							((fat32_inode*)new_node->fs_inode)->sector = newSector;
							((fat32_inode*)new_node->fs_inode)->cluster = fd->firstClusterNumber*10+fd->lowFirstCluster;
							((fat32_inode*)new_node->fs_inode)->drive = drive;
							/*char sector1[513];
							char fileName1[255];
							isLFN = false;
							seq = -1;
							read_sector_lba28(drive,addr,1,newSector,(unsigned char*)&sector1);
							for(int i1 = 0;i1<BytePerSector;i1+=32){
								fat32_dir *fd1 = (fat32_dir*)&sector1[i1];
								if (fd1->fileName[0]==0xE5 || fd1->fileName[0]==0x05) { 
									//skip
									continue;
								}
								else if (fd1->fileName[0]==0x00) { 
									i = BytePerSector; //end
									break;
								}
								else {
									//---
									if (fd1->attr == 0x0F){
										kprintf("part of lfn\n");
										if (seq == -1){
											seq = fd1->fileName[0]-0x40;
											fileName1[seq*13]='\0';
											isLFN = true;
										}
										kprintf("seq => %d %d %c\n",seq, fd1->fileName[0],fd1->fileName[0]);
										seq--;
										kprintf("seq => %d\n",seq);
										int nr = 0;
										for(int j = 1;j<=9;j+=2)
											fileName1[seq*13+(nr++)] = (&sector1[i1])[j];
										
										for(int j = 0;j<=10;j+=2)
											fileName1[seq*13+(nr++)] = (&sector1[i1])[j+0x0E];
										
										fileName1[seq*13+(nr++)] = (&sector1[i1])[0x1C];
										fileName1[seq*13+(nr++)] = (&sector1[i1	])[2+0x1C];
									}
									else{
										if (!isLFN){
											kprintf("just a short name!\n");
											int x;
											for(x = 7;x>=0;x--){
												if (fd1->fileName[x]!=' ') break;
											}
											x++;
											memcpy(fileName1,fd1->fileName,x);
											fileName1[x] = '.';
											fileName1[x+1]=fd1->fileName[8];
											fileName1[x+2]=fd1->fileName[9];
											fileName1[x+3]=fd1->fileName[10];
											fileName1[x+4]='\0';
											isLFN = false;
										}
										else kprintf("end of lfn\n");
										kprintf("%x %s\n\n",fd1->attr,fileName1);
										seq = -1;
									}
									//---
									}
								}
								while(1==1){}*/
								return new_node;
						}
					//	else kprintf("%s %s\n",fileName,name);
					}
				}else {
					if (strcmp(fileName,name)){
						unsigned int newSector = getFirstSector(0,fd->firstClusterNumber*10+fd->lowFirstCluster);
						//kprintf("sector %d\n",newSector);
						
						fs_node* new_node = (fs_node*)malloc(sizeof(fs_node));
						new_node->mask = new_node->uid = new_node->gid = new_node->inode = 0;
						new_node->flags = FS_FILE;
						new_node->length = fd->fileSize;
						new_node->read = &read_fat32;
						new_node->write = 0;
						new_node->open = 0;
						new_node->close = 0;
						new_node->readdir = 0;
						new_node->finddir = 0;
						new_node->ptr = 0;
						new_node->impl = 0;
						memcpy(new_node->name,fileName,128);
						new_node->fs_inode = malloc(sizeof(fat32_inode));
						unsigned int offset = ((fat32_inode*)node->fs_inode)->offset;
						unsigned int addr = ((fat32_inode*)node->fs_inode)->addr;
						unsigned int drive = ((fat32_inode*)node->fs_inode)->drive;
						((fat32_inode*)new_node->fs_inode)->offset = offset;
						((fat32_inode*)new_node->fs_inode)->addr = addr;
						((fat32_inode*)new_node->fs_inode)->sector = newSector;
						((fat32_inode*)new_node->fs_inode)->cluster = fd->firstClusterNumber*10+fd->lowFirstCluster;
						((fat32_inode*)new_node->fs_inode)->drive = drive;
						fat32_inode* fsInode = (fat32_inode*)(new_node->fs_inode);
						return new_node;
					}
				//	else kprintf("%s %s\n",fileName,name);
				}
				seq = -1;
			}
		}
	}
	return NULL;
}


/*
void fat32_readDir(dir* d){
	char sector[513];
	char fileName[256];
	int seq = -1;
	read_sector_lba28(d->mp->drive,d->mp->addr,1,d->sector,(unsigned char*)&sector);
	for(int i = 0;i<BytePerSector;i+=32){
		fat32_dir *fd = (fat32_dir*)&sector[i];
		if (fd->fileName[0]==0xE5 || fd->fileName[0]==0x05) { 
			//skip
		}
		else if (fd->fileName[0]==0x00) { 
			i = BytePerSector; //end
		}
		else {
			if (fd->attr == 0x0F){
				if (seq == -1){
					seq = fd->fileName[0]-0x40;
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
				if (fd->attr == 0x10){
					if (fileName[0] != '\0'){
						unsigned int newSector =getFirstSector(0,fd->firstClusterNumber*10+fd->lowFirstCluster);
						//fat32_readAsDir(driveNum,drive,addr,newSector,level+1);
						dir* newDir = (dir*)malloc(sizeof(dir));
						newDir->next = NULL;
						newDir->mp = d->mp;
						newDir->subFile = NULL;
						newDir->subDir = NULL;
						skprintf(newDir->name,fd->fileName);
						newDir->sector = newSector;
					}
				}
				else {
					file* f=(file*)malloc(sizeof(file));
					unsigned int newSector =getFirstSector(0,fd->firstClusterNumber*10+fd->lowFirstCluster);
					skprintf(f->name,fileName);
					f->addr = (void*)newSector;
					f->next = NULL;
					if (d->subFile == NULL) d->subFile = f;
					else {
						file* r = d->subFile;
						while (r->next != NULL) { r = r->next; }
						r->next = f;
					}
				}
				seq = -1;
			}
		}
	}
}
*/

void listDir(struct fs_node* nodeDir,int level){
	int i = 0;
	struct dirent* node = 0;
	while ((node=readdir_fs(nodeDir,i))!=0){
		fs_node* fsnode = finddir_fs(nodeDir,node->name);
		for(int i = 0;i<level;i++) kprintf("   "); 
		if (fsnode != NULL){
			if ((fsnode->flags&0x7)==FS_DIRECTORY){
				kprintf("[%s]\n",trim(fsnode->name));
				
				if (fsnode->name[0]!='.')
					listDir(fsnode,level+1);
			}
			else if ((fsnode->flags&0x1)==FS_FILE){
				kprintf("%s\n",fsnode->name);
				char buf[256];
				kprintf("----\n");
				unsigned x = 0;
				while(1==1){
					unsigned int sz = read_fs(fsnode,0+x,256,buf);
					x+=256;
					if (sz != 0){
						for(unsigned int i = 0;i<sz;i++)
							kprint(buf[i]);
					}
					else break;
				}
				kprintf("\n----\n");
			}
			//kprintf("\n");
		}
		i++;
	}
}

fs_node* fat32_init(unsigned int drive, unsigned int addr, unsigned int offset){
	int driveNum = 0; 
	fatOffset = offset;
	read_sector_lba28(drive,addr,sizeof(hdd[driveNum]),offset,(unsigned char*)&hdd[driveNum]);
	fat32* f = (fat32*)(&hdd[driveNum].extended);
	bpb* b = (bpb*)&hdd[driveNum];
	unsigned int firstSector = getFirstSector(0,f->rootCluster);

	//unsigned char* sector = (unsigned char*)malloc(hdd[driveNum].bytesPerSector);
	fs_node* fat32_root = (fs_node*)malloc(sizeof(fs_node));
	skprintf(fat32_root->name, "fat32");
	fat32_root->mask = fat32_root->uid = fat32_root->gid = fat32_root->inode = fat32_root->length = 0;
	fat32_root->flags = FS_DIRECTORY;
	fat32_root->read = 0;
	fat32_root->write = 0;
	fat32_root->open = 0;
	fat32_root->close = 0;
	fat32_root->readdir = &readdir_fat32;
	fat32_root->finddir = &finddir_fat32;
	fat32_root->ptr = 0;
	fat32_root->impl = 0;
	skprintf(fat32_root->name,"FS");
	fat32_root->fs_inode= malloc(sizeof(fat32_inode));
	((fat32_inode*)fat32_root->fs_inode)->drive = drive;
	((fat32_inode*)fat32_root->fs_inode)->addr = addr;
	((fat32_inode*)fat32_root->fs_inode)->offset = offset;
	((fat32_inode*)fat32_root->fs_inode)->sector = firstSector;
	
	/*fs_node* fat32_dev = (fs_node*)malloc(sizeof(fs_node));
	skprintf(fat32_dev->name, "dev");
	fat32_dev->mask = fat32_dev->uid = fat32_dev->gid = fat32_dev->inode = fat32_dev->length = 0;
	fat32_dev->flags = FS_DIRECTORY;
	fat32_dev->read = 0;
	fat32_dev->write = 0;
	fat32_dev->open = 0;
	fat32_dev->close = 0;
	fat32_dev->readdir = &readdir_fat32;
	fat32_dev->finddir = &finddir_fat32;
	fat32_dev->ptr = 0;
	fat32_dev->impl = 0; */
	
	listDir(fat32_root,0);

	return fat32_root;
}