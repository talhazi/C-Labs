all: exec

exec: addresses.c
	gcc -g -m32 -c -o addresses.o addresses.c
	gcc -g -m32  addresses.o  -o addresses
	rm addresses.o

.PHONY: clean
clean:
	rm -rf ./*.o addresses
