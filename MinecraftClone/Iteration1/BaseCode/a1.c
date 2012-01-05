
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"

extern void gradphicsInit(int *, char **);
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();



	/* background process, it is called when there are no other events */
void update() {

	/* your code goes here */

	/* uncomment the next line if you want the screen to be redrawn */
	/* at the end of the update */
//   glutPostRedisplay();
}



int main(int argc, char** argv)
{
int i;
	/* Initialize the graphics system */
   gradphicsInit(&argc, argv);

	/* your code goes here, before the mainloop */

	/* some sample objects */
	/* create two green boxes and one blue box */
   world[50][25][50] = 1;
   world[52][25][52] = 1;
   world[52][26][52] = 2;

	/* blue box shows xy bounds of the world */
   for(i=0; i<99; i++) {
      world[0][25][i] = 2;
      world[i][25][0] = 2;
      world[99][25][i] = 2;
      world[i][25][99] = 2;
   }

	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

