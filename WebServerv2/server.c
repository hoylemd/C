/* Web Server application by Michael Hoyle January 21st 2008 */
/* Derived from Web Server Example by D. Calvert */
/* ftp://ftp.isi.edu/in-notes/rfc2616.txt */
/* http://ftp.ics.uci.edu/pub/ietf/http/rfc1945.html */

/*  Make the necessary includes and set up the variables.  */

#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <mysql/mysql.h>
#include "hstring.h"

/*SQL defines*/
#define USERNAME  "mhoyle"
#define PASSWORD  "0553453"
#define DATABASE  "mhoyle"
#define MAX_LENGTH 1024
#define MAX_QUERY 512
#define BLOB_SIZE 65535
#define CHAR_SIZE 255
#define HOSTNAME  "dursley.cis.uoguelph.ca"

/*prototype for the fileno function. Compiler complains about it's use if it's not prototyped*/
int fileno(FILE* stream);

/*prototype the SSInclude Function*/
char * SSInclude( char * page, char *libName );

/*global debug variable for printing out debug messages*/
/*0 = no debug messages*/
/*1 = main debug messages*/
/*2 = verbose mode*/
int debug = 0;
int socketnumber = 8888;

/*struct to store tag tokens*/
typedef struct{
	char * tag;
	char * attributes;
} Tag;

/*struct to store tag tokens or string tokens*/
typedef struct{
	Tag * tagp;
	char * strp;
} Token;

/*linked list struct for the tokens list*/
typedef struct toklist{
	Token * val;
	struct toklist * next;
} TList;

/*Log struct*/
typedef struct {
   char pageName[MAX_LENGTH];
   char browserName[MAX_LENGTH];
   unsigned long date;
} Log;

/*prototype the getDate function (FOR USE IN LOGGING ONLY)*/
unsigned long getDate();

/*Blacklist object struct*/
typedef struct bllist{
	struct bllist * next;
	char lib[256];
} Blist;

/*blacklist linked list functions*/

/*function to make a new list object*/
Blist * makeBlackEntry(char * name){
	Blist * ret = NULL;
	int i = 0;

	if (debug) fprintf(stderr, "Making new blacklist entry for '%s'\n", name);
	
	/*malloc the object*/
	ret = malloc(sizeof(Blist));
	
	/*initialize it's fields*/
	for ( i = 0; i < 256; i++){
		ret->lib[i] = 0;
	}
	strncpy(ret->lib, name, strlen(name));
	ret->next = NULL;

	/*return the object*/
	return ret;
}

/*function to add a new entry to a Blacklist*/
Blist * addEntry(Blist * top, char * libr){
	Blist * curr = top, * new = NULL;
	if (debug) fprintf(stderr, "Adding entry '%s' to blacklist\n", libr);

	/*create the object*/
	new = makeBlackEntry(libr);

	/*add it to the end of the list and return a pointer to the top of the new list*/
	if (curr){
		while (curr->next) curr = curr->next;
		curr->next = new;
		if (debug) fprintf(stderr, "added new to blacklist\n");
		return top;
	} else {
		return new;
	}
}

/*function to remove an entry from a Blacklist*/
Blist * removeEntry(Blist * top, char * targ){
	Blist * curr = top, * tmp = NULL;;

	if (debug) fprintf(stderr, "removing entry '%s' from the blacklist\n", targ);

	/*step through the list*/
	while (curr){
		/*if the given string exists in the list, remove it*/
		if (curr == top){
			if (!strcmp(targ, curr->lib)){
				tmp = curr;
				curr = curr->next;
				if (tmp)free(tmp);
				/*if the first entry was the target, we return a pointer to the second entry, which is now the top*/
				return curr;
			} else {
				tmp = curr;
				curr = curr->next;
			}
		} else {
			if (!strcmp(targ, curr->lib)){
				tmp->next = curr->next;
				if (curr) free(curr);
				return top;
			} else {
				tmp = tmp->next;
				curr = curr->next;
			}
		}
	}

	/*return A pointer to the top of the list*/
	return top;
}

/*function to check if a string exists in the list*/
int blacklisted(Blist * top, char * targ){
	Blist * curr = top;

	if (debug) fprintf(stderr, "Checking if entry '%s' is blacklisted\n", targ);

	/*step through the list, comparing the entry strings. if a match is founbd return 1*/
	while (curr){
		if (!strcmp(targ, curr->lib)) return 1;
		else curr = curr->next;
	}
	return 0;
}

