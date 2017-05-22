CC=gcc
CFLAGS = -Wall -ansi -pedantic
PROGNAME=MyShell

$(PROGNAME): task_5.c wordlist.o runtree.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(PROGNAME)
	./$(PROGNAME)

clean:
	rm -f *.o *.out* $(PROGNAME)
