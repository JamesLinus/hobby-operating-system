#include "kio.h"

void kinit(){
	color = 0x0F;
}

void kStatus(char* name, char* status, bool good){
	kprintf("* %s",name);
	if (good)
		kColor(Black,LightGreen);
	else
		kColor(Black,LightRed);
	line_x = 80-5-strlen(status);
	kprintf("[ %s ]\n",status);
	kColorReset();
	kprintf("................................................................\n");
}
	
void kColorReset(){
	kColor(Black,White);
}

void kColor(int background, int foreground){
	color = background*10+foreground;
}

void update_cursor() {
	unsigned short cursorLocation = line_y * 80 + line_x;
	outb(0x3D4, 14);                 
	outb(0x3D5, cursorLocation >> 8); 
	outb(0x3D4, 15);                  
	outb(0x3D5, cursorLocation); 
} 

void kBackspace(){
	line_x--;
	if (line_x<0){
		line_x = 0;
		line_y--;
		if (line_y<0) line_y=0;
	}
	kprint(' ');
	line_x--;
	if (line_x<0){
		line_x = 0;
		line_y--;
		if (line_y<0) line_y=0;
	}
	update_cursor();
}

int amountOfSpecialChars(char* str){
	int count = 0;
	int i = 0;
	while (str[i]!='\0'){
		if (str[i]=='%' && str[i+1]!=' ') count++;
		i++;
	}
	return count;
}

void kclear(){
	int pos = 0;
	for(int pos = 0;pos<25*80;pos++){
		videoram[pos*2] = ' ';
		videoram[pos*2+1] = 0x0F;
	}
	line_x = line_y = 0;
	update_cursor();
}

void scroll(){
	if (line_y>=25) {
		for(int pos = 0;pos<24*80;pos++){
			videoram[pos*2] = videoram[(pos+80)*2];
			videoram[pos*2+1] = videoram[(pos+80)*2+1];
		}
		for(int pos = 24*80;pos<25*80;pos++){
			videoram[pos*2] = ' ';
			videoram[pos*2+1] = 0x0F;
		}
		line_y = 24;
		line_x = 0;
	}
}

void kprintSafe(char c){
	if ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
		kprint(c);
}

void kprint(char c){
	int temp = (line_y*80 + line_x)*2;
	switch (c){
		case '\r': line_x = 0;
					break;
		case '\b': line_x--;
					break;
		case '\n': 	line_y++; 
				line_x = 0; 
				scroll();
				break;
		case '\t': line_x+=8 & ~(8-1); 	break;
		default:
			videoram[temp] = c;
			videoram[temp+1] = color;			
			line_x++;
			break;
	}			
	
	if (line_x>=80){
		line_x=0;
		line_y++;	
		scroll();
	}
	update_cursor();
}

void kprintInt(char filler,int amount,int x){
	if (x == 0) {
		kprint(48); 
		return;
	}
	if (x<0) {
		kprint('-'); 
		x = x*-1;		//make it positive!
	}
	int base = 1000000000;
	int length = 10;
	bool nonZero = false;
	//find first 10^x that is smaller then the number
	while (base>=1) {
		int temp = x/base;
		if (temp != 0) break;
		else {
			base/=10;
			length--;
		}
	}
	unsigned char output[20];
	//unsigned char *output = (unsigned char*)kmalloc(sizeof(char)*length);
	base = 10;
	for(int i = length-1;i>=0;i--){
		output[i] = x%10;
		x = (x-(x%10)) / 10;
	}
	for(int i = 0;i<length;i++) kprint(output[i]+48);

}

void kDebug(bool debug){
	debugging = debug;
}

bool kIsDebug() { 
	return debugging; 
} 

void kprintHex(char filler,int amount,int num){
	bool nonZero = true;
	kprint('0'); kprint('x');
	if (num==0){
		kprintf("000");
		return;
	}
	int j = -1;
	for(int i = 28;i>=0;i-=4){
		int temp = (num>>i) & 0x0F;
		if (temp==0 && nonZero) continue;
		if (nonZero) j=1;
		else j++;
		if (temp>=0xA)
			kprint(temp-0xA+'A');		
		else
			kprint(temp+'0');
		//if (j>1 && j%3==0 && i>0) kprint('.');
		nonZero = false;
	}
}

void kprintHexShort(char filler,int amount,unsigned int num){
	bool nonZero = true;
	if (num==0){
		kprintf("00");
		return;
	}
	int j = -1;
	char ar[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	while (num>16){
		unsigned int x = num%16;
		kprint(ar[x+1]);
		num = num-x;
		num /= 10;
	}	
	kprintf(ar[num+1]);
}

void kprintFloat(char filler,int amount,float x){
	kprintInt(' ',0,(int)x);
	kprint('.');
	x -= (double)((int)x);
	int countZero = 0;
	while(x>(double)((int)x) && countZero < 3){
		x*=10;
		kprintInt(' ',0,(int)x);
		if (((int)x) == 0) countZero++;
		x-= (double)((int)x);	
	}	
}


void kprintf(char* str,...){
	va_list list;
	va_start(list, str);
	int i = 0;
	while (str[i] != '\0'){
		if (str[i]=='%' && str[i+1]!=' '){
			bool end=false;
			char filler = ' ';
			int amount = 0;
			while (!end){
				i++;
				switch(str[i]){
					case 'd':end= true;
							kprintInt(filler,amount,va_arg(list,int));
							break;
					case 'c':  end= true;
							kprint(va_arg(list,char));
							break;
					case 'x': end= true;
							kprintHex(filler,amount,va_arg(list,int));
							break;
					case 'X': end= true;
							kprintHexShort(filler,amount,va_arg(list,unsigned int));
							break;
					case 'l': end= true;
							kprintInt(filler,amount,va_arg(list,long));
							break;
					case 'f':  end= true;
							kprintFloat(filler,amount,va_arg(list,double));
							break;
					case 's':  end= true;
							kprintf(va_arg(list,char*));
							break;
					case 48:
					case 49:
					case 50:
					case 51:
					case 52:
					case 53:
					case 54:
					case 55:
					case 56:
					case 57:filler = str[i];
							i++;
							amount = str[i]-48;
							break;
				}
			}
		} 
		else {
			switch (str[i]){
				default: 	kprint(str[i]);		break;
			}
		}						
		i++;
	}
	va_end(list);                  // Cleans up the list
	update_cursor();
}
