#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "jabberwocky_func.h"
#include "create_table.h"
#include "parsing_tools.h"
#include "jabberwocky_io.h"

int getDBFD(char *);


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
    
    //TODO: вывалить в массив структур
    struct table *tableList;
    readTable(fd, tableList);
    while(1){
       printf("%s > ", dbName);
       char *query, *firstWord;
       query = (char *)malloc(1024);
       fgets(query, 1024, stdin);
       char *trimmed_query = trim(query);
       char *newquery;
       char *operation = cutTheFirstWord(trimmed_query, &newquery);
       strup(operation); // Special for Alexey 
       
       int ret = 0;
       if (!strcmp(operation, "CREATE")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ncreatetable works with \"%s\"\n", newquery);
             ret = create_table(fd, newquery, tableList);
       }else if (!strcmp(operation, "INSERT")){ 
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ninsertinto works with \"%s\"\n", newquery);//ret = insert_into(fd, query);  
       }else if (!strcmp(operation, "UPDATE")){
             printf("\nupdate works with \"%s\"\n", newquery);//ret = update(fd, query); 
       }else if (!strcmp(operation, "DROP")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ndroptable works with \"%s\"\n", newquery);//ret = drop_table(fd, query);
       }else if (!strcmp(operation, "DELETE")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ndeletefrom works with \"%s\"\n", newquery);//ret = delete_from(fd, query);      
       }else if (!strcmp(operation, "SELECT")){
             printf("\nselect works with \"%s\"\n", newquery);//ret = select(fd, query);
       }else if (!strcmp(operation, "QUIT") || !strcmp(operation, "Q") || 
				 !strcmp(operation, "EXIT") || !strcmp(operation, "E")){
		     // TODO: free memory
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
    close(fd);
    return 0;   
}

int getDBFD(char *base){
    char *dbPath = getDBPath();
    
    strcat(dbPath, "/");
    strcat(dbPath, base);
    strcat(dbPath, "/.");
    strcat(dbPath, base); //  extention - solved
    int fd = open(dbPath, O_RDWR | O_APPEND);
    
    free(dbPath);
    
    return fd;
}
