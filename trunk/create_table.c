#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "create_table.h"
#include "parsing_tools.h"
#include "jabberwocky_io.h"

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
		printf("check_table_name_length error\n");
		return NULL;
	}
	char *table_name = (char *) calloc(table_len + 1, sizeof(char));
	strncpy(table_name, create_query, table_len);
	table_name = trim(table_name);
	if (check_identifier_name(table_name)) {
		printf("'%s' table_name check_identifier_name error\n", table_name);
		free(table_name);
		return NULL;
	}
	return table_name;
}

struct column_declare *parse_column_declare(char *column_declare_str) {
	struct column_declare *column = (struct column_declare *) malloc(sizeof(struct column_declare));
	column_declare_str = trim(column_declare_str);

	column->column_name = trim(cutTheFirstWord(column_declare_str, &column_declare_str));
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
	}
	return column;
}

int parse_columns(struct table *result, char *create_query, size_t query_len) {
	result->columns = NULL;
	result->column_count = 0;
	int i;
	//size_t column_declare_len = strcspn(create_query, ",)");
	for (i = 0; i < query_len; i++) {
		if (create_query[i] == ',') {
			// create_query[i] = ' ';
			strncpy(result->columns, create_query, i);
			result->columns = (struct column_declare *) realloc(
				result->columns, (result->column_count + 1) * sizeof(struct column_declare)
			);
			columns[column_number] = *parse_column_declare(create_query);
			result->column_count++;
		}
		else if (create_query[i] == ')'/* && i == query_len - 1*/) {
			//strcpy(columns, create_query);
			create_query[i] = '\0';
			//printf("##%s\n", create_query);
			result->columns = (struct column_declare *) realloc(
				result->columns, (result->column_count + 1) * sizeof(struct column_declare)
			);
			columns[column_number] = *parse_column_declare(create_query);
			//printf("%s - %d\n", columns[column_number].column_name, columns[column_number].type);
			result->column_count++;
		}
	}
	return columns;
}

struct table *parse(char *create_query) {
	struct table *result = (struct table *) malloc(sizeof(struct table));
	//create_query = trim(create_query);
	size_t query_len = strlen(create_query);
	size_t table_len = strcspn(create_query, "(");
	//printf("\n\n%s#%d#%d\n\n", create_query, query_len, table_len);

	result->table_name = parse_table_name(create_query, query_len, table_len);
	if (result->table_name == NULL) {
		printf("parse_table_name error\n");
		free(result);
		return NULL;
	}

	create_query += table_len + 1;
	query_len -= table_len + 1;
	result->columns = parse_columns(result, create_query, query_len);
	if (result->columns == NULL) {
		printf("parse_columns error\n");
		free(result);
		return NULL;
	}

	return result;
}

int create_table_data_file(char *table_name) {
	return 0;
}

int write_table_structure(int fd, struct table *new_table) {
	//printf("DEBUG: table structure:%s\n", query);
	writeTable(fd, *new_table);
	return 0;
}

int create_table(int fd, char *create_query, struct table *table_list) {
	struct table *result = parse(create_query);
	if (result == NULL) {
		printf("parse error\n");
		return -1;
	}
	create_table_data_file(result->table_name);
	write_table_structure(fd, result);
	return 0;
}
