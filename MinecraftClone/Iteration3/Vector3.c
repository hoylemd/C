//vector3 source file

#include "Vector3.h"

// function too create a new vector3 object
Vector3 * newVector3(float x, float y, float z)
{
	// allocate
	Vector3 * v = malloc(sizeof(Vector3));
	
	// save
	v->x = x;
	v->y = y;
	v->z = z;
	
	// return the new Vector
	return v;
}

// function to destroy a vector
Vector3 * destroyVector3(Vector3 * v)
{
	if (v)
		free(v);
	return NULL;
}

// function to calculate the distance between 2 vectors
float VectorDistance( Vector3 * first, Vector3 * second)
{
	float deltaX, deltaY, deltaZ;
	float value;
	
	// calculate deltas
	deltaX = second->x - first->x;
	deltaY = second->y - first->y;
	deltaZ = second->z - first->z;
	
	// sum them
	value = (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ);
	
	// return the square root
	return  sqrt(value);
}

// function to check if 2 vectors are equal
int VectorIsEqual(Vector3 *first, Vector3 *second)
{
	// if they both exist
	if (first && second)
	{
		// and all components are equal
		if (first->x == second->x &&
			first->y == second->y &&
			first->z == second->z)
			
			// they be equal
			return true;
	}
	
	// otherwise they're not 
	return false;
}

// function to calculate the 2D angle between the 2 vectors in radians
float VectorAngle(Vector3 *first, Vector3 *second)
{
	float dx, dy;
	float mx, my;
	
	// if both exist
	if (first && second)
	{
		// calculate the deltas
		dx =  first->x - second->x;
		dy = first->z - second->z;
		
		// handle a delta x of 0
		if (dx == 0)
		{
			if (dy >= 0)
				return 0;
			else {
				return -0.5 * 3.14159;
			}
		}
		
		// handle a delta y of 0
		if (dy == 0)
		{
			if (dx >= 0)
				return (3.14159 / 2);
			else {
				return 0;;
			}

		}
		
		// calculate the magnitudes of the deltas
		if (dx > 0)
			mx = dx;
		else
			mx = - dx;
		if (dy > 0)
			my = dy;
		else
			my = -dy;
		
		// positive x
		if (dx > 0)
		{
			if (dy > 0)
			{
				// quadrant 0
				return atan(mx/my);
			}
			else
			{
				// quadrant 1
				return - (atan(mx/my));
			}

		}
		else
		{
			if (dy > 0)
			{
				// quadrant 3
				return (3.14159 / 2) + atan(mx/my);
			}
			else
			{
				// quadrant 2
				return - ((3.14159 / 2) + atan(mx/my));
			}
		}

		return atan(dx / dy);
	
	}
	
}