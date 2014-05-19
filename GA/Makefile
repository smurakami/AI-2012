CC := gcc
CFLAGS :=  -g `pkg-config opencv --cflags`
LIBS=`pkg-config opencv --libs`
OBJS := AI.o print.o util.o init.o run.o GA.o value.o main.o

all: AI

AI: $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f myAI
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<