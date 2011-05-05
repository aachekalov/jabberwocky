#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "jabberwocky_func.h"
#include "create_table.h"

char * getDBPath();

int getDBFD(char *);
char *cutTheFirstWord(char **);

int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s <base name>\n", argv[0]);
       exit(-1);         
    }
    
    char *dbName = argv[1];
    int fd = getDBFD(dbName);
    if (fd < 0){
       perror("\nError in db opening"); 
       exit(-1);         
    }
    
    while(1){
       printf("%s > ", dbName);
       char *query, *firstWord;
       query = (char *)malloc(511);
       scanf("%s", query);
       char *operation = cutTheFirstWord(&query); 
       //operation = strupr(operation); // Special for Alexey 
       
       int ret = 0;
       
       if (!strcmp(operation, "CREATE")){
             firstWord = cutTheFirstWord(&query);
             printf("\ncreatetable works with \"%s\"", query);
             ret = create_table(fd, query);
       }else if (!strcmp(operation, "INSERT")){ 
             firstWord = cutTheFirstWord(&query);
             printf("\ninsertinto works with \"%s\"", query);//ret = insert_into(fd, query);  
       }else if (!strcmp(operation, "UPDATE")){
             printf("\nupdate works with \"%s\"", query);//ret = update(fd, query); 
       }else if (!strcmp(operation, "DROP")){
             firstWord = cutTheFirstWord(&query);
             printf("\ndroptable works with \"%s\"", query);//ret = drop_table(fd, query);
       }else if (!strcmp(operation, "DELETE")){
             firstWord = cutTheFirstWord(&query);
             printf("\ndeletefrom works with \"%s\"", query);//ret = delete_from(fd, query);      
       }else if (!strcmp(operation, "SELECT")){
             printf("\nselect works with \"%s\"", query);//ret = select(fd, query);
       }else if (!strcmp(operation, "EXIT")){
		     // TODO: free memory
		     return 0;
       }else{
             ret = -1;   // here must be a timer, pthread_create and some cruel line   
       }
       if (ret < 0)
             printf("\tWrong query. Please, be more attentive.\n");
       // TODO: fix pointers
       /*free(firstWord);
       free(operation);
       free(query);*/
    }
    close(fd);
    return 0;   
}

int getDBFD(char *base){
    char *dbPath = getDBPath();
     
    strcat(dbPath, "/");
    strcat(dbPath, base);
    strcat(dbPath, "/.");
    strcat(dbPath, base); //  extention - solved
    int fd = open(dbPath, O_RDWR);
    
    free(dbPath);
    
    return fd;
}

char *cutTheFirstWord(char **query){
     int len = strcspn(*query, " "); 
     char *firstWord = (char *)calloc(255, sizeof(char));
     strncpy(firstWord, (*query), len);
     (*query) += ++len;
     return firstWord; 
}
