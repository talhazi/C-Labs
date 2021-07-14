all: exec

exec: hexaPrint.c
	gcc -g -m32 -c -o hexaPrint.o hexaPrint.c
	gcc -g -m32 hexaPrint.o  -o hexaPrint
	rm hexaPrint.o

.PHONY: clean
clean:
	rm -rf ./*.o hexaPrint
