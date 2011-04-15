#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Jabberwocky_func.h"

int isDBExist(char *);
char * getDBPath();

int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s, <base name>\n", argv[0]);
       exit(-1);         
    }
    
    char *dbName = argv[1];
    if (!isDBExist(dbName)){
       printf("\nDatabase %s was not found, sorry.\n", dbName); // bad! an error is possible, not only file is not exist
       exit(-1);          
    }
    
    char *dbPath = getDBPath();
    char *buf = "/";
    strcat(buf, dbName);
    strcat(dbPath, buf);
    if (rmdir(dbPath) < 0){
       printf("\nError in deleting database.\n");
       exit(-1);          
    }
}
