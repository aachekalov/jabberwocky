#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jabberwocky_io.h"

int newIndex;

int writeTables(int fd, struct table *tableList, int size){
    int i;
    newIndex = 0;
    for (i = 0; i < size; i++){
	   if (writeTable(fd, tableList, i))
           return -1;
	}
           
    return 0;
}

int writeTable (int fd, struct table *tableList, int index){
	int len = strlen(tableList[index].table_name);
    if(write(fd, &len, sizeof(int)) != sizeof(int)){
        perror("Writing error");
        return -1;
    }
printf("\nDEBUG: write table = '%s'\n", tableList[index].table_name);
    if(write(fd, tableList[index].table_name, len) != len){
        perror("Writing error");
        return -1;
    }
    if(write(fd, &tableList[index].column_count, sizeof(unsigned char)) != sizeof(unsigned char)){
        perror("Writing error");
        return -1;
    }
    int i;
    for(i = 0; i < tableList[index].column_count; i++){
printf("DEBUG: write column = '%s'\n", tableList[index].columns[i].column_name);
		newIndex++;
		if(write(fd, &newIndex, sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
printf("DEBUG: write index = %d\n", newIndex);
		len = strlen(tableList[index].columns[i].column_name);
		if(write(fd, &len, sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
printf("DEBUG: write len\n");
		if(write(fd, tableList[index].columns[i].column_name, len) != len){
			perror("Writing error");
			return -1;
		}
printf("DEBUG: write column name\n");
		if(write(fd, &tableList[index].columns[i].type, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Writing error");
			return -1;
		}
printf("DEBUG: write type\n");
		if(write(fd, &tableList[index].columns[i].constraints, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Writing error");
			return -1;
		}
printf("DEBUG: write constraints\n");
		if (tableList[index].columns[i].constraints >= 8){
printf("DEBUG: getFieldIndex foreign table = '%s', foreign column = '%s'\n", tableList[index].columns[i].foreign_table->table_name, 									tableList[index].columns[i].foreign_key->column_name);
			int findex = getFieldIndex(tableList[index].columns[i].foreign_table->table_name, 
									  tableList[index].columns[i].foreign_key->column_name, tableList, index, i);

			if(findex < 0){
				printf("Writing error: foreign key index was not found\n");
				return -1;
			}
			if(write(fd, &findex, sizeof(int)) != sizeof(int)){
				perror("Writing error");
				return -1;
			}
printf("DEBUG: write foreign findex = %d\n", findex);
		}
	}
    if(write(fd, "\n", 1) != 1){
			perror("Writing error");
			return -1;
	}
	return 0;
}

int getFieldIndex(char* tableName, char* fieldName, struct table *tableList, int ti, int fi){
	int k = 1, i, j, res = -1;
    for (i = 0; i < ti + 1; i++){
     	if (!strcmp(tableName, tableList[i].table_name)){
			int lim = tableList[i].column_count;
			if (i == ti)
				lim = fi;
       		for(j = 0; j < lim; j++, k++)
				if (!strcmp(tableList[i].columns[j].column_name, fieldName)){
					res = k;
					break;
				}
			break;
	 	}
		k += tableList[i].column_count;	
	}
		
	return res;	
}

int readTables (int fd, struct table **tableList){
	
	struct stat st;
	if(fstat(fd,&st) < 0)    
        return -1;
    if (st.st_size == 0)
		return 0;

	int k = 0, i, readen = 0;
	while(st.st_size > readen){
		int len, r;
		printf ("\nsize = %d, readen = %d\n", st.st_size, readen);

		*tableList = (struct table *)realloc(*tableList, (k + 1) * sizeof(struct table));		
		if((r = read(fd, &len, sizeof(int))) != sizeof(int)){
			perror("Reading error: len");
			return -1;
		}
		readen += r;
		
		(*tableList)[k].table_name = (char *)calloc((len+1), sizeof(char));
		if((r = read(fd, (*tableList)[k].table_name, len)) != len){
			perror("Reading error: table_name");
			return -1;
		}
		printf("Read table_name = %s\n", (*tableList)[k].table_name);
		readen += r;
		
		if((r = read(fd, &(*tableList)[k].column_count, sizeof(unsigned char))) != sizeof(unsigned char)){
			perror("Reading error: qcol");
			return -1;
		}
		printf("Read qcol = %d\n", (*tableList)[k].column_count);
		readen += r;
		
		(*tableList)[k].columns = (struct column_declare *)calloc((*tableList)[k].column_count, sizeof(struct column_declare));
			
		for(i = 0; i < (*tableList)[k].column_count; i++){
			int index;
			if((r = read(fd, &index, sizeof(int))) != sizeof(int)){
				perror("Reading error: index");
				return -1;
			}
			printf("Read index = %d\n", index);
			readen += r;

			int clen;
			if((r = read(fd, &clen, sizeof(int))) != sizeof(int)){
				perror("Reading error: clen");
				return -1;
			}
			printf("Read clen = %d\n", clen);
			readen += r;
			
			(*tableList)[k].columns[i].column_name = (char *)calloc(clen + 1, sizeof(char));
			if((r = read(fd, (*tableList)[k].columns[i].column_name, clen)) != clen){
				perror("Reading error: column_name");
				return -1;
			}
			printf("Read cols: name = %s\n", (*tableList)[k].columns[i].column_name);
			readen += r;

			if((r = read(fd, &(*tableList)[k].columns[i].type, sizeof(char))) != sizeof(char)){
				perror("Reading error: column_type");
				return -1;
			}
			printf("Read cols: type = %d\n", (*tableList)[k].columns[i].type);	
			readen += r;

			if((r = read(fd, &(*tableList)[k].columns[i].constraints, sizeof(char))) != sizeof(char)){
				perror("Reading error: column_constraints");
				return -1;
			}
			printf("Read cols: constr = %d\n", (*tableList)[k].columns[i].constraints);
			readen += r;

			if ((*tableList)[k].columns[i].constraints >= 8){
				int foreignIndex;
				if((r = read(fd, &foreignIndex, sizeof(int))) != sizeof(int)){
					perror("Reading error: column_foreign_index");
					return -1;
				}
				printf("Read: foreign key index = %d\n", foreignIndex);
				readen += r;
				setForeignKey(foreignIndex, (*tableList), k, i);				
			}
		}
		k++;

		char buf[1];
		r = read (fd, buf, 1);		
		readen += r;		
	}
	return k;
}

int setForeignKey(int foreignIndex, struct table *tableList, int ti, int fi){
	int i = 0;
	while(foreignIndex > tableList[i].column_count){
		foreignIndex -= tableList[i].column_count;
		i++;
	}
    tableList[ti].columns[fi].foreign_table =  &tableList[i];  
	tableList[ti].columns[fi].foreign_key =  &tableList[i].columns[--foreignIndex]; 
    return 0;     
}

