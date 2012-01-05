//TODO: make this C++

/* Derived from scene.c in the The OpenGL Programming Guides */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

#include "a2.h"

// global variables
int tim = 0;			// Seconds passed counter
int prevtime = 0;		// The time of the last "tick"
int currtime = 0;		// Current time variable
int lightpos = 0;		// The x-position of the light(sun)
int prevsunypos = 0;	// The previous y-position of the light(sun)
int prevsunxpos = 0;	// The previous x-position of the light(sun)

// player position
int playerX = 0;
int playerY = 0;
int playerZ = 0;

// player frustum
float frustum[6][4];

// List for things in the frustum
int frustumList[MAX_DISPLAY_LIST][3];
int frustumCount = 0;

// List for potentially visible cubes
int visibleList[MAX_DISPLAY_LIST][3];
int visibleCount = 0;

// List for Animated cubes
int animatedList[MAX_DISPLAY_LIST][3];
int animatedCount = 0;

// Lists for Dungeon generation
Room * roomsList[MAX_ROOMS];
int roomsCount;
Hall * hallsList[MAX_HALLS];
int hallsCount;

// Cube Lists

// Function to add a cube to the frustum list
int addFrustumList(int x, int y, int z) {
	// copy the cube coordinates
	frustumList[frustumCount][0] = x;
	frustumList[frustumCount][1] = y;
	frustumList[frustumCount][2] = z;
	
	// increment the counter
	frustumCount++;
	
	// panic if too many have been added
	if (frustumCount > MAX_DISPLAY_LIST) {
		printf("You have put more items in the frustum list then there are\n");
		printf("cubes in the world. Set frustumCount = 0 at some point.\n");
		exit(1);
	}
}
// Function to add a cube to the visible list
int addVisibleList(int x, int y, int z) {
	// copy the cube coordinates
	visibleList[visibleCount][0] = x;
	visibleList[visibleCount][1] = y;
	visibleList[visibleCount][2] = z;
	
	// increment the counter
	visibleCount++;
	
	// panic if too many have been added
	if (visibleCount > MAX_DISPLAY_LIST) {
		printf("You have put more items in the visible list then there are\n");
		printf("cubes in the world. Set frustumCount = 0 at some point.\n");
		exit(1);
	}
}
// Function to add a cube to the animated list
int addAnimatedList(int x, int y, int z) {
	// copy the coordinated
	animatedList[animatedCount][0] = x;
	animatedList[animatedCount][1] = y;
	animatedList[animatedCount][2] = z;
	
	// increment the counter
	animatedCount++;
	
	// panic if there's been too many
	if (animatedCount > MAX_DISPLAY_LIST) {
		printf("You have put more items in the animated list then there are\n");
		printf("cubes in the world. Set frustumCount = 0 at some point.\n");
		exit(1);
	}
}

// Dungeon Component Lists

// Function to add a room to the room list
int addRoomList(Room * rm) {
	// store the pointer to the room
	roomsList[roomsCount] = rm;
	
	// increment the counter
	roomsCount++;
	
	// if there's been too many, panic
	if (roomsCount > MAX_ROOMS) {
		printf("too many rooms\n");
		exit(1);
	}
}
// Function to add a hall to the hall list
int addHallList(Hall * hl) {
	// store the hall pointer
	hallsList[hallsCount] = hl;
	
	// increment the counter
	hallsCount++;
	
	// panic if there's been too many
	if (hallsCount > MAX_HALLS) {
		printf("too many Halls\n");
		exit(1);
	}
}

// Culling Functions

