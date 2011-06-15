#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "jabberwocky_func.h"

int removeDir(char *);

int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s, <base name>\n", argv[0]);
       exit(-1);         
    }
    
    char *dbName = argv[1];
    int exist = isDBExist(dbName);
    if (exist){
       printf("\nDatabase was not found or an error occurred.\n"); 
       exit(-1);          
    }
    
    char *dbPath = getDBPath();
    char *buf = (char *)malloc(255*sizeof(char));
    *buf = '/';
    strcat(buf, dbName);
    strcat(dbPath, buf);
    if (removeDir(dbPath)){
       printf("\nError in deleting database.\n");
       exit(-1);          
    }
    free(buf);
    free(dbPath);
    return 0;
}

int removeDir(char *dirPath) {
    strcat(dirPath, "/");
    DIR *pdir = opendir (dirPath);
    struct dirent *pent = NULL;
    if (pdir == NULL) { 
		perror("removeDir");
        return -1; 
    } 
    char *nextFile = (char *)malloc(255*sizeof(char));
	errno = 0;
    int counter = 1; // use this to skip the first TWO which cause an infinite loop (and eventually, stack overflow)
    while ((pent = readdir (pdir)) != NULL) { 
        if (counter > 2) {
            strcpy(nextFile, dirPath);
            strcat(nextFile, pent->d_name);
            //struct stat *s;
            //stat(nextFile, s); 
            //if (S_ISDIR(s->st_mode)) // TODO: @if@ for directory                 
                //removeDir(nextFile);
            //else
				remove(nextFile);
        } 
        else
			counter++;
    }
    
    if (errno){
		perror("removeDir");
		return -1;
	}
     
    closedir (pdir);
    if (rmdir(dirPath)) {
		perror("removeDir");
		return -1; 
	}
	free(nextFile);
    return 0;
}
