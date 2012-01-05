LIBS = -rdynamic -ldl -g
WARNS = -Wall -ansi

all: server libdate libserver liblastupdate libfilesize libowner libfont hstring.o A3.class A3.h A3 db

server: server.c hstring.o
	gcc -c server.c -o server.o $(WARNS)
	gcc -o server server.o hstring.o $(LIBS) -lmysqlclient -lmysys -lmystrings  -L/usr/lib/mysql

db: db.c hstring.o
	gcc -c db.c -o db.o $(WARNS)
	gcc -o db db.o hstring.o -lmysqlclient -lmysys -lmystrings  -L/usr/lib/mysql

libdate: libdate.c 
	gcc -fPIC -c libdate.c
	gcc -shared -Wl,-soname,libdate.so -o ./lib/libdate.so libdate.o 

liblastupdate: liblastupdate.c 
	gcc -fPIC -c liblastupdate.c
	gcc -shared -Wl,-soname,liblastupdate.so -o ./lib/liblastupdate.so liblastupdate.o 

libserver: libserver.c 
	gcc -fPIC -c libserver.c
	gcc -shared -Wl,-soname,libserver.so -o ./lib/libserver.so libserver.o 

libservername: libservername.c 
	gcc -fPIC -c libservername.c
	gcc -shared -Wl,-soname,libservername.so -o ./lib/libservername.so libservername.o 

libfilesize: libfilesize.c 
	gcc -fPIC -c libfilesize.c
	gcc -shared -Wl,-soname,libfilesize.so -o ./lib/libfilesize.so libfilesize.o 

libowner: libowner.c 
	gcc -fPIC -c libowner.c
	gcc -shared -Wl,-soname,libowner.so -o ./lib/libowner.so libowner.o 

libfont: libfont.c 
	gcc -fPIC -c libfont.c
	gcc -shared -Wl,-soname,libfont.so -o ./lib/libfont.so libfont.o

hstring.o: hstring.c hstring.h
	gcc -c hstring.c -o hstring.o $(WARNS)

clean:
	rm -f *.o *.so server access.log reader *~ Spipe Lpipe *.class A3*.h ./jlibs/*.so db

go: clean all run

rfb: rfb.c hstring.o
	gcc -c rfb.c -o rfb.o $(WARNS)
	gcc -o rfb rfb.o hstring.o $(LIBS)

A3.class: A3.java
	javac A3.java

A3.h: A3.class
	javah A3

A3: A3.o hstring.o
	gcc -shared -Wl,-soname,libSGVui.so \
	-I. \
	-I/usr/lib/jvm/java-6-sun-1.6.0.07/include/linux \
	-I/usr/lib/jvm/java-6-sun-1.6.0.07/include \
	-o ./jlibs/libA3.so \
	A3.o 

A3.o: A3.c
	gcc A3.c -o A3.o -c -fPIC \
	-I/usr/lib/jvm/java-6-sun-1.6.0.07/include/linux \
	-I/usr/lib/jvm/java-6-sun-1.6.0.07/include 