#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jabberwocky_io.h"

int lastIndex = 0;

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
		lastIndex++;
		if(write(fd, &lastIndex, sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
		len = strlen(newTable.columns[i].column_name);
		if(write(fd, &len, sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
		if(write(fd, newTable.columns[i].column_name, strlen(newTable.columns[i].column_name)) != strlen(newTable.columns[i].column_name)){
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
    if(write(fd, "\r\n", 2) != 2){
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
		tableList->table_name = (char *)malloc(len * sizeof(char));
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
				tableList->columns[i].column_name = (char *)malloc(clen * sizeof(char));
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

int readTable (int fd, struct table *tableList){
	
	struct stat st;
	if(fstat(fd,&st) < 0)    
        return -1;
    if (st.st_size == 0)
		return -1;
	int k = 1, l = 0, i;
	int *a = 0;
	while(!EOF){
		tableList = (struct table *)realloc(tableList, k * sizeof(struct table));
		int len;
		if(read(fd, &len, sizeof(int)) != sizeof(int)){
			perror("Reading error");
			return -1;
		}
		tableList[k-1].table_name = (char *)malloc(len * sizeof(char));
		if((read(fd, tableList[k-1].table_name, len)) != strlen(tableList[k-1].table_name)){
			perror("Reading error");
			return -1;
		}
		unsigned char qcol;
		if(read(fd, &tableList[k-1].column_count, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Reading error");
			return -1;
		}
		qcol = tableList[k-1].column_count;
		tableList[k-1].columns = (struct column_declare *)calloc(qcol, sizeof(struct column_declare));
		a = (int *)realloc(a, (l + qcol) * sizeof(int));
		k++;	
		for(i = 0; i < qcol; i++){
			if(read(fd, &lastIndex, sizeof(int)) != sizeof(int)){
				perror("Reading error");
				return -1;
			}
			a[l + i] = lastIndex;
			int clen;
			if(read(fd, &clen, sizeof(int)) != sizeof(int)){
				perror("Reading error");
				return -1;
			}
			tableList[k-1].columns[i].column_name = (char *)malloc(clen * sizeof(char));
			if(read(fd, tableList[k-1].columns[i].column_name, clen) != clen){
				perror("Reading error");
				return -1;
			}
			if(read(fd, &tableList[k-1].columns[i].type, sizeof(char)) != sizeof(char)){
				perror("Reading error");
				return -1;
			}
			if(read(fd, &tableList[k-1].columns[i].constraints, sizeof(char)) != sizeof(char)){
				perror("Reading error");
				return -1;
			}
			if (tableList[k-1].columns[i].constraints > 8){
				int foreignIndex;
				if(read(fd, &foreignIndex, sizeof(int)) != sizeof(int)){
					perror("Reading error");
					return -1;
				}
				if (setForeignKey(foreignIndex, tableList, a, i) < 0){
               printf("Wrong data: non-existent field");
               return -1;
			}
			
		}
		l += qcol;
	}
}

int setForeignKey(int foreignIndex, struct table *tableList, int *a, int fi){
	int i;
	for (i = 0; i < sizeof(a)/sizeof(int); i++)
     if (a[i] == foreignIndex)
       break;
   i++;
   int j, k;
   for (j = 0; j < sizeof(tableList)/sizeof(struct table); j++)
     if (i > tableList[j].column_count)
       i -= tableList[j].column_count;
     else
       if(a[i-1] == tableList[j].columns[i-1]){
         tableList[sizeof(tableList)/sizeof(struct table) - 1].columns[fi].foreign_table = &tableList[j];
         tableList[sizeof(tableList)/sizeof(struct table) - 1].columns[fi].foreign_key = &tableList[j].columns[i-1];
       }else
         return -1;
   return 0;     
}
