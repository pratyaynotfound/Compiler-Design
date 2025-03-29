objfiles = myLex.o lex.yy.o

a.out: $(objfiles)
	cc $(objfiles)

myLex.o: myLex.c y.tab.h
	cc -c -Wall myLex.c

lex.yy.c: lex.l y.tab.h
	flex lex.l
	
lex.yy.o :

clean:
	rm a.out lex.yy.c $(objfiles)
