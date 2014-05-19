CC=gcc
OBJS = NQueen.o TMSNQ.o BTNQ.o main.o
tokenizer : $(OBJS)
	gcc -o NQ -Wall $(OBJS)
clean :
	rm NQ $(OBJS)
%.o: %.c
	$(CC) -g -Wall -c -o $@ $<