/*function to print out the contents of a blacklist*/
void printlist(Blist * list){
	Blist * curr = list;

	fprintf(stderr, "Library Blacklist:\n");

	/*step through the list printing out each entry*/
	while (curr){
		fprintf(stderr,"%s\n", curr->lib);
		curr = curr->next;
	}
}

/*function to destroy a blacklist*/
Blist * destroylist(Blist * list){
	Blist * curr = list, * tmp = NULL;

	if (debug) fprintf(stderr, "Destroying a blacklist\n");

	/*step through the list, freeing each struct*/
	while(curr){
		if (curr->next) tmp = curr->next;
		if (curr) free(curr);
		curr = tmp;
		tmp = NULL;
	}

	/*return the current pointer; should be NULL*/
	return curr;
}

/*Main Function*/
int main() {
	/*Set up Socket Variables*/
	int server_sockfd, client_sockfd, server_len, count, i, size, newlineCount, ctr, exitflag = 0, pauseflag = 0, cont = 0, dbflag = 0, fileopened = 0;
	socklen_t client_len;
	struct sockaddr_in server_address, client_address;

	/*set up strings*/
	char ch, chr, request[2048], dnsname[2048], *path = NULL, *endChar = NULL, *file = NULL, *temp = NULL, libName[256], *browser =NULL, input[256], logstring[5000], temporary[5000],logentry[5000];
	/*set up file/directory pointers*/
	FILE * fp = NULL, *fd = NULL, *pipe = NULL, *lpipe = NULL;
	DIR * dp = NULL;
	struct dirent * curLib = NULL;

	/*set up Log pointer*/
	Log * nLog = NULL;

	/*set up the blacklist pointer*/
	Blist * blacklist;

	/*constant strings*/

	/* http header which is sent in response to a browser request */
	char *response1 = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n\r\n";
	/*hardcoded 404 error page*/
	char *E404 = "<HTML><HEAD><TITLE>404 error!</title></head><BODY BACKGROUND=\"#E0E0E0\"TEXT=   \"#000000\" LINK=   \"#FF0000\" ALINK=  \"#FF0000\" VLINK=  \"#0000FF\"><CENTER><H2>404 Error<BR>File not found</H2></center><BR>fore oh fore! I'm soooooooooooooo sorry. <br>that page doesnt exist. <br> It's ok to feel sad. I feel like a huge jerk not having what you want.<br> hug? <br></body></html>";

	/*sql variables*/
	MYSQL mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query[MAX_QUERY];
	char fbuffer[BLOB_SIZE], fbuffer2[BLOB_SIZE], filequery[BLOB_SIZE + MAX_QUERY];
	int p_keys[256], numpkeys =0;

	/*open the log file*/
	fd = fopen("access.log", "a+");

	/*initialize request string*/
	for (i = 0; i < 2048; i++){
		request[i] = 0;
		dnsname[i] = 0;
	}
	
	/*initialize libName string*/
	for (i = 0; i < 256; i++){
		libName[i] =0;
		input[i] = 0;
		p_keys[i] = 0;
	}

	/*destroy any lingering pipes*/
	unlink("Spipe");

	if(debug) fprintf(stderr,"opening the pipes...");

	/*connect to the pipe*/
	if (!mkfifo("Spipe", 0666 | O_RDWR)){
		if (debug) fprintf(stderr, "connecting to Spipe...");
		if (!debug) fprintf(stderr, "Please load the GUI now.\n");
		pipe = fopen("Spipe", "r");
		if (pipe) {if(debug) fprintf(stderr, "done\n");}
		else{
			if(debug) fprintf(stderr, "failed: unable to open file. exiting.\n");
			exitflag = 1;
		}
	} else{
		if(debug) fprintf(stderr, "failed: Unable to establish fifo pipe. exiting.\n");
		exitflag = 1;
	}

	if (debug) fprintf(stderr, "connecting to Lpipe...");
	while(!lpipe){
		lpipe = fopen("Lpipe", "w");
	}
	if (debug) fprintf(stderr, "done\n");
	fclose(lpipe);

	if (debug) fprintf(stderr, "done\n");

	/*set input to non-blocking */
;	if (pipe) if (fcntl(fileno(pipe), F_SETFL, O_NONBLOCK)) perror("fcntl for pipe");

	/*  Remove any old socket and create an unnamed socket for the server. Set it to non-blocking*/
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(server_sockfd, F_SETFL, O_NONBLOCK);

	/*  Name the socket.  */
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(socketnumber);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	/*  Create a connection queue and wait for clients.  */
	listen(server_sockfd, 5);

	/*main program loop*/
	while(!exitflag) { 
		/*initialize variables*/
		if (file != NULL){
			free(file);

			file = NULL;
		}
		file = NULL;
		if (fp) fclose(fp);
		fp = NULL;
		size = 0;
		for (i = 0; i < 2048; i++){ request[i] = 0; dnsname[i] = 0;}
		for (i = 0; i < 256; i++){libName[i] =0; input[i] = 0; p_keys[i] = 0;}
		for(i=0;i<MAX_QUERY;i++) query[i] = 0;
		for(i=0;i<BLOB_SIZE;i++) {fbuffer[i] = 0; fbuffer2[i] = 0;}
		for(i=0;i<(MAX_QUERY+BLOB_SIZE);i++) filequery[i] = 0;
		newlineCount = 0;
		path = NULL;
		endChar = NULL;
		cont = 0;
		chr = 0;
		ctr = 0;
		dbflag = 0;
		fileopened = 0;
		numpkeys = 0;

		/*check the pipe for incoming commands*/
		count = fread(&chr, sizeof(char), 1, pipe);
		if (debug && chr != 0) fprintf(stderr, "'%c' read from pipe\n", chr);
		
		/*shutdown command*/
		if (chr == 's'){ if (debug) fprintf(stderr, "s recieved thru pipe\n"); exitflag = 1;}

		/*pause command*/
		if (chr == 'p'){ if (debug) fprintf(stderr, "p recieved thru pipe\n"); pauseflag = 1;}

		/*unpause command*/
		if (chr == 'u'){ if (debug) fprintf(stderr, "u recieved thru pipe\n"); pauseflag = 0;}

		/*toggle library command*/
		if (chr == 't'){ if (debug) fprintf(stderr, "t recieved thru pipe\n"); 

			/*read the library name until semicolon (end of library name)*/
			while(1){
				fread(&chr, sizeof(char), 1, pipe);
				if (chr){
					if (chr == ';') break;
					if (chr != ' '){
						libName[ctr] = chr;
						ctr+=1;
					}
				}
			}
			if (debug) fprintf(stderr, "toggling library '%s'\n", libName);

			/*check the blacklist for the library. if it's there, remove it. if not, add it*/
			if (blacklisted(blacklist, libName)) blacklist = removeEntry(blacklist, libName);
			else blacklist = addEntry(blacklist, libName);

			if (debug) printlist(blacklist);
		}
		/*readlogs command*/
		if (chr == 'l'){ if (debug) fprintf(stderr, "l recieved thru pipe\n");

			/*initialize strings*/
			for( i = 0; i < 5000; i++) {temporary[i] = 0; logentry[i] = 0; logstring[i] = 0;} 
			
			fclose(fd);
			fd = fopen("access.log", "r");	
			/*on success*/
			if (fd){
				if (debug) fprintf(stderr, "opened pipe successfully");
				while(!feof(fd)){
					/*initialize the log storage area and read in the next log entry*/
					nLog=  malloc(sizeof(Log));
					for(i = 0; i < MAX_LENGTH; i++) {nLog->pageName[i] = 0; nLog->browserName[i] = 0;}
					nLog->date = 0;
					fread(nLog, sizeof(Log), 1, fd);
					if (debug) fprintf(stderr, "read log entry\n");
					/*generate a log entry string*/
					sprintf(logentry, "%s %s %lu", nLog->pageName, nLog->browserName, nLog->date);
					logentry[strlen(logentry)] = 0;
					if (debug) fprintf(stderr, "constructed log entry\n");
					/*if it is a valid log entry*/
					if (strcmp(logentry, "  0")){
						if (debug) fprintf(stderr, "valid log entry [%s]\n", logentry);
						/*add it to the return string*/
						if (strlen(logstring)){
							sprintf(temporary, "%s;%s", logstring, logentry);
							temporary[strlen(temporary)] = 0;
							strncpy(logstring, temporary, 5000);
						} else {
							strncpy(logstring, logentry, 5000);
						}
					}
					if (debug) fprintf(stderr, "done validation\n");
					/*clear the temporary strings*/
					for( i = 0; i < 5000; i++) {temporary[i] = 0; logentry[i] = 0;} 
					if (debug) fprintf(stderr, "cleared temp strings\n");
					/*clear the log stroage area*/
					if(nLog) free(nLog);
					nLog = NULL;
				}

				fclose(fd);
				fd =fopen("access.log", "a+");
	
				lpipe = fopen("Lpipe", "w");
				
				/*push the string into the pipe*/
				if (lpipe){
					if (strlen(logstring)){
						if(debug) fprintf(stderr, "Sending '%s' through the pipe...", logstring);
						fputs(logstring, lpipe);
						if (debug) fprintf(stderr, "done\n");
						fclose(lpipe);
						if (debug) fprintf(stderr, "pipe closed\n");
					} else {
						if(debug) fprintf(stderr, "No logs in file. sending error code");
						fputs("None", lpipe);
						fclose(lpipe);
						if (debug) fprintf(stderr, "pipe closed\n");
					}
				}else{
					if(debug) fprintf(stderr, "Pipe not connected; Can not send command.\n");
				}

			}
	
			if (debug) fprintf(stderr, "log entries read:%s\n", logstring);
		}
		
		/*reinitialize variables*/
		chr = 0;
		ctr = 0;


		/*if the server is paused, skip the networking sections*/
		if (!pauseflag){
			/*  Accept a connection.  */
			client_len = sizeof(client_address);
			client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

			/* We can now read/write to client on client_sockfd.*/

			/* read query from the client */
			while(!cont) {
				count = read(client_sockfd, &ch, 1);
				if (count == -1) cont = 1;
				if (count == 0) break;
	
				/* remove CR from CR-LF end of line markers */
				if (ch == '\r') count = read(client_sockfd, &ch, 1);
	
				/* look for empty line which signals the end of the request */
				if (ch == '\n') newlineCount++;
				else newlineCount = 0;
				if (newlineCount == 2) break;
	
				/*save the character read from the buffer*/
				if (ch) request[strlen(request)] = ch;
			
			}
			
			
			if(debug && strlen(request)>1) fprintf(stderr, "request length %d: \n::::::::::::::::::::::\n%s::::::::::::::::::::::\n",(int)strlen(request), request);

			/*if a request was recieved*/
			if(!cont){

				if(debug) fprintf(stderr, "Logging request...");

				/*isolate the browser name from the path*/
				browser = strstr(request, "User-Agent:");
				browser = strchr(browser, ' ');
				browser = &browser[1];
				while(browser[ctr] != 0 && browser[ctr] != ' ') ctr += 1;
				path = malloc(sizeof(char)*(ctr+1));
				for(i = 0; i <=ctr; i++) path[i] = 0;
				for(i = 0; i < ctr; i++) path[i] = browser[i];
				browser = path;
				path = NULL;

				/*isolate the URL used to find the server*/

				temp = strstr(request, "Host:");
				temp = &temp[6];
				strncpy(dnsname, temp, 2048);
				*strchr(dnsname, '\n') = 0;
				temp = NULL;
	
				if(debug) fprintf(stderr, "browser name isolated...");

				/*isolate the path from the request*/
				path = strchr(request, ' ');
				path = &path[2];
				endChar = strchr(path, ' ');
				*endChar = 0;


				if(debug) fprintf(stderr, "path isolated...");

				/*log the request*/
				nLog = malloc(sizeof(Log));
				strncpy(nLog->browserName, browser, (MAX_LENGTH -1));
				strncpy(nLog->pageName, path, (MAX_LENGTH -1));
				nLog->date = getDate();
				if(debug) fprintf(stderr, "request logged...");

				/*write the log to the file*/
				fwrite(nLog, sizeof(Log), 1, fd);
				if (nLog) free(nLog);
				nLog = NULL;
				if (browser) free(browser);
				browser = NULL;
	
				if(debug) fprintf(stderr, "done.\nOpening requested file (%s)...", path);
	
				/*if a path was given*/
				if (strlen(path)){

					/*check if it's a database path*/
					if (strlen(path) >= 4){
						if(path[0] == 'd' && path[1] == 'b' && path[2] == '-'){

							/*raise the database flag*/
							dbflag = 1;

							/*grab the database path*/
							temp = hstrclone(&path[3]);
							path = temp;
							temp = NULL;

							if (debug) fprintf(stderr, "database path: '%s'\n", path);

							/*connect to the database*/
							mysql_init(&mysql);
							mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "mydb");
							if (!mysql_real_connect(&mysql, HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0)) {
							   if (debug) fprintf(stderr, "Failed connecting to database\n");
							}
	
							if (path[0] == '-'){
								/*index path*/

								/*construct the query*/
								sprintf(query, "select id from pages");

								/*query the database*/
								if (mysql_query(&mysql,query))
									if (debug) fprintf(stderr, "failed to get primary keys from database\n");

								/*Store results from query into res structure.*/
								if (!(res = mysql_store_result(&mysql)))
									if(debug) fprintf(stderr,"store failed\n");

								/*put the returned keys into an array of ints*/
								if (res){
									while ((row = mysql_fetch_row(res))) {			
										for (i=0; i < mysql_num_fields(res); i++){	
											p_keys[numpkeys] = atoi(row[i]);
											numpkeys++;
										}
									}
								} else {
									if (debug) fprintf(stdout,"No pages stored\n");
								}

								/*build the index page*/
								file = malloc(sizeof(char)*2048);
								for(i=0;i<2048;i++)file[i] = 0;

								strncat(file, "<HTML><HEAD><TITLE>Database Index</TITLE></HEAD><BODY><PRE>   Page Number\n", 2048);
								for(i = 0; i< numpkeys; i++) sprintf(file, "%s             <a href = \"http://%s/db-%d\">%d</a>\n", file, dnsname, p_keys[i], p_keys[i]);
								sprintf(file, "%s</PRE></BODY></HTML>\n", file);

							} else {
								/*link to a page in the Database*/
								if (debug) fprintf(stderr, "retrieveing page %s...", path);
								/*build the query*/
								sprintf(query, "select page_content from pages where id = %s", path);

								/*query the database. null the file on failure*/
								if(mysql_query(&mysql,query)){
									if(debug) fprintf(stderr, "failed to retrieve page from database. returning 404\n");
									file = NULL;
								} else {
									/*on success, Store results from query into res structure.*/
									if (debug) fprintf(stderr, "sucess!...");
									file = malloc(sizeof(char) * BLOB_SIZE);
									if (!(res = mysql_store_result(&mysql)))
										if(debug) fprintf(stderr,"store failed\n");

									/*store the returned file in the sending buffer*/
									if (res){
										while ((row = mysql_fetch_row(res))) {			
											for (i=0; i < mysql_num_fields(res); i++){	
												strncpy(file, row[i], BLOB_SIZE);
											}
										}
									} else {
										if (debug) fprintf(stdout,"No pages stored\n");
										if (file) free(file);
										file = NULL;
									}
									if (debug) fprintf(stderr, "size = %d\n", strlen(file));
									/*confirm a file was actually returned. if not, free and null the buffer*/
									if (!strlen(file)){
										free(file);
										file = NULL;
									} else if ((strstr(file, "<HTML>") && strstr(file,"</HTML"))){
										free(file);
										file = NULL;

									}
									
								}
							}

							/*disconnect from the database*/
							mysql_close(&mysql);		
						}
					}

					if (!dbflag){
						/*if it was a normal request*/
						/*add the /cache directory to the path*/
						browser = happend("./cache/", path);
						path = browser;
						browser = NULL;

						if (debug) fprintf(stderr, "path contructed...");

						/*read the file indicated by the path*/
						
						if (file) free (file);
						file = malloc(sizeof(char)*1048576);
						for(i=0;i<1048576;i++) file[i]=0;
						fileopened = hreadfile(path, file, 1048576 );
						if (path) free(path);
						path = NULL;
					}

					/*attempt to open the library directory*/
					dp = opendir("./lib");

					if (debug && dp) fprintf(stderr, "directory opened...");
	
					/*on success...*/
					
					if (dp && !fileopened){

						if(debug) fprintf(stderr, "done (%d)\n", fileopened);
	
						do {
							/*reinitialize libName string*/
							for (i = 0; i < 256; i++){
								libName[i] = 0;
							}

							/*get the next dir entry*/
							curLib = readdir(dp);
	
							/*if there is one*/
							if (curLib){
	
								/*disregard . and .. entries*/
								if (strcmp(curLib->d_name, ".") && strcmp(curLib->d_name, "..") && strcmp(curLib->d_name, ".") ) {
									if (debug) fprintf(stderr, "Integrating library \"%s\"...", curLib->d_name);
	
										/*build the path string*/
									strcat(libName, "./lib/");
									strcat(libName, curLib->d_name);
			
									/*check if the dir entry is a .so*/
									if (strstr(libName, ".so")){
										if ( strlen(strstr(libName, ".so")) == 3 ){
	
											if (debug) fprintf(stderr, "comparing '%s' to the blacklisted libraries...", curLib->d_name);
	
											/*initiate server-side includes if the library in question is not blacklisted.*/
											if (!blacklisted(blacklist, curLib->d_name)) temp = SSInclude(file, libName);
	
											/*if it is, simply give back the file supplied, unchanged*/
											else temp = hstrclone(file);
	
											if(debug) fprintf(stderr, "done\n");
	
										} else { if (debug) fprintf(stderr, "Failed. is not a shared object file\n"); }
									} else { if (debug) fprintf(stderr, "Failed. is not a shared object file\n"); }
	
									/*clean up*/
									if (temp){
										if (file) free(file);
										file = temp;
										temp = NULL;
									}

								}
							}
						} while (curLib); /*do for every entry in this directory*/	
					} else {
						if(debug) fprintf(stderr, "could not open file\n"); 
						file = NULL;
					}

				} else {
				/*if no file given, return a null file string*/

					if (debug) fprintf(stderr, "No file specified. sending 404.\n");

					file = NULL;
				}
				if(debug) fprintf(stderr,"sending response...");
				/* send header */
				write(client_sockfd, response1, strlen(response1));
				/*if the file was found, send it. if not, send the 404 page*/
				if (file) write(client_sockfd, file, (strlen(file)));
				else write(client_sockfd, E404, (strlen(E404)));
				/*clean up*/
				if (file) free(file);
				file = NULL;

				/*close the socket*/
				close(client_sockfd);
				if(debug) fprintf(stderr, "done\n");
			}	
		}	
	}

	/*more cleanup*/
	if (fd) fclose(fd);
	if (pipe) fclose(pipe);
	unlink("Spipe");
	destroylist(blacklist);


	/*put message in log pipe to tell Gui the server is down*/
	lpipe = fopen("Lpipe", "w");
				
	/*push the string into the pipe*/
	if (lpipe){
		if(debug) fprintf(stderr, "Sending s through the pipe...");
		fputs("S;", lpipe);
		if (debug) fprintf(stderr, "done\n");
		fclose(lpipe);
		if (debug) fprintf(stderr, "pipe closed\n");		
	}else{
		if(debug) fprintf(stderr, "Pipe not connected; Can not send command.\n");
	}

	return 0;

}


