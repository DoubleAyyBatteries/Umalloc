CC = gcc
CFLAGS = -g -w -fsanitize=address,undefined

all: umalloc memgrind

umalloc:
	$(CC) $(CFLAGS) -o umalloc umalloc.c

memgrind:
	$(CC) $(CFLAGS) -o memgrind memgrind.c

clean:
	rm -rf umalloc memgrind *.o *.a *.dylib *.dSYM
