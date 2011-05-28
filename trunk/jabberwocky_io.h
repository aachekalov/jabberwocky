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

int write (int fd, struct table newTable);

int read (int fd, struct table *tableList);

int getFieldIndex(int fd, char* tableName, char* fieldName);
