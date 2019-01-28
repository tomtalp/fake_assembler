#include <stdio.h>
#define BITS_IN_INT 11
#define MAX_SYMBOL_NAME 10
#define MAX_SYMBOL_TABLE_ROWS 1024

typedef struct memKeyword {
    unsigned int encodingType : 2;
    unsigned int targetAddressingMode : 3;
    unsigned int opCode : 4;
    unsigned int sourceAddressingMode : 3;
} memKeyword;

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
    // printf("\n");
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

    // Example - MOV r1, r2
    // encodingType = 
    mem.opCode = 0; // MOV
    mem.encodingType = 0; // 0 (A) because this is a command type of instruction
    mem.targetAddressingMode = 5; // Because this is a register (r1)
    mem.sourceAddressingMode = 5; // Because this is a register (r2)

    memKeyword_to_binary(&mem);
    printf("\n");

    printf("Expected binary representation of MOV r1, r2 =  \n");
    printf("101000010100\n");
    return 0;
}