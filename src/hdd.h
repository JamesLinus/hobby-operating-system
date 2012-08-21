#ifndef HDD_H
#define HDD_H

#include "common.h"
#include "kio.h"
#include "fs/fat32.h"

void init_hdd();
void read_sector_lba28(unsigned short drive, unsigned int base, unsigned char sectorCount, unsigned int lba, unsigned char* buffer);

#endif