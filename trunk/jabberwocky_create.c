#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "Jabberwocky_func.h"

int isDBExist(char *);
char * getDBPath();

int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s, <base name>\n", argv[0]);
       exit(-1);         
    }
    
    char *dbName = argv[1];
    if (isDBExist(dbName)){
       printf("\nDatabase %s is already exist, sorry.\n", dbName);// bad! an error is possible, not only file is exist
       exit(-1);          
    }
    
    char *dbPath = getDBPath();
    char *buf = "/";
    strcat(buf, dbName);
    strcat(dbPath, buf);
    strcat(dbPath, buf); //  expansion????
    int fd = open(dbPath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0){
       printf("\nError in creating database.\n");
       exit(-1);  
    }
    close(fd);
    return 0;   
}
