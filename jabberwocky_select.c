#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "jabberwocky_func.h"
#include "drop_table.h"
#include "create_table.h"
#include "parsing_tools.h"
#include "jabberwocky_io.h"
#include "insert_into.h"

int getDBFD(char *);
char * getDBDirStr(char *base);


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
    char *dbPath = getDBDirStr(dbName);
    struct table *tableList = NULL;
    int q = readTables(fd, &tableList);    
    printf("\nq = %d\n", q);
    while(1){
       printf("%s > ", dbName);
       char *query, *firstWord;
       query = (char *)malloc(1024); //!!!
       fgets(query, 1024, stdin);
       char *newquery = trim(query);
       char *operation = cutTheFirstWord(newquery, &newquery);
       strup(operation); // Special for Alexey 
       newquery = trim(newquery);
       
       int ret = 0;
       if (!strcmp(operation, "CREATE")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             //TODO: firstWord == TABLE
             ret = create_table(fd, dbPath, trim(newquery), &tableList, q);
             if (ret == 0)
                q++;
             else printf("CREATE TABLE: FAIL\n");
       }else if (!strcmp(operation, "INSERT")){ 
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ninsertinto works with \"%s\"\n", newquery);
             ret = insert_into(trim(newquery), tableList, dbPath);  
       }else if (!strcmp(operation, "UPDATE")){
             printf("\nupdate works with \"%s\"\n", newquery);//ret = update(fd, newquery); 
       }else if (!strcmp(operation, "DROP")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ndroptable works with \"%s\"\n", newquery);
             ret = drop_table(fd, dbPath, trim(newquery), tableList, q);
             if (ret == 0)
                q--; 
       }else if (!strcmp(operation, "DELETE")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ndeletefrom works with \"%s\"\n", newquery);//ret = delete_from(fd, trim(newquery));      
       }else if (!strcmp(operation, "SELECT")){
             printf("\nselect works with \"%s\"\n", newquery);//ret = select(fd, newquery);
       }else if (!strcmp(operation, "QUIT") || !strcmp(operation, "Q") || 
				 !strcmp(operation, "EXIT") || !strcmp(operation, "E")){
		     free(query);
		     break;
	   }else if (!strcmp(operation, "")){
		     continue;     
       }else{
             ret = -1;   // here must be a timer, pthread_create and some cruel line   
       }
       if (ret < 0)
             printf("\tWrong query. Please, be more attentive.\n");
       
       free(query);
       
    }
    
    lseek(fd, 0, SEEK_SET);
    printf("\n1q = %d\n", q);
    writeTables(fd, tableList, q);
    printf("\n2q = %d\n", q);
    free(tableList);
    printf("\n3q = %d\n", q);
    free(dbPath);
    printf("\n4q = %d\n", q);
    close(fd);
    return 0;   
}

int getDBFD(char *base){
    char *dbPath = getDBPath();
    
    strcat(dbPath, "/");
    strcat(dbPath, base);
    strcat(dbPath, "/.");
    strcat(dbPath, base); 
    int fd = open(dbPath, O_RDWR);// | O_APPEND);
    
    free(dbPath);
    
    return fd;
}

char * getDBDirStr(char *base){
	char *dbPath = getDBPath();
    
    strcat(dbPath, "/");
    strcat(dbPath, base);
    strcat(dbPath, "/");
    
    return dbPath;
}
