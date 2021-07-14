all: myELF

myELF: task4.o
	ld -m elf_i386 task4.o -o myELF

task4.o: task4.s
	nasm -f elf32 task4.s -o task4.o
	nasm -f elf task4.s -l smple.lst

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o myELF