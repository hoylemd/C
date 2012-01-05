
Building and Running the Graphics System
----------------------------------------
The program consists of two .c files.  The a1.c file contains the main()
routine and the update() function. All of the changes necessary for the
assignment can be made to this file.  The graphics.c file contains all
of the code to create the 3D graphics for the assignment. You should not
need to change this code for assignment 1.

There is a makefile which will compile the code on the Macs.
The executable is named a1. If the program is run with the -full
command line option then it will run in fullscreen.

When the program runs the view is controlled through the mouse and
keyboard. The mouse moves the viewpoint left-right and up-down.
The keyboard controls do the following:
	w  move forward
	a  strafe left
	s  move forward
	d  strafe right
	q  quit

The 1,2,3 buttons also change the rendering options.
	
Note: If the controls appear to be reversed then the viewpoint is upside down.
Pull or push the mouse until you turn over.

There are a few sample boxes drawn in the middle of the world and a
set of boxes which show the outer width and depth of the world.
These are defined in a1.c and should be removed before your
assignment is submitted.


Programming Interface to the Graphics System
--------------------------------------------

1. Drawing the world
--------------------

The only shape drawn by the graphics system is the cube. The data
structure which holds all of the objects is the three dimensional array:

	GLubyte world[100][50][100]

The GLubyte is an unsigned byte defined by OpenGL. It is the same as
any other unsigned byte.

The indices of the array correspond to the dimensions of the world.
In order from left to right they are x,y,z.  This means the world is 100 units
in the x dimension (left to right), 50 units in the y dimension (up and down),
and 100 units in z (back to front).

The cube at location world[0][0][0] is in the lower corner of the 3D world.
The cube at location world[99][49][99] is diagonally across from
world[0][0][0] in the upper corner of the world.

Each cube drawn in the world is one unit length in each dimension.

Values are stored in the array to indicate if that position in the
world is occupied or are empty. The following would mean that
position 25,25,25 is empty:
	world[25][25][25] = 0

If the following were used:
	world[25][25][25] = 1
then position 25,25,25 would contain a green cube. 

Cubes can be drawn in different colours depending on that value stored
in the world array. The current colours which can be drawn are:
	0 empty
	1 green
	2 blue
	3 red
	4 black
	5 white

2. Setting the Light Position
-----------------------------
There is a single light in the world.  The position of the light
is controlled through two functions:

	void setLightPosition(GLfloat x, GLfloat y, GLfloat z);
	GLfloat* getLightPosition();

The setLightPosition() function moves the light source to location x,y,z in the
world. The getLightPosition() function returns a pointer to an array
of floats containing current the x,y,z location of the light source.

To see the effect of a change through setLightPosition() you will
need to call glutPostRedisplay() to update the screen. 
 

3. Timing Events
----------------
OpenGL is event driven. The events which this program will respond to 
include keyboard and mouse input. The glutMainLoop() function receives
these inputs and processes them. 

The glutMainLoop() function will loop until the program ends. This means
that all of your code to initialize the world must be run before this
function is called. It also means that changes to the world must occur
inside function called by OpenGL. The only function which you have
access to to make these updates is named update() in a1.c.

When it is not otherwise drawing the scene the system will call the
update() function. This is where you can make changes to the world
array and lighting while program is running.

If you make changes to the world or the light in the udpate()
function then you should call glutPostRedisplay() to refresh the screen.

The update() function is not called on a predictable schedule. You will
need to check the time during updates to control the rate at which
the world changes. 



Changing graphics.c
-------------------
You can make changes to graphics.c if you wish but you are responsible
for making them work. If you break the graphics system then you have
to fix it yourself.


