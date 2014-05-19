CC = g++
CFLAGS = -framework GLUT -framework OpenGL
OBJS = display.o ACO.o main.o

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS)
.cpp.o:
	$(CC) -c $< $(CFLAGS)
clean:
	rm a.out $(OBJS)