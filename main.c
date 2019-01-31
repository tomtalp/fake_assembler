#include <stdio.h>

#define BITS_IN_INT 11
#define MAX_SYMBOL_NAME 10
#define MAX_SYMBOL_TABLE_ROWS 1024
#define MAX_KEYWORD_BINARY_LENGTH 12
#define MAX_INSTRUCTIONS 1024

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
typedef struct memKeywordBinary{
    char keyword[MAX_KEYWORD_BINARY_LENGTH];
} memKeywordBinary;

/* The code instructions section - contains the actual code instructions  */
typedef struct codeInstructionsTable {
    int instructionCount;
    char memKeywordBinary[MAX_INSTRUCTIONS];
} codeInstructionsTable;

/* The data definition section - contains the data counter (DC) and an array of data value
encoded in binary*/
typedef struct dataDefinitionsTables {
    int dataCounter;
    char memKeywordBinary[MAX_INSTRUCTIONS];
} dataDefinitionsTables;

typedef struct symbolTableRow {
    char symbolName[MAX_SYMBOL_NAME];
    int memoryAddress;
    int isExternal;
    int isDefinitionSymbol; /* Is the symbol pointing to a definition statement? */
} symbolTableRow;

typedef struct symbolTable {
    symbolTableRow rows[MAX_SYMBOL_TABLE_ROWS];
    int rowsCounter;
} symbolTable;

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

char getBinaryChar(int mask, int value) {
    // return (mask & value == 0) ? '0' : '1';
    char val;
    if ((mask & value) == 0) {
        return '0';
    }
    return '1';

    // return 
}

void memKeywordToBinary(memKeyword *mk, memKeywordBinary *mkb) {
    printf("Hello\n");
    int i, j, mask;
    char binary[MAX_KEYWORD_BINARY_LENGTH];
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

int main() {
    memKeyword mem;
    memKeywordBinary mkb;

    // Example - MOV r1, r2
    // encodingType = 
    mem.opCode = 0; // MOV
    mem.encodingType = 0; // 0 (A) because this is a command type of instruction
    mem.targetAddressingMode = 5; // Because this is a register (r1)
    mem.sourceAddressingMode = 5; // Because this is a register (r2)

    printf("Trying 'memKeyword_to_binary' \n");
    memKeyword_to_binary(&mem);
    printf("\n");

    memKeywordToBinary(&mem, &mkb);

    printf("%s\n", mkb.keyword);

    printf("Expected binary representation of MOV r1, r2 =  \n");
    printf("101000010100\n");

    return 0;
}