
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */
// Global includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Local includes
#include "graphics.h"
#include "perlin.h"

// Links to graphics functions
extern void gradphicsInit(int *, char **);
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

// global variables
// TODO: make this C++ and OO it up!
int tim = 0;
int prevtime = 0;
int currtime = 0;
int lightpos = 0;
int prevsunypos = 0;
int prevsunxpos = 0;

// variables to control behavior of world generation
const int SUN_ANCHOR_X = 49;
const int SUN_ANCHOR_Y = 0;
const int SUN_RADIUS = 49;
const int DAY_LENGTH = 30;
const int CLOUD_ALTITUDE = 48;
const int MEAN_GROUND_ALTITUDE=24;

// background process, it is called when there are no other events */
void update() {
  // iterators
  int i = 0, k = 0;
  
  // coordinate containers
  int sunypos = 0;
  int sunxpos = 0;

  // time marches on
  currtime = time(0);
  
  // if we're at time 0, ensure we dont hav an initial quantum leap going on
  if (prevtime == 0) prevtime = currtime;
  tim += currtime - prevtime;
  
  // Draw time (only if a second has passed since the last one)
  if (currtime - prevtime)
  {
    // Perlin-generate the clouds
    for(i=0;i<100;i++)
    for(k=0;k<100;k++)
      if(PerlinNoise3D(i/55.0,k/65.0, tim/50.0,1.23,1.97,4) > 0)	// I should really make constants instead of literals here
	world[i][CLOUD_ALTITUDE][k] = 5;
      else
	world[i][CLOUD_ALTITUDE][k] = 0;

    // position the sun
    // calculate next x and y coordinates for it
    sunxpos = (int)((((double)(tim % DAY_LENGTH))/DAY_LENGTH)*99.0);
    sunypos = (sqrt(pow(SUN_RADIUS,2)-pow((sunxpos - SUN_ANCHOR_X),2)))+(SUN_ANCHOR_Y);//sqrt((double)((36 ^ 2) - ((sunxpos-49)^2)))+ 15;
    
    // move it!
    world[sunxpos][sunypos][49] = 6;
    world[prevsunxpos][prevsunypos][49] = 0;
    setLightPosition(sunxpos, sunypos, 49);
    
    // save the current position so this sun can be deleted on next draw
    prevsunypos = sunypos;
    prevsunxpos = sunxpos;

    // force a screen redraw
    glutPostRedisplay();
  }
  
  // save the current time for next round
  prevtime = currtime;
}

// Main loop
int main(int argc, char** argv)
{
  // local iterator
  int i,j,k;
  double dbl;
  /* Initialize the graphics system */
  gradphicsInit(&argc, argv);

  // generate perlin noise terrain
    for(i=0;i<100;i++)
    for(k=0;k<100;k++){
      dbl = PerlinNoise2D(i/75.0,k/75.0,0.98,2.11,3)*5+MEAN_GROUND_ALTITUDE;
      world[i][(int)dbl][k] = 1;
    }

    
  /* starts the graphics processing loop */
  /* code after this will not run until the program exits */
  glutMainLoop();
  return 0;
}

