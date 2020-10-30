PKGS=glfw3 gl
CFLAGS=-Wall -Werror -std=gnu11 -ggdb `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)` -lm

pp: main.c
	$(CC) $(CFLAGS) -o pp main.c $(LIBS)
