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

int getDBFD(char *str, int flags);
char * getDBDirStr(char *base);


int main(int argc, char *argv[]){
    if (argc != 2){
       printf("\nUsage: %s <base name>\n", argv[0]);
       exit(-1);         
    }
    
    char *dbName = argv[1];
    int fd = getDBFD(dbName, O_RDONLY);
    if (fd < 0){
       perror("\nError in db opening"); 
       exit(-1);         
    }
    char *dbPath = getDBDirStr(dbName);

    struct table *tableList = NULL;
    int q = readTables(fd, &tableList); 

	int dirtyBit = 0;   
    while(1){        
       printf("%s > ", dbName);
       char *query, *firstWord;
       query = (char *)calloc(1024, sizeof(char)); //!!!
       fgets(query, 1024, stdin);
       char *newquery = trim(query);
       char *operation = cutTheFirstWord(newquery, &newquery);
       strup(operation); // Special for Alexey 
       newquery = trim(newquery);

       
       int ret = 0;
       if (!strcmp(operation, "CREATE")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             ret = create_table(fd, dbPath, trim(newquery), &tableList, q); 
             if (ret == 0){
				dirtyBit = 1;
                q++; 
	     	 }
       }else if (!strcmp(operation, "INSERT")){ 
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ninsertinto works with \"%s\"\n", newquery);
             ret = insert_into(trim(newquery), tableList, q, dbPath);  
       }else if (!strcmp(operation, "UPDATE")){
             printf("\nupdate works with \"%s\"\n", newquery);//ret = update(fd, newquery); 
       }else if (!strcmp(operation, "DROP")){
             firstWord = cutTheFirstWord(newquery, &newquery);
             printf("\ndroptable works with \"%s\"\n", newquery);
             ret = drop_table(fd, dbPath, trim(newquery), &tableList, q);
             if (ret == 0){
				dirtyBit = 1;
                q--; 
	      	 }
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
             printf("\n\tWrong query. Please, be more attentive.\n");
        
       free(query);
    }
	
    
    if(dirtyBit){
	    close(fd);
		fd = getDBFD(dbName, O_WRONLY|O_TRUNC);
		if (fd < 0){
       		perror("\nError in db file opening"); 
       		exit(-1);         
    	}
		lseek(fd, 0, SEEK_SET);
	    writeTables(fd, tableList, q);
    }
	int i, j;
	for (i = 0; i < q; i++){
		for (j = 0; j < tableList[i].column_count; j++)
			free(tableList[i].columns[j].column_name);
		free(tableList[i].columns);
		free(tableList[i].table_name);		
	} 
	free(tableList);
    free(dbPath);
    close(fd);
    return 0;   
}

int getDBFD(char *base, int flags){
    char *dbPath = getDBPath();
    
    strcat(dbPath, "/");
    strcat(dbPath, base);
    strcat(dbPath, "/.");
    strcat(dbPath, base); 
    int fd = open(dbPath, flags);
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
