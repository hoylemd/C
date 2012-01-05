/************************************************************
 * Early networking exercise to request a file from a server
 *  and have it delivered through multiple connections
************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFFSIZE 2000
#define NUMCHUNKS 6
#define MAX_FILE_SIZE 500000
#define DEBUG 1
#define DATAPORT 11808
#define FILENAME "Assignment2.pdf"
#define WRITENAME "Assignment2b.pdf"
#define DD(x) fprintf(stderr, "%s;%d: %s\n"m __FILE__, __LINE__, x);

int Open (char *, int);
char * buildReq (short, short, char*);
int Listen (int);
void printoutfromsocket(int);

int main (void){
	char * req = NULL;
	char mess[BUFFSIZE];
	char * data = NULL;
	char file[MAX_FILE_SIZE];
	int sockNum = 0, recSock = 0;
	int i = 0;
	int rc=0, rcr = 0;
	int accepted = 0;
	int cs=0;
	int chunkiterator = 0;
	int offset= 0;
    struct sockaddr_in from;
    socklen_t fromlen;
	FILE * fp = NULL;
	int bytecounter = 0;

	while(1){

	req = NULL;
	for(i = 0; i < BUFFSIZE ; i++) mess[i] = 0;
	data = NULL;
	for(i = 0; i < MAX_FILE_SIZE ; i++) file[i] = 0;
	sockNum = 0;
	recSock = 0;
	i = 0;
	rc=0;
	rcr = 0;
	accepted = 0;
	cs=0;
	chunkiterator = 0;
	offset= 0;
    fromlen = 0;
	fp = NULL;
	bytecounter = 0;

	//build the request string
	req = buildReq(NUMCHUNKS, DATAPORT, FILENAME);

	//report request
	printf("sent request for \"%s\" in %d chunks over port %d\n", FILENAME, NUMCHUNKS, DATAPORT);

	//initialize buffers



	//open socket
	sockNum = Open("localhost", 11708);
	if (DEBUG) fprintf(stderr, "Socket Open\n");

	//start listening for response
	recSock = Listen(DATAPORT);

	//send the request
	rc = send( sockNum , req , (strlen(&req[4])+ 4) , 0 ) ;
	printf( "Sent %d\n%s\n" , rc , req ) ;

	//clean up
	free(req);
	req = NULL;
	close(sockNum);
		
	//start loop for each connection
	for (chunkiterator = 0; chunkiterator < NUMCHUNKS; chunkiterator++){
		//initialize variables
		for(i = 0; i < BUFFSIZE ; i++) mess[i] = 0;
		accepted = 0;
		offset =0;

		//try to accept a connection
		while (!accepted){
		    if (DEBUG) fprintf(stderr, "trying to accept connection. accepted = %d\n", accepted);
			fromlen = sizeof(from);
			cs = accept( recSock , (struct sockaddr *)&from  , &fromlen );
	
		    if (DEBUG) fprintf(stderr, "cs = %d\n", cs);
	
			//if it failed, report it
			//if it succeeded, raise tthe flafg
		    if (cs == -1){
				perror("accept:");
		    }else{
				(accepted = 1);
			}
		}
	
		if (DEBUG) fprintf(stderr, "done accepting. cs = %d\n", cs);
	   
		do {
			//cleart buffer
			if (DEBUG) fprintf(stderr, "clearing buffer\n");
			for(i = 0; i < BUFFSIZE ; i++) mess[i] = 0;

			//read from the socket into the buffer
			rc = recv( cs , &mess , sizeof(mess) , 0 ) ;
			if (DEBUG) fprintf(stderr, "finished reading %d bytes from socket. offset is currently %d\n", rc, offset);		
			if (rc == -1)
			    perror("recv");
			//if something was in fact read
			if (rc){
				//print it out
				//if (DEBUG) fprintf(stderr, "recieved: [%s]\n", mess);

				//if this  is the start of the packet find the offset
				if (offset == 0){
					offset = ntohl(*((int*)mess));
					if(DEBUG) fprintf(stderr, "firstoffset = %d\n", offset);
					data = &mess[4];
					rcr = rc - 4;

				// otherwise deal with it as normal
				} else {
					data = mess;
					rcr = rc;
				}

				if (DEBUG) fprintf(stderr, "done calculating offset and data pointers\n");
				
				//copy the data character by character into the file buffer
				for(i = 0; i < rcr; i++){
					//if (DEBUG) fprintf(stderr, "112: writing \'%c\' from index %d in buffer to index %d in file\n", data[i], i, *offset+i);
					file[offset] = data[i];
					offset++;
					bytecounter++;
				}
				if (DEBUG) fprintf(stderr, "done copying data\n");
			}
		} while (rc);

		close(cs);
	}

	//fprintf(stderr, "Final File:\n%s\nEnd of File\n", file);

	fp = fopen(WRITENAME, "w+");

	fwrite(file, bytecounter, 1, fp);

	fclose(fp);
	
	printf("wrote %d bytes to the file\n", bytecounter);

	close(recSock);	

	usleep(5000000);
	

	}

	return 0;
}

void printoutfromsocket(int cs){
    char mess[50000];
    int a;
    int rc;
    
    for (a=0;a<50000;a++)mess[a]=0;
    rc = recv(cs, &mess, sizeof(mess), 0);
    
    fprintf(stderr, "Recieved %d bytes:\n[%s]\n", rc, mess);
    
    if (rc == -1) perror ("recv in printoutfromsocket:");
    
}

int Listen( int port)
{
	struct sockaddr_in lsSA ;
	int ls ;



	if (DEBUG) fprintf(stderr, "Lysander listening to port number %d\n" , port ) ;
	ls = socket( PF_INET , SOCK_STREAM , 0 ) ; // 0 = TCP
	if( ls == -1 ) {
		perror( "Socket" ) ;
		exit( -1 ) ;
	}

	bzero( &lsSA , sizeof lsSA ) ;
	lsSA.sin_family = AF_INET ;
	lsSA.sin_addr.s_addr = htonl( INADDR_ANY ) ;
	lsSA.sin_port = htons( port ) ;


	if( bind( ls , (void *) &lsSA , sizeof lsSA ) == -1 ) {
		perror( "Bind" ) ;
		exit( -1 ) ;
	}
	if( listen( ls , 5 ) < 0 ) {
		perror( "Listen" ) ;
		exit( -1 ) ;
	} 
	
	if (DEBUG) fprintf(stderr, "Lysander done listening\n");
	return ls ;
}

char * buildReq(short num, short port, char * file){
	int mun = 0, trop = 0;
	int i = 0;
	char * req = NULL;

	req = malloc(sizeof(char) * 64);

	mun = htons(num);
	trop = htons(port);

	req[0] = mun % 256;
	req[1] = mun / 256;
	req[2] = trop % 256;
	req[3] = trop / 256;

	printf("file:%s\n", file);

	for (i=0; i< strlen(file); i++){
		req[i+4] = file[i];
	}

	req[strlen(file)+4] = 0;

	return req;
}

int Open( char *url, int port )
{
	int cs;
	struct sockaddr_in *csSA ;
	struct hostent *server ;
	//fprintf(stderr, "35\n");
	bzero( &csSA , sizeof csSA ) ;
	//fprintf(stderr, "37\n");
	
	csSA = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset( (char *)csSA , 0 , sizeof( struct sockaddr_in) ) ;

	csSA->sin_family = AF_INET;
	csSA->sin_port = htons(port);
	//fprintf(stderr, "44\n");
	server = gethostbyname(url);

	//fprintf(stderr, "45\n");
	memcpy( (char *) &csSA->sin_addr , server->h_addr , server->h_length ) ;
	//fprintf(stderr, "47\n");
	cs = socket(PF_INET , SOCK_STREAM , 0);    // Stolen arguments!!!
	if( cs == -1 ) {
		printf( "Socket failed\n" ) ;
		exit( -1 ) ;
	}
	//fprintf(stderr, "53\n");
	if( connect(  cs , (struct sockaddr*) csSA , sizeof( struct sockaddr_in))){ // Stolen arguments!!!
		printf( "Connect failed c\n") ;
		exit( -1 ) ;
	}
	//fprintf(stderr, "58\n");
	
	free(csSA);

	return cs ;
}
