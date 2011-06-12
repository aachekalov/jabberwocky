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
	   if (writeTable(fd, tableList[i]))
           return -1;
	}
           
    return 0;
}

int writeTable (int fd, struct table newTable){
	int len = strlen(newTable.table_name);
    if(write(fd, &len, sizeof(int)) != sizeof(int)){
        perror("Writing error");
        return -1;
    }
    if(write(fd, newTable.table_name, len) != len){
        perror("Writing error");
        return -1;
    }
    if(write(fd, &newTable.column_count, sizeof(unsigned char)) != sizeof(unsigned char)){
        perror("Writing error");
        return -1;
    }
    int i;
    for(i = 0; i < newTable.column_count; i++){
		newIndex++;
		if(write(fd, &newIndex, sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
		len = strlen(newTable.columns[i].column_name);
		if(write(fd, &len, sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
		if(write(fd, newTable.columns[i].column_name, len) != len){
			perror("Writing error");
			return -1;
		}
		if(write(fd, &newTable.columns[i].type, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Writing error");
			return -1;
		}
		if(write(fd, &newTable.columns[i].constraints, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Writing error");
			return -1;
		}
		if (newTable.columns[i].constraints > 8){
			int index = getFieldIndex(fd, newTable.table_name, newTable.columns[i].column_name);
			if(write(fd, &index, sizeof(int)) != sizeof(int)){
				perror("Writing error");
				return -1;
			}
		}
	}
    if(write(fd, "\n", 1) != 1){
			perror("Writing error");
			return -1;
	}
	return 0;
}

int getFieldIndex(int fd, char* tableName, char* fieldName){
	struct stat st;
	if(fstat(fd,&st) < 0)    
        return -1;
    if (st.st_size == 0)
		return -1;
	int k = 1, i;
	struct table * tableList;	
	int index = -1;
	while(!EOF){
		tableList = (struct table *)malloc(sizeof(struct table));
		int len;
		if(read(fd, &len, sizeof(int)) != sizeof(int)){
			perror("Reading error");
			return -1;
		}
		tableList->table_name = (char *)calloc(len + 1, sizeof(char));
		if((read(fd, tableList->table_name, len)) != len){
			perror("Reading error");
			return -1;
		}
		if (!strcmp(tableName, tableList->table_name)){
			unsigned char qcol;
			if(read(fd, &tableList->column_count, sizeof(unsigned char)) != sizeof(unsigned char)){
				perror("Reading error");
				return -1;
			}
			
			tableList->columns = (struct column_declare *)calloc(qcol, sizeof(struct column_declare));			
			for(i = 0; i < qcol; i++){
				int nextIndex;
				if(read(fd, &nextIndex, sizeof(int)) != sizeof(int)){
					perror("Reading error");
					return -1;
				}
				int clen;
				if(read(fd, &clen, sizeof(int)) != sizeof(int)){
					perror("Reading error");
					return -1;
				}
				tableList->columns[i].column_name = (char *)calloc(clen + 1, sizeof(char));
				if((read(fd, tableList->columns[i].column_name, clen)) != clen){
					perror("Reading error");
					return -1;
				}
				if (!strcmp(tableList->columns[i].column_name, fieldName)){
					index = nextIndex;
					break;
				}
			}
			break;
		}else{
			continue;
		}
	}
		
	return index;	
}

int readTables (int fd, struct table **tableList){
	
	struct stat st;
	if(fstat(fd,&st) < 0)    
        return -1;
    if (st.st_size == 0)
		return 0;
	int k = 1, l = 0, i;
	int readen = 0;
	int *a = NULL;
	while(st.st_size > readen){
		printf ("\nsize = %d, readen = %d\n", st.st_size, readen);
		*tableList = (struct table *)realloc(*tableList, k * sizeof(struct table));
		int len, r;
		r = read(fd, &len, sizeof(int));
		if(r != sizeof(int)){
			perror("Reading error: len");
			return -1;
		}
		readen += r;
		
		(*tableList)[k-1].table_name = (char *)calloc((len+1), sizeof(char));
		r = read(fd, (*tableList)[k-1].table_name, len);
		if(r != len){
			perror("Reading error: table_name");
			return -1;
		}
		readen += r;
		printf("Read table_name = %s\n", (*tableList)[k-1].table_name);

		unsigned char qcol;
		if((r = read(fd, &(*tableList)[k-1].column_count, sizeof(unsigned char))) != sizeof(unsigned char)){
			perror("Reading error: qcol");
			return -1;
		}
		printf("Read qcol = %d\n", (*tableList)[k-1].column_count);
		qcol = (*tableList)[k-1].column_count;
		(*tableList)[k-1].columns = (struct column_declare *)calloc(qcol, sizeof(struct column_declare));
		a = (int *)realloc(a, (l + qcol) * sizeof(int));		
			
		for(i = 0; i < qcol; i++){
			int index;
			if((r = read(fd, &index, sizeof(int))) != sizeof(int)){
				perror("Reading error: index");
				return -1;
			}
			readen += r;
			a[l + i] = index;
			int clen;
			if((r = read(fd, &clen, sizeof(int))) != sizeof(int)){
				perror("Reading error: clen");
				return -1;
			}
			printf("Read clen = %d\n", clen);
			readen += r;
			
			(*tableList)[k-1].columns[i].column_name = (char *)calloc(clen + 1, sizeof(char));
			if((r = read(fd, (*tableList)[k-1].columns[i].column_name, clen)) != clen){
				perror("Reading error: column_name");
				return -1;
			}
			printf("Read cols: name = %s\n", (*tableList)[k-1].columns[i].column_name);
			readen += r;
			if((r = read(fd, &(*tableList)[k-1].columns[i].type, sizeof(char))) != sizeof(char)){
				perror("Reading error: column_type");
				return -1;
			}
			printf("Read cols: type = %d\n", (*tableList)[k-1].columns[i].type);	
			readen += r;
			if((r = read(fd, &(*tableList)[k-1].columns[i].constraints, sizeof(char))) != sizeof(char)){
				perror("Reading error: column_constraints");
				return -1;
			}
			printf("Read cols: constr = %d\n", (*tableList)[k-1].columns[i].constraints);
			readen += r;
			if ((*tableList)[k-1].columns[i].constraints > 8){
				int foreignIndex;
				if((r = read(fd, &foreignIndex, sizeof(int))) != sizeof(int)){
					perror("Reading error: column_foreign_index");
					return -1;
				}
				readen += r;
				if (setForeignKey(foreignIndex, (*tableList), k, a, l + qcol, k-1, i) < 0){
					printf("Wrong data: non-existent field");
					return -1;
				}
				printf("Read: foreign key index = %d", foreignIndex);
			}
		}
		l += qcol;
		k++;
		char buf[1];
		r = read (fd, buf, 1);		
		readen += r + 1;		
	}
	return k-1;
}

int setForeignKey(int foreignIndex, struct table *tableList, int structSize, int *a, int aSize, int ti, int fi){
	int i;
	for (i = 0; i < aSize; i++)
     if (a[i] == foreignIndex)
       break;
   i++;
   int j, k;
   for (j = 0; j < structSize; j++)
     if (i > tableList[j].column_count){
       i -= tableList[j].column_count;
     }else {
         tableList[ti].columns[fi].foreign_table = &tableList[j];
         tableList[ti].columns[fi].foreign_key = &tableList[j].columns[i-1];
	 }
       
   return 0;     
}

