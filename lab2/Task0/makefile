all: exec

exec: count-words.c
	gcc -g -m32 -c -o count-words.o count-words.c
	gcc -g -m32  count-words.o  -o count-words
	rm count-words.o

.PHONY: clean
clean:
	rm -rf ./*.o count-words
