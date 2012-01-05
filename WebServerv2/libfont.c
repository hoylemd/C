/*liblastupdate.c*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "hstring.h"



char * searchString() {
	return "FONT =x str";
}

/*format: = NUM STR*/
char * replaceString(char * params) {
	char *returntag = NULL, *temp2 = NULL, *temp = NULL, *temp3 = NULL;
	int ctr = 0, i= 0, fd = 0;

	if (params){
		/*start building the font tag*/
		returntag = hstrclone("<font size = ");

		/*append the size number*/
		temp = &params[1];
		while(*temp == ' ') temp = &temp[1];
		while(temp[ctr] != ' ') ctr+=1;
		temp2 = malloc(sizeof(char)*(ctr+1));
		for(i = 0; i <=ctr; i++) temp2[i] = 0;
		for(i = 0; i < ctr; i++) temp2[i] = temp[i];
		temp3 = happend(returntag, temp2);
		if (returntag) free(returntag);
		returntag = temp3;
		temp3 = NULL;

		/*append the face attribute*/
		if (temp2) free(temp2);
		temp2 = NULL;
		temp2 = happend(returntag, " face = ");
		if (returntag) free(returntag);
		returntag = temp2;
		temp2 = NULL;

		/*append the font face string*/
		temp = &temp[ctr+1];
		while(*temp == ' ') temp = &temp[1];
		temp2 = happend(returntag, temp);
		temp = NULL;
		if(returntag) free(returntag);
		returntag = temp2;
		temp2 = NULL;

		/*close the tag*/
		temp2 = happend(returntag, ">");
		if (returntag) free(returntag);
		returntag = temp2;
		temp2 = NULL;
	} else  returntag = "</font>"; /*if there are no parameters, return it as a closing tag*/

	return returntag;
}



