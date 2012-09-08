#include "kio.h"
#include "util.h"

/*void memset(void* ptr, unsigned char off, unsigned long long length){
	unsigned long long val = (off & 0xFF);
	val |= ((val << 8) & 0xFF00);
	val |= ((val << 16) & 0xFFFF0000);
	val |= ((val << 32) & 0xFFFFFFFF00000000);
	
	while (length>=8){ *(unsigned long long*)ptr = (unsigned long long)val; ptr+=8; length-=8; }
	if (length>=4){ *(unsigned int*)ptr = (unsigned int)val; ptr+=4; length-=4; }
	if (length>=2){ *(unsigned short*)ptr = (unsigned short)val; ptr+=2; length-=2; }
	if (length>=1){ *(unsigned char*)ptr = (unsigned char)val; ptr+=1; length-=1; }
	return;
}
*/
void memcpy(char *dest, const char *src, unsigned int len) {
    const char *sp = (const char *)src;
    char *dp = (char*)dest;
    for(; len != 0; len--) *dp++ = *sp++;
}

unsigned int exec(unsigned int *addr){
	int (*reosapp)(void);
	reosapp = (int (*)())addr;
	int retvalue = reosapp();
	return retvalue;
}

unsigned int pow(unsigned int base, unsigned int pot){
	unsigned int orgBase = base;
	if (pot == 0) return 1;
	pot--;
	if (pot>0){
		while (pot>0){
			base *= orgBase;
			pot--;
		}
	}
	return base;
}

unsigned int hex2dec(unsigned int hex){
	unsigned int base = 10;
	unsigned int sum = 0;
	int i = 0;
	int max = 1;
	while (base<hex)  {
		max++;
		base*=10;
	}
	base = 10;
	while (i<max){
		int x = hex%base;
		x *= pow(16,i++);
		sum+=x;
		hex-=x;
		if (hex<10) break;
		hex/=10;
	}
	return sum;
}

void keyboardHandler(bool released, KbKey key){
	if (released){
		if (key.code == 156){
			kprintf("\n");
			command[command_offset]='\0';
			interpret_command(command);
			command_offset = 0;
			kprint('>');
		}
		else if (key.code== 142){
			kBackspace();
			command_offset--;
		}
		else{
			command[command_offset++]=key.c;
			kprint(key.c);
		}
	}
}

void interpret_command(char* str){
/*	if (strcmp("mem",str)){
		pmm_printStat();
	}
	else if (strcmp("map",str)){
		pmm_printMemMap();
	}
	else if (strStart(str,"alloc")){
		char* temp1 = (char*)kmalloc(sizeof(char)*100);
		char* temp2 = (char*)kmalloc(sizeof(char)*100);
		kprintf("alloc'd two char*; length=100 each\n");
	}
	else if (strStart(str,"heap used"))
		printHeap(USED);
	else if (strStart(str,"heap empty"))
		printHeap(EMPTY);
	else*/
		kprintf("i dont know!\n");
}

int scancodeToCode(int scancode){
	return scancode;
}

char scancodeToChar(int scancode){
	return kbde[scancode%128];
}

unsigned int amountOfRam(){
	return ramAmount;
}

int strlen(char* str){
	int i = 0;
	while (str[i]!='\0') i++;
	return i;
}

bool strcmp(char* str1,char* str2){
	int i = 0;
	while(str1[i]!='\0'){
		if (str1[i] != str2[i]){
			return false;
		}
		i++;
	}
	if (str1[i]=='\0' && str2[i]!='\0') return false;
	if (str1[i]!='\0' && str2[i]=='\0') return false;
	return true;
}

char* trim(char* str){
	unsigned int start = -1;
	unsigned int end = -1;
	
	unsigned int i = 0;
	while (str[i] == ' ' && str[i] != '\0') i++;
	if (str[i] ==  '\0') start = 0;
	else start = i;

	i = strlen(str)-1;
	while (str[i] == ' ' && i>=0) i--;
	i++;
	end = i;
	
	char* ret = (char*)malloc(sizeof(char)*(end-start));
	for(unsigned int i = start;i<end-start;i++){
		ret[i] = str[start+i];
	}
	return ret;
}

bool strStart(char* str1,char* str2){
	int i = 0;
	while(str2[i]!='\0'){
		if (str1[i] != str2[i]) return false;
		i++;
	}
	return true;
}
