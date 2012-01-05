LIBS = -rdynamic -ldl 
WARNS = -Wall -ansi

all: server libdate

server: a1c.c
	gcc a1c.c -o server $(LIBS) $(WARNS)

libdate: libdate.c 
	gcc -fPIC -c libdate.c
	gcc -shared -Wl,-soname,libdate.so -o ./lib/libdate.so libdate.o 

clean:
	rm *.o *.so main

