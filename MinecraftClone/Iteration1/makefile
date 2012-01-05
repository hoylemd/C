all : a1

INCLUDES = -F/System/Library/Frameworks -lglut -lGLU

a1: a1.c graphics.c perlin.c
	gcc a1.c graphics.c perlin.c -o a1 $(INCLUDES) 
	
clean:
	rm a1

