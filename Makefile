all: build
clean:
	rm *.out *.ob *.ext *.ent
build: 
	gcc -Wall -ansi -pedantic main.c parser.c dataStructures.c assembler.c utils.c errors.c  -o assembler.out
debug:
	gcc -g main.c parser.c dataStructures.c assembler.c utils.c -o a.out
test:
	gcc -g tests/parser.c parser.c dataStructures.c assembler.c utils.c errors.c -o test.out	
