/*libdate.c*/

#include <time.h>
#include <stdio.h>

char * searchString() {
	return "CURRENTDATE";
}

char * replaceString() {
	time_t tyme = (time_t)(-1);
	while(tyme == (time_t)(-1)){
		tyme = (int)time(NULL);
	}
	return ctime(&tyme);
}



