
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

// Installed Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Local includes
#include "graphics.h"
#include "perlin.h"

// Definitions because this aint C++ yet!
#define false 0
#define true 1

// Constants
#define SUN_ANCHOR_X 49
#define SUN_ANCHOR_Y 0
#define SUN_RADIUS 49
#define DAY_LENGTH 30
#define CLOUD_ALTITUDE 48
#define MEAN_GROUND_ALTITUDE 24
#define MAX_ROOMS 4
#define MAX_HALLS 50

typedef struct
{
	int id;
	int x;
	int y;
	int l;
	int w;
} Room;

typedef struct
{
	Room * r1;
	Room * r2;
	int xLength;
	int yLength;
	double dLength;
} Hall;

typedef struct
{
	int id;
	int numRooms;
	Room * rooms[MAX_ROOMS];
	int numHalls;
	Hall * halls[MAX_HALLS];
} Dungeon;

// Graphics function links
extern void gradphicsInit(int *, char **);					// Initialize the Graphics
extern void setLightPosition(GLfloat, GLfloat, GLfloat);	// Set the light(sun)'s position
extern GLfloat* getLightPosition();							// Get the light(sun)'s position
extern void setViewPosition(float, float, float);			// Set the viewpoint position
extern void getViewPosition(float *, float *, float *);		// Get the viewpoint position
extern void getOldViewPosition(float *, float *, float *);	// Get the viewpoint's last position
extern void getViewOrientation(float *, float *, float *);	// Get the viewpoint's orientation
extern int addDisplayList(int , int , int );				// Add a cube to the display list

// flag which is set to 1 when flying behaviour is desired
extern int flycontrol;

// flag used to indicate that the test world should be used
extern int testWorld; 

// list and count of polygons to be displayed, set during culling
extern int displayList[MAX_DISPLAY_LIST][3];
extern int displayCount;

