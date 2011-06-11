#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

#include "jabberwocky_io.h"
#include "parsing_tools.h"

#include "insert_into.h"

int insert_into(char *query, struct table *tableList, char *dbPath){
	char *q = query;
	char *tableName = getTableName(&q);
	if(!tableName){
		printf("Error: '(' expected");
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
		printf("Error: wrong query");
		free(tableName);
   		free(columns);
		free(colVals);
		return -1;
	}	
	char **cols = 0, **vals = 0;
	int size = split(columns, ",", cols);
	int ssize, i;
	if (size != (ssize = split(colVals, ",", vals))){
		for (i = 0; i < size; i++)
			free(cols[i]);		
		
		for (i = 0; i < ssize; i++)
			free(vals[i]);
		free(tableName);
		free(columns);
		free(colVals);
		printf("Error: quant of cols is more than quant of vals");
		return -1;
	}
	free(columns);
	free(colVals);
	
	int index = isTableName(tableName, tableList);
	if (index < 0){
		printf("Error: non-existent table");		
		return -1;
	}
	char *resLine = (char *)malloc(1024 * sizeof(char));  // very-very bad (((
	if ((resLine = checkConstraints(tableList[index], cols, vals, size, dbPath)) == NULL){
		printf("Error: wrong query");
		return -1;
	}	
	for (i = 0; i < size; i++){
		free(cols[i]);
		free(vals[i]);
	}
	char *path = dbPath;
	strcat(path, tableName);
	free(tableName);
	FILE * fd = fopen(path, "a");
	fprintf(fd, "%s\n", resLine);
	fclose(fd);
	return 0;
}

int isTableName(char *name, struct table *tableList){
	int i;
	for (i = 0; i < sizeof(tableList)/sizeof(struct table); i++)
		if (!strcmp(tableList[i].table_name, name))
			return i;
	return -1;
}

char* checkConstraints (struct table t, char **cols, char **vals, int size, char *dbPath){
    char line[1024];
	int i, index, count = 0;
	if (size > t.column_count){
		printf("Error: too many columns for %s", t.table_name);
		return NULL;
	}
	for(i = 0; i < t.column_count; i++){
        if ((index = isIn(t.columns[i].column_name, cols, size) < 0)){
               if (t.columns[i].constraints & 1 == t.columns[i].constraints){
			      if (isIn(t.columns[i].column_name, cols, size) < 0){
				     printf("Error: column %s must be not null", t.columns[i].column_name);
				     return NULL;
                  }
               }else{
			         strcat(line, "NULL|");
               } 
		}else{
               count ++;
               if (t.columns[i].type == 1){
                  char *end;                  
                  long val = strtol(vals[index], &end, 10);
                  if(end || (val == INT_MAX || val == INT_MIN)){
                          printf("Error: field %s must be integer or your value is too big", t.columns[i].column_name);
                          free(end);
                          return NULL;        
                  }                    
               }
               if (t.columns[i].type == 2){
                  char *end;                  
                  double val = strtod(vals[index], &end);
                  if(end || (val >= FLT_MAX || val >= FLT_MIN)){
                          printf("Error: field %s must be float or your value is too big", t.columns[i].column_name);
                          free(end);
                          return NULL;        
                  }                
               } 
               if (t.columns[i].type == 4){
				   if (vals[index][0] == '\'' && vals[index][strlen(vals[index]) - 1] == '\''){
						int g;
						for (g = 0; g < strlen(vals[index]) - 2; g++)
							vals[index][g] = vals[index][g + 1];
						vals[index][strlen(vals[index]) - 2] = '\0';
					}else{
						printf("Error: field %s must be char, \"\'\" not faund", t.columns[i].column_name);
                        return NULL; 
					}			
               } 
               if (t.columns[i].constraints & 2 == t.columns[i].constraints || 
                   t.columns[i].constraints & 4 == t.columns[i].constraints){
                    if(!isValueExist(dbPath, t, t.columns[i].column_name, vals[index])){// TODO: search for cortege with equal value of the field    
                            printf("Error: field %s must be unique", t.columns[i].column_name);
                            return NULL;
                    }                  
               }
               if (t.columns[i].constraints >= 8){
                  if(isValueExist(dbPath, *t.columns[i].foreign_table, t.columns[i].foreign_key->column_name, vals[index])){// TODO: search for foreign key   
                            printf("Error: non-existent foreign key");
                            return NULL;
                  }                    
               }
               strcat(line, vals[index]);
               strcat(line, "|");            
		}
	}
	if (count != size){
              printf("Error: there are some non-existent fields in the query");
              return NULL;
    }
	line[strlen(line) - 1] = '\0';
	return line;
}

int isValueExist (char *dbPath, struct table t, char *columnName, char *value) {
    int i, j, res = 1;
    char *path = dbPath;
    strcat(path, t.table_name);
    for (i = 0; i < t.column_count; i++)
        if (!strcmp(t.columns[i].column_name, columnName))
           break;
    char **values = (char **)malloc(t.column_count * sizeof(char));
    FILE *fd = fopen(path, "r");
    while(feof(fd)){    
        for (j = 0; j < t.column_count; j++){
            char buf[1024]; // What an awfull line!
            if (j == t.column_count - 1)
               fscanf(fd, "%s\n", buf);
            else
               fscanf(fd, "%s|", buf);
            if (j == i && !strcmp(buf, value)){
               res = 0;
               break; 
            }
        }            
    }
    for (i = 0; i < t.column_count; i++)
        free(values[i]);
    fclose(fd);
    return res;
}

int isIn(char *col, char **cols, int size){
	int i;
	for (i = 0; i < size; i++)
		if (!strcmp(col, cols[i]))
			return i;
	return -1;
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
