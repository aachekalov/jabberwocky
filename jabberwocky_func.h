#include <stdlib.h>

char * getDBPath(){
     char *homePath = getenv("HOME");
    char *dbPath = (char *)malloc(255*sizeof(char));
    strcpy(dbPath, homePath);
    strcat(dbPath, "/Jabberwocky/DBs");
    return dbPath;
}

int isDBExist(char *base){
    struct dirent *entry;
    int ret = 1;
    char *dbPath = getDBPath(); 
    DIR *dir = opendir(dbPath);
    
    if (!dir)
      ret = -1;
    
    errno = 0;
    while((entry = readdir(dir)) != NULL)
            if(!strcmp(entry->d_name, base)){
                 ret = 0;
                 break;
            }
    
    if (errno && !entry)
       ret = -1;
    
    free(dbPath);
    closedir(dir);
    return ret;
}
