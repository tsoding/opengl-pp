CFLAGS=-Wall -Werror -std=c11 -ggdb
LIBS=

pp: main.c
	$(CC) $(CFLAGS) -o pp main.c $(LIBS)
