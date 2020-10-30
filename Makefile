PKGS=glfw3 gl
CFLAGS=-Wall -Werror -std=c11 -ggdb `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)`

pp: main.c
	$(CC) $(CFLAGS) -o pp main.c $(LIBS)
