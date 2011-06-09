#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "parsing_tools.h"
//#include "jabberwocky_io.h" moved to "create_table.h"
#include "create_table.h"

int check_table_name_length(size_t table_len) {
	if (table_len == 0) {
		printf("table name must be not empty\n");
		return -1;
	} else if (table_len > 18) {
		printf("table name lenght must be <= 18\n");
		return -1;
	}
	return 0;
}

char *parse_table_name(char *create_query, size_t table_len) {
	if (check_table_name_length(table_len)) {
		printf("CALL: check_table_name_length\n");
		return NULL;
	}

	char *table_name = (char *) calloc(table_len + 1, sizeof(char));
	strncpy(table_name, create_query, table_len);
	trim(table_name);
	if (check_identifier_name(table_name)) {
		printf("CALL: check_identifier_name\n");
		free(table_name);
		return NULL;
	}
	return table_name;
}

struct column_declare *parse_column_declare(char *column_declare_str) {
	printf("DEBUG: the column declare '%s'\n", column_declare_str);

	struct column_declare *column = (struct column_declare *) malloc(sizeof(struct column_declare));

	/*column->column_name = trim(cutTheFirstWord(column_declare_str, &column_declare_str));
	if (check_identifier_name(column->column_name)) {
		printf("'%s' column_name check_identifier_name error\n", column->column_name);
		return NULL;
	}

	trim(column_declare_str);
	char *type = cutTheFirstWord(column_declare_str, &column_declare_str);
	strup(type);
	if (!strcmp(type, INT)) {
		column->type = 1;
	} else if (!strcmp(type, FLOAT)) {
		column->type = 2;
	} else if (!strcmp(type, CHAR)) {
		column->type = 4;
	} else {
		printf("type '%s' no allowed\n", type);
		return NULL;
	}

	column_declare_str = trim(column_declare_str);
	char *constraint = cutTheFirstWord(column_declare_str, &column_declare_str);
	while (strcmp(constraint, "")) {
		printf("##'%s'", constraint);
		strup(constraint);
		if (!strcmp(constraint, NOT)) {
			trim(column_declare_str);
			if (!strcmp(cutTheFirstWord(column_declare_str, &column_declare_str), _NULL)) {
				column->constraints += 1;
			} else {
				printf("NULL sould be after NOT\n");
				return NULL;
			}
		} else if (!strcmp(constraint, _NULL)) {
			column->constraints += 0;
		} else if (!strcmp(constraint, UNIQUE)) {
			column->constraints += 2;
		} else if (!strcmp(constraint, PRIMARY)) {
			trim(column_declare_str);
			if (!strcmp(cutTheFirstWord(column_declare_str, &column_declare_str), KEY)) {
				column->constraints += 4;
			} else {
				printf("KEY is missing\n");
				return NULL;
			}
		} else if (!strcmp(constraint, FOREIGN)) {
			trim(column_declare_str);
			if (!strcmp(cutTheFirstWord(column_declare_str, &column_declare_str), KEY)) {
				column->constraints += 8;
				char *foreign_table = trim(cutTheFirstWord(column_declare_str, &column_declare_str));
			} else {
				printf("KEY is missing\n");
				return NULL;
			}
		} else {
			printf("constraint '%s' no allowed\n", constraint);
			return NULL;
		}
		constraint = cutTheFirstWord(column_declare_str, &column_declare_str);
		trim(column_declare_str);
	}*/
	return column;
}

int parse_columns(struct table *result, char *columns_str) {
	printf("DEBUG: columns declare '%s'\n", columns_str);

	result->columns = NULL;
	result->column_count = 0;

	size_t column_len;
	while (column_len = strcspn(columns_str, ",")) {
		char *column_declare_str = (char *) calloc(column_len + 1, sizeof(char));
		strncpy(column_declare_str, columns_str, column_len);
		struct column_declare *column = parse_column_declare(trim(column_declare_str));
		if (!column) {
			printf("CALL: parse_column_declare\n");
			free(column_declare_str);
			if (!result->columns) {
				// TODO: free(result->columns);
			}
			return -1;
		}
		result->columns = (struct column_declare *) realloc(
			result->columns,
			(result->column_count + 1) * sizeof(struct column_declare)
		);
		result->columns[result->column_count] = *column;
		result->column_count++;
		free(column_declare_str);
		columns_str += column_len + 1;
	}
	columns_str--;
	if (*columns_str) {
		if (!result->columns) {
			// TODO: free(result->columns);
		}
		printf("PARSE ERROR: check , in columns declare\n");
		return -1;
	}
	return 0;
}

struct table *parse(char *create_query) {
	size_t query_len = strlen(create_query);
	if (!query_len) {
		printf("PARSE ERROR: empty query\n");
		return NULL;
	}

	size_t table_len = strcspn(create_query, "(");
	if (table_len == query_len) {
		printf("PARSE ERROR: columns declare not found\n");
		return NULL;
	}

	if (create_query[query_len - 1] == ';') {
		create_query[--query_len] = '\0';
	}
	trim(create_query);
	if (create_query[query_len - 1] == ')') {
		create_query[--query_len] = '\0';
	} else {
		printf("PARSE ERROR: check ) at the end\n");
		return NULL;
	}

	struct table *result = (struct table *) malloc(sizeof(struct table));

	result->table_name = parse_table_name(create_query, table_len);
	if (!result->table_name) {
		printf("CALL: parse_table_name\n");
		free(result);
		return NULL;
	}
	printf("DEBUG: table_name '%s'\n", result->table_name);

	create_query += table_len + 1;
	if (parse_columns(result, trim(create_query))) {
		printf("CALL: parse_columns\n");
		free(result->table_name);
		free(result);
		return NULL;
	}

	return result;
}

int create_table_data_file(char *table_name) {
	int fd = open(table_name, O_WRONLY | O_CREAT | O_TRUNC, 0744);
    if (fd < 0){
       perror("\nError in creating database");
       exit(-1);  
    }
	return 0;
}

int write_table_structure(int fd, struct table *new_table) {
	writeTable(fd, *new_table);
	return 0;
}

int create_table(int fd, char *create_query, struct table *table_list) {
	printf("DEBUG: initial create query '%s'\n", create_query);
	struct table *result = parse(create_query);
	if (!result) {
		printf("CALL: parse\n");
		return -1;
	}
	// TODO: check table not exists;
	// TODO: chech constraints (foreign key, ...)
	//write_table_structure(fd, result);
	//create_table_data_file(result->table_name);
	printf("CREATE TABLE: SUCCESS\n");
	return 0;
}
