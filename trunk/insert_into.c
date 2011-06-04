#include <stdio.h>

#include "jabberwocky_io.h"
#include "parsing_tools.h"

#include "insert_into.h"

int insert_into(char *query, struct table *tableList){
	int len = strcspn(query, "(");
	char *tableName = (char *) calloc(len + 1, sizeof(char));
	strncpy(tableName, query, len);
	tableName = trim(tableName);
	char q = query + len +1;
	q = trim(q);
   int index = isTableName(tableName, tableList);
   if (index < 0){
     printf("Error: non-existent table");
     return -1;
   }
   
	return 0;
}

int isTableName(char *name, struct table *tableList){
   int i;
   for (i = 0; i < sizeof(tableList)/sizeof(struct table); i++)
     if (!strcmp(tableList[i], name))
         return i;
   return -1;
}

int checkConstraints (char *q, struct table t){

}