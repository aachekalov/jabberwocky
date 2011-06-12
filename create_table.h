//#include "jabberwocky_io.h"

int check_table_name_length(size_t table_len);
char *parse_table_name(char *create_query, size_t table_len);

struct table *create_table_by_name(char *query, size_t table_len);
struct column_declare *create_column_by_name(char *column_name);

struct column_declare *parse_column_declare(char *column_declare_str);
int parse_columns(struct table *result, char *columns_str);

struct table *parse(char *create_query);

int create_table_data_file(char *db_path, char *table_name);

int write_table_structure(int fd, struct table *new_table, struct table **table_list, int size);

int is_in_table_list(char *table_name, struct table *table_list, int size);

/**
 * @param fd file descriptor main database file
 * @param create_query query such as "table_name (column_name0 column_type0 [constraints],
 *                                      ... ..., column_nameN column_typeN [constraints])"
 */
int create_table(int fd, char *db_path, char *create_query, struct table **table_list, int size);
