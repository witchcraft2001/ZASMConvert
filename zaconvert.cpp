#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

char tokens[168][8]={"ld","ex","im","rst","ret","add","adc","sub","sbc","and","xor","or","cp","push","pop","inc","dec","in","out","jp",
				"call","jr","djnz","rlc","rrc","rl","rr","sla","sra","sli","srl","bit","res","set","nop","halt","di","ei","rlca","rla",
				"rrca","rra","exx","daa","cpl","ccf","scf","ldi","ldir","ldd","lddr","cpi","cpir","cpd","cpdr","neg","inf","ini","inir",
				"ind","indr","outi","otir","outd","otdr","reti","retn","rld","rrd","org","equ","db","dw","ds","defb","defw","defs","insert",
				"include","if","ifdef","ifndef","ifused","ifnused","else","endif","make","b","c","d","e","h","l","(hl)","a","xh","xl",
				"yh","yl","(ix","(iy","(bc)","(de)","i","r","af","bc","de","hl","ix","iy","sp","(sp)","af'","(c)","nz","z","nc","c","po",
				"pe","p","m","phase","unphase","dc","ent","rept","endr","loadtab","macro","endm","create","makelab","saveobj","exitm",
				"ifp","exa","retz","retnz","retc","retnc","retm","retp","retpo","retpe","jpz","jpnz","jpc","jpnc","jpm","jpp","jppo","jppe",
				"callz","callc","callm","callpe","callnz","callnc","callp","callpo","jrz","jrnz","jrc","jrnc","project","eq"};

bool FileExists (const char *fname)
			{
			  _finddata_t data;
			  long nFind = _findfirst(fname,&data);
			  if (nFind != -1)
			  {
			    // Если этого не сделать, то произойдет утечка ресурсов
			    _findclose(nFind);
			    return true;
			  }
			  return false;
			}

void convert(const char *fname)
{
	static char strerr[256];
	char _fname[256];
	int i,j,k;
	unsigned char c=0,old;
	int flen;
 	char str[256];

	_finddata_t	data;
	long hFile = _findfirst(fname,&data);
	do
	{
		printf("%s ... ", data.name);

		FILE *fh = fopen(data.name,"rb");
		if(!fh) {
			printf("ERR: Can\'t open file!\n");
			continue;
		}
		fseek(fh, 0, SEEK_END);
		flen = ftell(fh);
		fseek(fh, 0, SEEK_SET);

		if (strstr(data.name,"$") || strstr(data.name,"!")) {
			 /*Hobbeta*/
			fseek(fh, 17, SEEK_SET);
			printf("\nThis is Hobbeta-file! Skiping header.\n");
			flen -=17;
		}
		sprintf(_fname,"%s.a80",data.name);
		FILE *fa = fopen(_fname,"wt");
		if(fa==NULL)
		{
			fclose(fh);
			continue;
		}
		char spc_comp = 0;
		char comment,tkn,quat;
		old = 0;
		comment = 0;
		tkn	= 0;	//Предыдущий байт был токен-командой
		quat = 0;
		j = 0;
		while(flen--)
			{
				old	= c;
				c = fgetc(fh);
				if (c==0) {
					//конец файла
					c = 0x0d;
					flen = 0;
				}
				//flen--;
				if (spc_comp)
				{
					//SPC-компрессор
					c = c & 127;
					while (c--) str[j++]=' ';
					spc_comp = 0;
					continue;
				}
				if (tkn)
				{			
					str[j] = 0;
					if (c>=32 && c<=0xc8) {
	//					c = c-32;
						strcat(str,tokens[c-32]);
						j += strlen(tokens[c-32]);
						if (old & 4) str[j++]=' ';
					} else {
						sprintf(str,"<Unknown token: #%02hX>",c);
						strcat(str,strerr);
					}
					tkn = 0;
					continue;
				}
				if (c==0x0a) continue;
				if (c==0x0d) {
					str[j]=0;
					fprintf(fa,"%s\n",str);
					j=0;
					if(flen<=0) break;
					continue;
				}
				if	(c==0x09) {
					str[j] = 0;
					strcat(str,"\t"); 
					j++;
					continue;
				}
				if	(c==0x06) {
					spc_comp = 1;
					continue;
				}
				if (c>=2 && c<=5) {
					tkn = 1;
					continue;
				}
				str[j++]=c;
			}
		 fclose(fh);
		 fclose(fa);
		 printf("OK!\n");
	}while(_findnext(hFile, &data) == 0);
	_findclose(hFile);
}

int main(int argc,char **argv)
{
 printf("\nZA2ASM v1.0 (ZAsm 3.X files converter)\n Copyright\t(c) 2013, Dmitry Mikhaltchenkov witchcraft2001@mail.ru\n\n");
 if(argc<2)
 {
    printf("\n\nZA2ASM file.zas\n\n");
    return 1;
 }
 for (int i=1;i<argc;i++) {
	 if (FileExists(argv[i])) convert(argv[i]);
 }
 return 1;
}