jabberwocky: jabberwocky.o
	gcc -o $@ $^

jabberwocky.o: jabberwocky.c
	gcc -c $^

clean:
	rm -f *~ *.o jabberwocky
