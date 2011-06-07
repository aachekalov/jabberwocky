int insert_into(char *query, struct table *tableList);

int isTableName(char *name, struct table *tableList);

int checkConstraints (char *q, struct table t);

char * getColumnsStr (char **q);

char * getTableName (char **q);

int isValuesExists(char **q);
