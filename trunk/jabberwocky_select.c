#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Jabberwocky_func.h"

char * getDBPath();

int getDBFD(char *);
char *cutTheFirstWord(char **);

int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s, <base name>\n", argv[0]);
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
       strupr(operation); // Special for Alexey 
       
       int ret = 0;
       
       if (!strcmp(operation, "CREATE")){
             firstWord = cutTheFirstWord(&query);
             printf("\ncreatetable works with \"%s\"", query);//ret = createtable(fd, query);
       }else if (!strcmp(operation, "INSERT")){ 
             firstWord = cutTheFirstWord(&query);
             printf("\ninsertinto works with \"%s\"", query);//ret = insertinto(fd, query);  
       }else if (!strcmp(operation, "UPDATE")){
             printf("\nupdate works with \"%s\"", query);//ret = update(fd, query); 
       }else if (!strcmp(operation, "DROP")){
             firstWord = cutTheFirstWord(&query);
             printf("\ndroptable works with \"%s\"", query);//ret = droptable(fd, query);
       }else if (!strcmp(operation, "DELETE")){
             firstWord = cutTheFirstWord(&query);
             printf("\ndeletefrom works with \"%s\"", query);//ret = deletefrom(fd, query);      
       }else if (!strcmp(operation, "SELECT")){
             printf("\nselect works with \"%s\"", query);//ret = select(fd, query);
       }else{
             ret = -1;   // here must be a timer, pthread_create and some cruel line   
       }
       if (ret < 0)
             printf("\nWrong query. Please, be more attentive.\n");
              
       free(firstWord);
       free(operation);
       free(query);   
    }
    close(fd);
    return 0;   
}

int getDBFD(char *base){
    char *dbPath = getDBPath();
    char *buf = (char *)calloc(255, sizeof(char));
    *buf = '/';
    strcat(buf, base);
    strcat(dbPath, buf);
    strcat(dbPath, buf); //  extention - solved
    int fd = open(dbPath, O_RDWR);
    
    free(dbPath);
    free(buf);
    
    return fd;
}

char *cutTheFirstWord(char **query){
     int len = strcspn(*query, " "); 
     char *firstWord = (char *)calloc(255, sizeof(char));
     strncat(firstWord, (*query), len);
     (*query) += ++len;
     return firstWord; 
}
