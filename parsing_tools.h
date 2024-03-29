#define base_accepting_chars "etaonrishdlfcmupgywbvkjxzqETAONRISHDLFCMUPGYWBVKJXZQ"
#define free_accepting_chars "_1234567890"
#define trimming_chars " \t\n\r"

#define INT   "INTEGER" /// 1
#define FLOAT "FLOAT"   /// 2
#define CHAR  "CHAR"    /// 4

#define NOT     "NOT"     /// 1
#define _NULL   "NULL"    /// 0
#define UNIQUE  "UNIQUE"  /// 2
#define PRIMARY "PRIMARY" /// 4
#define FOREIGN "FOREIGN" /// 8
#define KEY     "KEY"

char *trim(char* str);

/**
 * Test 1st character in identifier name
 * @param c character to test
 * @return 0 if accept else -1
 */
int base_accept(char c);

/**
 * Test character in identifier name
 * @param c character to test
 * @return 0 if accept else -1
 */
int free_accept(char c);

/**
 * Check identifier name: [:alpha:][:word:]*
 */
int check_identifier_name(char *name);

char *cutTheFirstWord(char *query, char **newquery);

void strup(char *str);

int split(char *str, char *sprtr, char ***cols);

int count (char *str, char *sprtr);
