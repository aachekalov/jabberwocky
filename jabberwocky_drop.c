#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
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
       perror("\nDatabase was not found or an error occurred"); // !!
       exit(-1);          
    }
    
    char *dbPath = getDBPath();
    char *buf = (char *)malloc(255*sizeof(char));
    *buf = '/';
    strcat(buf, dbName);
    strcat(dbPath, buf);
    if (rmdir(dbPath) < 0){
       perror("\nError in deleting database");
       exit(-1);          
    }
    free(buf);
    free(dbPath);
    return 0;
}
