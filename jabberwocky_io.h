struct table {
	char *table_name;
	unsigned char column_count;
	struct column_declare *columns;
};

struct column_declare {
	char *column_name;
	unsigned char type;
	unsigned char constraints;
	struct table *foreign_table;
	struct column_declare *foreign_key;
};

int writeTables(int fd, struct table *tableList, int size);

int writeTable (int fd, struct table *tableList, int index);

int readTables (int fd, struct table **tableList);

int getFieldIndex(char* tableName, char* fieldName, struct table *tableList, int ti, int fi);

int setForeignKey(int foreignIndex, struct table *tableList, int ti, int fi);
