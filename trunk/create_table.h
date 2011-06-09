#include "jabberwocky_io.h"

int check_table_name_length(size_t table_len);
char *parse_table_name(char *create_query, size_t table_len);

struct column_declare *parse_column_declare(char *column_declare_str);
int parse_columns(struct table *result, char *columns_str);

struct table *parse(char *create_query);

int create_table_data_file(char *table_name);

int write_table_structure(int fd, struct table *new_table);

/**
 * @param fd file descriptor main database file
 * @param create_query query such as "table_name (column_name0 column_type0 [constraints],
 *                                      ... ..., column_nameN column_typeN [constraints])"
 */
int create_table(int fd, char *create_query, struct table *table_list);