/*tool to convert a month 3-letter string into it's numerical representation*/
int MonthtoNum(char * month){
	if (!strcmp(month, "Jan")) return 1;
	else if (!strcmp(month, "Feb")) return 2;
	else if (!strcmp(month, "Mar")) return 3;
	else if (!strcmp(month, "Apr")) return 4;
	else if (!strcmp(month, "May")) return 5;
	else if (!strcmp(month, "Jun")) return 6;
	else if (!strcmp(month, "Jul")) return 7;
	else if (!strcmp(month, "Aug")) return 8;
	else if (!strcmp(month, "Sep")) return 9;
	else if (!strcmp(month, "Oct")) return 10;
	else if (!strcmp(month, "Nov")) return 11;
	else if (!strcmp(month, "Dec")) return 12;
	else return 0;

}

/*function to get the date in the form of MMddyyhrmnsc*/
unsigned long getDate(){
	unsigned long ret;
	unsigned long i = 0, sec=0, min=0, hour=0, day=0, month=0, year=0;
	char * datestr = NULL, * curr = NULL, name[4];
	for (i = 0; i < 4; i++) name[i] = 0;

	/*get the time string*/
	time_t tyme = (time_t)(-1);
	while(tyme == (time_t)(-1)){
		tyme = (int)time(NULL);
	}
	datestr = hstrclone(ctime(&tyme));

	if (debug) fprintf(stderr, "date: %s\n", datestr);

	/*parse to the month token*/
	curr = strchr(datestr, ' ');
	curr = &curr[1];

	/*parse the parts of the string into their numerical slots*/
	for(i = 0; i < 3; i++) name[i] = curr[i];
	month = MonthtoNum(name);
	curr = &curr[4];
	day = atoi(curr);
	curr = &curr[3];
	hour = (atoi(curr));
	curr = &curr[3];
	min = (atoi(curr));
	curr = &curr[3];
	sec = atoi(curr);
	curr = &curr[5];
	year = (atoi(curr));

	/*kill the string*/
	if (datestr)free(datestr);
	datestr = NULL;

	ret = (sec);

	ret += (100*min);

	ret += (10000*hour);
	
	ret += (1000000*year);

	ret += (100000000*day);

	ret += (10000000000*month);

	/*pass the number back*/
	return (ret);
}

