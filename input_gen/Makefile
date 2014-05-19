CCPP=g++
CC=gcc
CFLAGS=-framework GLUT -framework OpenGL
OBJS= hopfield.o print.o

all: $(OBJS)
	$(CCPP) $(OBJS) $(CFLAGS)
ex: all
	./a.out
.cpp.o:
	$(CCPP) -c $< $(CFLAGS)
clean:
	rm a.out $(OBJS)