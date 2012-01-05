/*********************************************************************
 * Early networking exercise
 * Requests a file to be sent in short packages, handles lost packets
 ********************************************************************/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

#define BUFF 1024
#define PORT 4950
#define DEBUGMODE 1	
#define SERVERADDY "127.0.0.1"
#define byte unsigned char
#define FILENAME "spacemarine.jpg"
#define WRITENAME "kantor.jpg"

//function to print an error message and quit
void diep(char *s){
	perror(s);
	exit(1);
}

void sendRequest(short * packets, short numpackets, int sock, const struct sockaddr* si, int slen){
	short * req;
	int i = 0;
	
	req = (short *) malloc(sizeof(short)*256);
	
	req[0] = htons(numpackets);
	
	for (i=0; i< numpackets; i++){
			req[i+1] = htons(packets[i]);
	}
	if (sendto(sock, req, 2*(numpackets+1), 0, si, slen) ==-1){
		diep("request sendto()");
	}
	
	free(req);
}

int recieveDgrams(int s, char ** map, int mapsize, int fieldsize, int * sizes, int entries){
	int recflag = 1, numbytes = 0, numpackets = 0, i;
	fd_set sset;
	struct timeval timeout;
	struct sockaddr * from;
	socklen_t *flen;
	char * buffer;
	
	from = malloc(sizeof(struct sockaddr));
	flen = malloc(sizeof(socklen_t));
	while (recflag){
									
		FD_ZERO(&sset);					//set up the fd set
		FD_SET(s,&sset);
		
		timeout.tv_sec = 5;				//set the timeout
		timeout.tv_usec = 0;
		//printf("timeout set\n");
		
		buffer = (char *) malloc(sizeof(char) * fieldsize);
		for(i=0;i<fieldsize;i++) buffer[i] = 0;
		
		//printf("fd set up\n");
										//check if the socket has stuff
										//times out after 10 seconds
		if(select(s+1, &sset, (fd_set *) 0, (fd_set *) 0, &timeout)){
			//printf("select done\n");
										//read data from the buffer
			if ((numbytes = recvfrom(s, buffer, fieldsize, 0,from, flen)) == -1) {
				diep("recvfrom");
			} else {
				//printf("recieved %d bytes\n", numbytes);
										//store the data in a map slot
				buffer[numbytes] = 0;
				map[entries+numpackets] = buffer;
				sizes[entries+numpackets] = numbytes;
				//printf("copied\n");
				numpackets++;			//increment the packet counter
			}
		} else {						//if it timed out
			//printf("timeout\n");
			recflag = 0;
		}
	}
	
	return numpackets;
}

char * getChunk(char ** map, int * sizes, short size, int numchunks){
	int i = 0;	
	
	//printf("finding chunk %d\n", size);
	for (i = 0; i < numchunks; i++){
		//printf("comparing %d, to target %d\n", (short)strlen(map[i]), size);
		if (sizes[i] == size){
			//printf("match\n");
			return map[i];
		}
	}
	
	return NULL;
}

//main function
int main(void){
	struct sockaddr_in si_other;		//address structs
	int s = 0, slen = sizeof(si_other);	//socket fd and address struct size 
	int numpackets = 0, totalpacks = 0, offset = 0;					//counter for packets recieved
	int i = 0, n = 0, f, l, r, nummissing=0;				//loop controllers
	char * buff, * file, *chunk = NULL;						//buffer for recieved data
	char ** map, **dummy;						//map to store incoming data
	short list[500], mlist[500];					//list of packets to request 
	FILE* fp;
	int * sizes, *dsizes;
										//malloc the buffer
	buff = (char *) malloc(sizeof(char) * BUFF);
	map = (char **) malloc(sizeof(char * ) * 500);
	sizes = (int *) malloc(sizeof(int) * 500);
	dummy = (char**) malloc(sizeof(char * ) * 500);
	dsizes = (int *) malloc(sizeof(int) * 500);
	file = (char*) malloc(sizeof(char) * (1024 * 500));
	
										//initialize the map and buffer
	memset((char *) map, 0, sizeof(map));	
	memset((char *) buff, 0, sizeof(buff));
	memset((char *) list, 0, sizeof(list));
	memset((char *) mlist, 0, sizeof(mlist));
	memset((char *) dummy, 0, sizeof(dummy));
										//set up the socket
	if ((s=socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP))==-1){
		diep("Socket");
	}
	
										//initialize the address struct
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	if (inet_aton(SERVERADDY, &si_other.sin_addr) == 0){
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
				
	printf("sending initial request\n");			//send the request
	sprintf(buff, FILENAME);
	//printf("packet construicted\n");
	if (sendto(s, buff, BUFF, 0, (const struct sockaddr*)&si_other, slen) ==-1){
		diep("sendto()");
	}
	
	printf("recieving datagrams\n");
	numpackets = recieveDgrams(s, map, 500, BUFF, sizes, 0);
	printf("data recieved\n");
	/*printf("data recieved:\n");			//printf out the data recieved
	for(i=0;i<numpackets;i++){
			printf("%s", map[i]);
	}*/
	
										//set up the list of packets to request
	list[0] = 1024;
	list[1] = 2;
	list[2] = 1;
	sendRequest(list, 3, s, (const struct sockaddr *)&si_other, slen);
	
	while (recieveDgrams(s, dummy, 500, BUFF, dsizes,0) != 3);
	
	f = dsizes[0];
	l = dsizes[1];
	r = dsizes[2];
	
	printf("f = %d, l = %d, r = %d\n", f, l, r);
	
	//compose packet list
	list[0] = f;
	list[1] = l;
	list[2] = r;
	totalpacks = 3;
	for(i = f-1; i > l; i--){
		list[totalpacks] = i;
		totalpacks++;
	}
	
	//printf("list composed\n");
	
	//ensure all packets were recieved
	while (numpackets != totalpacks){
		//printf("we have some missing packets\n");
		for (i=0; i <totalpacks; i++){
			//printf("checking for packet %d\n", list[i]);
			chunk = getChunk(map,sizes,list[i], totalpacks);
			if (chunk == NULL){
				//printf("chunk %d is missing\n", list[i]);
				mlist[nummissing] = list[i];
				//printf("added missing chunk to list");
				nummissing++;
			}
		}
		
		for(i=0; i< nummissing; i++) printf("Missing datagram %d\n", mlist[i]);
		sendRequest(mlist, nummissing, s, (const struct sockaddr *)&si_other, slen);
		//printf("sent request for missing\n");
		numpackets += recieveDgrams(s,map,500,BUFF,sizes, numpackets);
		//printf("reieved response of %d\n", numpackets);
		
	}
	
	//for(i=0; i< totalpacks; i++) printf("%d\n", list[i]);
	
	printf("saving file\n");
	
	//print the file
	for (i=0; i <totalpacks; i++){
		chunk = getChunk(map,sizes,list[i], totalpacks);
		for(n=0; n < list[i]; n++){
			file[offset] = chunk[n];
			offset++;
		}
	}
	
	//printf("\n%s\n", file);
	
	fp = fopen(WRITENAME, "w+");

	fwrite(file, offset, 1, fp);

	fclose(fp);
	
	printf("\n");
	
	free(buff);							//clean up
	free(map);
	free(dummy);
	free(file);
	close(s);

	return 0;							// Exit
}

