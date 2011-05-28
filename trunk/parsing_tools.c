#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsing_tools.h"

void trim(char* str) {
	size_t str_len = strlen(str);
	if (str_len > 0) {
		int i;
		for (i = 0; i < strlen(trimming_chars); i++)
			if (str[0] == trimming_chars[i]) {
				(*str)++;
				trim(str);
				break;
			} else if (str[str_len - 1] == trimming_chars[i]) {
				str[str_len - 1] = '\0';
				trim(str);
				break;
			}
	}
}

int base_accept(char c) {
	int i;
	for (i = 0; i < strlen(base_accepting_chars); i++) {
		if (base_accepting_chars[i] == c)
			return 0;
	}
	return -1;
}

int free_accept(char c) {
	if (base_accept(c))
		return -1;
	int i;
	for (i = 0; i < strlen(free_accepting_chars); i++) {
		if (free_accepting_chars[i] == c)
			return 0;
	}
	return -1;
}

int check_identifier_name(char *name) {
	if (base_accept(name[0])) {
		printf("no %c character allowed\n", name[0]);
		printf("first char must be literal\n");
		return -1;
	}
	int i;
	for (i = 1; i < strlen(name); i++) {
		if (free_accept(name[i])) {
			printf("no %c character allowed\n", name[i]);
			printf("char must be literal, digit or _\n");
			return -1;
		}
	}
	return 0;
}

char *cutTheFirstWord(char *query, char **newquery){
     int len = strcspn(query, " "); 
     if (len == strlen(query))
		return NULL;
     char *firstWord = (char *)calloc(1024, sizeof(char));
     strncpy(firstWord, query, len);
     (*newquery) = query + len + 1;
     return firstWord; 
}

void strup(char *str)
{ 
	char *s = str;
	while (*s){ 
		*s = toupper((unsigned char) *s); 
		s++; 
	} 
}
