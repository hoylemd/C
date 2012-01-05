/**********************************************************
 * Early networking exercise
 * requests a page from www.cbc.ca and prints it to stdout
***********************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define Debug 0
#define OpenDebug 0
#define BUFFSIZE 500

//Current task: functionalize the request


int Open( char * ) ;
int Send( char *, int ) ;
int main(void){
	int cs, a;
	char req[] = "GET http://www.cbc.ca/news/ HTTP/1.1\r\nHost: www.cbc.ca\r\nContent-Length: 0\r\nKeep-Alive: timeout=10000\r\n\r\n" ;
	char req2[] = "GET http://www.cbc.ca/money/ HTTP/1.1\r\nHost: www.cbc.ca\r\nContent-Length: 0\r\nConnection: close\r\n\r\n" ;
	int rc ;
	char *mess = NULL, *mess2 = NULL;
	int dflag = 0;
	char * target = NULL;

	//Initialize buffers
	if(Debug) fprintf(stderr, "Initializing buffers...");
	mess = malloc(sizeof(char) * BUFFSIZE);
	mess2 = malloc(sizeof(char) * BUFFSIZE);
	for(a=0; a<BUFFSIZE;a++) {mess[a] = 0; mess2[a] = 0;}

	//Establish Connection
	if(Debug) fprintf(stderr, "Done.\nEstablishing connection...");
	cs = Open( "www.cbc.ca" ) ; // creates a socket
		// and opens a connection to the http server specified

	//Send( req, cs);

	///Send Request
	if(Debug) fprintf(stderr, "Done.\nSending request packet...");
	rc = send( cs , req , sizeof req , 0 ) ;

	if(Debug) fprintf(stderr, "Done.\n");
	printf( "Sent %d\n%s\n" , rc , req ) ;
	//Recieve Response and print it to screen
	do {

		//Buffer a block of the response
		rc = recv( cs , mess , BUFFSIZE , 0 ) ;
		
		if (Debug) fprintf(stderr, "Read from buffer\n");

		//If anything was read (nothing indicating the end of the response)
		if (rc){

			//Check if this is the beginning of the packet
			if (strstr(mess, "HTTP/1.1")){
				strncpy(mess2, strstr(mess, "HTTP/1.1" ), BUFFSIZE);
				target = strstr(mess2, "00004000");
				*target = 0; 				
				printf("%s\n", mess2);
				printf("..........\n");
			} else if (strstr(mess, "</body>")){
				dflag = 1;
				if (Debug) fprintf(stderr, "</body> tag found...\n");
				strncpy(mess2, strstr(mess, "</body>"), BUFFSIZE);
				printf("End:\n%s", mess2);
			} else if (dflag){
				if (Debug) fprintf(stderr, "Last little bit...\n");
				printf("%s", mess);
			}
			
			for (a=0; a<BUFFSIZE; a++) mess2[a] = 0;
			for (a=0; a<BUFFSIZE; a++) mess[a] = 0;
		}
	} while (rc && !dflag);
	dflag = 0;

	printf("\n..........\n\n\n");

	//Restart connection
	close( cs ) ;
	cs = Open( "www.cbc.ca" ) ;

	if(Debug) fprintf(stderr, "Done.\nSending request packet...");
	rc = send( cs , req2 , sizeof req2 , 0 ) ;
	rc = send( cs , req2 , sizeof req2 , 0 ) ;
	if(Debug) fprintf(stderr, "Done.\n");
	printf( "Sent %d\n%s\n" , rc , req2 ) ;
	
	//Recieve Response and print it to screen
	do {
		//Buffer a block of the response
		//if (Debug) fprintf(stderr, "Reading Response\n");
		rc = recv( cs , mess , BUFFSIZE , 0 ) ;

		//If anything was read (nothing indicating the end of the response)
		if (rc){
			if (Debug) fprintf(stderr, "Buffer Full...{%s}\n", mess);
			//Check if this is the beginning of the packet
			if (strstr(mess, "HTTP/1.1 200")){
				fprintf(stderr, "HEADER FOUND\n");
				strncpy(mess2, strstr(mess, "HTTP/1.1 200" ), BUFFSIZE);
				target = strstr(mess2, "00004000");
				*target = 0; 				
				printf("%s\n", mess2);
				printf("..........\n");
			} else if (strstr(mess, "</body>")){
				fprintf(stderr, "TRAILER FOUND\n");
				dflag = 1;
				if (Debug) fprintf(stderr, "</body> tag found...\n");
				strncpy(mess2, strstr(mess, "</body>"), BUFFSIZE);
				printf("End:\n%s", mess2);
			} else if (dflag){
				fprintf(stderr, "DFLAG IS UP\n");
				if (Debug) fprintf(stderr, "Last little bit...\n");
				printf("%s", mess);
			}
			for (a=0; a<BUFFSIZE; a++) mess2[a] = 0;
			for (a=0; a<BUFFSIZE; a++) mess[a] = 0;
			
		}
	} while (rc && !dflag);
	printf("\n..........\n\n");

	
	if(Debug) fprintf(stderr, "Finished. Closing Connection...");
	close( cs ) ;


	if(Debug) fprintf(stderr, "Done. \nFreeing Memory...");
	free(mess);
	mess = NULL;
	free(mess2);
	mess2 = NULL;


	if(Debug) fprintf(stderr, "Done. \nExiting\n"); 
	return 0;
}

/*Function to convert a string into an ip address useable by the system*/

