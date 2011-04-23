jwcreatedb: jabberwocky_create.o
	gcc -o $@ $^

jabberwocky_create.o: jabberwocky_create.c jabberwocky_func.h
	gcc -c jabberwocky_create.c
   
jwdropdb: jabberwocky_drop.o
	gcc -o $@ $^
   
jabberwocky_drop.o: jabberwocky_drop.c jabberwocky_func.h
	gcc -c jabberwocky_drop.c

jwselectdb: jabberwocky_select.o
	gcc -o $@ $^
   
jabberwocky_select.o: jabberwocky_select.c jabberwocky_func.h
	gcc -c jabberwocky_select.c

.PHONY : create clean

clean:
	rm -f *~ *.o jabberwocky
   
create:
  if NOT EXIST "$(HOMEPATH)/Jabberwocky/DBs" mkdir "$(HOMEPATH)/Jabberwocky/DBs" # try $(HOME) 
