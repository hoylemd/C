#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include "A3.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "hstring.h"
#include <dlfcn.h>
#include <unistd.h>
#define MAX_LENGTH 1024

/*pipe name*/
char pipeName[64], lpipeName[64];

/*log pipe*/
FILE * lpipe = NULL;

/*log struct*/
typedef struct {
   char pageName[MAX_LENGTH];
   char browserName[MAX_LENGTH];
   unsigned long date;
} Log;

/*global debug variable for printing out debug messages*/
/*0 = no debug messages*/
/*1 = main debug messages*/
/*2 = verbose mode*/
int debug = 0;

/*function to connect to the pipe*/
JNIEXPORT jint JNICALL Java_A3_openFifo(JNIEnv *env, jobject obj, jstring name){
	jboolean iscopy;
	int i = 0;
	FILE * cpipe = NULL;

	/*convert the string argument into a c string*/
	const char *cName = (*env)->GetStringUTFChars(env, name, &iscopy);

	/*save the pipe name*/
	for (i=0; i < 64; i++) pipeName[i] = 0;
	strncpy(pipeName, cName, strlen(cName));

	if (debug)fprintf(stderr, "opening pipe '%s'\n", cName);

	cpipe = fopen(pipeName, "w");

	/*return success or failure, and failure message if applicable*/
	if (cpipe){
		if(debug) fprintf(stderr, "success\n");
		fclose(cpipe);
		return 1;
	}else{
		if(debug) perror("openFifo");
		return 0;
	}
}

/*function to send a command through the pipe*/
JNIEXPORT jint JNICALL Java_A3_sendCommand (JNIEnv *env , jobject obj, jstring command){
	jboolean iscopy;
	FILE * cpipe = NULL;

	/*convert the string srgument to a c string*/
	const char *cCom = (*env)->GetStringUTFChars(env, command, &iscopy);

	return pushCommand(cCom);
}

int pushCommand (const char * string){
	/*open the pipe*/
	FILE * cpipe = fopen(pipeName, "w");

	/*push the string into the pipe*/
	if (cpipe){
		if(debug) fprintf(stderr, "Sending '%s' through the pipe\n", string);
		fputs(string, cpipe);
		fclose(cpipe);
		return 1;
	}else{
		if(debug) fprintf(stderr, "Pipe not connected; Can not send command.\n");
		return 0;
	}
}

/*function to load the names of all the libraries in a given directory*/
JNIEXPORT jstring JNICALL Java_A3_getlibs(JNIEnv *env, jobject obj, jstring path){
	DIR * dp;
	jboolean iscopy;
	char libName[256], * ret = NULL, * tmp = NULL;
	struct dirent *curlib;
	int i;	

	/*convert the argument string into a c string*/
	const char * cpath = (*env)->GetStringUTFChars(env, path, &iscopy);

	/*open the directory*/
	dp = opendir(cpath);

	/*on success*/
	if (dp){
		do {
			/*initialize libName string*/ 
			for (i = 0; i < 256; i++){
				libName[i] = 0;
			}

			/*read in the next entry*/
			curlib = readdir(dp);

			/*if an entry was read*/
			if (curlib){
				/*read only .so files*/
				if (strstr(curlib->d_name, ".so")) {
					if (strlen(strstr(curlib->d_name, ".so")) == 3){
						if (ret){
							/*add a space*/
							tmp = happend(ret, " ");
							if(ret) free(ret);
							ret = tmp;

							/*add the library name*/
							tmp = happend(ret, curlib->d_name);
							if(ret) free(ret);
							ret = tmp;
							
tmp = NULL;
						} else {
							/*create the initial return string*/
							ret = hstrclone(curlib->d_name);
						}
					}
				}
			}
		}while (curlib);
		if (debug) fprintf(stderr, "Library names read: %s\n", ret);
	}

	/*close the directory*/
	closedir(dp);

	/*return the appropriate string*/
	if (ret) return (*env)->NewStringUTF(env, ret);
	else return (*env)->NewStringUTF(env, "");
}

