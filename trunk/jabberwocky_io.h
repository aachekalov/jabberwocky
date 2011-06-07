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

int writeTable (int fd, struct table newTable);

int readTable (int fd, struct table *tableList);

int getFieldIndex(int fd, char* tableName, char* fieldName);

int setForeignKey(int foreignIndex, struct table *tableList, int structSize, int *a, int aSize, int ti, int fi);
