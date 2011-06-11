#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "parsing_tools.h"
#include "jabberwocky_io.h" //moved to "create_table.h"
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

struct table *create_table_by_name(char *query, size_t table_len) {
	size_t query_len = strlen(query);
	if (!query_len) {
		printf("PARSE ERROR: table name must be not empty\n");
		return NULL;
	}

	if (table_len == query_len) {
		printf("PARSE ERROR: column not found\n");
		return NULL;
	}

	if (query[query_len - 1] == ';') {
		query[--query_len] = '\0';
	}
	trim(query);
	if (query[query_len - 1] == ')') {
		query[--query_len] = '\0';
	} else {
		printf("PARSE ERROR: check )\n");
		return NULL;
	}

	struct table *result = (struct table *) malloc(sizeof(struct table));

	result->table_name = parse_table_name(query, table_len);
	if (!result->table_name) {
		printf("CALL: parse_table_name\n");
		free(result);
		return NULL;
	}
	printf("DEBUG: table name '%s'\n", result->table_name);

	return result;
}

struct column_declare *create_column_by_name(char *column_name) {
	if (check_identifier_name(column_name)) {
		printf("CALL: check_identifier_name\n");
		return NULL;
	}
	struct column_declare *column = (struct column_declare *) malloc(sizeof(struct column_declare));
	column->column_name = (char *) calloc(strlen(column_name) + 1, sizeof(char));
	strcpy(column->column_name, column_name);
	printf("DEBUG: column name '%s'\n", column->column_name);
	return column;
}

struct column_declare *parse_column_declare(char *column_declare_str) {
	if (!(*column_declare_str)) {
		printf("PARSE ERROR: empty column declare\n");
		return NULL;
	}

	printf("DEBUG: the column declare '%s'\n", column_declare_str);

	char *next_token = column_declare_str;

	char *column_name = cutTheFirstWord(next_token, &next_token);
	struct column_declare *column = create_column_by_name(column_name);
	if (!column) {
		printf("CALL: create_column_by_name\n");
		return NULL;
	}

	char *type = trim(cutTheFirstWord(next_token, &next_token));
	if (!(*type)) {
		printf("PARSE ERROR: column type not found\n");
		return NULL;
	}
	strup(type);
	if (!strcmp(type, INT)) {
		column->type = 1;
	} else if (!strcmp(type, FLOAT)) {
		column->type = 2;
	} else if (!strcmp(type, CHAR)) {
		column->type = 4;
	} else {
		printf("PARSE ERROR: type '%s' not allowed\n", type);
		free(column->column_name);
		free(column);
		return NULL;
	}
	printf("DEBUG: column type %d\n", column->type);

	char *constraint = trim(cutTheFirstWord(next_token, &next_token));
	column->constraints = 0;
	while (!strcmp(constraint, "")) {
		strup(constraint);
		if (!strcmp(constraint, NOT)) {
			constraint = trim(cutTheFirstWord(next_token, &next_token));
			strup(constraint);
			if (!strcmp(constraint, _NULL)) {
				column->constraints += 1;
			} else {
				printf("PARSE ERROR: NULL must be after NOT\n");
				free(column->column_name);
				free(column);
				return NULL;
			}
		} else if (!strcmp(constraint, _NULL)) {
			// nothing
		} else if (!strcmp(constraint, UNIQUE)) {
			column->constraints += 2;
		} else if (!strcmp(constraint, PRIMARY)) {
			constraint = trim(cutTheFirstWord(next_token, &next_token));
			strup(constraint);
			if (!strcmp(constraint, KEY)) {
				column->constraints += 4;
			} else {
				printf("PARSE ERROR: KEY must be after PRIMARY\n");
				free(column->column_name);
				free(column);
				return NULL;
			}
		} else if (!strcmp(constraint, FOREIGN)) {
			constraint = trim(cutTheFirstWord(next_token, &next_token));
			strup(constraint);
			if (!strcmp(constraint, KEY)) {
				column->constraints += 8;
				char *foreign = trim(cutTheFirstWord(next_token, &next_token));
				printf("DEBUG: foreign key '%s'\n", foreign);
				size_t table_len = strcspn(foreign, "(");
				column->foreign_table = create_table_by_name(foreign, table_len);
				if (!(column->foreign_table)) {
					printf("CALL: create_table_by_name\n");
					free(column->column_name);
					free(column);
					return NULL;
				}
				foreign += table_len + 1;
				column->foreign_key = create_column_by_name(trim(foreign));
				if (!(column->foreign_key)) {
					printf("CALL: create_column_by_name\n");
					free(column->foreign_table);
					free(column->column_name);
					free(column);
					return NULL;
				}
			} else {
				printf("PARSE ERROR: KEY must be after FOREIGN\n");
				free(column->column_name);
				free(column);
				return NULL;
			}
		} else {
			printf("constraint '%s' not allowed\n", constraint);
			return NULL;
		}
		constraint = trim(cutTheFirstWord(next_token, &next_token));
	}
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
	size_t table_len = strcspn(create_query, "(");
	struct table *result = create_table_by_name(create_query, table_len);
	if (!result) {
		printf("CALL: create_table_by_name\n");
		return NULL;
	}

	create_query += table_len + 1;
	if (parse_columns(result, trim(create_query))) {
		printf("CALL: parse_columns\n");
		free(result->table_name);
		free(result);
		return NULL;
	}

	return result;
}

int create_table_data_file(char *db_path, char *table_name) {
	int fd = open(table_name, O_WRONLY | O_CREAT | O_TRUNC, 0744);
	if (fd < 0) {
       perror("\nError in creating database");
       exit(-1);  
    }
	return close(fd);
}

int write_table_structure(int fd, struct table *new_table, struct table **table_list, int size) {
	*table_list = (struct table *)realloc(*table_list, (size + 1) * sizeof(struct table));
	(*table_list)[size] = *new_table;
	return 0;
}

int create_table(int fd, char *db_path, char *create_query, struct table **table_list, int size) {
	printf("DEBUG: initial create query '%s'\n", create_query);
	struct table *result = parse(create_query);
	if (!result) {
		printf("CALL: parse\n");
		return -1;
	}
	// TODO: check table not exists;
	// TODO: chech constraints (foreign key, ...)
	write_table_structure(fd, result, &table_list, size);
	return create_table_data_file(db_path, result->table_name);
}
