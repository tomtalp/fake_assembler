#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS_IN_INT 11
#define MAX_SYMBOL_NAME 256
#define MAX_SYMBOL_TABLE_ROWS 1024
#define MAX_KEYWORD_BINARY_LENGTH 12
#define MAX_INSTRUCTIONS 1024
#define MAX_INSTRUCTION_LENGTH 256 /* TODO - IS THIS OK? */

#define KEYWORD_ENCODING_TYPE_BITS 2
#define KEYWORD_ADDRESSING_MODE_BITS 3
#define KEYWORD_OPS_CODE_BITS 4

#define BASE_MEM_ADDRESS 100 /* The base memory address our program starts with*/

/* Representation of a keyword in INT's */ 
typedef struct memKeyword {
    unsigned int encodingType : KEYWORD_ENCODING_TYPE_BITS;
    unsigned int targetAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
    unsigned int opCode : KEYWORD_OPS_CODE_BITS;
    unsigned int sourceAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
} memKeyword;

/* Representation of a keyword as a String (char array of size 12)*/
typedef struct memKeywordBinaryString{
    char keyword[MAX_KEYWORD_BINARY_LENGTH];
} memKeywordBinaryString;

/* The code instructions section - contains the actual code instructions  */
typedef struct codeInstructionsTable {
    int instructionCount;
    memKeywordBinaryString *rows[MAX_INSTRUCTIONS];
} codeInstructionsTable;

/* The data definition section - contains the data counter (DC) and an array of data value
encoded in binary*/
typedef struct dataDefinitionsTables {
    int dataCounter;
    memKeywordBinaryString *rows[MAX_INSTRUCTIONS];
} dataDefinitionsTables;

typedef struct symbolTableRow {
    char symbolName[MAX_SYMBOL_NAME];
    int memoryAddress;
    int isExternal;
    int isDefinitionSymbol; /* Is the symbol pointing to a definition statement? */
} symbolTableRow;

typedef struct symbolTable {
    symbolTableRow *rows[MAX_SYMBOL_TABLE_ROWS];
    int rowsCounter;
} symbolTable;

symbolTable symbTable;
dataDefinitionsTables dataTable;
codeInstructionsTable codeTable;


struct {
    char *code_name;
    int op_code;
} OP_CODES[] = {
    {"mov", 0},
    {"cmp", 1},
    {"add", 2},
    {"sub", 3},
    {"not", 4},
    {"clr", 5},
    {"lea", 6},
    {"inc", 7},
    {"dec", 8},
    {"jmp", 9},
    {"bne", 10},
    {"red", 11},
    {"prn", 12},
    {"jsr", 13},
    {"rts", 14},
    {"stop", 15}
};

void printSymbolTableRow(symbolTableRow *row) {
    printf("Name = %s | Mem addr = %d | Is Extern = %d | Is Def symbol = %d\n", row->symbolName, row->memoryAddress, row->isExternal, row->isDefinitionSymbol);
}

// void printSymbolTable(symbolTable *symbTable) {
void printSymbolTable() {
    int i;
    printf("Symbol Table has %d rows\n", symbTable.rowsCounter);
    for(i = 0; i < symbTable.rowsCounter; i++) {
        printf("Symbol table row #%d = name = %s, addr = %d\n", i, symbTable.rows[i]->symbolName, symbTable.rows[i]->memoryAddress);
    }
}

void printDataTable() {
    int i;

    printf("Data Table has %d rows\n", dataTable.dataCounter);
    for (i = 0; i < dataTable.dataCounter; i++) {
        printf("Data table row #%d mem addr = %s\n", i, dataTable.rows[i]->keyword);
    }
}

char getBinaryChar(int mask, int value) {
    // return (mask & value == 0) ? '0' : '1';
    char val;
    if ((mask & value) == 0) {
        return '0';
    }
    return '1';

    // return 
}

void memKeywordToBinaryString(memKeyword *mk, memKeywordBinaryString *mkb) {
    int i, j, mask;
    char binaryChar;

    /* Convert encoding type */
    i = MAX_KEYWORD_BINARY_LENGTH-1;

    mask = 1 << KEYWORD_ENCODING_TYPE_BITS-1;
    while (mask) {
        binaryChar = getBinaryChar(mask, mk->encodingType);
        mkb->keyword[i] = binaryChar;
        i--;
        mask >>= 1;
    }
    
    /* Convert target addressing mode */
    mask = 1 << (KEYWORD_ADDRESSING_MODE_BITS-1);
    while (mask) {
        binaryChar = getBinaryChar(mask, mk->targetAddressingMode);
        mkb->keyword[i] = binaryChar;
        i--;
        mask >>= 1;
    }

    /* Convert Operation code */
    mask = 1 << KEYWORD_OPS_CODE_BITS-1;
    while (mask) {
        mkb->keyword[i] = getBinaryChar(mask, mk->opCode);
        i--;
        mask >>= 1;
    }

    /* Convert source addressing mode */
    mask = 1 << KEYWORD_ADDRESSING_MODE_BITS-1;
    while (mask) {
        mkb->keyword[i] = getBinaryChar(mask, mk->sourceAddressingMode);
        i--;
        mask >>= 1;
    }
}

void intToBinaryString(int n, memKeywordBinaryString *mkb) {
    int mask, i;
    i = 0;

    mask = 1 << MAX_KEYWORD_BINARY_LENGTH;
    while (mask) {
        mkb->keyword[i++] = getBinaryChar(mask, n);
        mask >>= 1;
    }
}

void print_int_as_binary(int n, int expected_bits);

