all : a2

INCLUDES = -F/System/Library/Frameworks -lglut -lGLU

a2: a2.c graphics.c graphics.h perlin.c
	gcc a2.c graphics.c perlin.c -o a2 -g $(INCLUDES)
	
clean:
	rm -f a2 *~

