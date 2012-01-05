#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define MYPORT "4950"	// the port users will be connecting to
#define HTTPPORT 80

#define MAXWAIT 300
#define SHUFFLE 25
#define LOST 15

#define MAXBUFLEN 200
#define MAXDTGRMS 700
#define MAXCLIENTS 20
#define MAXSIZE 500000

struct Client {
	int inuse ;
	int idle ;
	struct sockaddr_in addr ;
	size_t addr_len ;
	char Im[MAXSIZE] ;
	int Imlen ;
	int numdat ;
	int Dlen[MAXDTGRMS] ;
	int Dptr[MAXDTGRMS] ;
} Client[MAXCLIENTS] ;

int Three[3] ;

int startserver() ;
void waitforinput( int ) ;
void getrequest( int ) ;
void sendimage( int , struct Client * ) ;
void retransmit( int , struct Client , char * ) ;
void copyimage( char * , struct Client * ) ;
void partition( struct Client * ) ;
void shuffle( struct Client * ) ;
void lose( struct Client * ) ;
int Open( char * ) ;