/*tool to clone a string*/
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

/* tool to append a string onto another and creat a new string*/
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

/*function to read a log file*/
JNIEXPORT jstring JNICALL Java_A3_readlogs(JNIEnv *env, jobject obj){
	FILE * logpipe;
	char ret[5000], chr;
	int ctr = 0, i = 0;
	
	for(i = 0; i < 5000; i++) ret[i] = 0;

	pushCommand("l");

	logpipe = fopen(lpipeName, "r");
	
	/*read the log entry until end of string*/
	while (!strlen(ret)){
		do{
			if (fread(&chr, sizeof(char), 1, logpipe)){
				ret[ctr] = chr;
				ctr += 1;
			} else chr = 0;
		} while(chr);
	}
	if (debug) fprintf(stderr, "read '%s' from the pipe\n", ret);
	return (*env)->NewStringUTF(env, ret);
}

JNIEXPORT jint JNICALL Java_A3_openLFifo(JNIEnv *env, jobject obj, jstring name){
	jboolean iscopy;
	int i = 0;
	char dummy;
	/*convert the string argument into a c string*/
	const char *cName = (*env)->GetStringUTFChars(env, name, &iscopy);

	/*save the pipe name*/
	for (i=0; i < 64; i++) lpipeName[i] = 0;
	strncpy(lpipeName, cName, strlen(cName));

	if (debug)fprintf(stderr, "opening pipe '%s'\n", cName);

	mkfifo(cName, 0666 | O_RDWR);

	lpipe = fopen(lpipeName, "r");

	/*return success or failure, and failure message if applicable*/
	if (lpipe){
		if(debug) fprintf(stderr, "success\n");\
		fread(&dummy, sizeof(char), 1, lpipe);
		return 1;
	}else{
		if(debug) perror("openFifo");
		return 0;
	}
}

JNIEXPORT jstring JNICALL Java_A3_readlogsfromfile(JNIEnv *env, jobject obj){
	
	FILE * fd = NULL;
	int i = 0;
	char logentry[5000], logstring[5000], temporary[5000];
	Log * nLog = NULL;
	
	for (i=0; i<5000; i++){ logentry[i] = 0; logstring[i] = 0; temporary[i] = 0;} 

	fd = fopen("access.log", "r");	

	if (fd){
		if (debug) fprintf(stderr, "opened file successfully");
		while(!feof(fd)){
			/*initialize the log storage area and read in the next log entry*/
			nLog=  malloc(sizeof(Log));
			for(i = 0; i < MAX_LENGTH; i++) {nLog->pageName[i] = 0; nLog->browserName[i] = 0;}
			nLog->date = 0;
			fread(nLog, sizeof(Log), 1, fd);
			if (debug) fprintf(stderr, "read log entry\n");
			/*generate a log entry string*/
			sprintf(logentry, "%s %s %lu", nLog->pageName, nLog->browserName, nLog->date);
			logentry[strlen(logentry)] = 0;
			if (debug) fprintf(stderr, "constructed log entry\n");
			/*if it is a valid log entry*/
			if (strcmp(logentry, "  0")){
				if (debug) fprintf(stderr, "valid log entry [%s]\n", logentry);
				/*add it to the return string*/
				if (strlen(logstring)){
					sprintf(temporary, "%s;%s", logstring, logentry);
					temporary[strlen(temporary)] = 0;
					strncpy(logstring, temporary, 5000);
				} else {
					strncpy(logstring, logentry, 5000);
				}
			}
			if (debug) fprintf(stderr, "done validation\n");
			/*clear the temporary strings*/
			for( i = 0; i < 5000; i++) {temporary[i] = 0; logentry[i] = 0;} 
			if (debug) fprintf(stderr, "cleared temp strings\n");
			/*clear the log stroage area*/
			if(nLog) free(nLog);
			nLog = NULL;
		}
		fclose(fd);
	} else sprintf(logstring,"None\n");

	if (!strlen(logstring)) sprintf(logstring,"None\n");

	return (*env)->NewStringUTF(env, logstring);	

}
