/*hstring.c*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hstring.h"

#define BLOB_SIZE 65535

/*String appending tool*/
/*creates a new string of the appropriate length, derived by appending str2 to the snd of str1*/
/*Does not affect input strings. creates new return string*/
/*don't forget to free the new string in the upper function*/
char * happend(char * str1, char * str2){
	char * ret = NULL;
	int a = 0, len = 0;
	/*calculate the length of the new string*/
	if (str1 && str2){
		len = (strlen(str1) +strlen(str2) + 1);
	
		/*allocate and initialize*/
		ret = malloc(sizeof(char)*len);
		for (a = 0; a < len; a++){
			ret[a] = 0;
		}

		/*stick the strings in there*/
		strcat(ret,str1);
		strcat(ret,str2);
		
		/*null-terminate the string*/
		ret[strlen(ret)] = 0;
	} else if (str1){
		return hstrclone(str1);
	} else if (str2){
		return hstrclone(str2);
	} 
	
	return ret;
}

/*String cloning tool*/
/*creates a new string of the appropriate length, and copies the contents of the old one into it*/
/*Does not affect the input string. creates a new return string*/
/*don't forget to free the new string in the upper function*/
char * hstrclone(char * str){
	
	char * ret = NULL;

	if (str){
		/*allocate the new string in memory*/
		ret =  malloc(sizeof(char)*(strlen(str)+1));

		/*copy the old string in and null-term it*/
		strcpy(ret, str);
		ret[strlen(str)] = 0;
	}

	return ret;
}

/*Substring removal tool*/
/*creates a new string of the appropriate length, derived by removing targ from str*/
/*Does not affect input strings. creates new return string*/
/*don't forget to free the new string in the upper function*/
char * hremovestr(char * str, char * targ){

	char * end = NULL, * ret = NULL, * tmp = NULL;
	int a = 0, len = 0;

	if (str && targ){
		/*clone the string to avoid messing with input*/
		ret = hstrclone(str);

		/*find the target string*/
		tmp = strstr(ret, targ);

		/*if it's found...*/
		if (tmp){

			/*calc length of trailing string*/
			len = strlen(tmp) - strlen(targ) + 1;

			/*allocate and initialize trailing string*/
			end = malloc(sizeof(char)*len);
			for (a=0; a< len; a++){
				end[a] = 0;
			}

			/*copy the end of the old string into the trail*/
			strcat(end, &tmp[strlen(targ)]);

			/*terminate the pre-target string*/
			tmp[0] = 0;

			/*append the trail to the new 'end' of the string*/
			tmp = happend(ret,end);

			/*clean up*/
			if(ret) free(ret);
			ret = tmp;
			tmp = NULL;
			free(end);
		}
	}
	
	return ret;
}

int hreplacestring(char * str, char * targ, char * new, char * buf, int size){
	char temp[size], ret[size], * tmp = NULL;
	int i = 0, len = 0, flag = 0;
	
	for(i=0;i<size;i++){ temp[i] = 0; ret[i] = 0; }
	strncpy(ret, str, size);
	tmp = &ret[0];
	while(tmp){
		if (tmp){
			tmp = strstr(tmp, targ);
			if (tmp){
				flag = 1;
				len = strlen(ret) - strlen(tmp);
				for(i=0;i<len;i++) temp[i] = ret[i];
				strncat(temp, new, size - strlen(temp));
				tmp = &tmp[strlen(targ)];
				strncat(temp, tmp, size - strlen(temp));
				strncpy(ret, temp, size);
				tmp = &tmp[strlen(new)-1];
				for(i=0;i<size;i++) temp[i] = 0;
			}
		}
	}

	strncpy(buf, ret, size);
	
	return flag;

}

/*reads at most size characters from a file and strores them in buf*/

int hreadfile(char * filename, char * buf, int size){
	FILE * fp = NULL;
	int i = 0;
	char file[size], ch = 0;

	for(i=0;i<size;i++) file[i] = 0;
	
	fp = fopen(filename, "r");
	if (fp){
		
		for(i=0;ch!=EOF && i < size;i++){
			ch = fgetc(fp);
			file[i] = ch;
		}
		if (i == size){
			/*if file is too large, return -1*/
			if (fp) fclose(fp);
			return -1;
		}
		/*zero the rest of the string*/
		for(i=i-1;i<size;i++)file[i] = 0;
	
		for (i=0;i<strlen(file);i++){
			buf[i] = file[i];
		}
		/*close the file*/
		if (fp) fclose(fp);
		return 0;
	} else {
		buf = NULL;
		return -2; /*error -2, file does not exist*/
	}

}

/*tool to save a string as a textfile*/
void hwritefile(char * filename, char * file){
	FILE * fp = fopen(filename, "w");

	/*open the file, push the string into it, and null-term it*/
	if (fp){;
		fputs(file, fp);
		fputc(0, fp);
		fclose(fp);
	}
}

/*tool to append a string to the end of a file*/
void happendfile(char * filename, char *str, int size){
	char * file = NULL, * new = NULL;
	int rval = 0;

	rval = hreadfile(filename, file, size);
	
	/*read in the file to a string*/
	if (file && !rval){
		/*append the tail string to the file string*/
		new = happend(file, str);
		free(file);

		/*write the new file*/
		hwritefile(filename, new);
	} else {
		/*if the file doesnt alreayd exist, just create it with the given string*/
		hwritefile(filename, str);
	}
}

/*tool to remove any whitespace at the beginning of a string*/
/*NOTE: THIS WILL BE UPGRADED TO REMOVE WHITESPACE AT THE BIGINNING AND END, INCLUDING NEWLINES AND TABS*/
char * hftrim(char * input){
	char * start = NULL;
	start = input;
	while(start[0] == ' ') start = &start[1];
	return hstrclone(start);
}


/*Functions to make:
queues, stacks, linked lists*/
