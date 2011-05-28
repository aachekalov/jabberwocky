#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
//#include <fcntl.h>
#include <sys/stat.h>

int lastIndex;

int write (int fd, struct table newTable){
	if(write(fd, strlen(newTable.table_name), sizeof(int)) != sizeof(int)){
        perror("Writing error");
        return -1;
    }
	if(write(fd, newTable.table_name, strlen(newTable.table_name) + 1) != strlen(newTable.table_name) + 1){
        perror("Writing error");
        return -1;
    }
    if(write(fd, &newTable.column_count, sizeof(unsigned char)) != sizeof(unsigned char)){
        perror("Writing error");
        return -1;
    }
    int i;
    for(i = 0; i < newTable.column_count; i++){
		if(write(fd, &(++lastIndex), sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
		if(write(fd, strlen(newTable.columns[i].column_name), sizeof(int)) != sizeof(int)){
			perror("Writing error");
			return -1;
		}
		if(write(fd, newTable.columns[i].column_name, strlen(newTable.columns[i].column_name) + 1) != strlen(newTable.columns[i].column_name) + 1){
			perror("Writing error");
			return -1;
		}
		if(write(fd, newTable.columns[i].type, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Writing error");
			return -1;
		}
		if(write(fd, newTable.columns[i].constraints, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Writing error");
			return -1;
		}
		if (newTable.columns[i].constraints > 8)
			if(write(fd, getFieldIndex(fd, newTable.table_name, newTable.columns[i].column_name), sizeof(int)) != sizeof(int)){
				perror("Writing error");
				return -1;
			}
	}
    if(write(fd, "\r\n", 2) != 2){
			perror("Writing error");
			return -1;
	}
	return 0;
}

int getFieldIndex(int fd, char* tableName, char* fieldName){
	
	
}

int read (int fd, struct table *tableList){
	
	struct stat st;
	if(fstat(fd,&st) < 0)    
        return 1;
    if (st.st_size == 0)
		return NULL;
	int k = 1, i;	
	while(!EOF){
		tableList = (struct table *)realloc(tableList, k * sizeof(struct table));
		int len;
		if(read(fd, &len, sizeof(int)) != sizeof(int)){
			perror("Reading error");
			return -1;
		}
		tableList[k-1].table_name = (char *)malloc(len * sizeof(char));
		if(read(fd, tableList[k-1].table_name, strlen(tableList[k-1].table_name)) != strlen(tableList[k-1].table_name)){
			perror("Reading error");
			return -1;
		}
		unsigned char qcol;
		if(read(fd, &tableList[k-1].column_count, sizeof(unsigned char)) != sizeof(unsigned char)){
			perror("Reading error");
			return -1;
		}
		for(i = 0; i < qcol; i++){
			tableList.columns = (struct column_declare *)realloc(tableList.columns, qcol * sizeof(struct column_declare));	
			if(read(fd, &lastIndex, sizeof(int)) != sizeof(int)){
				perror("Reading error");
				return -1;
			}
			int clen;
			if(read(fd, &clen, sizeof(int)) != sizeof(int)){
				perror("Reading error");
				return -1;
			}
			char* tableName = (char *)malloc(len * sizeof(char));
			if(read(fd, tableName, strlen(tableName)) != strlen(tableName)){
				perror("Reading error");
				return -1;
			}
		}
	}
}

