#include <stdlib.h>

char * getDBPath(){
     char *homePath = getenv("HOME");
    char *dbPath = 0;
    strcpy(dbPath, homePath);
    strcat(dbPath, "/Jabberwocky/DBs");
    return dbPath;
}

int isDBExist(char *base){
    struct dirent *entry;
    int ret = -1;
    DIR *dir = opendir(getDBPath());
    
    errno = 0;
    while((entry = readdir(dir)) != NULL)
            if(!strcmp(entry->d_name, base)){
                 ret = 0;
                 break;
            }
    
    if (errno && !entry){
       closedir(dir);
       return -1;
    }
    
    closedir(dir);
    return ret;
}
