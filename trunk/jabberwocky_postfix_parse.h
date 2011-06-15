 /* Описание стpуктуpы(элемента стека) */
struct st{ 
	char c;
	struct st *next;
};
struct st *push(struct st *, char); 
char DEL(struct st **);
int PRIOR(char);

char * postfix_parse(char *a);
