#include <stdio.h>
#include <string.h>

#include "where_condition.h"
#include "jabberwocky_io.h"
#include "jabberwocky_postfix_parse.h"

int recursive_tree (char *in, struct node *parent, int shift){
	int i = strlen(in) - shift - 1;
	struct node *new_parent = 0;
	if (in[i] == '&' || in[i] == '=' || in[i] == '>' || in[i] == '<' || in[i] == '|'){
		new_parent = create_node(in[i--], 0, parent);
		recursive_tree(in, new_parent, strlen(in) - i + 1);
	}
	if (isspace(in[i]))
		i--;
	char *word = (char *)calloc(strlen(in), sizeof (char));
	int k = 0;
	while(isalnum(in[i]) || in[i] == '\'')
		word[k++] = in[i--];	
	word[k] = '\0';
	if (strlen(word))
		create_node(0, word, parent);
	free(word);
	return 0;
}

int parse_condition(char *condition) {
	size_t condition_len = strlen(condition);
	char *out = postfix_parse(condition);
	recursive_tree(out, 0, 0);
	
	
	free(out);
	return 0;
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
