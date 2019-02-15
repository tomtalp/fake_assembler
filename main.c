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
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DEFINITIONS_COUNT 2
#define MAX_OP_CODE_CHARS_LENGTH 4
#define OP_CODES_COUNT 16

#define KEYWORD_ENCODING_TYPE_BITS 2
#define KEYWORD_ADDRESSING_MODE_BITS 3
#define KEYWORD_OP_CODE_BITS 4

#define BASE_MEM_ADDRESS 100 /* The base memory address our program starts with*/

/* Representation of a keyword in INT's */ 
typedef struct memKeyword {
    unsigned int encodingType : KEYWORD_ENCODING_TYPE_BITS;
    unsigned int targetAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
    unsigned int opCode : KEYWORD_OP_CODE_BITS;
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

typedef struct symbolTableNode {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    int memoryAddress;
    int isExternal;
    int isDefinitionSymbol; /* Is the symbol pointing to a definition statement? */
    struct symbolTableNode *next;
} symbolTableNode;

typedef struct symbolTable {
    symbolTableNode *head;
    int symbolsCounter;
} symbolTable;

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
    "rts", "stop", "entry", "extern", "string", "data"
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

int getOpCodeNumber(char *opCode) {
    int i;

    for (i = 0; i < OP_CODES_COUNT; i++) {
        if (strcmp(opCode, OP_CODES[i].code_name) == 0) {
            return OP_CODES[i].op_code;
        }
    }

    return -1;
}

void initDataTable() {
    dataTable.dataCounter = 0;
}

void initSymbolTable(symbolTable *symbTable) {
    printf("inside initSymbolTable\n");
    symbTable->head = NULL;
    symbTable->symbolsCounter = 0;
}

void printSymbolTableRow(symbolTableNode *row) {
    printf("Name = %s | Mem addr = %d | Is Extern = %d | Is Def symbol = %d\n", row->symbolName, row->memoryAddress, row->isExternal, row->isDefinitionSymbol);
}

void printSymbolTable(symbolTable *tb) {
    int i;
    symbolTableNode *temp = tb->head;

    printf("Symbol Table has %d rows\n", tb->symbolsCounter);
    // for(i = 1; temp ; temp = temp->next, i++)  {   
    for(i = 0; i < tb->symbolsCounter; temp = temp->next, i++)  {   
        printf("Symbol table row #%d = name = %s, addr = %d\n", i+1, temp->symbolName, temp->memoryAddress);
    }
}

void addNodeToSymbolTable(symbolTable *tb, symbolTableNode *newNode) {
    symbolTableNode *temp = (symbolTableNode*)malloc(sizeof(symbolTableNode));

    if (tb->symbolsCounter == 0) {
        tb->head = newNode;
        tb->symbolsCounter += 1;
        return;
    }

    tb->head->next = newNode;
    newNode->next = temp;
    tb->symbolsCounter += 1;
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
    mask = 1 << KEYWORD_OP_CODE_BITS-1;
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
    // TODO REMOVE MAGIC NUMBERS (3-1, 4-1, 2-1)
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

// /* Check if a command is a valid symbol decleration*/
// char *getSymbolName(char *command) {
//     char *symbolName = malloc(MAX_SYMBOL_NAME_LENGTH);

//     printf("Received command = %s\n", command);

//     symbolName = strtok(command, ":");

//     printf("Detected symbol name %s\n", symbolName);

//     if (isReservedKeyword(symbolName)) {
//         printf("Can't declare symbol %s (reserved keyword)\n", command);
//         return NULL;
//     }
//     if (isalpha(symbolName[0]) == 0) {
//         printf("Symbol name must begin with an alphabet character (a-z, A-Z)\n");
//         return NULL;
//     }
//     return symbolName;
// }

/* Receive a raw command and a symbolName, extract the symbol name from the command and put it 
in the symbolName variable */
void getSymbolName(char *rawCommand, char *symbolName) {
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
    char defType[MAX_RESERVED_KEYWORD_SIZE];
    printf("Working on %s\n", command);
    i = 0;
    /* Iterate until we find a space. That's where the potential data def should end */
    while (*command != 0 && !isspace(*command)) {
        if (i >= MAX_RESERVED_KEYWORD_SIZE) {
            return 0;
        }
        defType[i] = *command;
        i++;
        command++;
    }

    defType[i] = '\0';

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

    // This is redundant since we already check this before calling the function
    // if (!isDataDefinition(defType)) {
    //     printf("defType %s is not a valid def type, meaning command isn't a def type\n", defType);
    //     *defType = '\0';
    //     return;
    // }

    /* Remove the def type we extracted from rawCommand */
    while (*rawCommandStart != 0) {
        *rawCommand = *rawCommandStart;
        rawCommand++;
        rawCommandStart++; 
    }
    *rawCommand = '\0';
}

int isExternOrEntry(char *command) {
    int i;
    char potentialKeyword[MAX_RESERVED_KEYWORD_SIZE];
    i = 0;
    /* Iterate until we find a space. That's where the potential exter/entry declaration should end */
    while (*command != 0 && !isspace(*command)) {
        if (i >= MAX_RESERVED_KEYWORD_SIZE) {
            return 0;
        }
        potentialKeyword[i] = *command;
        i++;
        command++;
    }

    potentialKeyword[i] = '\0';

    if ((strcmp(potentialKeyword, ".extern") == 0) || (strcmp(potentialKeyword, ".entry") == 0)) {
        return 1;
    }
    return 0;
}

void addToDataTable(char *inputRow, char *dataDefType) {
    int num;

    if (strcmp(dataDefType, ".string") == 0 ) {
        printf("Adding .string data type\n");
        // TODO ADD STRING
    } else if (strcmp(dataDefType, ".data") == 0 ) {
        printf("Adding .data data type. now inputRow is - '%s'\n", inputRow);
        printf("Print all numbers - \n");
        // TODO - 
        // 1. Handle non digits
        // 2. Handle negative numbers
        while (*inputRow != 0) {
            num = 0;
            // printf("Inside outer while. Input row right now - %s\n", inputRow);
            while (*inputRow != 0 && !isspace(*inputRow) && *inputRow != ',') {
                num = num * 10;
                // printf("%c", *inputRow);
                if (!isdigit(*inputRow)) {
                    printf("Woopsie! TODO HANDLE THIS\n");
                }
                num += (*inputRow - '0'); // Add the character digit as an int
                inputRow++;
            }
            inputRow++;
            trimLeadingWhitespace(inputRow);
            printf("Dected number = %d\n", num);

            memKeywordBinaryString *mkb1 = (memKeywordBinaryString*)malloc(sizeof(memKeywordBinaryString));
            intToBinaryString(num, mkb1);
            dataTable.rows[dataTable.dataCounter] = mkb1;
            (dataTable.dataCounter)++;

            printf("\n\n");
        }
    }
}

void getOpCode(char *inputRow, char *opCode) {
    char *rawCommandStart = inputRow;
    int opCodeSize;
    printf("Received raw command = %s\n", inputRow);
    
    /* Locate the command name */
    while (*rawCommandStart != 0 && !isspace(*rawCommandStart)) {
        *opCode = *rawCommandStart;
        opCode++;
        opCodeSize++;
        rawCommandStart++;
    }

    *opCode = '\0';
    opCode = opCode - opCodeSize; /* Go back to beginning of string */

    /* Remove the op code we extracted from the command */
    while (*rawCommandStart != 0) {
        *inputRow = *rawCommandStart;
        inputRow++;
        rawCommandStart++; 
    }
    *inputRow = '\0';
}

void firstIteration(symbolTable *symbTable) {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    char dataDefType[MAX_RESERVED_KEYWORD_SIZE];
    char opCode[MAX_OP_CODE_CHARS_LENGTH];
    int isSymbolFlag = 0;
    // 1. Read row from file
    // char inputRow[] = "movv @r3, @r1";
    char inputRow[] = "XYZ: .data 5, 3, 1";
    // char inputRow[] = "  .data 5";
    // char inputRow[] = ".extern Z";
    // char *dataTypeDef;

    trimLeadingWhitespace(inputRow);

    printf("firstIteration is working on %s\n", inputRow);
    // 2. Is it a symbol? 
    isSymbolFlag = isSymbolDefinition(inputRow); // TODO - Boolean should be unsigned char, not INT
    if (isSymbolFlag) {
        getSymbolName(inputRow, symbolName);
        printf("Got symbol name = %s, left with %s\n", symbolName, inputRow);
    }
    printf("isSymbolFlag = %d\n", isSymbolFlag);

    trimLeadingWhitespace(inputRow);

    if (isDataDefinition(inputRow)) { // Is this an instruction to store data? (.string or .data)
        printf("Row is a data def!\n");
        
        if (isSymbolFlag) {
            printf("Data def AND symb flag is on!!!\n");

            symbolTableNode *symbNode = (symbolTableNode*)malloc(sizeof(symbolTableNode));

            strcpy(symbNode->symbolName, symbolName);
            symbNode->memoryAddress = dataTable.dataCounter;
            symbNode->isExternal = 0;
            symbNode->isDefinitionSymbol = 0;
            symbNode->next = NULL;
            printf("Initialized a node\n");

            addNodeToSymbolTable(symbTable, symbNode);
        }
        trimLeadingWhitespace(inputRow);
        getDataDefinionType(inputRow, dataDefType);
        printf("Got data def type = %s, left with %s\n", dataDefType, inputRow);
        trimLeadingWhitespace(inputRow);
        addToDataTable(inputRow, dataDefType);
        
    } else if (isExternOrEntry(inputRow)) {
        printf("Command is external/entry.\n");
        printf("TBD\n");
        // Here we need to insert "EXTERN" into the symbol table
    } else { // Code instruction
        printf("Inside the Code instruction part\n");
        if (isSymbolFlag) {
            printf("Code instruction AND symb flag is on!!!\n");

            symbolTableNode *symbNode = (symbolTableNode*)malloc(sizeof(symbolTableNode));

            strcpy(symbNode->symbolName, symbolName);
            symbNode->memoryAddress = codeTable.instructionCount;
            symbNode->isExternal = 0;
            symbNode->isDefinitionSymbol = 1;
            symbNode->next = NULL;
            printf("Initialized a node\n");

            addNodeToSymbolTable(symbTable, symbNode);
        }
        // Handle the actual code instruction
        trimLeadingWhitespace(inputRow);

        // 1. Extract the op code
        printf("Row before getting op code - '%s'\n", inputRow);
        getOpCode(inputRow, opCode);
        trimLeadingWhitespace(inputRow);
        int opCodeNum = getOpCodeNumber(opCode);
        if (opCodeNum == -1) {
            printf("Op code %s doesn't exist!\n", opCode);
            return;
        }
        printf("Row AFTER getting op code - '%s' , opCode = '%s' (#%d) \n", inputRow, opCode, opCodeNum);
    }
    return;


    int x_val = 10;
    memKeywordBinaryString *mkb1 = (memKeywordBinaryString*)malloc(sizeof(memKeywordBinaryString));
    intToBinaryString(x_val, mkb1);
    dataTable.rows[dataTable.dataCounter] = mkb1;
    (dataTable.dataCounter)++;

    // symbolTableNode *symbNode2 = (symbolTableNode*)malloc(sizeof(symbolTableNode));
    // strcpy(symbNode2->symbolName, "Y");
    // symbNode2->memoryAddress = dataTable.dataCounter;
    // // symbNode2->memoryAddress = 123456;
    // symbNode2->isExternal = 0;
    // symbNode2->isDefinitionSymbol = 0;
    // symbNode2->next = NULL;

    // addNodeToSymbolTable(symbTable, symbNode2);
    // // symbTable.rows[symbTable.rowsCounter] = symbRow2;
    // // (symbTable.rowsCounter)++;

    // int y_val = 7;
    // memKeywordBinaryString *mkb2 = (memKeywordBinaryString*)malloc(sizeof(memKeywordBinaryString));
    // intToBinaryString(y_val, mkb2);
    // dataTable.rows[dataTable.dataCounter] = mkb2;
    // (dataTable.dataCounter)++;
}



int main(int argc, char *argv[]) {
    symbolTable symbTable;
    initSymbolTable(&symbTable);
    printf("Printing initial symbol table - \n");
    printSymbolTable(&symbTable);
    printf("Data table size before 1st iteration = %d\n", dataTable.dataCounter);
    firstIteration(&symbTable);
    printf("Printing symbol table after firstIteration - \n");
    printSymbolTable(&symbTable);

    printf("Data table size AFTER 1st iteration = %d\n", dataTable.dataCounter);
    printDataTable();

    printf("##############################\n");
    firstIteration(&symbTable);
    printf("Printing symbol table after second firstIteration - \n");
    printSymbolTable(&symbTable);

    printf("Data table size AFTER 1st iteration = %d\n", dataTable.dataCounter);
    printDataTable();

    

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
    // getSymbolName(str3, symbName3);
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