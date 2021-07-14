#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define buffer_length 100

/*Global variables*/
int currFD=-1;
void* map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */
Elf32_Ehdr *header; /* this will point to the header structure */

/*function decleration*/
int LoadFile(char*);
void examineFile(char*);
void printProgramHeaders();
char* convertType(int);
char* convertFlg(int);



int LoadFile(char* filename){     //reading material
    int fd;
    if((fd = open(filename, O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
    if(fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }
   if(currFD!=-1){close(currFD);}
    currFD=fd;
    return currFD;
}

void examineFile(char* filename){
    if(LoadFile(filename)==-1){exit(EXIT_FAILURE);}
    header = (Elf32_Ehdr *) map_start;
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0){    //chech if ELF file
        printProgramHeaders();
   }
    else{
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
        currFD=-1;
    }
}

void printProgramHeaders(){
    if(currFD!=-1){
        printf("Type\t\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");   
    	for (size_t i = 0; i < header->e_phnum; i++){
    		Elf32_Phdr* entry = map_start+header->e_phoff+(i* header->e_phentsize);
            printf("%s\t\t%#08x\t%#08x\t%#10.08x\t%#07x\t%#07x\t%s\t%#-6.01x\n",
            convertType(entry->p_type),entry->p_offset,entry->p_vaddr,entry->p_paddr,entry->p_filesz,entry->p_memsz,convertFlg(entry->p_flags),entry->p_align);
    	}
  	}
  	else{perror("No file is currently open\n");}
}

char* convertType(int type){
    switch (type){
        case PT_NULL: return "NULL";
        case PT_LOAD: return "LOAD";
        case PT_DYNAMIC: return "DYNAMIC";
        case PT_INTERP: return "INTERP";
        case PT_NOTE: return "NOTE";
        case PT_SHLIB: return "SHLIB";
        case PT_PHDR: return "PHDR";
        case PT_TLS: return "TLS";
        case PT_NUM: return "NUM";
        case PT_GNU_EH_FRAME: return "GNU_EH_FRAME";
        case PT_GNU_STACK: return "GNU_STACK";
        case PT_GNU_RELRO: return "GNU_RELRO";
        case PT_SUNWBSS: return "SUNWBSS";
        case PT_SUNWSTACK: return "SUNWSTACK";
        case PT_HIOS: return "HIOS";
        case PT_LOPROC: return "LOPROC";
        case PT_HIPROC: return "HIPROC"; 
        default:return "Unknown";
    }
}

char* convertFlg(int flg){
    switch (flg){
        case 0x004: return "R";
        case 0x005: return "R E";
        case 0x006: return "RW";
        default:return "Unknown";
    }
}

int main(int argc, char **argv){
    if(argc==2){
        examineFile(argv[1]);
    }
    return 0;
}