// Get the frustum
// taken from http://www.crownandcutlass.com/features/technicaldetails/frustum.html   
ExtractFrustum() {
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
   frustum[0][0] /= t;
   frustum[0][1] /= t;
   frustum[0][2] /= t;
   frustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
   frustum[1][0] /= t;
   frustum[1][1] /= t;
   frustum[1][2] /= t;
   frustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
   frustum[2][0] /= t;
   frustum[2][1] /= t;
   frustum[2][2] /= t;
   frustum[2][3] /= t;

   /* Extract the TOP plane */
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
   frustum[3][0] /= t;
   frustum[3][1] /= t;
   frustum[3][2] /= t;
   frustum[3][3] /= t;

   /* Extract the FAR plane */
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
   frustum[4][0] /= t;
   frustum[4][1] /= t;
   frustum[4][2] /= t;
   frustum[4][3] /= t;

   /* Extract the NEAR plane */
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}

// check if a point is in the frustum
// taken from http://www.crownandcutlass.com/features/technicaldetails/frustum.html 
int PointInFrustum( float x, float y, float z )
{
   int p;

   for( p = 0; p < 6; p++ )
      if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] < 0 )
         return false;
   return true;
}
// Check if a cube is in the frustum
// taken from http://www.crownandcutlass.com/features/technicaldetails/frustum.html 
int CubeInFrustum( float x, float y, float z, float size )
{
   int p;

   for( p = 0; p < 6; p++ )
   {
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y - size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y - size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y + size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y + size) + frustum[p][2] * (z - size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y - size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y - size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x - size) + frustum[p][1] * (y + size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         continue;
      if( frustum[p][0] * (x + size) + frustum[p][1] * (y + size) + frustum[p][2] * (z + size) + frustum[p][3] > 0 )
         continue;
      return false;
   }
   return true;
}

// Collision detection and Gravity
void collisionResponse() {
    float x, y, z;
    float ox, oy, oz;
    int ix,iy,iz;
    int iox,ioy,ioz;
	
	// get the player's position
    getViewPosition(&x,&y,&z);
	
	// normalize for the world cube array
    ix = (int) x * (-1);
    iy = (int) y * (-1);
    iz = (int) z * (-1);

    // if we're not in ghost mode
    if (!flycontrol)
    {
		// Collisions
		// check that we're not trying to enter an occupied cube
		if (world[ix][iy][iz] != 0)
		{
			// DEBUGGING: report a collision has ocurred
			//fprintf(stderr,"collision at %d %d %d (%d)\n",ix,iy,iz,world[ix][iy][iz]); 
			
			// get the last position
			getOldViewPosition(&ox,&oy,&oz);
			
			// normalize for the world array
			iox = (int) ox * (-1);
			ioy = (int) oy * (-1);
			ioz = (int) oz * (-1);
			
			// check if there is space above the cube so we can climb onto it
			if (world[ix][iy+1][iz] == 0 && world[iox][ioy+1][ioz] == 0)
			{
				// DEBUGGING: report a climb has occurred
				//printf("climbing from %f,%f,%f to %f,%f,%f\n", ox,oy,oz,x,y-1,z);
				
				// increase the player's vertical position
				setViewPosition(x,y-1,z);
				
				// remember this change has occurred
				y-= 1;
				iy = (int) y * (-1);
			}
			else
			{
				// if the climb was not possible, bump back to previous location
				setViewPosition(ox,oy,oz);
				
				// remember that we've been moved back
				ix = (int) ox * (-1);
				iy = (int) oy * (-1);
				iz = (int) oz * (-1);
				
				// TODO: make is so that there can be wall sliding.
			}
			
			

		}
		
		// Gravity
		
		// if there is nothing underneath the player
		if (world[ix][iy-1][iz] == 0) 
		{
			// drop the player half a unit
			setViewPosition(x,y+0.5,z);
			
			// TODO: implement gravitational accelleration
		}
    }
}

