all: Server Calgar Empra

run: all
	./Calgar

runEmpra: Empra
	./Empra

runServer: Server
	./Server

Server: assignment3-server.c assignment3-server.h
	gcc assignment3-server.c -o Server -Wall
    
Calgar: calgar.c
	gcc calgar.c -o Calgar -Wall
    
Empra: empra.c
	gcc empra.c -o Empra -Wall

clean:
	rm -rf Server
	rm -rf Calgar
	rm -rf Empra
	rm -rf *~

