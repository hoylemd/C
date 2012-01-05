/*hstring.c*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*String appending tool*/
/*creates a new string of the appropriate length, derived by appending str2 to the snd of str1*/
/*Does not affect input strings. creates new return string*/
/*don't forget to free the new string in the upper function*/
char * happend(char * str1, char * str2);

/*String cloning tool*/
/*creates a new string of the appropriate length, and copies the contents of the old one into it*/
/*Does not affect the input string. creates a new return string*/
/*don't forget to free the new string in the upper function*/
char * hstrclone(char * str);

/*Substring removal tool*/
/*creates a new string of the appropriate length, derived by removing targ from str*/
/*Does not affect input strings. creates new return string*/
/*don't forget to free the new string in the upper function*/
char * hremovestr(char * str, char * targ);

/*tool to save a string as a textfile*/
void hwritefile(char * filename, char * file);

/*tool to append a string to the end of a file*/
void happendfile(char * filename, char *str, int);

/*tool to trim whitespace off the front of a string*/
char * hftrim(char *);

int hreadfile(char * filename, char * buf, int size);

int hreplacestring(char * str, char * targ, char * new, char * buf, int size);