// Idle update function
void update() {
	// Position indicators for gravity
	float x, y, z;
    int ix,iy,iz;
	
	// iterators
	int i = 0, k = 0, j = 0;
  
	// sun coordinate containers
	int sunypos = 0;
	int sunxpos = 0;

	// time marches on
	currtime = time(0);
  
	// if we're at time 0, ensure we dont hav an initial quantum leap going on
	if (prevtime == 0) prevtime = currtime;
	tim += currtime - prevtime;
  
	// if a second has passed, update the world
	if (currtime - prevtime/* && !testWorld*/)
	{
		// clear the animaded list
		animatedCount = 0;
		
		// Perlin-generate the clouds
		// TODO: change these literals to constants
		for(i=0;i<100;i++)
			for(k=0;k<100;k++)
				if(PerlinNoise3D(i/55.0,k/65.0, tim/50.0,1.23,1.97,4) > 0)
				{	
					// generate the cubes
					world[i][CLOUD_ALTITUDE][k] = 5;
					
					// add them to the animated list
					addAnimatedList(i,CLOUD_ALTITUDE,k);
				}else
					world[i][CLOUD_ALTITUDE][k] = 0;

		// position the sun
		// calculate next x and y coordinates
		sunxpos = (int)((((double)(tim % DAY_LENGTH))/DAY_LENGTH)*99.0);
		sunypos = (sqrt(pow(SUN_RADIUS,2)-pow((sunxpos - SUN_ANCHOR_X),2)))+(SUN_ANCHOR_Y);
		
		// move the sun
		world[sunxpos][sunypos][49] = 6;
		world[prevsunxpos][prevsunypos][49] = 0;
		setLightPosition(sunxpos, sunypos, 49);
		
		// add it to the animated list
		addAnimatedList(sunxpos,sunypos,49);
    
		// save the current position so this sun can be deleted on next draw
		prevsunypos = sunypos;
		prevsunxpos = sunxpos;	
	
		// force a screen redraw
		glutPostRedisplay();
	}

    // Gravity
    // if we are not in ghost mode
    if(!flycontrol)
	{		
		// get the player's position
		getViewPosition(&x,&y,&z);
		
		// normalize for the world cube array
		ix = (int) x * (-1);
		iy = (int) y * (-1);
		iz = (int) z * (-1);
		
		// if there is nothing underneath the player
		if (world[ix][iy-1][iz] == 0) 
		{
			// drop the player half a unit
			setViewPosition(x,y+0.5,z);
			
			// TODO: implement gravitational accelleration
		}
	}
	
	// save the current time for next round
	prevtime = currtime;
}

// Cube Culling and performance measuring
void buildDisplayList() {
	// used to calculate frames per second
	static int frame=0, time, timebase=0;

	// iterators
	int i, j, k;
	int x,y,z;

	// get the frustum
	ExtractFrustum();
	
	// clear the display list
	displayCount = 0;
	
	// go through the visible list and check which ones are in the frustum
	for(i = 0; i< visibleCount; i++)
	{
		// grab the coordinates of this cube
		x = visibleList[i][0];
		y = visibleList[i][1];
		z = visibleList[i][2];
		
		// check if it's in the frustum
		if (CubeInFrustum( ((float)x)+0.5, ((float)y)+0.5,((float)z)+0.5, 0.5))
		{
			// add it to the display list
			addDisplayList(x,y,z);
		}
	}
	
	// go through the animated list and check what's in the frustum
	for(i=0; i<animatedCount; i++)
	{
		// grab the coordinates of the cube
		x = animatedList[i][0];
		y = animatedList[i][1];
		z = animatedList[i][2];
		
		// check if it's in the frustum
		if (CubeInFrustum( ((float)x)+0.5, ((float)y)+0.5,((float)z)+0.5, 0.5))
		{
			// add it to the display list
			addDisplayList(x,y,z);
		}
	}
	
	// FPS calculation
	// taken from http://www.lighthouse3d.com/opengl/glut/index.php?fps
	// increment the frame counter
	frame++;
	
	// get the glut time
	time=glutGet(GLUT_ELAPSED_TIME);
	
	// if sufficient time has passed, calulate and display the FPS
	if (time - timebase > 1000) {
		printf("FPS:%4.2f\n", frame*1000.0/(time-timebase));
		timebase = time;		
		frame = 0;
	}
	
	// redraw the screen at the end of the update
   glutPostRedisplay();
}

