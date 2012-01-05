
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"

extern void update();

	/* flags used to control the appearance of the image */
int lineDrawing = 0;	// draw polygons as solid or lines
int lighting = 1;	// use diffuse and specular lighting
int smoothShading = 1;  // smooth or flat shading
int textures = 0;

	/* texture data */
GLubyte  Image[64][64][4];
GLuint   textureID[1];

	/* viewpoint coordinates */
float vpx = 0.0, vpy = -50.0, vpz = 0.0;
	/* mouse direction coordiates */
float mvx = 0.0, mvy = 45.0, mvz = 0.0;

	/* location for the light source (the sun), the first three
	   values are the x,y,z coordinates */
GLfloat lightPosition[] = {0.0, 100.0, 0.0, 0.0};

	/* allows user to set position of the light */
void setLightPosition(GLfloat x, GLfloat y, GLfloat z) {
   lightPosition[0] = x;
   lightPosition[1] = y;
   lightPosition[2] = z;
   glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
}

	/* returns current position of the light */
GLfloat* getLightPosition() {
   return(lightPosition);
}



/*  Initialize material property and light source.  */
void init (void)
{
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};


	/* if lighting is turned on then use ambient, diffuse and specular
	   lights, otherwise use ambient lighting only */
   if (lighting == 1) {
      glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
      glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
      glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   } else {
      glLightfv (GL_LIGHT0, GL_AMBIENT, light_full_on);
      glLightfv (GL_LIGHT0, GL_DIFFUSE, light_full_off);
      glLightfv (GL_LIGHT0, GL_SPECULAR, light_full_off);
   }
   glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
   
   glEnable (GL_LIGHTING);
   glEnable (GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

	/* called each time the world is redrawn */
void display (void)
{
GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat skyblue[]  = {0.52, 0.74, 0.84, 1.0};
GLfloat black[] = {0.0, 0.0, 0.0, 1.0};
int i, j, k;

   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* position viewpoint based on mouse rotation and keyboard 
	   translation */
   glLoadIdentity ();
   glRotatef(mvx, 1.0, 0.0, 0.0);
   glRotatef(mvy, 0.0, 1.0, 0.0);
   glTranslatef(vpx, vpy, vpz);

	/* draw surfaces as either smooth or flat shaded */
   if (smoothShading == 1)
      glShadeModel(GL_SMOOTH);
   else
      glShadeModel(GL_FLAT);

	/* draw polygons as either solid or outlines */
   if (lineDrawing == 1)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else 
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* give all objects the same shininess value and specular colour */
   glMaterialf(GL_FRONT, GL_SHININESS, 30.0);

	/* set starting location of objects */
   glPushMatrix ();

	/* make a blue sky cube */
   glShadeModel(GL_SMOOTH);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, skyblue);
   glPushMatrix ();
	/* move the cube center to middle of world space */
   glTranslatef(50, 25, 50);
   glutSolidCube(150.0);
   glPopMatrix ();
   glShadeModel(GL_SMOOTH);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

	/* draw objects in world array */
   for(i=0; i<100; i++) {
      for(j=0; j<50; j++) {
         for(k=0; k<100; k++) {
            if (world[i][j][k] != 0) {
			/* select colour based on value in the world array */
               if (world[i][j][k] == 1) 
                  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
               else if (world[i][j][k] == 2) 
                  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
               else if (world[i][j][k] == 3) 
                  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
               else if (world[i][j][k] == 4) 
                  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
               else 
                  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);

               glPushMatrix ();
               glTranslatef(i, j, k);
               glutSolidCube(1.0);
               glPopMatrix ();
            }
         }
      }
   }

   glPopMatrix();
   glutSwapBuffers();
}

	/* sets viewport information */
void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 300.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
}

	/* respond to keyboard events */
void keyboard(unsigned char key, int x, int y)
{
float rotx, roty;

   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
      case '1':		// draw polygons as outlines
         lineDrawing = 1;
         lighting = 0;
         smoothShading = 0;
         textures = 0;
         init();
         glutPostRedisplay();
         break;
      case '2':		// draw polygons as filled
         lineDrawing = 0;
         lighting = 0;
         smoothShading = 0;
         textures = 0;
         init();
         glutPostRedisplay();
         break;
      case '3':		// diffuse and specular lighting, flat shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 0;
         textures = 0;
         init();
         glutPostRedisplay();
         break;
      case '4':		// diffuse and specular lighting, smooth shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         textures = 0;
         init();
         glutPostRedisplay();
         break;
      case '5':		// texture with  smooth shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         textures = 1;
         init();
         glutPostRedisplay();
         break;
      case 'w':		// forward motion
         rotx = (mvx / 180.0 * 3.141592);
         roty = (mvy / 180.0 * 3.141592);
         vpx -= sin(roty);
         vpy += sin(rotx);
         vpz += cos(roty);
         glutPostRedisplay();
         break;
      case 's':		// backward motion
         rotx = (mvx / 180.0 * 3.141592);
         roty = (mvy / 180.0 * 3.141592);
         vpx += sin(roty);
         vpy -= sin(rotx);
         vpz -= cos(roty);
         glutPostRedisplay();
         break;
      case 'a':		// strafe left motion
         roty = (mvy / 180.0 * 3.141592);
         vpx += cos(roty) * 1.0;
         vpz += sin(roty) * 1.0;
         glutPostRedisplay();
         break;
      case 'd':		// strafe right motion
         roty = (mvy / 180.0 * 3.141592);
         vpx -= cos(roty) * 1.0;
         vpz -= sin(roty) * 1.0;
         glutPostRedisplay();
         break;
   }
}

	/* load a texture from a file */
	/* not currently used */
void loadTexture() {
FILE *fp;
int  i, j;
int  red, green, blue;

   if ((fp = fopen("image.txt", "r")) == 0) {
      printf("Error, failed to find the file named image.txt.\n");
      exit(0);
   } 

   for(i=0; i<64; i++) {
      for(j=0; j<64; j++) {
         fscanf(fp, "%d %d %d", &red, &green, &blue);
         Image[i][j][0] = red;
         Image[i][j][1] = green;
         Image[i][j][2] = blue;
         Image[i][j][3] = 255;
      }
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glGenTextures(1,textureID);
   glBindTexture(GL_TEXTURE_2D, textureID[0]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, Image);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T);

   fclose(fp);
}

void motion(int x, int y) {
static float oldx, oldy;
   mvx += (float) y - oldy;
   mvy += (float) x - oldx;
   oldx = x;
   oldy = y;
   glutPostRedisplay();

}

void mouse(int button, int state, int x, int y) {
/* capture mouse button events - not currently used */
/*
   if (button == GLUT_LEFT_BUTTON)
      printf("left button - ");
   else if (button == GLUT_MIDDLE_BUTTON)
      printf("middle button - ");
   else
      printf("right button - ");

   if (state == GLUT_UP)
      printf("up - ");
   else
      printf("down - ");

   printf("%d %d\n", x, y);
*/
}


void gradphicsInit(int *argc, char **argv) {
	/* set GL window information */
   glutInit(argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

   if ((*argc > 1) && (strcmp(argv[1],"-full") == 0) ) {
      glutGameModeString("1024x768:32@75");
      glutEnterGameMode();
   } else {
      glutInitWindowSize (1024, 768);
      glutCreateWindow (argv[0]);
   }

   init();

/* not used at the moment */
//   loadTexture();

	/* attach functions to GL events */
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutPassiveMotionFunc(motion);
   glutMouseFunc(mouse);
   glutIdleFunc(update);
}


