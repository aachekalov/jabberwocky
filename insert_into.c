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
	return 0;
}
