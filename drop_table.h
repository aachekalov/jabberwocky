#include "jabberwocky_io.h"

int drop_table(int fd, char *dbPath, char *query, struct table *tableList, int size);

int removeTable(char *tableName, struct table **tableList, int size);

int isForeignKey(char *tableName, struct table *tableList, int size);