unsigned long atoip( char *text )
{
    unsigned long ip ;
    int i , t ;
    i = 0 ;
    ip = t = 0 ;

    while( text[i] != '\0' ) {
        if( text[i] == '.' ) {
            ip = (ip<<8) + t ;
            t = 0 ;
        } else
            t = t*10 + text[i] - '0' ;
        i++ ;
    }
    return (ip<<8) + t ;
}

int Open( char *url )
{
	int cs;
	struct sockaddr_in *csSA ;
	struct hostent *server ;
	if(OpenDebug) fprintf(stderr, "Open subroutine initialized\n");
	bzero( &csSA , sizeof csSA ) ;
	if(OpenDebug) fprintf(stderr, "Zeroed Socket structure\n");
	
	csSA = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset( (char *)csSA , 0 , sizeof( struct sockaddr_in) ) ;

	csSA->sin_family = AF_INET;
	csSA->sin_port = htons(80);
	server = gethostbyname(url);
	memcpy( (char *) &csSA->sin_addr , server->h_addr , server->h_length ) ;
	if(OpenDebug) fprintf(stderr, "Socket initialized\n");

	cs = socket(PF_INET , SOCK_STREAM , 0);    // Stolen arguments!!!
	if( cs == -1 ) {
		printf( "Socket failed\n" ) ;
		exit( -1 ) ;
	}
	if(OpenDebug) fprintf(stderr, "Socket Locked. Chevron 1 Encoded...");
	if( connect(  cs , (struct sockaddr*) csSA , sizeof( struct sockaddr_in))){ // Stolen arguments!!!
		printf( "Connect failed c\n") ;
		exit( -1 ) ;
	}
	if(OpenDebug) fprintf(stderr, "Chevron 1 Locked. Wormhole Established.\n");
	
	free(csSA);

	return cs ;
}

int Send( char * req, int cs ){

	int rc = 0, a = 0, dflag = 0;
	char * mess = NULL, * mess2 = NULL, * target = NULL;

	//Initialize buffers
	if(Debug) fprintf(stderr, "Initializing buffers...");
	mess = malloc(sizeof(char) * BUFFSIZE);
	mess2 = malloc(sizeof(char) * BUFFSIZE);
	for(a=0; a<BUFFSIZE;a++) {mess[a] = 0; mess2[a] = 0;}

	//Send Request
	if(Debug) fprintf(stderr, "Done.\nSending request packet...");
	rc = send( cs , req , sizeof req , 0 ) ;
	if(Debug) fprintf(stderr, "Done.\n");
	printf( "Sent %d\n%s\n" , rc , req ) ;
	
	//Recieve Response and print it to screen
	do {
		if(Debug) fprintf(stderr, "Processing response\n");

		//Buffer a block of the response
		rc = recv( cs , mess , BUFFSIZE , 0 ) ;
		if(Debug) fprintf(stderr, "Read from Socket %d\n%s\n", rc, mess);
		//If anything was read (nothing indicating the end of the response)
		if (rc){
			if (Debug) fprintf(stderr, "Buffer Full...\n");
			//Check if this is the beginning of the packet
			if (strstr(mess, "HTTP/1.1 200 OK")){
				strncpy(mess2, strstr(mess, "HTTP/1.1 200 OK" ), BUFFSIZE);
				target = strstr(mess2, "00004000");
				*target = 0; 				
				printf("Header:\n%s\n", mess2);
				printf("***************************************************************\n");
			} else if (strstr(mess, "</body>")){
				dflag = 1;
				if (Debug) fprintf(stderr, "</body> tag found...\n");
				strncpy(mess2, strstr(mess, "</body>"), BUFFSIZE);
				printf("End:\n%s", mess2);
			} else if (dflag){
				if (Debug) fprintf(stderr, "Last little bit...\n");
				printf("%s", mess);
			}
			for (a=0; a<BUFFSIZE; a++) mess2[a] = 0;
			for (a=0; a<BUFFSIZE; a++) mess[a] = 0;
			
		}
	} while (rc);

	printf("\n");

	if(Debug) fprintf(stderr, "Done. \nFreeing Memory...");
	free(mess);
	mess = NULL;
	free(mess2);
	mess2 = NULL;

	return 0;
}

