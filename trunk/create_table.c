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

char *parse_columns_declare(char *create_query, size_t query_len, size_t table_len) {
	char *columns_declare = ""; // TODO: malloc
	int i;
	for (i = table_len + 1; i < query_len; i++) {
		if (create_query[i] == ',')
			strcpy(columns_declare, create_query);
	}
	return columns_declare;
}

struct create *parse(char *create_query) {
	struct create *result = (struct create *) malloc(sizeof(struct create));
	trim(create_query);
	size_t query_len = strlen(create_query);
	size_t table_len = strcspn(create_query, "(");

	result->table_name = parse_table_name(create_query, query_len, table_len);
	if (result->table_name == NULL) {
		free(result);
		return NULL;
	}

	char *columns_declare = parse_columns_declare(create_query, query_len, table_len);
	if (columns_declare == NULL) {
		free(result);
		return NULL;
	}
	// result->query = result->table_name + '(' + columns_declare + ')';

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
	struct create *result = parse(create_query);
	if (result == NULL)
		return -1;
	create_table_data_file(result->table_name);
	write_table_structure(fd, result->query);
	return 0;
}