/*function to tokenize the contents if < > into a tag struct for easy handling*/
Tag * tokenizeTag(char * tstr){
	int ctr = 0, i = 0;
	char * tag1 = NULL, *tag2 = NULL;
	Tag * rtag = NULL;

	/*split the tag contents into a tag and attribute substring*/
	tag1 = hftrim(tstr);
	while(tag1[ctr] != ' ' && tag1[ctr] != 0) ctr += 1;
	tag1 = malloc(sizeof(char)*(ctr+1));
	for(i = 0; i <= ctr; i++) tag1[i] = 0;
	for(i = 0; i < ctr; i++) tag1[i] = tstr[i];

	tag2 = hstrclone(hftrim(&tstr[strlen(tag1)]));

	/*create the tag struct and store the strings in it*/
	rtag = malloc(sizeof(Tag));
	rtag->tag = tag1;
	tag1 = NULL; 

	if(strlen(tag2)) rtag->attributes = tag2;
	else {
		rtag->attributes = NULL;
		if (tag2) free (tag2);
	}
	tag2 = NULL;

	return rtag;
}

/*Function to implement server-side includes with a specific dynamically loaded library*/
char * SSInclude( char * page, char *libName ){
	/*set up variables*/

	/* handle used to reference library */
	void *handle = NULL;

	/* function pointesr - used to point at functions in the library */
	char * (*search)() = NULL;
	char * (*replace)() = NULL;

	/*pointer to an instance of the search string*/
	char * curr =  NULL, *temp = NULL;

	/*return string and temporary page string*/
	char * ret = NULL, * work = NULL;

	/* character counter */
	int ctr = 0, i = 0;


	/*tags list*/
	TList * tagsq = NULL, * tmptlist = NULL, * tmptlist2 = NULL;
	Tag * searchtag;

	/*handle null input*/
	if (page == NULL) return NULL;

	/* open library */
	if (debug) fprintf(stderr, "opening library...(%s)", page);
	handle = dlopen(libName, RTLD_LAZY);
	if (debug) fprintf(stderr, "opened library...");
	/*parse the tags into a list*/
	curr = page;
	while (*curr != 0){
		if(curr[0] == '<'){

			if (debug > 1) fprintf(stderr, "found a tag, tokenizing...");

			ctr = 0;

			/*find how long the tags contents are*/
			while(curr[ctr+1] != '>')ctr+= 1;

			/*read the tags contents into a temporary string*/
			temp = malloc(sizeof(char)*(ctr+1));
			for (i = 0; i <= ctr; i++) temp[i] = 0;
			for(i = 0; i < ctr; i++){
				temp[i] = curr[i+1];

			}

			/*create the token queue object*/
			tmptlist = malloc(sizeof(TList));
			tmptlist->next = NULL;
			tmptlist->val = malloc(sizeof(Token));
			tmptlist->val->tagp = tokenizeTag(temp);
			tmptlist->val->strp = NULL;

			/*add it to the queue*/
			if (tagsq){
				tmptlist2 = tagsq;
				while (tmptlist2->next) tmptlist2 = tmptlist2->next;
				tmptlist2->next = tmptlist;
				tmptlist2 = NULL;
			} else {
				tagsq = tmptlist;
			}
			tmptlist = NULL;

			/*clean up*/
			if (temp) free(temp);
			temp = NULL;
			ctr = 0;

			/*advance the pointer past the tag*/
			curr = strchr(curr, '>');
			curr = &curr[1];
		} else {

			if (debug> 1) fprintf(stderr, "found a string, tokenizing...");

			/*find how long the strings content is*/
			while(curr[ctr] != 0 && curr[ctr] != '<') ctr+= 1;
			temp = malloc(sizeof(char) * (ctr+1));
			for (i = 0; i <= ctr; i++) temp[i] = 0;
			for (i = 0; i < (ctr); i++) temp[i] = curr[i];

			/*create the token queue object and store the string in it*/
			tmptlist = malloc(sizeof(TList));

			tmptlist->val = malloc(sizeof(Token));
			tmptlist->next = NULL;
			tmptlist->val->strp = temp;
			tmptlist->val->tagp = NULL;
			temp = NULL;

			/*add it to the queue*/
			if (tagsq){
				tmptlist2 = tagsq;
				while (tmptlist2->next) tmptlist2 = tmptlist2->next; 
				tmptlist2->next = tmptlist;
				tmptlist2 = NULL;
			} else {
				tagsq = tmptlist;
			}


			/*advance past the string*/
			tmptlist = NULL;
			curr = &curr[ctr];
			ctr = 0;
		}
	}

	if (debug) fprintf(stderr, "done tokenizing. Tokens List:\n");

	curr = NULL;
	if(debug>1){
		tmptlist = tagsq;
		while (tmptlist){
			if (tmptlist->val->tagp){
				fprintf(stderr, "Token: Tag: tag:<%s> attribute:[%s]\n", tmptlist->val->tagp->tag, tmptlist->val->tagp->attributes);
				tmptlist = tmptlist->next;
			} else if (tmptlist->val->strp){
				fprintf(stderr, "Token: String:[%s]\n", tmptlist->val->strp);
				tmptlist = tmptlist->next;
			}
		}
		fprintf(stderr, "end of tokens list\n");
	}
	tmptlist = NULL;

	/* link the functions named searchString and replaceString to the function pointers */
	if (handle) {
		search = dlsym(handle, "searchString");
		replace = dlsym(handle, "replaceString");

		/* call the function in the library to get the search tag*/
		searchtag = tokenizeTag(search());

		/*step through the token list and replace the tokens that correspond to the correct tag*/
		if (!searchtag->attributes){ 
			/*simple tag(*/
			tmptlist = tagsq;

			while (tmptlist){
				if (tmptlist->val->tagp){
					if (!strcmp(tmptlist->val->tagp->tag, searchtag->tag)){

						/*put the replace string in the string field*/
						tmptlist->val->strp = hstrclone(replace());
	
						/*get rid of the tag token*/
						if (tmptlist->val->tagp->tag) free (tmptlist->val->tagp->tag);
						tmptlist->val->tagp->tag = NULL;
						if (tmptlist->val->tagp->attributes) free (tmptlist->val->tagp->attributes);
						tmptlist->val->tagp->attributes = NULL;
						if (tmptlist->val->tagp) free (tmptlist->val->tagp);
						tmptlist->val->tagp = NULL;
					}
				}
				tmptlist = tmptlist->next;
			}
		} else { 
			/*not-so simple tag*/
			tmptlist = tagsq;

			while (tmptlist){
				if (tmptlist->val->tagp){
					if (!strcmp(tmptlist->val->tagp->tag, searchtag->tag)){

						/*put the replace string in the string field*/
						tmptlist->val->strp = hstrclone(replace(tmptlist->val->tagp->attributes));
	
						/*get rid of the tag token*/
						if (tmptlist->val->tagp->tag) free (tmptlist->val->tagp->tag);
						tmptlist->val->tagp->tag = NULL;
						if (tmptlist->val->tagp->attributes) free (tmptlist->val->tagp->attributes);
						tmptlist->val->tagp->attributes = NULL;
						if (tmptlist->val->tagp) free (tmptlist->val->tagp);
						tmptlist->val->tagp = NULL;
					}
				}
				tmptlist = tmptlist->next;
			}
		}

		/*close the handle pointing to the library */
		dlclose(handle);
	}

	/*reconstruct the page from the tokens*/
	while (tagsq){
		if (tagsq->val->tagp){

			/*if the token is a tag*/

			/*start with the opening angle bracket*/
			temp = happend(work, "<");
			if(work) free(work);
			work = temp;
			temp = NULL;

			/*append the tag*/
			temp = happend(work, tagsq->val->tagp->tag);
			if (work) free(work);
			work = temp;
			temp = NULL;
			if (tagsq->val->tagp->tag) free (tagsq->val->tagp->tag);
			tagsq->val->tagp->tag = NULL;

			/*append a space*/
			temp = happend(work, " ");
			if(work) free(work);
			work = temp;

			/*append the attributes, if any*/
			temp = happend(work, tagsq->val->tagp->attributes);
			if (work) free(work);
			work = temp;
			if (tagsq->val->tagp->attributes) free (tagsq->val->tagp->attributes);

			/*close the angle braces*/
			temp = happend(work, ">");

			/*clean up*/
			if (work) free(work);
			work = temp;
			temp = NULL;
			if (tagsq->val->tagp) free (tagsq->val->tagp);
			tagsq->val->tagp = NULL;

		} else if (tagsq->val->strp){

			/*for string tokens, just append the string and nuke the token*/
			temp = happend(work, tagsq->val->strp);
			if (work) free(work);
			work = temp;
			temp = NULL;
			if (tagsq->val->strp) free (tagsq->val->strp);
			tagsq->val->strp = NULL;
		}

		/*nuke the remaning parts of the token*/
		tmptlist = tagsq;
		tagsq = tagsq->next;
		if (tmptlist->val) free(tmptlist->val);
		tmptlist->val = NULL;
		if (tmptlist) free(tmptlist);
		tmptlist = NULL;
	}

	/*clean up*/
	ret = NULL;
	
	/*make sure the string is terminated*/
	work[strlen(work)]=0;

	/*return the temp pointer, as this has the new file whether it was mdoified or not*/
	return work;
}
