#include <stdio.h>
#include <string.h>

#include "where_condition.h"
#include "jabberwocky_io.h"

int parse_condition(char *condition) {
	size_t condition_len = strlen(condition);
	int i;
	for (i = 0; i < condition_len; i++) {
		
	}
}

int open_data_file(char *db_path, char *table_name) {
	size_t db_path_len = strlen(db_path);
	size_t table_name_len = strlen(table_name);
	char *pathname = (char *) calloc(db_path_len + table_name_len + 1, sizeof(char));
	strcpy(pathname, db_path);
	strcat(pathname, table_name);
	int fd = open(pathname, O_RDONLY);
	free(pathname);
	return fd;
}

int[] where_condition(struct table *table, char *db_path, char *condition) {
	parse_condition(condition);

	int fd;
	if ((fd = open_data_file(db_path, result->table_name)) < 0) {
		perror("open_data_file");
		return -1;
	}

	close(fd);
	return 0;
}
