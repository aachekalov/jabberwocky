struct create {
	char *table_name;
	char *query;
};

struct create *parse(char *create_query);

int create_data_file(char *table_name);

int write_to_fd(int fd, char *query);

/**
 * @param fd           -- File descriptor main database file
 * @param create_query -- Query "<table_name> (field0 type0 [constraints], ..., fieldN typeN [constraints])"
 */
int create_table(int fd, char *create_query);
