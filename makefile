LIBS_FLAGS = -lgdi32 -lopengl32 -lfreeglut
INCLUDE_FLAGS= -Iinclude
CFLAGS= $(INCLUDE_FLAGS)

.PHONY: all clean

all: test.exe

clean:
	$(RM) *.o
	$(RM) *.exe

test.exe: main.o gui.o geometry.o
	gcc  $^ $(LIBS_FLAGS) -o $@

main.o: main.c
	gcc -x c $^ $(CFLAGS) -c -o $@

gui.o: gui.c
	gcc -x c $^ $(CFLAGS) -c -o $@

geometry.o: geometry.c
	gcc -x c $^ $(CFLAGS) -c -o $@
