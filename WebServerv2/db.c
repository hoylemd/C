/* program to facilitate usage of a database*/

/*Command line arguments:
	clear : remove all records frm the database
	add # filename : adds the contents of the file named filename to the database with a primarykey of #
	remove # : removes the record qith the primary key #
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "hstring.h"

#define MAX_QUERY 512
#define BLOB_SIZE 65535
#define CHAR_SIZE 255
#define HOSTNAME  "dursley.cis.uoguelph.ca"
#define DEBUG 0
/*
	HOSTNAME can be defined with IP or hostname.

	eg: #define HOSTNAME "131.104.49.147"
	    #define HOSTNAME "gradpc-38.cis.uoguelph.ca"
	
*/
#define USERNAME  "mhoyle"
#define PASSWORD  "0553453"
#define DATABASE  "mhoyle"


	


int main(int argc, char* argv[]){
	MYSQL mysql;
	char query[MAX_QUERY];
	char fbuffer[BLOB_SIZE], fbuffer2[BLOB_SIZE], filequery[BLOB_SIZE + MAX_QUERY];
	int i = 0, errnum = 0;

	/*initializing strings*/
	for(i=0;i<MAX_QUERY;i++) query[i] = 0;
	for(i=0;i<BLOB_SIZE;i++) {fbuffer[i] = 0; fbuffer2[i] = 0;}
	for(i=0;i<(MAX_QUERY+BLOB_SIZE);i++) filequery[i] = 0;

	/* 
		Connect to database server.
		Username and password must be filled in here.
		If no username or password is stored in DB then use NULL.
	*/
	if (DEBUG) fprintf(stderr, "Connecting...\n");
	
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0)) {
	   if (DEBUG) fprintf(stderr, "Failed connecting to database\n");
	}
	
	/*create the table if it doesn't exist*/
	for(i=0;i<MAX_QUERY;i++) query[i] = 0;
	strcat(query, "create table if not exists pages (id int not null auto_increment,");
	sprintf(query, "%spage_content blob(%d),", query, BLOB_SIZE);
	sprintf(query, "%spage_name char(%d),", query, CHAR_SIZE);
	strcat(query, " primary key(id) )");


	if(mysql_query(&mysql, query)){
	  if(DEBUG)fprintf(stderr, "Could not create table!\n");
	}

	if (argc == 2){
		/*clear command*/
		if (!strcmp(argv[1], "clear")){

			if (DEBUG) fprintf(stderr, "clearing table\n");

			/*
				remove the table
			*/
			for(i=0;i<MAX_QUERY;i++) query[i] = 0;
			strncpy(query, "drop table pages", MAX_QUERY);
	
			if(mysql_query(&mysql,query))
				if(DEBUG) fprintf(stderr,"query failed: '%s'\n", query);

			/*
				recreate it
			*/
			for(i=0;i<MAX_QUERY;i++) query[i] = 0;
			strcat(query, "create table if not exists pages (id int not null auto_increment,");
			sprintf(query, "%spage_content blob(%d),", query, BLOB_SIZE);
			sprintf(query, "%spage_name char(%d),", query, CHAR_SIZE);
			strcat(query, " primary key(id) )");

			if(mysql_query(&mysql, query))
				if(DEBUG) fprintf(stderr,"query failed: '%s'\n", query);


		}
		/*remove command*/
	} else if (argc == 3){
		if (!strcmp(argv[1], "remove")){
			
			/*build the query*/
			for(i=0;i<MAX_QUERY;i++)query[i] = 0;
			sprintf(query, "delete from pages where id = %s", argv[2]);

			if (DEBUG) fprintf(stderr, "removing entry with primary key %s\n", argv[2]);
			
			/*send the query*/
			if(mysql_query(&mysql,query))
				if(DEBUG) fprintf(stderr, "query failed: '%s'\n", query);
		}
	} else if (argc == 4){
		/*add command*/
		if (!strcmp(argv[1], "add")){

			if (DEBUG) fprintf(stderr, "adding new entry '%s'\n", argv[3]);

			/*read in the file*/
			errnum = hreadfile(argv[3], fbuffer, BLOB_SIZE);

			/*handle errors*/
			if (errnum){
				if (errnum == -1 && DEBUG) fprintf(stderr, "File too large. could not read\n");
				else if (errnum == -2 && DEBUG) fprintf(stderr, "File does not exist. could not read\n");
				else if (DEBUG) fprintf(stderr, "Unknown error. error number: %d\n", errnum);
			} 
	
			if (errnum == 0){
				/*fix any ' in the file*/
				(hreplacestring(fbuffer, "'", "\\'",fbuffer2, BLOB_SIZE));
				
				if (DEBUG) fprintf(stderr, "file read. adding\n");
				if (DEBUG > 1) fprintf(stderr, "%s\n", fbuffer2);

				/*clear the query buffer*/
				for(i=0;i<MAX_QUERY;i++) query[i] = 0;

				/*build the initial query*/
				sprintf(query, "insert into pages values (%s,'PLACEHOLDER','%s')", argv[2], argv[3]);

				/*add the entry*/
				if(mysql_query(&mysql,query)){
					if(DEBUG) fprintf(stderr,"query failed: '%s'\n", query);
					errnum = -3;
				}
		
				/*on success...*/
				if (!errnum){

					/*load the page content into ther large query buffer*/
					sprintf(filequery, "update pages set page_content = '%s' where page_name = '%s'", fbuffer2, argv[3]);

					if (DEBUG > 1) fprintf(stderr, "file query:\n%s\n\n", filequery);

					/*insert the page content*/
					if(mysql_query(&mysql,filequery)){
						if(DEBUG) fprintf(stderr,"query failed: '%s'\n", filequery);
						
						/*on failure, attempt to remove the bad entry*/
						for(i=0;i<MAX_QUERY;i++)query[i]=0;
						sprintf(query, "delete from pages where page_name = '%s'", argv[3]);

						/*send bad page removal query*/
						if(mysql_query(&mysql,query))
							if(DEBUG) fprintf(stderr,"query failed: '%s'\n", query);

						/*set the error number*/
						errnum = -4;
					}
				}
			}		
		}
	}

	/*
		Finally close connection to server
	*/
	mysql_close(&mysql);
	
	/*print exit state on error*/
	if (DEBUG){
		fprintf(stderr, "exit state:");
		if (errnum == 0) fprintf(stderr, " OK\n");
		else if (errnum == -1) fprintf(stderr, " error: could not read file, file too large\n");
		else if (errnum == -2) fprintf(stderr, " error: could not read file, file does not exist\n");
		else if (errnum == -3) fprintf(stderr, " error: failed to insert new entry\n");
		else if (errnum == -4) fprintf(stderr, " error: failed to insert page content to new entry\n");

	}

	return 0;
}
