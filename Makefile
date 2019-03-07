build: 
	gcc main.c parser.c dataStructures.c assembler.c utils.c errors.c  -o a.out
debug:
	gcc -g main.c parser.c dataStructures.c assembler.c utils.c -o a.out
test:
	gcc -g tests/parser.c parser.c utils.c errors.c -o test.out	