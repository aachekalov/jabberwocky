#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
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
       printf("\nDatabase %s was not found, sorry.\n", dbName); // bad! an error is possible, not only file is not exist
       exit(-1);         
    }
    
    while(1){
       printf("%s > ", dbName);
       char *query;
       scanf("%s", query);
       char *operation = cutTheFirstWord(&query); 
       strupr(operation); // Special for Alexey 
       int ret = 0;
       if (!strcmp(operation, "CREATE")){
             cutTheFirstWord(&query);
             ret = createtable(fd, query);
       }else if (!strcmp(operation, "INSERT")){ 
             cutTheFirstWord(&query);
             ret = insertinto(fd, query);  
       }else if (!strcmp(operation, "UPDATE")){
             ret = update(fd, query); 
       }else if (!strcmp(operation, "DROP")){
             cutTheFirstWord(&query);
             ret = droptable(fd, query);
       }else if (!strcmp(operation, "DELETE")){
             cutTheFirstWord(&query);
             ret = deletefrom(fd, query);      
       }else if (!strcmp(operation, "SELECT")){
             ret = select(fd, query);
       }else{
             ret = -1;   // here must be a timer, pthread_create and some cruel line   
       }
       if (ret < 0)
             printf("\nWrong query. Please, be more attentive.\n");    
    }
    close(fd);
    return 0;   
}

int getDBFD(char *base){
    char *dbPath = getDBPath();
    char *buf = "/";
    strcat(buf, base);
    strcat(dbPath, buf);
    strcat(dbPath, buf); //  extention????
    
    return open(dbPath, O_RDWR);
}

char *cutTheFirstWord(char **query){
     int len = strcspn((*query), " "); 
     char *firstWord = "";
     strncat(firstWord, (*query), len);
     (* query) += ++len;
     return firstWord; 
}