// Funtion to check if two rooms overlap
int overlap(Room * r1, Room* r2)
{
	// check x-coords
	if (	
		((r1->x <= r2->x) && 			// if room 1 is to the left of room 2
		((r2->x + r2->l) <= r1->x)) 	// and it's rightmost side is left of room 2's rightmost
		||
		(((r1->x + r1->l) <= r2->x) && 	// if room 1 is to the right of room 2
		((r2->x + r2->l) <= (r1->x + r1->l)))	// and it's leftmost side is right of room 2's leftmost
	)
	{
		// check y-coords
		if (
			((r1-> y <= r2-> y) &&			// if room 1 is below room 2
			((r2-> y + r2->w) <= r1-> y)) 	// and it's upper side is above room 2's lower side
			||
			(((r1-> y + r1->w) <= r2-> y) &&// if room 1 is above room 2
			((r2-> y + r2->w) <= (r1-> y + r1->w)))	//and it's lower side is below room 2's upper side
		)
		{
			// if all this is true, they are overlaping
			return true;
		}
	}
	
	// one or more tests failed, there is no overlap.
	return false;
}

// Functions to print out dungeon components

// Funtion to print out a room
int printRoom(Room * r)
{
	printf("Room:\n  ID: %d\n  X: %d\n  Y: %d\n  L: %d\n  W: %d\n", r->id, r->x, r->y, r->l, r->w);
	return 0;
}
// Function to print out a list of rooms
int printRooms(Room * list[], int count)
{
	int i = 0;
	Room * currentRoom = NULL;
	
	// print the header and count
	printf("Rooms (%d):::\n", count);
	
	// iterate through the list and print all rooms out
	for(i=0; i<count; i++)
	{
		currentRoom = list[i];
		printRoom(currentRoom);
	}
}
// Function to print out a hall
int printHall(Hall * h)
{
    printf("Hall:\n  R1:  %d\n R2: %d\n Dist: %f\n", h->r1->id, h->r2->id, h->dLength);
    return 0;
}
// Function to print out a lst of halls
int printHalls(Hall * list[], int count)
{
	int i = 0;
	Hall * currentHall = NULL;
	
	// print the header and count
	printf("Halls (%d):::\n", count);
	
	// iterate through the list and print the halls
	for(i=0; i<count; i++)
	{
		currentHall = list[i];
		printHall(currentHall);
	}
	return 0;
}
// Function to print out a dungeon
int printDungeon(Dungeon * d)
{
	if (d)
	{
		// print out the header
		printf("Dungeon:-------------------\n");
		
		// print out the rooms
		printRooms(d->rooms, d->numRooms);
		printf("\n");
		
		// print out the halls
		printHalls(d->halls, d->numHalls);
		
		// print out the footer
		printf("\nEnd of Dungeon Block-------\n");
	}
	else {
		// print out error if a null dungoen was passed
		printf("Dungeon is NULL\n");
	}

	return 0;
}

// Functions to generate a random dungeon

