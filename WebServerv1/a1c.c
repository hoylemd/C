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

/*prototype the SSInclude Function*/
char * SSInclude( char * page, char *libName );

/*Main Function*/
int main() {

	/*Set up Socket Variables*/
	int server_sockfd, client_sockfd, server_len, count, i, size, newlineCount;
	socklen_t client_len;
	struct sockaddr_in server_address, client_address;

	/*set up strings*/
	char ch, request[2048], *path = NULL, *endChar = NULL, *file = NULL, libName[265];

	/*set up file/directory pointers*/
	FILE * fp = NULL;
	DIR * dp = NULL;
	struct dirent * curLib = NULL;

	/*constant strings*/

	/* http header which is sent in response to a browser request */
	char *response1 = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n\r\n";
	/*hardcoded 404 error page*/
	char *E404 = "<HTML><HEAD><TITLE>404 error!</title></head><BODY BACKGROUND=\"#E0E0E0\"TEXT=   \"#000000\" LINK=   \"#FF0000\" ALINK=  \"#FF0000\" VLINK=  \"#0000FF\"><CENTER><H2>404 Error<BR>File not found</H2></center><BR>fore oh fore! I'm soooooooooooooo sorry. <br>that page doesnt exist. <br> It's ok to feel sad. I feel like a huge jerk not having what you want.<br> hug? <br></body></html>";

	/*initialize request string*/
	for (i = 0; i < 2048; i++){
		request[i] = 0;
	}
	
	/*initialize libName string*/
	for (i = 0; i < 265; i++){
		libName[265] =0;
	}

	/*  Remove any old socket and create an unnamed socket
	    for the server.  */
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/*  Name the socket.  */
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(8888);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	/*  Create a connection queue and wait for clients.  */
	listen(server_sockfd, 5);

	/*main program loop*/
	while(1) {
		/*initialize variables*/
		if (file != NULL){
			free(file);
			file = NULL;
		}
		file = NULL;
		if (fp) fclose(fp);
		fp = NULL;
		size = 0;
		for (i = 0; i < 2048; i++){
			request[i] = 0;
		}
		newlineCount = 0;
		path = NULL;
		endChar = NULL;

		/*printf("server waiting\n");*/
		
		/*  Accept a connection.  */
		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

		/*  We can now read/write to client on client_sockfd.  */

		/* read query from the client */
		while(1) {
			count = read(client_sockfd, &ch, 1);
			if (count == 0) break;

			/* remove CR from CR-LF end of line markers */
			if (ch == '\r') count = read(client_sockfd, &ch, 1);

			/* look for empty line which signals the end of the request */
			if (ch == '\n') newlineCount++;
			else newlineCount = 0;
			if (newlineCount == 2) break;

			/*print out the request (for debugging)*/
			/*fprintf(stderr, "%c", ch);*/

			/*save the character read from the buffer*/
			request[strlen(request)] = ch;
		
		}
		/*fprintf(stderr, "outside while loop\n\n");*/

		/*isolate the path from the request*/
		path = strchr(request, ' ');
		path = &path[2];
		endChar = strchr(path, ' ');
		*endChar = 0;

		/*open the file indicated by the path*/
		fp = fopen(path, "r");

		/*if the file is found...*/
		if (fp) {
			/*calculate the file size*/
			fseek(fp,0,SEEK_END);
			size = ftell(fp);
			fseek(fp,0,SEEK_SET);
			size = (size / sizeof(char));

			/*allocate memory for the file*/
			file = malloc(sizeof(char)*size);
			for (i = 0; i < size; i++){
				file[i] = 0;
			}
			
			/*read the file from memory*/
			fread(file, size, sizeof(char), fp);
			file[size] = 0;

			/*close the file*/
			fseek(fp,0,SEEK_SET);
			/*fclose(fp);*/

			/*attempt to open the library directory*/
			dp = opendir("./lib");

			/*on success...*/
			if (dp){
				do {
					/*reinitialize libName string*/
					for (i = 0; i < 265; i++){
						libName[i] = 0;
					}
					curLib = readdir(dp);
					if (curLib){
		
						/*disregard . and .. entries*/
						if (strcmp(curLib->d_name, ".") && strcmp(curLib->d_name, "..")) {

							/*build the path string*/
							strcat(libName, "./lib/");
							strcat(libName, curLib->d_name);

							/*initialte server side includes with the current library*/
							file = SSInclude(file, libName);

						}
					}
				} while (curLib); /*do for every entry in this directory*/	
			}
		}

		/* send header */
		write(client_sockfd, response1, strlen(response1));
		
		/*if the file was found, send it. if not, send the 404 page*/
		if (fp) write(client_sockfd, file, (strlen(file)));
		else write(client_sockfd, E404, (strlen(E404)));

		/*close the socket*/
		close(client_sockfd);
		
	}
}

/*Function to implement serrver-side includes with a specific dynamically loaded library*/
/*Derived from Dynamically Linked Library examples by D. Calvert*/
char * SSInclude( char * page, char *libName ){
	/*set up variables*/

	/* handle used to reference library */
	void *handle = NULL;

	/* function pointesr - used to point at functions in the library */
	char * (*search)() = NULL;
	char * (*replace)() = NULL;

	/* points to the return values of functions */
	char * srch = NULL;
	char * rplc = NULL;

	/*pointer to an instance of the search string*/
	char * curr =  NULL;

	/*string size counter*/
	int ctr = 0, newlen = 0;

	/*return string and temporary page string*/
	char * ret = NULL, * work = NULL;

	/*loop iterator and controller*/
	int i = 0;

	/* open library */
	handle = dlopen(libName, RTLD_LAZY);

	/*fprintf(stderr, "library open: [%s]\n", libName);*/

	/* link the functions named searchString and replaceString to the function pointers */
	if (handle) {
		search = dlsym(handle, "searchString");
		replace = dlsym(handle, "replaceString");

		/* call the function in the library to get the search String*/
		srch = search();
		curr = page;
	
		/*create initial temporary file*/
		work = malloc(sizeof(char) * strlen(page));
		strcpy(work, page);


		/*parse the document for that search string*/
		do { 
			
			curr = work;
			
			/*search for the target string*/
			curr = strstr(curr,srch);

			/*if not found, exit*/
			if (!curr) break;

			/*get the replacement string*/
			rplc = replace();

			/*calculate the # of characters before the target string*/
			ctr = strlen(work) - strlen(curr);

			/*create/initialize the new file*/
			newlen = (ctr + strlen(rplc) + strlen(&curr[strlen(srch)]));

			ret = malloc(sizeof(char)*(newlen));
			for (i = 0; i < (newlen); i++){
				ret[i] = 0;
			}

			/*scan the star of the old file into the new one*/
			for (i = 0; i < ctr; i++){
				ret[i] = work[i];
			}

			/*append the replacment string and the rest of the file*/
			strcat(ret, rplc);
			strcat(ret, &curr[strlen(srch)]);

			/*destroy old temp and replace it with the new ret*/
			if (work) free(work);
			work = NULL;
			work = ret;

		} while (1);

		/* close the handle pointing to the library */
		dlclose(handle);
	}

	/*clean up*/
	if (page) free(page);
	page = NULL;
	ret = NULL;
	

	/*make sure the string is terminated*/
	work[strlen(work)]=0;

	/*return the temp pointer, as this has the new file whether it was mdoified or not*/
	return work;
}

