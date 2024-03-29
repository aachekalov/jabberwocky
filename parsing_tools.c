#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parsing_tools.h"

char *trim(char *str) {
	size_t str_len = strlen(str);
	if (str_len > 0) {
		int i;
		for (i = 0; i < strlen(trimming_chars); i++)
			if (str[0] == trimming_chars[i]) {
				return trim(++str);
				//break;
			} else if (str[str_len - 1] == trimming_chars[i]) {
				str[str_len - 1] = '\0';
				trim(str);
				break;
			}
	}
	return str;
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
	if (!base_accept(c))
		return 0;
	int i;
	for (i = 0; i < strlen(free_accepting_chars); i++) {
		if (free_accepting_chars[i] == c)
			return 0;
	}
	return -1;
}

int check_identifier_name(char *name) {
	if (base_accept(name[0])) {
		printf("no '%c' character allowed\n", name[0]);
		printf("first char must be literal\n");
		return -1;
	}
	int i;
	//printf("len = %d\n", strlen(name));
	for (i = 1; i < strlen(name); i++) {
		if (free_accept(name[i])) {
			printf("no '%c' character allowed\n", name[i]);
			printf("char must be literal, digit or _\n");
			return -1;
		}
	}
	return 0;
}

char *cutTheFirstWord(char *query, char **newquery) {
	if (strlen(query) == 0)
		return calloc(1, sizeof(char));
	int len = strcspn(query, trimming_chars);
	char *firstWord = (char *) calloc(len + 1, sizeof(char));
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

int split(char *str, char *sprtr, char ***cols){	
	int len = 0, i = 0, j;
	int res = count(str, sprtr);
	printf("\nsplit: initial str = '%s', count = %d\n", str, res);
	char **c = (char **)malloc(res*sizeof(char *));
	for(j = 0; j < res; j++){//while(len < strlen(str)){
		len = strcspn(str, sprtr);
		char* buf = (char *)calloc(len, sizeof(char));
		strncpy(buf, str, len);
		c[i] = (char *)calloc((len+1), sizeof(char));
		strcpy(c[i], trim(buf));
		str += len + 1;
printf("split: i = %d, len = %d, c[i] = '%s'\n", i, len, c[i]);
		i++;
		free(buf);
	}
	*cols = c;
	return res;
}

int count (char *str, char *sprtr){
	int len = 0, i = 0;
	while(len < strlen(str)){
		len += strcspn(str + len, sprtr);
		len++;
		i++;
	}
	return i;
}
