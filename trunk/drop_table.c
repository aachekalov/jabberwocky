#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//#include "jabberwocky_io.h"
#include "parsing_tools.h"

#include "drop_table.h"

int dropTable(int fd, char *dbPath, char *query, struct table *tableList, int size){
    char **ptr = 0;
    int q, i;
    if ((q = split(query, " ", ptr)) > 1){
       for (i = 0; i < q; i++)
           free(ptr[i]); 
       printf("Error: wrong query");
       return -1;      
    }
    if (';' == query[strlen(query)-1])
       query[strlen(query)-1] = '\0';
       
    if (!isForeignKey(query, tableList, size)){
       printf("Error: another table has link to this table");
       return -1;                      
    }
    // TODO: delete from main file
    char *path = dbPath;
    strcat(path, query);
    remove(path);
    removeTable(query, &tableList, size);
    return 0;
}

int removeTable(char *tableName, struct table **tableList, size){
    int i, k;
    for (i = 0; i < size - 1; i++){
        if (!strcmp(*tableList[i].table_name, tableName))
           i = k;
        if (i >= k)
           *tableList[i] = *tableList[i+1];
    }        
    *tableList = (struct table *)realloc(tableList, (size - 1) * sizeof(struct table));
    return 0;
}

int isForeignKey(char *tableName, struct table tableList, int size){
    int i, j;
    for (i = 0; i < size; i++)
        for (j = 0; j < tableList[i].column_count; j++){
            if (tableList[i].columns[j].constraints > 8 && !strcmp(tableList[i].columns[j].foreign_table.table_name, tableName))
               return 0;
        }
    return 1;        
}
