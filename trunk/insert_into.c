#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "jabberwocky_io.h"
#include "parsing_tools.h"

#include "insert_into.h"

int insert_into(char *query, struct table *tableList){
	char *q = query;
	char *tableName = getTableName(&q);
	if(!tableName){
		printf("Error: '(' expected");
		return -1;
	}
	int index = isTableName(tableName, tableList);
	if (index < 0){
		printf("Error: non-existent table");
		free(tableName);
		return -1;
	}
	if (!strlen(q)){
		free(tableName);
		return -1;
	}
   	char *columns = getColumnsStr(&q);
	if(!columns){
		printf("Error: ')' expected");
		free(tableName);
		return -1;
	}
	if (!strlen(q)){
		free(tableName);
   		free(columns);
		return -1;
	}	
	int isvex = isValuesExists(&q);
	if (isvex > 0){
		printf("Error: 'VALUES' expected");
		free(tableName);
   		free(columns);
		return -1;
   	}
	if (isvex < 0){
		printf("Error: '(' expected");
		free(tableName);
   		free(columns);
		return -1;
	}
	if (!strlen(q)){
		free(tableName);
   		free(columns);
		return -1;
	}
	char *colVals = getColumnsStr(&q);
	if(!columns){
		printf("Error: ')' expected");
		free(tableName);
		free(columns);
		return -1;
	}
	if (strlen(q) && *q != ';'){
		free(tableName);
   		free(columns);
		free(colVals);
		return -1;
	}	
	
	

	free(tableName);
	free(columns);
	free(colVals);
	return 0;
}

int isTableName(char *name, struct table *tableList){
	int i;
	for (i = 0; i < sizeof(tableList)/sizeof(struct table); i++)
		if (!strcmp(tableList[i].table_name, name))
			return i;
	return -1;
}

int checkConstraints (char *q, struct table t){

}

char * getColumnsStr (char **q){
	int len = strcspn(*q, ")");
	if (len == strlen(*q))
		return NULL;

	char *columns = (char *)calloc(len + 1, sizeof(char));
	strncpy(columns, *q, len++);
	columns = trim(columns); //!!
	*q += len;
	*q = trim(*q);
	return columns;
}

char * getTableName (char **q){
	int len = strcspn(*q, "(");
	if (len == strlen(*q))
		return NULL;
	
	char *tableName = (char *) calloc(len + 1, sizeof(char));
	strncpy(tableName, *q, len++);
	tableName = trim(tableName); //!!
	*q += len;
	*q = trim(*q);
	return tableName;
}

int isValuesExists(char **q){
	int len = strcspn(*q, "(");
	if (len == strlen(*q))
		return -1;
	
	char *values = (char *) calloc(len + 1, sizeof(char));
	strncpy(values, *q, len++);
	values = trim(values);
	strup(values); 
	if(strcmp(values, "VALUES")){
		free(values);
		return 1;
	}
	*q += len;
	*q = trim(*q);
	free(values);
	return 0;

}
