CC := gcc
CFLAGS := -Wall -g
SYNTREE_OBJS := MyFirstAI.o

all: MyFirstAI

MyFirstAI: $(SYNTREE_OBJS) main.o
	$(CC) -o $@ $^

clean:
	rm -f $(SYNTREE_OBJS) main.o

