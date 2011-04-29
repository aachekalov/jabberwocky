#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "jabberwocky_func.h"

int isDBExist(char *);
char * getDBPath();

int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s, <base name>\n", argv[0]);
       exit(-1);         
    }
    
    char *dbName = argv[1];
    if (!isDBExist(dbName)){
       perror("\nDatabase already exists or an error occurred");
       exit(-1);          
    }
    
    char *dbPath = getDBPath();
    strcat(dbPath, "/");
    strcat(dbPath, dbName);
    
    if(mkdir(dbPath, 0744)){
       perror("\nError in creating database");
       exit(-1);
    }      
    
    strcat(dbPath, "/.");
    strcat(dbPath, dbName);//  expansion - solved
    
    int fd = open(dbPath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0){
       perror("\nError in creating database");
       exit(-1);  
    }
    free(dbPath);
    close(fd);
    return 0;   
}
