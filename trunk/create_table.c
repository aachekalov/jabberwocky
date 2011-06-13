#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "parsing_tools.h"
#include "jabberwocky_io.h"
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
		printf("PARSE ERROR: column is not found\n");
		return NULL;
	}

	if (query[--query_len] == ')') {
		query[query_len] = '\0';
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

int free_column(struct column_declare *column) {
	printf("DEBUG: free column '%s'\n", column->column_name);
	free(column->column_name);
	if (column->foreign_table) {
		printf("DEBUG: free foreign table '%s'\n", column->foreign_table->table_name);
		free(column->foreign_table->table_name);
		free(column->foreign_table);
	}
	if (column->foreign_key) {
		printf("DEBUG: free foreign key '%s'\n", column->foreign_key->column_name);
		free(column->foreign_key->column_name);
		free(column->foreign_key);
	}
	return 0;
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
	free(column_name);
	if (!column) {
		printf("CALL: create_column_by_name\n");
		return NULL;
	}

	column->foreign_table = NULL;
	column->foreign_key = NULL;

	char *word = cutTheFirstWord(next_token, &next_token);
	char *type = trim(word);
	if (!(*type)) {
		printf("PARSE ERROR: column type is not found\n");
		free(word);
		free_column(column);
		free(column);
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
		printf("PARSE ERROR: type '%s' is not allowed\n", type);
		free(word);
		free_column(column);
		free(column);
		return NULL;
	}
	printf("DEBUG: column type %d\n", column->type);
	free(word);

	word = cutTheFirstWord(next_token, &next_token);
	char *constraint = trim(word);
	column->constraints = 0;
	while (strcmp(constraint, "")) {
		strup(constraint);
		if (!strcmp(constraint, NOT)) {
			free(word);
			word = cutTheFirstWord(next_token, &next_token);
			constraint = trim(word);
			strup(constraint);
			if (!strcmp(constraint, _NULL)) {
				if ((column->constraints & 1) == 1) {
					printf("PARSE ERROR: duplicated NOT NULL\n");
					free(word);
					free_column(column);
					free(column);
					return NULL;
				}
				column->constraints += 1;
			} else {
				printf("PARSE ERROR: NULL must be after NOT\n");
				free(word);
				free_column(column);
				free(column);
				return NULL;
			}
		} else if (!strcmp(constraint, _NULL)) {
			// column->constraints += 0;
		} else if (!strcmp(constraint, UNIQUE)) {
			if ((column->constraints & 2) == 2) {
				printf("PARSE ERROR: duplicated UNIQUE\n");
				free(word);
				free_column(column);
				free(column);
				return NULL;
			}
			column->constraints += 2;
		} else if (!strcmp(constraint, PRIMARY)) {
			free(word);
			word = cutTheFirstWord(next_token, &next_token);
			constraint = trim(word);
			strup(constraint);
			if (!strcmp(constraint, KEY)) {
				if ((column->constraints & 4) == 4) {
					printf("PARSE ERROR: duplicated PRIMARY KEY\n");
					free(word);
					free_column(column);
					free(column);
					return NULL;
				}
				column->constraints += 4;
			} else {
				printf("PARSE ERROR: KEY must be after PRIMARY\n");
				free(word);
				free_column(column);
				free(column);
				return NULL;
			}
		} else if (!strcmp(constraint, FOREIGN)) {
			free(word);
			word = cutTheFirstWord(next_token, &next_token);
			constraint = trim(word);
			strup(constraint);
			if (!strcmp(constraint, KEY)) {
				if ((column->constraints & 8) == 8) {
					printf("PARSE ERROR: duplicated FOREIGN KEY\n");
					free(word);
					free_column(column);
					free(column);
					return NULL;
				}
				free(word);
				word = cutTheFirstWord(next_token, &next_token);
				column->constraints += 8;
				char *foreign = trim(word);
				printf("DEBUG: foreign key '%s'\n", foreign);
				size_t table_len = strcspn(foreign, "(");
				column->foreign_table = create_table_by_name(foreign, table_len);
				if (!(column->foreign_table)) {
					printf("CALL: create_table_by_name\n");
					free(word);
					free_column(column);
					free(column);
					return NULL;
				}
				foreign += table_len + 1;
				column->foreign_key = create_column_by_name(trim(foreign));
				if (!(column->foreign_key)) {
					printf("CALL: create_column_by_name\n");
					free(word);
					free_column(column);
					free(column);
					return NULL;
				}
				column->foreign_key->type = column->type;
			} else {
				printf("PARSE ERROR: KEY must be after FOREIGN\n");
				free(word);
				free_column(column);
				free(column);
				return NULL;
			}
		} else {
			printf("constraint '%s' is not allowed\n", constraint);
			free(word);
			free_column(column);
			free(column);
			return NULL;
		}
		free(word);
		word = cutTheFirstWord(next_token, &next_token);
		constraint = trim(word);
	}
	printf("DEBUG: column constraints %d\n", column->constraints);
	free(word);
	return column;
}

int free_all_columns(struct table *result) {
	int i;
	for (i = 0; i < result->column_count; i++) {
		free_column(&(result->columns[i]));
	}
	free(result->columns);
	return 0;
}

int parse_columns(struct table *result, char *columns_str) {
	printf("DEBUG: columns declare '%s'\n", columns_str);

	result->columns = NULL;
	result->column_count = 0;

	size_t column_len;
	while (column_len = strcspn(columns_str, ",")) {
		char *column_declare_str = (char *) calloc(column_len + 2, sizeof(char));
		strncpy(column_declare_str, columns_str, column_len);
		struct column_declare *column = parse_column_declare(trim(column_declare_str));
		if (!column) {
			printf("CALL: parse_column_declare\n");
			free(column_declare_str);
			if (result->columns) {
				free_all_columns(result);
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
		if (result->columns) {
			free_all_columns(result);
		}
		printf("PARSE ERROR: check , in columns declare\n");
		return -1;
	}
	return 0;
}

struct table *parse(char *create_query) {
	size_t query_len = strlen(create_query);
	if (create_query[--query_len] == ';') {
		create_query[query_len] = '\0';
	}
	trim(create_query);

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

int check_column_unique(struct table *result) {
	return 0;
}

int is_in_table_list(char *table_name, struct table *table_list, int size){
	int i;
	for (i = 0; i < size; i++)
		if (!strcmp(table_list[i].table_name, table_name))
			return -1;
	return 0;
}

int check_foreign_key(struct column_declare *column, struct table *table_list, int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		if (!strcmp(table_list[i].table_name, column->foreign_table->table_name)) {
			free(column->foreign_table->table_name);
			free(column->foreign_table);
			column->foreign_table = NULL;
			for (j = 0; j < table_list[i].column_count; j++) {
				if (!strcmp(table_list[i].columns[j].column_name, column->foreign_key->column_name) &&
					table_list[i].columns[j].type == column->foreign_key->type) {
					free(column->foreign_key->column_name);
					free(column->foreign_key);
					column->foreign_table = &(table_list[i]);
					column->foreign_key = &(table_list[i].columns[j]);
					return 0;
				}
				if (j == table_list[i].column_count - 1) {
					printf("ERROR: no such foreign key '%s' or types are different\n",
						column->foreign_key->column_name);
					return -1;
				}
			}
		}
		if (i == size - 1) {
			printf("ERROR: no such foreign table '%s'\n", column->foreign_table->table_name);
				return -1;
		}
	}
	return 0;
}

int check_constraints(struct table *result, struct table *table_list, int size) {
	int i;
	for (i = 0; i < result->column_count; i++) {
		if (result->columns[i].constraints >= 12) {
			printf("ERROR: PRIMARY KEY + FOREIGN KEY is not allowed\n");
			free_all_columns(result);
			free(result->table_name);
			return -1;
		}
	}

	for (i = 0; i < result->column_count; i++) {
		if (result->columns[i].foreign_table) {
			if (check_foreign_key(&(result->columns[i]), table_list, size)) {
				free_all_columns(result);
				free(result->table_name);
				return -1;
			}
		}
	}
	return 0;
}

int write_table_structure(int fd, struct table *new_table, struct table **table_list, int size) {
   *table_list = (struct table *)realloc(*table_list, (size + 1) * sizeof(struct table));
   (*table_list)[size] = *new_table;
   return 0;
}

int create_table_data_file(char *db_path, char *table_name) {
	size_t db_path_len = strlen(db_path);
	size_t table_name_len = strlen(table_name);
	char *pathname = (char *) calloc(db_path_len + table_name_len + 1, sizeof(char));
	strcpy(pathname, db_path);
	strcat(pathname, table_name);
	int fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(pathname);
	if (fd < 0) {
		perror("open");
	return -1;
	}
	return close(fd);
}

int create_table(int fd, char *db_path, char *create_query, struct table **table_list, int size) {
	printf("DEBUG: initial create query '%s'\n", create_query);
	struct table *result = parse(create_query);
	if (!result) {
		printf("CALL: parse\n");
		return -1;
	}

	if (check_column_unique(result)) {
		printf("ERROR: column names must be unique\n");
		free_all_columns(result);
		free(result->table_name);
		free(result);
		return -1;
	}

	if (is_in_table_list(result->table_name, *table_list, size)){
		printf("Error: table with name '%s' is already exists\n", result->table_name);
		free_all_columns(result);
		free(result->table_name);
		free(result);
		return -1;
	}

	if (check_constraints(result, *table_list, size)) {
		printf("CALL: check_constraints\n");
		free(result);
		return -1;
	}

	write_table_structure(fd, result, table_list, size);

	if (create_table_data_file(db_path, result->table_name)) {
		printf("CALL: create_table_data_file\n");
		return -1;
	}

	printf("CREATE TABLE: SUCCESS\n");
	return 0;
}
