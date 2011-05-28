#define base_accepting_chars "etaonrishdlcmupgywbvkjxzETAONRISHDLCMUPGYWBVKJXZ" // skip 1 char (guess what?)
#define free_accepting_chars "_1234567890"
#define trimming_chars " \n\t\r"
#define INT   "INTEGER" /// 1
#define FLOAT "FLOAT"   /// 2
#define CHAR  "CHAR"    /// 4


void trim(char* str);

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
