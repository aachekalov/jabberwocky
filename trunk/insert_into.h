int insert_into(char *query, struct table *tableList, char *dbPath);

int isTableName(char *name, struct table *tableList);

char * checkConstraints (struct table t, char **cols, char **vals, int size, char *path);

char * getColumnsStr (char **q);

char * getTableName (char **q);

int isValuesExists(char **q);

int isIn(char *col, char **cols, int size);
