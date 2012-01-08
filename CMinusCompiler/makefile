CC = g++

all : cm tm

full: clean y.tab.c lex.yy.c globals.h main.cpp TreeStuff.cpp TreeNode.cpp
	${CC} -c y.tab.c -o y.tab.o -pedantic
	${CC} -c lex.yy.c -o lex.yy.o -pedantic
	${CC} -c main.cpp -o main.o -pedantic
	${CC} -c TreeStuff.cpp -o TreeStuff.o -pedantic
	${CC} -c TreeNode.cpp -o TreeNode.o -pedantic
	${CC} y.tab.o lex.yy.o main.o TreeStuff.o TreeNode.o -o cm -pedantic

cm : y.tab.c lex.yy.c globals.h main.cpp TreeStuff.cpp TreeNode.cpp
	${CC} main.cpp lex.yy.c y.tab.c TreeStuff.cpp TreeNode.cpp -o cm -pedantic -g

tm :
	cd vm && $(MAKE)
	cp vm/tm .

lex.yy.c : cminus.l
	flex cminus.l
	
y.tab.c : cminus.y
	yacc -d --verbose --debug cminus.y
	
clean : 
	rm -f lex.yy.c y.tab.c y.tab.h *~ *.o cm cminus.output out*.txt output.tm tm valgrind-results.txt
	cd vm && $(MAKE) clean
	cd cminusCode && rm -f *~

tidy :
	rm -f cminus.tab.c cminus.tab.h *.0
	
test : clean cm tm tidy
	./cm cminusCode/1.cm output.tm
	diff output.tm template.tm

memtest : clean cm
	valgrind --log-file=valgrind-results.txt ./cm cminusCode/1.cm output.txt 