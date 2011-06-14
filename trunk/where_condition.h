struct leaf {
	char *column_value;
	int *ivalue;
	float *fvalue;
	char *cvalue;
}

struct node {
	char operation;
	char *operand1;
	char *operand2;
}

int parse_condition(char *condition);

int open_data_file(char *db_path, char *table_name);

int[] where_condition(struct table *table, char *db_path, char *condition);
