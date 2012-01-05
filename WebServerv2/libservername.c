/*libdate.c*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

char * searchString() {
	return "SERVERNAME";
}

char * replaceString() {
	char * hname;
	int a;

	hname = malloc(sizeof(char)*256);

	for (a=0; a<128; a++) hname[a] = 0;

	/*grab the host name from the system and return it*/
	if (!gethostname(hname, 256)) return hname;
	else return "Host Name Error";
}
