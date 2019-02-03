#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define BITS_IN_INT 11
#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_SYMBOL_TABLE_ROWS 1024
#define MAX_KEYWORD_BINARY_LENGTH 12
#define MAX_INSTRUCTIONS 1024
#define MAX_INSTRUCTION_LENGTH 256 /* TODO - IS THIS OK? */
#define MAX_RESERVED_KEYWORD_SIZE 4
#define RESERVED_KEYWORDS_COUNT 23
#define DATA_DEFINITIONS_COUNT 2
#define MAX_DATA_DEF_LENGTH 7

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
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
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

char *reservedKeywords[RESERVED_KEYWORDS_COUNT] = {
    "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "mov", "cmp", "add", "sub", "not", "clr", "lea",
    "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    "rts", "stop"
};

char *dataDefinitionKeywords[DATA_DEFINITIONS_COUNT] = {
    ".data", ".string"
};

/* Test if a given keyword is a reserved keyword by the language, which can't be
reassigned as a user variable */
int isReservedKeyword(char *keyword) {
    int i;
    for(i = 0; i < RESERVED_KEYWORDS_COUNT; i++) {
        if (strcmp(keyword, reservedKeywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

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
    char val;
    if ((mask & value) == 0) {
        return '0';
    }
    return '1';
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

/* Check if a command is a symbol definition */
int isSymbolDefinition(char *command) {
    if (strchr(command, ':') == NULL) {
        return 0;
    }
    return 1;
} 

/* Check if a command is a valid symbol decleration*/
char *getSymbolName(char *command) {
    char *symbolName = malloc(MAX_SYMBOL_NAME_LENGTH);

    printf("Received command = %s\n", command);

    symbolName = strtok(command, ":");

    printf("Detected symbol name %s\n", symbolName);

    if (isReservedKeyword(symbolName)) {
        printf("Can't declare symbol %s (reserved keyword)\n", command);
        return NULL;
    }
    if (isalpha(symbolName[0]) == 0) {
        printf("Symbol name must begin with an alphabet character (a-z, A-Z)\n");
        return NULL;
    }
    return symbolName;
}

/* Receive a raw command and a symbolName, extract the symbol name from the command and put it 
in the symbolName variable */
void getSymbolName2(char *rawCommand, char *symbolName) {
    char *rawCommandStart = rawCommand;

    /* Locate the command name */
    while (*rawCommandStart != 0 && !isspace(*rawCommandStart) && *rawCommand != ':') {
        *symbolName = *rawCommandStart;
        symbolName++;
        rawCommandStart++;
    }

    *(--symbolName) = '\0'; /* We extracted the ':' part as well, so get rid of it before terminating the symb name */
    
    /* Remove the command name from rawCommand */
    while (*rawCommandStart != 0) {
        *rawCommand = *rawCommandStart;
        rawCommand++;
        rawCommandStart++; 
    }
    *rawCommand = '\0';
}

/*
    Trim all leading whitespaces in the received source string. This function modifies the original string!

    @param source (*char) - The string to be trimmed
*/
void trimLeadingWhitespace(char *source) {
    char *start = source;
    
    while (isspace(*start)) {
        start++;
    }

    while (*start != 0) {
        *source = *start;
        source++;
        start++;
    }   
    *source = '\0'; /* Terminate the string */
}

int isDataDefinition(char *command) {
    int i;
    char defType[MAX_DATA_DEF_LENGTH];
    printf("Working on %s\n", command);
    i = 0;
    /* Iterate until we find a space. That's where the potential data def should end */
    while (*command != 0 && !isspace(*command)) {
        if (i >= MAX_DATA_DEF_LENGTH) {
            return 0;
        }
        defType[i] = *command;
        i++;
        command++;
    }

    // trimLeadingWhitespace(potentialDefType);
    printf("Potential data def - %s\n", defType);
    for (i = 0; i < DATA_DEFINITIONS_COUNT; i++) {
        if (strcmp(defType, dataDefinitionKeywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void getDataDefinionType(char *rawCommand, char *defType) {
    char *rawCommandStart = rawCommand;
    int defSize;
    printf("Received raw command = %s\n", rawCommand);
    /* Locate the command name */

    while (*rawCommandStart != 0 && !isspace(*rawCommandStart)) {
        *defType = *rawCommandStart;
        defType++;
        defSize++;
        rawCommandStart++;
    }

    *defType = '\0';
    defType = defType - defSize; /* Go back to beginning of string */

    if (!isDataDefinition(defType)) {
        printf("defType %s is not a valid def type, meaning command isn't a def type\n", defType);
        *defType = '\0';
        return;
    }

    /* Remove the def type we extracted from rawCommand */
    while (*rawCommandStart != 0) {
        *rawCommand = *rawCommandStart;
        rawCommand++;
        rawCommandStart++; 
    }
    *rawCommand = '\0';
}

void firstIteration() {
    // 1. Read row from file
    // char inputRow[] = "XYZ: .data 5";
    char inputRow[] = "  .data 5";
    // char *dataTypeDef;

    trimLeadingWhitespace(inputRow);

    printf("Working on %s\n", inputRow);
    // 2. Is it a symbol? 
    if (isSymbolDefinition(inputRow)) {
        printf("Row is a symbol!\n");
    } else if (isDataDefinition(inputRow)) { // Is this an instruction to store data? (.string or .data)
        printf("Row has no symbol but is a data def!\n");
    } else {
        printf("Nothing.... :(\n");
    }

    // 3. If it's a symbol & symbol type '.data' or '.string' then add to symbol table & data table, and increase DC

    symbolTableRow *symbRow = (symbolTableRow*)malloc(sizeof(symbolTableRow));
    // char symbName[MAX_SYMBOL_NAME_LENGTH] = "X";

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

    int y_val = 7;
    memKeywordBinaryString *mkb2 = (memKeywordBinaryString*)malloc(sizeof(memKeywordBinaryString));
    intToBinaryString(y_val, mkb2);
    dataTable.rows[dataTable.dataCounter] = mkb2;
    (dataTable.dataCounter)++;
}



int main(int argc, char *argv[]) {
    firstIteration();

    // FILE *fp;
    // char *commands[2] = {"MOV @r1, @r2", "X: .data 3"};

    // for (int i=0; i < 2; i++) {
    //     printf("%s\n", commands[i]);
    // }

    // memKeyword mem;
    // memKeywordBinaryString mkb;

    // printf("Hello!\n");

    // ######################################################
    // ### first Iteration tests (currently only a simple data table initialization)
    // printf("Symbol table size before 1st iteration = %d\n", symbTable.rowsCounter);
    // printf("Data table size before 1st iteration = %d\n", dataTable.dataCounter);
    // printf("\n\n");
    // firstIteration();
    // printf("Finished first iteration\n");

    // printf("Symbol table size AFTER 1st iteration = %d\n", symbTable.rowsCounter);

    // printSymbolTable();
    // printf("\n\n");
    // printf("Data table size AFTER 1st iteration = %d\n", dataTable.dataCounter);
    // printDataTable();
    // printf("\n\nDone!\n");

    
    // ######################################################
    // ############# TEST SYMBOL VALIDATIONS ################
    // printf("'MOV @r1, @r2' is symbol == %d (should be 0)\n", isSymbolDefinition("MOV @r1, @r2"));
    // printf("'X: .data 5' is symbol == %d (should be 1) \n", isSymbolDefinition("X: .data 5"));
    // printf("'X: .data:: 5' is symbol == %d (should be 1) \n", isSymbolDefinition("X: .data:: 5"));
    // printf("'.extern Z' is symbol == %d (should be 0) \n", isSymbolDefinition(".extern Z"));
    
    // char str[] = "r1: .data 5";
    // printf("'%s' is valid symbol == %d\n", str, isValidSymbol(str));

    // char str2[] = "1x: .data 5";
    // printf("'%s' is valid symbol == %d\n", str2, isValidSymbol(str2));

    // char str3[] = "XYZ: .data 5";
    // char symbName3[MAX_SYMBOL_NAME_LENGTH];
    // printf("Command before getting symb name = '%s'\n", str3);
    // symbName3 = getSymbolName(str3);
    // getSymbolName2(str3, symbName3);
    // printf("Got symbol name %s from command '%s'\n", symbName3, str3);

    // ################### TEST IF COMMAND IS A DATA DEFINITION ###################
    // char str4[] = " .data XYZ:  .data 3";
    // printf("Command '%s' is data def? %d\n", str4, isDataDefinition(str4));
    // printf("'%s' is valid symbol == %d\n", str3, isValidSymbol(str3));
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

    // memKeyword mem;
    // memKeywordBinaryString mkb;

    // mem.opCode = 0; // MOV
    // mem.encodingType = 0; // 0 (A) because this is a command type of instruction
    // mem.targetAddressingMode = 5; // Because this is a register (r1)
    // mem.sourceAddressingMode = 5; // Because this is a register (r2)

    // printf("Trying 'memKeyword_to_binary' \n");
    // memKeyword_to_binary(&mem);
    // printf("\n");

    // memKeywordToBinaryString(&mem, &mkb);

    // printf("%s\n", mkb.keyword);

    // printf("Expected binary representation of MOV r1, r2 =  \n");
    // printf("101000010100\n");

    return 0;
}