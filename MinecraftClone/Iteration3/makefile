all : a3

SOURCES = a3.c graphics.c perlin.c dungeon.c cubeList.c adjacencyMatrix.c culling.c mob.c Vector3.c

INCLUDES = -F/System/Library/Frameworks -framework OpenGL -framework GLUT -lm

a3: $(SOURCES)
	gcc $(SOURCES) -o a3 $(INCLUDES) 
	
clean:
	rm -f a3 *~

