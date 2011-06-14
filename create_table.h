int check_table_name_length(size_t table_len);
char *parse_table_name(char *create_query, size_t table_len);

struct table *create_table_by_name(char *query, size_t table_len);
struct column_declare *create_column_by_name(char *column_name);

int free_column(struct column_declare *column);
struct column_declare *parse_column_declare(char *column_declare_str);
int free_all_columns(struct table *result);
int parse_columns(struct table *result, char *columns_str);

struct table *parse_create_query(char *create_query);

int check_column_unique(struct table *result);
int is_in_table_list(char *table_name, struct table *table_list, int size);
int check_foreign_key(struct column_declare *column, struct table *table_list, int size);
int check_constraints(struct table *result, struct table *table_list, int size);

int write_table_structure(int fd, struct table *new_table, struct table **table_list, int size);

int create_table_data_file(char *db_path, char *table_name);

/**
 * @param fd file descriptor main database file
 * @param create_query query such as "table_name (column_name0 column_type0 [constraints],
 *                                      ... ..., column_nameN column_typeN [constraints])"
 */
int create_table(int fd, char *db_path, char *create_query, struct table **table_list, int size);
