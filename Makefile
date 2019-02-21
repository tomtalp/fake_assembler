build: 
	gcc main.c parser.c dataStructures.c assembler.c utils.c -o a.out
debug:
	gcc -g main.c parser.c dataStructures.c assembler.c utils.c -o a.out