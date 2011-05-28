#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "create_table.h"
#include "parsing_tools.h"

int check_table_name_length(size_t query_len, size_t table_len) {
	if (table_len == 0) {
		printf("table name must be not empty\n");
		return -1;
	} else if (table_len == query_len) {
		printf("incorrect query: '(' not found\n");
		return -1;
	} else if (table_len > 18) {
		printf("table name lenght must be <= 18\n");
		return -1;
	}
	return 0;
}

char *parse_table_name(char *create_query, size_t query_len, size_t table_len) {
	if (check_table_name_length(query_len, table_len)) {
		return NULL;
	}
	char *table_name = (char *) malloc(table_len * sizeof(char));
	strncpy(table_name, create_query, table_len);
	trim(table_name);
	if (check_identifier_name(table_name)) {
		free(table_name);
		return NULL;
	}
	return table_name;
}

struct column_declare parse_column_declare(char *column_declare_str) {
	struct column_declare column;
	trim(column_declare_str);

	//column.column_name = next_token(column_declare_str);

	//column.type = next_token(column_declare_str);

	//column.constraints = next_token(column_declare_str);

	return column;
}

struct column_declare *parse_columns(char *create_query, size_t query_len, size_t table_len) {
	struct column_declare *columns;
	int column_number = 0;
	int i;
	for (i = table_len + 1; i < query_len; i++) {
		if (create_query[i] == ',') {
			//strcpy(columns, create_query);
			columns = (struct column_declare *) realloc(columns, (column_number + 1) * sizeof(struct column_declare));
			columns[column_number] = parse_column_declare(create_query);
			column_number++;
		}
		else if (create_query[i] == ')' && i == query_len - 1) {
			//strcpy(columns, create_query);
			columns = (struct column_declare *) realloc(columns, (column_number + 1) * sizeof(struct column_declare));
			columns[column_number] = parse_column_declare(create_query);
			column_number++;
		}
	}
	return columns;
}

struct table *parse(char *create_query) {
	struct table *result = (struct table *) malloc(sizeof(struct table));
	trim(create_query);
	size_t query_len = strlen(create_query);
	size_t table_len = strcspn(create_query, "(");

	result->table_name = parse_table_name(create_query, query_len, table_len);
	if (result->table_name == NULL) {
		free(result);
		return NULL;
	}

	result->columns = parse_columns(create_query, query_len, table_len);
	if (result->columns == NULL) {
		free(result);
		return NULL;
	}

	return result;
}

int create_table_data_file(char *table_name) {
	return 0;
}

int write_table_structure(int fd, char *query) {
	printf("DEBUG: table structure:%s\n", query);
	return 0;
}

int create_table(int fd, char *create_query) {
	struct table *result = parse(create_query);
	if (result == NULL)
		return -1;
	create_table_data_file(result->table_name);
	//write_table_structure(fd, result->query);
	return 0;
}