// Function to generate roooms for the dungeon
int generateRooms(int rooms)
{
	Room * newRoom = NULL;
	int i = 0, j = 0, success = 0;
	
	// generate the starting room
	newRoom = malloc(sizeof(Room));
	newRoom->id = 0;
	newRoom->x = 48;
	newRoom->y = 48;
	newRoom->w = 3;
	newRoom->l = 3;
	
	// add it to the list
	addRoomList(newRoom);
	
	// randomly generate the remaining rooms
	for(i = roomsCount; i<MAX_ROOMS; i++)
	{
		// clear success flag
		success = false;
		
		// Allocate the new room
		newRoom = malloc(sizeof(Room));
		
		// loop until successful
		while (!success)
		{
			// generate the room size & coords
			newRoom->id = i;
			newRoom->w = (rand() % 10) + 3;
			newRoom->l = (rand() % 10) + 3;
			newRoom->x = (rand() % (97 - newRoom->l))+1;
			newRoom->y = (rand() % (97 - newRoom->w))+1;
			
			// check for overlap
			for(j=0;j<roomsCount;j++)
			{
				if (overlap(newRoom,roomsList[j]))
				{
					success = false;
					
					// exit the loop on failure
					j = roomsCount;
				} else {
					success = true;
				}

			}
			
			// if it did not overlap, add it to the list;
			if (success)
			{
				addRoomList(newRoom);
			}
			else
			{
				//fprintf(stderr, "... unsuccessful\n", i);
			}
		}
	}
	return 0;
}
// Function to merge two dungoens together
Dungeon * mergeDungeons(Dungeon * d1, Dungeon * d2)
{
	int i = 0, j = 0;
	int dupe = 0;
	Room * currentRoom = NULL;
	Hall * currentHall = NULL;
	
	// Allocate & initialize the new dungeon
	Dungeon * newD = malloc(sizeof(Dungeon));
	newD->numRooms = 0;
	newD->numHalls = 0;
	
	//add rooms from d1;
	for(i=0;i<d1->numRooms;i++)
	{
		// get next room;
		currentRoom = d1->rooms[i];
		
		// add it
		newD->rooms[newD->numRooms] = currentRoom;
		newD->numRooms++;
	}
	//add rooms from d2;
	for(i=0;i< d2->numRooms;i++)
	{
		// clear dupe flag
		dupe = 0;
		
		// get next room
		currentRoom = d2->rooms[i];
		
		// ensure we arent duplicating
		for(j=0; j< newD->numRooms; j++)
			if (currentRoom == newD->rooms[j])
			{
				dupe = 1;
				j = newD->numRooms;
			}
		
		// add it if it's not a duplicate
		if (!dupe)
		{
			newD->rooms[newD->numRooms] = d2->rooms[i];
			newD->numRooms++;
		}
	}
	
	//add halls from d1;
	for(i=0;i<d1->numHalls;i++)
	{
		// get next hall;
		currentHall = d1->halls[i];
		
		// add it
		newD->halls[newD->numHalls] = currentHall;
		newD->numHalls++;
	}
	//add halls from d2;
	for(i=0;i<d2->numHalls;i++)
	{
		// clear dupe flag
		dupe = 0;
		
		// get next hall
		currentHall = d2->halls[i];
		
		// ensure we arent duplicating
		for(j=0; j< newD->numHalls; j++)
			if (currentHall == newD->halls[j])
			{
				dupe = 1;
				j = newD->numHalls;
			}
		
		// add it if it's not a duplicate
		if (!dupe)
		{
			newD->halls[newD->numHalls] = d2->halls[i];
			newD->numHalls++;
		}
	}
	
	return newD;
}

