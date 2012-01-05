/*liblastupdate.c*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include "hstring.h"

char * searchString() {
	return "LASTUPDATE str";
}

/*format: name path*/
char * replaceString(char * params) {
	char * ret = NULL, *name = NULL, *path = NULL;
	int ctr = 0, i= 0, fd = 0;
	struct stat buffer; 

	/*parse the first word as the name*/
	while(params[ctr] != ' ' && params[ctr] != 0) ctr += 1;
	name = malloc(sizeof(char)*(ctr+1));
	for (i = 0; i <= ctr; i++) name[i] = 0;
	for (i = 0; i < ctr; i++) name[i] = params[i];

	/*parse the second word as the path name*/
	if (strchr(params, ' ')) path = hftrim(strchr(params, ' '));
	else path = NULL;

	/*translate the stat's time into human time*/
	if(stat(path, &buffer) == 0)
		return ctime(&(buffer.st_mtime));
	else {
		return "FILE DOES NOT EXIST";
	}	
}



