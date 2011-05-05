#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "create_table.h"
#include "parsing_tools.h"

struct create *parse(char *create_query) {
	struct create *result = (struct create *) malloc(sizeof(struct create));

	trim(create_query);

	size_t query_len = strlen(create_query);

	size_t table_len = strcspn(create_query, "(");
	if (table_len == 0) {
		printf("table name must be not empty\n");
		return NULL;
	} else if (table_len == query_len) {
		printf("incorrect query: '(' not found\n");
		return NULL;
	} else if (table_len > 18) {
		printf("table name lenght must be <= 18\n");
		return NULL;
	}
	char *table_name = (char *) malloc(table_len * sizeof(char));
	strncpy(table_name, create_query, table_len);
	trim(table_name);
	if (check_identifier_name(table_name))
		return NULL;

	int i;
	for (i = table_len + 1; i < query_len; i++) {
		if (create_query[i] == ',')
			strcpy(create_query, table_name);
	}
	return result;
}

int create_data_file(char *table_name) {
	return 0;
}

int write_to_fd(int fd, char *query) {
	return 0;
}

/**
 * @param fd file descriptor main database file
 * @param create_query query such as "table_name (field0 type0 [constraints], ..., fieldN typeN [constraints])"
 */
int create_table(int fd, char *create_query) {
	struct create *result = parse(create_query);
	if (result == NULL)
		return -1;
	create_data_file(result->table_name);
	write_to_fd(fd, result->query);
	return 0;
}