// Function to delete a dungeon
// Not stable/ incomplete
Dungeon * deleteDungeon (Dungeon * d)
{
	int i;
	
	// disconnect rooms
	for(i=0; i < d->numRooms; i++)
		d->rooms[i] = NULL;
	
	//fprintf(stderr, "Rooms disconnected. %d halls\n", d->numHalls);

	// disconnect halls
	for(i=0; i < d->numHalls; i++)
		d->halls[i] = NULL;
	
	fprintf(stderr, "Halls disconnected\n");
	
	// derez it!
	if (d)free(d);
	
	return NULL;
}
// Function to generate halls for a dungeon
// not optimized or *good*, but stable
// TODO: fix how it finds the next shortest hall
// TODO: also, functionalize this. it's huge
int generateHalls()
{
	Hall* potentials[roomsCount][roomsCount];
	Hall * currentHall = NULL;
	Hall * newHall = NULL;
	Hall * shortest = NULL;
	int lastI = 0, lastJ = 0;
	Dungeon * forest[roomsCount], *newD = NULL, *finalDungeon = NULL;
	int dungeons = 0;
	int i = 0, j = 0, roomToConnect = 0, graphConnected = 0;
	int k = 0, l = 0;
	Room * r1= NULL, * r2 = NULL;
	
	// Populate the halls list;
	// Initialize it
	for(i=0;i<roomsCount;i++)
		for(j=0;j<roomsCount;j++)
			potentials[i][j] = NULL;
		
	// generate a hall for each possible room-room connection
	for(i = 0; i<roomsCount; i++)
	{
		// start with the next room, so we only have one hallway per room pair
		for(j = i+1; j <roomsCount; j++)
		{
			// if we're not trying to connect a room to itself
			if (i != j)
			{
				// allocate the room
				currentHall = malloc(sizeof(Hall));
				
				// insert the room pointers
				r1 = roomsList[i];
				r2 = roomsList[j];
				currentHall->r1 = r1;
				currentHall->r2 = r2;
				
				// calculate displacement between rooms
				currentHall->xLength = r2->x - r1->x;
				currentHall->yLength = r2->y - r1->y;
				currentHall->dLength = sqrt(pow((double)currentHall->xLength,2.0) + pow((double)currentHall->yLength,2.0));

				// add the new hall to the list
				potentials[i][j] = currentHall;
			}
			else {
				// put nulls in invalid hall places
				potentials[i][j] = NULL;
			}

		}
	}
	
	// Generate the hallways between rooms via Prim's algorithm
	// generate the dungeons
	for(i=0; i< MAX_ROOMS; i++)
	{
		// allocate dungeons
		forest[i] = malloc(sizeof(Dungeon));
		
		// add the rooms
		forest[i]->rooms[0] = roomsList[i];
		forest[i]->numRooms = 1;
		
		// add the halls
		forest[i]->numHalls = 0;
	}
	
	// loop until the first dungeon is connected to all rooms
	while(forest[0]->numRooms < roomsCount)
	{
		// initialize the shortest pointer
		shortest = NULL;

		// find the shortest hall, picking up from where we last left off
		for(i=lastI; i < roomsCount; i++)
		{
			for(j=lastJ+1; j <roomsCount; j++)
			{
				// if such a hall exists
				if(i != j && potentials[i][j])
				{
					// if a shortest hall has not yet been found, grab the next hall as the shortest
					if (!shortest) shortest = potentials[i][j];
				
					// grab the next hall
					currentHall = potentials[i][j];

					// if this one is shorter, save it as the new shortest
					if (currentHall->dLength < shortest->dLength && forest[i] != forest[j])
						shortest = currentHall;
				}
			}
			
			// reset the leaveoff flag
			lastJ = 0;
		}
		
		// if no shortest was found (?) just stop.
		if (!shortest) break;
	
		// extract the indicies of the rooms
		i = shortest->r1->id;
		j = shortest->r2->id;
		
		// remember where we left off
		lastI = i;
		lastJ = j;
	
		// add it to it's connected dungeons
		forest[i]->halls[forest[i]->numHalls] = shortest;
		forest[i]->numHalls++;
		forest[j]->halls[forest[j]->numHalls] = shortest;
		forest[j]->numHalls++;
		
		// add the connected rooms to the dungeons
		newD = mergeDungeons(forest[i], forest[j]);
	
		// clean up the old dungeons
		// TODO: this is broken
		/*if (forest[i] != forest[j])
		{
			if(forest[i])
				forest[i] = deleteDungeon(forest[i]);
			if(forest[j])
				forest[j] = deleteDungeon(forest[j]);
			//fprintf(stderr, "deleted old dungeons\n");
		} else
		{
			if(forest[i])
				forest[i] = deleteDungeon(forest[i]);
				forest[j] = forest[i];
		}*/
		
		// store the new dungeon in the old one's places
		forest[i] = newD;
		forest[j] = newD;
		
	}
	
	// grab the newly made MST
	finalDungeon = forest[0];

	// transcribe the dungeon to the global lists
	for (i=0; i<finalDungeon->numHalls; i++)
	{
		// allocate a hall object for transcription
		newHall = malloc(sizeof(Hall));
		
		// get the next hall to transcribe
		currentHall = finalDungeon->halls[i];
		
		// copy the data and pointers
		newHall-> r1 = currentHall -> r1;
		newHall-> r2 = currentHall -> r2;
		newHall-> xLength = currentHall ->xLength;
		newHall-> yLength = currentHall ->yLength;
		newHall-> dLength = currentHall ->dLength;
		
		// add the hall to the hall list
		addHallList(newHall);
	}

	// clean up
	// dungeons
	deleteDungeon(finalDungeon);
	// halls
	for(i=0; i<roomsCount; i++)
		for(j=0; j<roomsCount-1; j++)
			if (potentials[i][j]) free(potentials[i][j]);
	
	return 0;
}
// Function to dig hallways.
// TODO: make this not suck 
int drawAt(int x, int y, int z, int val)
{
	world[x][y][z] = val;
	world[x][y+1][z] = val;
	fprintf(stderr, "drew %d at (%d,%d,%d)\n",val,x,y,z);
}
// Function to dig out all the hallways
int subtractHalls()
{
	Hall * hallCurrent = NULL;
	Room * r1 = NULL, *r2 = NULL;
	int i = 0;
	int x, y, z;
	int xLen = 0, yLen = 0;
	int sourceX, sourceY;
	int destX, destY;
	int dist;
	int dir = 0; //North = 0, East = 1, West = 2, South = 3
	int leftTurn = 0;
	
	// go through the list of halls
	for(i=0; i<hallsCount; i++)
	{
		// initialize the left turn signal
		leftTurn = 0;
		
		// grab the next hall
		hallCurrent = hallsList[i];
		
		// grab the rooms
		r1 = hallCurrent->r1;
		r2 = hallCurrent->r2;

		// grab the displacement components
		xLen = hallCurrent->xLength;
		yLen = hallCurrent->yLength;
		
		// grab the source and destination coords
		sourceX = r1->x;
		sourceY = r1->y;
		destX = r2->x;
		destY = r2->y;
		
		// dig out the halls
		// X axis
		if (xLen > 0)
		    for(x = sourceX; x < destX; x++)
			drawAt(x,14,sourceY,0);
		else
		    for(x = sourceX; x > destX; x--)
			drawAt(x,14,sourceY,0);
		 
		// Y axis
		if (yLen > 0)
		    for(y = sourceY; y < destY; y++)
			drawAt(destX,14,y,0);
		else
		    for(y = sourceY; y > destY; y--)
			drawAt(destX,14,y,0);
		
	}
		
}
// Function to dig out a dungeon
int subtractDungeon()
{
	Room * currentRoom = NULL;
	int i = 0, x = 0, y = 0, z = 0;
	
	// dig out the rooms
	for (i = 0; i < roomsCount; i++)
	{
		currentRoom = roomsList[i];
		for(x = 0; x < currentRoom->l; x++)
			for(y = 0; y < currentRoom->w; y++)
				for(z = MEAN_GROUND_ALTITUDE - 10 ; z < MEAN_GROUND_ALTITUDE - 7; z++)
					world[x+currentRoom->x][z][y+currentRoom->y] = 0;
	}

	// dig out the halls
	subtractHalls();
	
	// clean up the rooms
	for(i=0; i< roomsCount; i++)
	{
		if (currentRoom)
			free(currentRoom);
		currentRoom = NULL;
		roomsList[i] = NULL;
	}
	
	// dig out the entrance hole & place the marker obelisk
	for(i = 15; i< 30;i++)
	{
	    world[49][i][49] = 0;
	    if (i > 20)
		world[48][i][49] = 3;
	}
}

