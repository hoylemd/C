#include"assignment3-server.h"

int main(void)
{
	int sid ;

	sid = startserver() ;
	while( 1 )
		 getrequest( sid ) ;
	close(sid);
	return 0;
 }

int startserver()
{
	 int sid;
	 struct addrinfo hints, *servinfo ;
	 int i , rv ;

	 memset(&hints, 0, sizeof hints);
	 hints.ai_family = AF_INET;
	 hints.ai_socktype = SOCK_DGRAM;
	 hints.ai_flags = AI_PASSIVE; // use the host's IP 
	 if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) == -1) {
		 fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		 exit( -1 ) ;
	 }
	if ((sid = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		perror("server: socket");
		exit( -1 ) ;;
	}
	if (bind(sid, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		perror("server: bind");
		close(sid);
        exit( -1 ) ;
	 }
	 freeaddrinfo(servinfo);

	 for( i = 0 ; i < MAXCLIENTS ; i++ )
		 Client[i].inuse = 0 ;

	 printf( "Server starts using port %s\n" , MYPORT ) ;
	 fflush( stdout ) ;
	 return sid ;
 }

void getrequest( int sid )
{
	char buf[MAXBUFLEN];
	int i , numbytes;
	struct sockaddr_in from ;
	size_t from_len ;

	printf("server: waiting to recvfrom...\n");
	fflush( stdout ) ;
	waitforinput( sid ) ;	// returns when recvfrom() will not block
	from_len = sizeof from;
	if ((numbytes = recvfrom(sid, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&from, &from_len)) == -1) {
		perror("recvfrom");
		return ;
	}

	printf("server: request from %s:%d  "
        , inet_ntoa( from.sin_addr ) , from.sin_port ) ;
	printf(" %d bytes\n", numbytes );
	fflush( stdout ) ;
    buf[numbytes] = '\0' ;
	for( i = 0 ; i < MAXCLIENTS ; i++ )
		if( Client[i].inuse
				 && Client[i].addr.sin_addr.s_addr
					 == from.sin_addr.s_addr
				 && Client[i].addr.sin_port == from.sin_port )
			 break ;		// existing client
	 if( buf[0] == '\0' ) {	// cryptic: network = bigendian
		 if( i == MAXCLIENTS ) {
			 printf( "Unknown client wants a retransmission\n" ) ;
			 return ;
		 }
		 Client[i].idle = 0 ;
		 retransmit( sid , Client[i] , buf ) ;
	 } else {	// request for object
		 printf( "New request %s\n" , buf ) ;
		 printf(" \"%s\"\n", buf); 
		 if( i == MAXCLIENTS ) { 	// new client
			 for( i = 0 ; i < MAXCLIENTS ; i++ )
				 if( !Client[i].inuse ) break ;
			 if( i == MAXCLIENTS ) {
				 for( i = 0 ; i < MAXCLIENTS ; i++ )
					 if( Client[i].idle > MAXWAIT ) {
						 printf( "Closing client %d\n" , i ) ;
						 Client[i].inuse = 0 ;
					 }
				 for( i = 0 ; i < MAXCLIENTS ; i++ )
					 if( !Client[i].inuse ) break ;
				 if( i == MAXCLIENTS ) {
					 printf( "No more clients allowed\n" ) ;
					 return ;
				 }
			 } 
			 printf( "New client %d\n" , i ) ;
		 } else
			 printf( "Existing client %d\n" , i ) ;
		 Client[i].idle = 0 ;
		 Client[i].inuse = 1 ;
		 Client[i].addr = from ;
		 Client[i].addr_len = from_len ;
		 copyimage( buf , &Client[i] ) ;
		 sendimage( sid , &Client[i] ) ;
	 } 
}

void copyimage( char *image , struct Client *C )
{
	int isock ;
	int n ;
    int i , j ;

	printf( "server: getting %s\n" , image ) ;
	fflush( stdout ) ;
    if( image[0] == 'h' && image[1] == 't'
        && image[2] == 't' && image[3] == 'p' ) {
            char req[300] = "GET " ;
            char host[100] ;
            for( i = 0 ; image[i] != ':' ; i++ ) ;
            if( image[i+1] != '/' || image[i+2] != '/' ) {
                strcpy( C->Im , "Incorrect format of url" ) ;
                C->Imlen = strlen( C->Im ) ;
            } else {
                i += 3 ;
                for( j = 0 ; image[i] != '/' ; j++ , i++ )
                    host[j] = image[i] ;
                host[j] = '\0' ;
                strcat( req , image ) ;
                strcat( req , " HTTP/1.1\r\nHost: " ) ;
                strcat( req , host ) ;
                strcat( req , "\r\nContent-Length: 0\r\nConnection: close\r\n\r\n" ) ;
                printf( "Contacting %s with request:\n%s" , host , req ) ;
                fflush( stdout ) ;
                isock = Open( host ) ;
                n = send( isock , req , strlen( req ) , 0 ) ;
                printf( "server sent %d bytes to %s\n" , n , host ) ;
                C->Imlen = recv( isock , C->Im , MAXSIZE , MSG_WAITALL ) ;
                printf( "Received from the web %d bytes\n" , C->Imlen ) ;
                for( i = 0 ; i < C->Imlen-4 ; i++ )
                    if( C->Im[i] == '\r' && C->Im[i+1] == '\n'
                        && C->Im[i+2] == '\r' && C->Im[i+3] == '\n' )
                        break ;
                if( i < C->Imlen-5 ) {
                    j = 0 ;
                    for( i = i + 4 ; i < C->Imlen ; i++ )
                        C->Im[j++] = C->Im[i] ;
                    C->Imlen = j ;
                }
                fflush( stdout ) ;
            }
            C->Im[C->Imlen] = '\0' ;
            printf( "server received from the web: %s\n" , C->Im ) ;
    } else {    // it must be a local file
        int fd ;
        printf( "opening %s\n" , image ) ;
        fflush( stdout ) ;
        if( (fd = open( image , O_RDONLY )) < 0 ) {
            perror( "open" ) ;
            strcpy( C->Im , "File could not be opened" ) ;
            C->Imlen = strlen( C->Im ) ;
        } else if( (n = read( fd , C->Im , MAXSIZE )) < 0 ) {
            perror( "Could not get image" ) ;
            strcpy( C->Im , "File could not be read" ) ;
            C->Imlen = strlen( C->Im ) ;
        } else {
            printf( "copied %d bytes from a local file\n" , n ) ;
            fflush( stdout ) ;
            C->Imlen = n ;
        }
        close( fd ) ;
    }
	return ;
}

void sendimage( int sid , struct Client *C )
{
	int i ;
	partition( C ) ;
	for( i = 0 ; i < C->numdat ; i++ )
		 if( C->Dlen[i] > 0 )
			 if( (sendto( sid , C->Im+C->Dptr[i] , C->Dlen[i]
             , 0,  (struct sockaddr*)&C->addr , C->addr_len )) < 0 ) {
				 perror( "send" ) ;
				 return ;
			 }
}

void partition( struct Client *C )
{
	int N , F , L /*, R*/ ;
    int i , ind , count ;
    int P[8] ;
// 3 datagrams are necessary
	if( C->Imlen < 16 ) {
		printf( "No way to send %d bytes in 3 datagrams\n" , C->Imlen ) ;
		C->numdat = 0 ;
		return ;
	} else if( C->Imlen > 500000 ) {
        printf( "This protocol cannot handle %d bytes\n" , C->Imlen ) ;
        C->numdat = 0 ;
        return ;
    }
    printf( "Dividing image of %d bytes into datagrams\n" , C->Imlen ) ;
    for( ind = 0 ; ind < 7 ; P[ind++] = 0 ) ;
    F = 1024 ;
    ind = 7 ;
    count = 0 ;
    while( F > 4 ) {
        N = C->Imlen - F - (F - 1)  ;
        for( L = F-2 ; L < N && L > 1 ; L-- ) 
            N -= L ;
        if( N > 0 && L > 1 ) {   // Success
            P[ind] = 1 ;
            count++ ;
        }
        F = F / 2 ;
        ind-- ;
    }
    srand( getpid() ) ;
    ind = 1 + rand() % count ;
    F = 4 ;
    for( i = 0 ; ind > 0 ; i++ ) {
        F *= 2 ;
        if( P[i] )
            ind-- ;
    }
    N = C->Imlen - F ;
    C->Dlen[0] = F ;
    for( i = 3 , L = F-1 ; L < N && L > 1 ; i++ , L-- ) {
        N -= L ;
        C->Dlen[i] = L ;
    }
    C->Dlen[1] = L + 1 ;
    C->Dlen[2] = N ;
    C->numdat = F - L + 1 ;
    C->Dptr[0] = 0 ;
    for( i = 1 ; i < C->numdat ; i++ )
        C->Dptr[i] = C->Dptr[i-1] + C->Dlen[i-1] ;
    for( i = 0 ; i < C->numdat ; i++ )
        printf( " %d (%d) " , C->Dptr[i] , C->Dlen[i] ) ;
    printf( "\n" ) ;
    for( i = 0 ; i < 3 ; i++ )
        Three[i] = C->Dlen[i] ;

	if( SHUFFLE > 0 ) shuffle( C ) ;
	if( LOST > 0 ) lose( C ) ;
}

void shuffle( struct Client *C )
{
	int i , j , k , t ;
	for( i = 0 ; i < SHUFFLE ; i++ ) {
		k = rand() % (C->numdat < 5 ? C->numdat-1 : 4) + 1 ;
		j = rand() % (C->numdat-k) ;
		printf( "Swapping %d %d\n" , j , j+k ) ;
		t = C->Dlen[j] ;
		C->Dlen[j] = C->Dlen[j+k] ;
		C->Dlen[j+k] = t ;
		t = C->Dptr[j] ;
		C->Dptr[j] = C->Dptr[j+k] ;
		C->Dptr[j+k] = t ;
	}
	printf( "SHUFFLE: " ) ;
	for( i = 0 ; i < C->numdat ; i++ )
		printf( "%d[%d]  " , C->Dlen[i] , C->Dptr[i] ) ;
	printf( "\n" ) ;
}

void lose( struct Client *C )
{
	int i , k ;
	for( i = 0 ; i < LOST ; i++ ) {
		do {
			k = rand() % C->numdat ;
		} while( C->Dlen[k] == 0 ) ;
		C->Dlen[k] = -C->Dlen[k] ;
	}
	printf( "DELETED: " ) ;
	for( i = 0 ; i < C->numdat ; i++ )
		printf( "%d  " , C->Dlen[i] ) ;
	printf( "\n" ) ;
}

void retransmit( int sid , struct Client C , char buf[] )
{
	short Breq[256] ;
	int i , j , n , len ;
	bcopy( buf , (char *)Breq , 2 ) ;
	n = ntohs( Breq[0] ) ;
	printf( "server: retransmission list length=%d\n" , n ) ;
	fflush( stdout ) ;
	bcopy( buf , (char *)Breq , 2*n+2 ) ;
	for( i = 1 ; i <= n ; i++ ) {
		len = ntohs(Breq[i]) ;
		printf( "%d  " , len ) ;
        if( len == 1024 || len == 2 || len == 1 ) {
            if( len == 1024 )
                len = Three[0] ;
            else if( len == 2 )
                len = Three[1] ;
            else
                len = Three[2] ;
        }
		for( j = 0 ; j < C.numdat ; j++ )
			if( C.Dlen[j] == len || C.Dlen[j] == -len )
				break ;
		if( j < C.numdat && len > 0 ) {
			printf( "\nretransmitting %d bytes\n" , len ) ;
			fflush( stdout ) ;
			if( (sendto( sid , C.Im+C.Dptr[j] , len , 0,  (struct sockaddr*)&C.addr , C.addr_len )) < 0 ) {
				 perror( "send" ) ;
				 return ;
			 }
		} else
            printf( "unknown datagram\n" ) ;
	}
	fflush( stdout ) ;
	return ;
}

void waitforinput( int sid )
{
	fd_set fds , tfds ;
	struct timeval timeout ;
	int i , rv ;
	int elapsed = 0 ;
#define WAIT 30

	FD_ZERO( &fds ) ;
	FD_SET( sid , &fds ) ;
	while( 1 ) {
		tfds = fds ;
		timeout.tv_sec = WAIT ;
		timeout.tv_usec= 0 ;
		rv = select( sid+1 , &tfds , NULL , &tfds , &timeout ) ;
		switch( rv ) {
			case -1:
				perror( "Select failed" ) ;
                close( sid ) ;
				exit( -1 ) ;
			case 0:
				elapsed += WAIT ;
				continue ;
			default:
				for( i = 0 ; i < MAXCLIENTS ; i++ )
					Client[i].idle += elapsed ;
				return ;
		}
	}
}

int Open( char *url )
{
	int cs ;
	struct sockaddr_in csSA ;
	struct hostent *server ;

	bzero( &csSA , sizeof csSA ) ;

	csSA.sin_family = AF_INET ;
	csSA.sin_port = htons( HTTPPORT ) ;
	server = gethostbyname( url ) ;
	memcpy( (char *) &csSA.sin_addr , server->h_addr , server->h_length ) ;

	if( (cs = socket( PF_INET , SOCK_STREAM , 0 )) == -1 ) { // 0 = TCP
		printf( "Socket failed\n" ) ;
		exit( -1 ) ;
	}
	if( connect( cs , (void *) &csSA , sizeof csSA ) == -1 ) {
		printf( "Connect failed\n" ) ;
		exit( -1 ) ;
	}
    printf( "server: connected to %s\n" , url ) ;
	return cs ;
}
