all : cm tm

full: clean y.tab.c lex.yy.c globals.h main.c TreeStuff.c
	gcc -c y.tab.c -o y.tab.o -pedantic
	gcc -c lex.yy.c -o lex.yy.o -pedantic
	gcc -c main.c -o main.o -pedantic
	gcc -c TreeStuff.c -o TreeStuff.o -pedantic
	gcc y.tab.o lex.yy.o main.o TreeStuff.o -o cm -pedantic

cm : y.tab.c lex.yy.c globals.h main.c TreeStuff.c
	gcc main.c lex.yy.c y.tab.c TreeStuff.c -o cm -pedantic -g

tm :
	cd vm && $(MAKE)
	cp vm/tm .

lex.yy.c : cminus.l
	flex cminus.l
	
y.tab.c : cminus.y
	yacc -d --verbose --debug cminus.y
	
clean : 
	rm -f lex.yy.c y.tab.c y.tab.h *~ *.o cm y.output out*.txt *.tm tm valgrind-results.txt
	cd vm && $(MAKE) clean
	cd cminusCode && rm -f *~
	
test : clean cm
	./cm cminusCode/1.cm output.tm > result.txt

memtest : clean cm
	valgrind --log-file=valgrind-results.txt ./cm cminusCode/1.cm output.txt 