void memKeyword_to_binary(memKeyword *mem) {
    // printf("sourceAddressingMode = ");
    print_int_as_binary(mem->sourceAddressingMode, 3-1);
    // printf("\n");

    // printf("opCode = ");
    print_int_as_binary(mem->opCode, 4-1);
    // printf("\n");

    // printf("targetAddressingMode = ");
    print_int_as_binary(mem->targetAddressingMode, 3-1);
    // printf("\n");

    // printf("encodingType = ");
    print_int_as_binary(mem->encodingType, 2-1);
    // printf("\n");/

    // printf("DONE!\n\n\n");
}

void print_int_as_binary(int n, int expected_bits) {
    unsigned int mask = 1 << expected_bits;
    while (mask) {
        // printf("n & mask = %d\n", n & mask);
        if((n & mask) == 0) {
            printf("0");
        } else {
            printf("1");
        }
        mask >>= 1;
    }
}

void firstIteration() {
    // 1. Read row from file

    // 2. Is it a symbol? 

    // 3. If it's a symbol & symbol type '.data' or '.string' then add to symbol table & data table, and increase DC

    symbolTableRow *symbRow = (symbolTableRow*)malloc(sizeof(symbolTableRow));
    // char symbName[MAX_SYMBOL_NAME] = "X";

    // strcpy(symbRow->symbolName, symbName);
    strcpy(symbRow->symbolName, "X");
    symbRow->memoryAddress = dataTable.dataCounter;
    symbRow->isExternal = 0;
    symbRow->isDefinitionSymbol = 0;

    symbTable.rows[symbTable.rowsCounter] = symbRow;
    (symbTable.rowsCounter)++;

    int x_val = 10;
    memKeywordBinaryString *mkb1 = (memKeywordBinaryString*)malloc(sizeof(memKeywordBinaryString));
    intToBinaryString(x_val, mkb1);
    dataTable.rows[dataTable.dataCounter] = mkb1;
    (dataTable.dataCounter)++;

    symbolTableRow *symbRow2 = (symbolTableRow*)malloc(sizeof(symbolTableRow));
    strcpy(symbRow2->symbolName, "Y");
    symbRow2->memoryAddress = dataTable.dataCounter;
    symbRow2->isExternal = 0;
    symbRow2->isDefinitionSymbol = 0;

    symbTable.rows[symbTable.rowsCounter] = symbRow2;
    (symbTable.rowsCounter)++;
}



int main(int argc, char *argv[]) {
    FILE *fp;
    // memKeyword mem;
    // memKeywordBinaryString mkb;

    printf("Hello!\n");

    printf("Symbol table size before 1st iteration = %d\n", symbTable.rowsCounter);
    printf("Data table size before 1st iteration = %d\n", dataTable.dataCounter);
    printf("\n\n");
    firstIteration();
    printf("Finished first iteration\n");

    printf("Symbol table size AFTER 1st iteration = %d\n", symbTable.rowsCounter);

    printSymbolTable();
    printf("\n\n");
    printf("Data table size AFTER 1st iteration = %d\n", dataTable.dataCounter);
    printDataTable();
    printf("\n\nDone!\n");
    // char *fileRows[MAX_INSTRUCTIONS];
    // char row[MAX_INSTRUCTION_LENGTH];
    // int i=0;
    // if (argc == 1) {
    //     printf("Missing file arg!!\n");
    //     return -1;
    // }

    // if ((fp = fopen(*++argv, "r")) == NULL) {
    //     printf("Failed opening %s\n", *argv);
    // } else {
    //     printf("In else, opening %s\n", *argv);
    //     // char ch = fgetc(fp);
    //     // cat(fp);
    //     // int c;
    //     // while ((c = getc(fp)) != EOF) {
    //     //     putc(c, stdout);
    //     // }
    //     // printf("%c\n", c);
    //     // fclose(fp);
    //     while (!feof(fp)) {
    //         printf("First iteration!\n");
    //         // printf("fileRows[i] = %s\n", fileRows[i]);

    //         /* MALLOC VERSION - SEEMS TO HAVE ISSUES! */
    //         // fileRows[i] = malloc(MAX_INSTRUCTION_LENGTH);
    //         // while (fgets(fileRows[i], MAX_INSTRUCTIONS, fp) != NULL) {
    //         //     printf("hi\n");
    //         //     printf("Got %s\n", fileRows[i]);
    //         //     fileRows[++i] = malloc(MAX_INSTRUCTION_LENGTH);
    //         // }

    //         /* Regular Sring version, works fine but sucks*/
    //         while (fgets(row, MAX_INSTRUCTIONS, fp) != NULL) {
    //             printf("Got %s\n", row);
    //             // fileRows[i] = malloc(MAX_INSTRUCTION_LENGTH);
    //             // fileRows[i] = row;
    //             // printf("fileRows[%d] =  %s\n", i, fileRows[i]);
    //             i++;
    //         }
    //     }
    // }

    // Example - MOV 5, r5
    // encodingType = 

    // 001000010100
    // 000000000101
    // 000000000101

    memKeyword mem;
    memKeywordBinaryString mkb;

    mem.opCode = 0; // MOV
    mem.encodingType = 0; // 0 (A) because this is a command type of instruction
    mem.targetAddressingMode = 5; // Because this is a register (r1)
    mem.sourceAddressingMode = 5; // Because this is a register (r2)

    // printf("Trying 'memKeyword_to_binary' \n");
    // memKeyword_to_binary(&mem);
    // printf("\n");

    // memKeywordToBinaryString(&mem, &mkb);

    // printf("%s\n", mkb.keyword);

    // printf("Expected binary representation of MOV r1, r2 =  \n");
    // printf("101000010100\n");

    return 0;
}