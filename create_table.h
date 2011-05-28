struct column_declare {
	char *column_name;
	unsigned char type;
	char **constraints;
};

struct table {
	char *table_name;
	unsigned char column_count;
	struct column_declare *columns;
};

int check_table_name_length(size_t query_len, size_t table_len);
char *parse_table_name(char *create_query, size_t query_len, size_t table_len);

struct column_declare parse_column_declare(char *column_declare_str);
struct column_declare *parse_columns(char *create_query, size_t query_len, size_t table_len);

struct table *parse(char *create_query);

int create_table_data_file(char *table_name);

int write_table_structure(int fd, char *query);

/**
 * @param fd file descriptor main database file
 * @param create_query query such as "table_name (column_name0 column_type0 [constraints],
 *                                      ... ..., column_nameN column_typeN [constraints])"
 */
int create_table(int fd, char *create_query);