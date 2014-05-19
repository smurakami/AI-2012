CC = g++
OBJS = protain.o util.o GA.o map.o main.o
FLUGS = -g -Wall

all:$(OBJS)
	$(CC) $(FLUGS) $(OBJS)
.cpp.o:
	$(CC) -c $(FLUGS) $<
clean:
	rm $(OBJS)