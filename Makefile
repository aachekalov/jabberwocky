all: jwcreatedb jwdropdb jwselectdb create_work_dir

jwcreatedb: jabberwocky_create.o jabberwocky_func.o
	gcc -o $@ $^

jabberwocky_create.o: jabberwocky_create.c
	gcc -c $^

jwdropdb: jabberwocky_drop.o jabberwocky_func.o
	gcc -o $@ $^

jabberwocky_drop.o: jabberwocky_drop.c
	gcc -c $^

jwselectdb: jabberwocky_select.o jabberwocky_func.o jabberwocky_func.o create_table.o parsing_tools.o insert_into.o jabberwocky_io.o drop_table.o
	gcc -o $@ $^

jabberwocky_select.o: jabberwocky_select.c
	gcc -c $^

jabberwocky_func.o: jabberwocky_func.c
	gcc -c $^

create_table.o: create_table.c
	gcc -c $^

parsing_tools.o: parsing_tools.c
	gcc -c $^

jabberwocky_io.o: jabberwocky_io.c
	gcc -c $^

insert_into.o: insert_into.c
	gcc -c $^
	
drop_table.o: drop_table.c
	gcc -c $^

.PHONY : create_work_dir clean
.NOPARALLEL : create_work_dir

create_work_dir:
	@if [ ! -d $(HOME)/Jabberwocky ]; then \
	mkdir $(HOME)/Jabberwocky; \
	mkdir $(HOME)/Jabberwocky/DBs; \
	fi

clean:
	rm -f *~ *.o jwcreatedb jwdropdb jwselectdb
	rm -f -r $(HOME)/Jabberwocky
