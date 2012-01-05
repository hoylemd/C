// dungeons header

// includes
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#ifndef GRAPHICS_H
	#define GRAPHICS_H
	#include "graphics.h"
#endif
#ifndef CUBELIST_H
	#define CUBELIST_H
	#include "cubeList.h"
#endif
#ifndef VECTOR3_H
	#define VECTOR3_H
	#include "Vector3.h"
#endif
#include "adjacencyMatrix.h"

// Generation constants
#define MAX_ROOMS 4
#define MAX_HALLS 6
#define MAX_WIDTH 15
#define MIN_WIDTH 3
#define MAX_LENGTH 15
#define MIN_LENGTH 3
#define DUNGEON_FLOOR 16
#define DUNGEON_HEIGHT 2

// Definitions because this aint C++ yet!
#ifndef false
	#define false 0
#endif
#ifndef true
	#define true 1
#endif

// Room structure
typedef struct room
{
	int id;
	int x;
	int y;
	int l;
	int w;
} Room;

// Hall structure
typedef struct hall
{
	Room * r1;
	Room * r2;
	int xLength;
	int yLength;
	double dLength;
} Hall;

// Dungeon structure
typedef struct dungeon
{
	int id;
	int numRooms;
	Room * rooms[MAX_ROOMS];
	int numHalls;
	Hall * halls[MAX_HALLS];
	cubeList * map;
} Dungeon;

// Lists for Dungeon generation
Room * roomsList[MAX_ROOMS];
int roomsCount;
Hall * hallsList[MAX_HALLS];
int hallsCount;

// Functions to affect Dungeon Component Lists
// Add a room to the room list
int addRoomList(Room *);
// Add a Hall to the hall List
int addHallList(Hall *);

// Functions to generate and destroy dungeons
Dungeon * generateDungeon();
Dungeon * deleteDungeon();

// Function to create an adjacency Matrix out of a dungeon
AdjacencyMatrix * generateDungeonMatrix(Dungeon *);

// function to determine if 2 cubes are adjacent to one another
int cubeAdjacent(int x1, int y1, int z1, int x2, int y2, int z2);