// Function to generate a dungeon
int generateDungeon(int rooms)
{
	// generate rooms
	generateRooms(rooms);
	
	// generate halls
	generateHalls();
	
	// subtract from world
	subtractDungeon();
	
	return 0;
}

// Main program loop
int main(int argc, char** argv)
{
int i, j, k;
  double dbl;
	/* Initialize the graphics system */
   gradphicsInit(&argc, argv);
   
	// seed random
	srand(time(NULL));
    
	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. It will be used to test the timing */
	/* of your culling solution. Put your code in the else statment below */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<100; i++)
         for(j=0; j<50; j++)
            for(k=0; k<100; k++)
               world[i][j][k] = 0;
	
	/* some sample objects */
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 1;
      world[52][26][52] = 2;

	/* red platform */
      for(i=0; i<100; i++) {
         for(j=0; j<100; j++) {
            world[i][24][j] = 3;
         }
      }
	/* fill in world under platform */
      for(i=0; i<100; i++) {
         for(j=0; j<100; j++) {
            for(k=0; k<24; k++) {
               world[i][k][j] = 3;
            }
         }
      }
	/* blue box shows xy bounds of the world */
      for(i=0; i<99; i++) {
         world[0][25][i] = 2;
         world[i][25][0] = 2;
         world[99][25][i] = 2;
         world[i][25][99] = 2;
      }
   } else {

    // generate perlin noise terrain
    for(i=0;i<100;i++)
    for(k=0;k<100;k++){
		dbl = PerlinNoise2D(i/75.0,k/75.0,0.98,2.11,3)*5+MEAN_GROUND_ALTITUDE;
		for(j=0;j<(int)dbl;j++)
			world[i][j][k] = 1;
    }

   }

	// generate the dungeon
	generateDungeon(MAX_ROOMS);
	
	// generate the visible list
	visibleCount = 0;
	
	// add the bottom
	j = 0;
	for (i=0; i< 100; i++) {
		for (k=0;k<100; k++) {
			if(world[i][j][k])
			{
				addVisibleList(i,j,k);
			}
		}
	}
	
	// add the top
	j = 49;
	for (i=0; i< 100; i++) {
		for (k=0;k<100; k++) {
			if(world[i][j][k])
			{
				addVisibleList(i,j,k);
			}
		}
	}
	
	
	// add the left side
	k = 0;
	for (i=0; i< 100; i++) {
		for (j=0;j<49; j++) {
			if(world[i][j][k])
			{
				addVisibleList(i,j,k);
			}
		}
	}
	
	
	// add the right side
	k = 99;
	for (i=0; i< 100; i++) {
		for (j=0;j<49; j++) {
			if(world[i][j][k])
			{
				addVisibleList(i,j,k);
			}
		}
	}
	
	
	// add the front
	i = 0;
	for (j=0; j< 49; j++) {
		for (k=0;k<100; k++) {
			if(world[i][j][k])
			{
			addVisibleList(i,j,k);
			}
		}
	}
	
	
	// add the back
	i = 99;
	for (j=0; j< 49; j++) {
		for (k=0;k<100; k++) {
			if(world[i][j][k])
			{
				addVisibleList(i,j,k);
			}
		}
	}
	
	// figure out which cubes are visible
	// look at each position
	for(i = 1; i < 99; i++)
		for(j = 1; j< 49; j++)
			for(k = 1; k < 99; k++)
			{
				// if there's something here
				if (world[i][j][k])
				{
					// and one of it's 6 faces are exposed
					if(!(world[i+1][j][k]) || !(world[i-1][j][k]) ||
					   !(world[i][j+1][k]) || !(world[i][j-1][k]) ||
					   !(world[i][j][k+1]) || !(world[i][j][k-1])
					   )
					{
						// add it to the visible list
						addVisibleList(i,j,k);
					}
				}
			}
	
	//exit(0);
	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

