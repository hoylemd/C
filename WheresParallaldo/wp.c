/* Michael D. Hoyle
	0553453
	wp.c
	Desription:
	A working version of the Where's Parallaldo! program.  searches for image patterns in other images... with parallalism!
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include "pilot.h"

#define DEBUG 0 


// Pilot variables
PI_PROCESS ** Workers;	// Worker processes
PI_CHANNEL ** ToWorker;	// Worker Channels
PI_CHANNEL ** Results;	// Result channels
PI_BUNDLE  * Selector; // selectir bundle

// struct to hold information about the location of a parallaldo
typedef struct
{
	int x;
	int y;
	int rotation;
} parallaldo_loc;

// struct to hold information about an image (target or parallaldo)
typedef struct
{
	int height;
	int width;
	char ** matrix;
} image;

void DebugMessage(char * mess)
{
	if(DEBUG) printf("%s\n", mess);
}

// function to read in a file as an image struct
image * parseImage(char * filename, int index)
{
	FILE * fp;
	int i = 0;
	char buffer[8192];
	image * ret = malloc(sizeof(image));

	 if (DEBUG) printf("parseImage (%d)", index);
	DebugMessage(filename);
	// open the file
	fp = fopen(filename, "r");
	DebugMessage("File Opened:");
	// read in the header
	fgets(buffer, 8192, fp);
	DebugMessage("header read");
	// parse out the height and width
	sscanf(buffer, "%d %d", &ret->height, &ret->width);

	DebugMessage("Allocating Matrix");
	// allocate the rows
	ret-> matrix = malloc(sizeof(char*)*ret->height);
	if(DEBUG) printf("Matrix Allocated (%d x %d)\n", ret->height, ret->width);
	// allocate and read in the columns
	for(i=0; i< ret->height; i++)
	{
		
		//if(DEBUG)printf("allocating row %d of %d\n", i+1, ret->height);
		ret->matrix[i] = malloc(sizeof(char)*ret->width);
		//printf("58(%d)\n", i);
		fgets(buffer, ret->width + 2, fp);
		strncpy(ret->matrix[i], buffer, ret->width);
		
	}

	// close the file
	fclose(fp);

	DebugMessage("Parse Complete");
	// return the image struct
	return ret;
}

// function to deconstruct an image struct
void freeImage(image * target)
{
	int i = 0;

	DebugMessage("freeImage");

	// free the columns
	for (i=0; i < target->height; i++)
	{
		free(target->matrix[i]);
	}

	// free the rows
	free(target->matrix);

	// free the struct
	free(target);

	DebugMessage("Image Freed");
}

// image to rotate an image struct 90 degrees
// returns a new one
image * rotateImage( image * original )
{
	int i = 0, j = 0;
	image * new = malloc(sizeof(image));

	DebugMessage("rotateImage");
	
	// switch the height and width in the new one
	new ->height = original ->width;
	new ->width = original ->height;

	// allocate the rows in the new one
	new -> matrix = malloc(sizeof(char*)*new->height);

	// place the matrix from the old one itno the new one, rotated 90 degrees
	for (i=0; i < new->height; i++)
	{
		new->matrix[i] = malloc(sizeof(char)*new->width);

		for (j=0; j< new->width; j++)
		{
			new->matrix[i][j] = original->matrix[original->height - (1 + j)][i];
		}
	}

	// return the new image struct
	return new;
}

void printImage(image * target)
{
	int i = 0;
	for (i=0; i < target->height; i++)
		printf(">>> %s\n",target->matrix[i]);
}

// function to find the first occurance of a parallaldo in a target image
parallaldo_loc * findParallaldo( image * target, image * parallaldo)
{
	parallaldo_loc * ret = NULL;
	image * rot0, * rot90, *rot180, * rot270;
	int i = 0, j = 0, okRows = 0;
	char potential[8192];

	DebugMessage("findParallaldo");
	
	// calculate the rotations of the parallaldo
	rot0 = parallaldo;
	rot90 = rotateImage(rot0);
	rot180 = rotateImage(rot90);
	rot270 = rotateImage(rot180);

	//if(DEBUG) printImage(target);
	//if(DEBUG) printf("\n");
	//if(DEBUG) printImage(parallaldo);
	if (DEBUG) printf("target height %dx%d\n", target->height, target->width);
	DebugMessage("images Loaded");

	// step through each point of the target matrix
	for (i=0; i< target->height; i++)
	{
		//printf("164\n");
		//printf("%d:::rot90->width: %d\n", i, rot90->width);

		//printf("\n");
		for(j=0; j < target->width; j++)
		{
			//printf("!!!!!!!!!!!!!!!Checking at [%d][%d] in file sized %d by %d\n", i, j, target->height, target->width);	
			// check for unrotated parallaldo
			okRows = 0;
			ret = NULL;
			//printf("173(%d)(%d)\n",i, j);
			strncpy(potential, &(target->matrix[i][j]), strlen(&(target->matrix[i][j])));
			//printf("175(%d)(%d) pot: %s targ %s\n", i, j, potential, &(target->matrix[i][j]));
			potential[rot0->width] = 0;
			//printf("176(%d)(%d) rot90->width = %d\n",i, j, rot90->width);
			if ((rot0-> height <= (target->height - i))&& (rot0-> width <= (target->width - j)))
			{
				
				while ((!strncmp(potential, rot0->matrix[okRows], rot0->width)) && ((okRows+1) < rot0->height))
				{
					DebugMessage("potential rot0");
					okRows++;
					strncpy(potential, &target->matrix[i+okRows][j], strlen(&target->matrix[i+okRows][j]));
					potential[rot0->width] = 0;
	
				}
				if ((okRows+1) == rot0->height)
				{
					// we've found it, construct the return struct
					ret = malloc(sizeof(parallaldo_loc));
					ret-> y = (j+1);
					ret-> x = (i+1);
					ret-> rotation = 0;
				
					DebugMessage("found unrotated parallaldo!!!!!");	
					// advance the indicies of the loops to the end
					i = target->height;
					j = target->width;
					break;
				}
			}
			//printf("203(%d)(%d) rot90->width = %d\n",i, j, rot90->width);
			// check for parallaldo rotated 90 degrees
			okRows = 0;
			//printf("207  strlen = %c, @ij = ", target->matrix[i][j]);	
			strncpy(potential, &target->matrix[i][j], strlen(&target->matrix[i][j]));
			//printf("b4208\n");
			//printf("208(%d) - %d\n", j, rot90->width);
			potential[rot90->width] = 0;
			//printf("210(%d)\n", j);
			if ((rot90-> height <= (target->height - i))&& (rot90-> width <= (target->width - j)))
			{
				while ((!strncmp(potential, rot90->matrix[okRows], rot90->width)) && ((okRows+1) < rot90->height))
				{
					DebugMessage("potential find 90");
					okRows++;
					strncpy(potential, &target->matrix[i+okRows][j], strlen(&target->matrix[i+okRows][j]));
					potential[rot90->width] = 0;
	
				}
				if ((okRows+1) == rot90->height)
				{
					// we've found it, construct the return struct
					ret = malloc(sizeof(parallaldo_loc));
					ret-> y = (j+1);
					ret-> x = (i+1);
					ret-> rotation = 90;
					
					DebugMessage("found rotated 90 parallaldo!!!!!");	
					// advance the indicies of the loops to the end
					i = target->height;
					j = target->width;
					break;
				}
			}
			//printf("234(%d)\n", j);	
			// check for parallaldo rotated 180 degrees
			okRows = 0;

			strncpy(potential, &target->matrix[i][j], strlen(&target->matrix[i][j]));
			potential[rot180->width] = 0;
			if ((rot180-> height <= (target->height - i))&& (rot180-> width <= (target->width - j)))
			{
				while ((!strncmp(potential, rot180->matrix[okRows], rot180->width)) && ((okRows+1) < rot180->height))
				{
					DebugMessage("potential find 180\n");
					okRows++;
					strncpy(potential, &target->matrix[i+okRows][j], strlen(&target->matrix[i+okRows][j]));
					potential[rot180->width] = 0;
	
				}
				if ((okRows+1) == rot180->height)
				{
					// we've found it, construct the return struct
					ret = malloc(sizeof(parallaldo_loc));
					ret-> y = (j+1);
					ret-> x = (i+1);
					ret-> rotation = 180;

					DebugMessage("found rotated 180 parallaldo!!!!!");	
					// advance the indicies of the loops to the end
					i = target->height;
					j = target->width;
					break;
				}
			}

			//printf("266(%d)\n", j);
			// check for parallaldo rotated 270 degrees
			okRows = 0;
			strncpy(potential, &target->matrix[i][j], strlen(&target->matrix[i][j]));
			potential[rot270->width] = 0;
			if ((rot270-> height <= (target->height - i))&& (rot270-> width <= (target->width - j)))
			{
				while ((!strncmp(potential, rot270->matrix[okRows], rot270->width)) && ((okRows+1) < rot270->height))
				{
					DebugMessage("potential find 270");
					okRows++;
					strncpy(potential, &target->matrix[i+okRows][j], strlen(&target->matrix[i+okRows][j]));
					potential[rot270->width] = 0;
	
				}
				if ((okRows+1) == rot270->height)
				{
					// we've found it, construct the return struct
					ret = malloc(sizeof(parallaldo_loc));
					ret-> y = (j+1);
					ret-> x = (i+1);
					ret-> rotation = 270;
	
					DebugMessage("found rotated 270 parallaldo!!!!!");	
					// advance the indicies of the loops to the end
					i = target->height;
					j = target->width;
					break;
				}
			
			}
			//DebugMessage("not here!");
			//ret = NULL;
		}
	}

	// deconstruct the rotated parallaldos
	freeImage(rot90);
	freeImage(rot180);
	freeImage(rot270);

	DebugMessage("done findParallaldo");
	// return the location structure of the parallaldo
	// will be null if not found
	return ret;
}

// function to run a worker process
int doWork(int index, void * arg2)
{
	int DONE_FLAG = 0, i = 0;
	char buffer[128], target[128], parallaldo[128];
	image * targetImage, *parallaldoImage;
	parallaldo_loc * ret = NULL;
	char reporting[128];

	sprintf(reporting, "Worker # %d started%c", index, 0);
	DebugMessage(reporting);
	
	// main loop
	while (!DONE_FLAG)
	{
		// Rinitialize buffers.
		for(i=0; i<128; i++)
		{
			buffer[i] = 0;
			target[i] = 0;
			parallaldo[i] = 0;
		}
		// read in a string from the channel
		PI_Read(ToWorker[index], "%128c", buffer);

		// if it's not an end-of-line char we have a job to do
		if (strncmp(buffer, "ALL_DONE", 9) && strncmp( buffer, "", 1))
		{	
			// save the string read as the target filename
			strncpy(target, buffer, strlen(buffer));

			// read in the parallaldo filename
			PI_Read(ToWorker[index], "%128c", parallaldo);
			
			// construct the image structs
			targetImage = parseImage(target, index);
			parallaldoImage = parseImage(parallaldo, index);

			// find parallaldo!
			ret = findParallaldo(targetImage, parallaldoImage);
			if(DEBUG) printf("findParallaldo complete %d\n", index);
			
			// deconstruct the image structs
			freeImage(targetImage);
			targetImage = NULL;
			freeImage(parallaldoImage);
			parallaldoImage = NULL;
		
			
			DebugMessage(" images deconstructed"); 
			// if parallaldo was found
			if (ret != NULL)
			{
				DebugMessage("parallaldo Found.");
				if (ret == NULL) printf("Null\n");
				ret->y = 5;
				if(DEBUG) printf("parallaldo at: %d,%d . %drotated degrees\n", ret->y, ret->x, ret->rotation);
				// report that it was found
				PI_Write(Results[index], "%6c", "found");
				PI_Write(Results[index], "%d %d %d %128c %128c", ret->y, ret->x, ret->rotation, parallaldo, target);
				DebugMessage("success reported");
			} else
			{
				DebugMessage("parallaldo not found\n");
				PI_Write(Results[index], "%6c", "not");
				DebugMessage("failure message sent\n");
			}
		}  else	
		{	// if the process recieved an end-of-line, it's all done and can exit.
			DONE_FLAG = 1;	
			// write "not found" to the result channel so if it gets queried stuff doesnt dealock.
			PI_Write(Results[index], "%6c", "not");
		}
	}
	return 0;
}

int main( int argc, char *argv[] )
{
	// Pilot configuration phase; return no. of processes available	
	int N = PI_Configure( &argc, &argv );
	int i = 0, j = 0, k = 0;
	int W = N-1;
	int y = 0, x = 0, r = 0;
	int lastWorker = 0, numRounds = 0, taskNum = 0;
	DIR * dp = NULL;
	struct dirent * file;
	char targetList[128][128], parallaldoList[128][128], filename[128];
	char taskList[1024][2][128]; 			//task list: [task id][parallaldo(0)/target(1)][filename]
	double op1 = 0, op2;
	char * targ, * paral, report[32], buff1[128], buff2[128];
	int numTargets = 0, numParallaldos = 0, numTasks = 0, finished = 0, numDone = 0;
	parallaldo_loc * result = NULL;
	image * laldo = NULL, *arget = NULL;

	DebugMessage("wp Process initiated");
	// Create the procs and channels, if we have enough processors for parallelization
	if (W)
	{
		// allocate the process pointer array
		Workers = malloc(sizeof(PI_PROCESS*)*W);

		// allocate channel arrays
		ToWorker = malloc(sizeof(PI_CHANNEL*)*W);
		Results = malloc(sizeof(PI_CHANNEL*)*W);
		

		// allocate selector bundle
		Selector = malloc(sizeof(PI_BUNDLE*));

		// create the processes and channels
		for(i = 0; i< W; i++)
		{
			Workers[i] = PI_CreateProcess(doWork, i, (void*)NULL);
			ToWorker[i] = PI_CreateChannel(PI_MAIN, Workers[i]);
			Results[i] = PI_CreateChannel(Workers[i], PI_MAIN);	
		} 
	Selector = PI_CreateBundle(PI_SELECT, Results, W);
	// Otherwise, it's serial time!
	}
	
	// read in the directories
	dp = opendir(argv[argc-2]);

	while((file = readdir(dp)) != NULL)
	{
		if (strncmp(file->d_name, ".", strlen(file->d_name)) && strncmp(file->d_name, "..", strlen(file->d_name)))
		{
			sprintf(filename, "%s/%s", argv[argc-2], file->d_name);
			strncpy(parallaldoList[numParallaldos], filename, strlen(filename));
			numParallaldos++;
		}
	}

	closedir(dp);

	dp = opendir(argv[argc-1]);
	
	while((file = readdir(dp)) != NULL)
	{
		if (strncmp(file->d_name, ".", strlen(file->d_name)) && strncmp(file->d_name, "..", strlen(file->d_name)))
		{
			sprintf(filename, "%s/%s", argv[argc-1], file->d_name);
			strncpy(targetList[numTargets], filename, strlen(filename));
			numTargets++;
		}
	}

	closedir(dp);


	// Compile task list
	for (i = 0; i < numParallaldos; i++)
	{
		for(j = 0; j < numTargets; j++)
		{
			strncpy(taskList[numTasks][0], parallaldoList[i], strlen(parallaldoList[i])); 
			strncpy(taskList[numTasks][1], targetList[j], strlen(targetList[j]));
			numTasks++; 	
		}
	}

	// PI_MAIN stuff

	if (W)
	{
		
		lastWorker = 0;
		op1 = numTasks;
		op2 = W;
		numRounds = ceil(op1 / op2);
	
		PI_StartAll();

		if (strncmp("-b", argv[1], 2))
		{
			if (DEBUG) printf("MASTER: numRounds = %d numTasks = %d\n", numRounds, numTasks);
			for (i=0; i < numRounds; i++)
			{
				// assign tasks round-robin
				for (j = 0; j < W; j++)
				{
					taskNum = (i * W)+j;
					if(DEBUG) printf("MASTER: assigning task %d to Worker %d\n", taskNum, j);				
					if (taskNum <= numTasks)
					{
						PI_Write(ToWorker[j], "%128c", taskList[taskNum][1]);
						PI_Write(ToWorker[j], "%128c", taskList[taskNum][0]);
					} else
					{
						PI_Write(ToWorker[j], "%128c", "ALL_DONE");
					}
				}
				// collect results round-robin
				for (j = 0; j < W; j++)
				{
					for( k = 0; k < 7; k++) report[k] = 0;
					taskNum = (i*W)+j;
					if(DEBUG) printf("MASTER: retrieving result %d from Worker %d\n", taskNum, j);
					PI_Read(Results[j], "%32c", report);
					if(DEBUG) printf("MASTER: result recieved: [%s]\n", report);
					if (!strncmp(report, "found", strlen("found")))
					{
						//parallaldo found!
						DebugMessage("MASTER: reporting found parellaldo");
						PI_Read(Results[j], "%d %d %d %128c %128c", &y, &x, &r, buff1, buff2);
	
						paral = strchr(taskList[taskNum][0], '/');
						paral = &paral[1];
		
						targ = strchr(taskList[taskNum][1], '/');
						targ = &targ[1];
		
						printf("$%s %s (%d,%d,%d)\n", paral,targ,y, x, r);

					} else DebugMessage("MASTER: task completed, no parellaldo");
				}
			}
			free(result);
		} else
		{
			//distribvute initial jobs
			for (i=0; i<W; i++)
			{
				PI_Write(ToWorker[i], "%128c", taskList[i][1]);
				PI_Write(ToWorker[i], "%128c", taskList[i][0]);
				taskNum++;
			}	

			while(numDone < W)
			{
				DebugMessage("MASTER: waiting for results\n");
				finished = PI_Select(Selector);
				PI_Read(Results[finished], "%32c", report);
				if(DEBUG) printf("MASTER: result recievedi: %s\n", report);
				if (!strncmp(report, "found", strlen("found")))
				{
					DebugMessage("MASTER: parallaldo found. printinf information");
					//parallaldo found!
					PI_Read(Results[finished], "%d %d %d %128c %128c", &y, &x, &r, buff1, buff2);
	
					if(DEBUG) printf("MASTER: results, last task assigned - %d: y = %d x = %d r = %d\n",numTasks, y, x, r);
					paral = strchr(buff1, '/');
					if(DEBUG) printf("MASTER: %s\n", paral);
					paral = &paral[1];
	
					if(DEBUG) printf("MASTER: parapath constructed: %s\n", paral);
					targ = strchr(buff2, '/');
					targ = &targ[1];
					
					if(DEBUG)printf("MASTER: targetpath constructed: %s\n", targ);
					printf("$%s %s (%d,%d,%d)\n", paral,targ,y, x, r);
					DebugMessage("MASTER: Parallaldo info reported");
				}
				
				if (taskNum < numTasks)
				{
					if (DEBUG) printf("MASTER: Assigning new task %d of %d to Worker %d\n", taskNum, numTasks, finished);
					PI_Write(ToWorker[finished], "%128c", taskList[taskNum][1]);
					PI_Write(ToWorker[finished], "%128c", taskList[taskNum][0]);
					taskNum++;
					if (DEBUG) printf("MASTER: New Task Assigned\n");
				} else
				{
					if (DEBUG) printf("MASTER: Worker %d is done.\n", finished);
					PI_Write(ToWorker[finished], "%128c", "ALL_DONE");
					numDone++;
					if (DEBUG)printf("Worker %d done\n", finished);
				}
			}
		}
	} else
	{
		for(i=0; i<numTasks; i++)
		{
			DebugMessage("GETTING IMAGES");
			laldo = parseImage(taskList[i][1], 0);
			arget = parseImage(taskList[i][0], 0);
			result = findParallaldo(laldo, arget);
			freeImage(laldo);
			freeImage(arget);

			if (result)
			{ 
				paral = strchr(taskList[i][0], '/');
				paral = &paral[1];

				targ = strchr(taskList[i][1], '/');
				targ = &targ[1];

				printf("$%s %s (%d,%d,%d)\n", paral,targ,result->y, result->x, result->rotation);
			}
			if(result) free(result);
		}
	}

	// end program
	

	return 0;
}
