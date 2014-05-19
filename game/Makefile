CC=gcc
OBJS = util.o print.o init.o finish.o run.o move.o estimate.o check.o minmax.o main.o
game : $(OBJS)
	gcc -o game -Wall $(OBJS)
clean :
	rm game $(OBJS)
%.o: %.c
	$(CC) -g -Wall -c -o $@ $<