all: jwcreatedb jwdropdb jwselectdb create

jwcreatedb: jabberwocky_create.o
	gcc -o $@ $^

jabberwocky_create.o: jabberwocky_create.c
	gcc -c $^

jwdropdb: jabberwocky_drop.o
	gcc -o $@ $^

jabberwocky_drop.o: jabberwocky_drop.c
	gcc -c $^

jwselectdb: jabberwocky_select.o
	gcc -o $@ $^

jabberwocky_select.o: jabberwocky_select.c
	gcc -c $^

#.PHONY : create clean

create:
#if NOT EXIST "$(HOME)/Jabberwocky/DBs";	then mkdir "$(HOME)/Jabberwocky/DBs"; fi
#	@if test -d $(HOME)/Jabberwocky; then \
#	mkdir $(HOME)/Jabberwocky; \
#	mkdir $(HOME)/Jabberwocky/DBs; \
#	fi
	@if [ -f $(HOME)/Jabberwocky ]; then \
	echo file exist; \
	else \
	echo no; \
	mkdir $(HOME)/Jabberwocky; \
	mkdir $(HOME)/Jabberwocky/DBs; \
	fi

clean:
	rm -f -r *~ *.o jwcreatedb jwdropdb jwselectdb $(HOME)/Jabberwocky
