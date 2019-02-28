#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"

// char *reservedKeywords[RESERVED_KEYWORDS_COUNT] = {
//     "r1", "r2", "r3", "r4", "r5", "r6", "r7",
//     "mov", "cmp", "add", "sub", "not", "clr", "lea",
//     "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
//     "rts", "stop", "entry", "extern", "string", "data"
// };

char *LEGAL_REGISTERS[REGISTERS_COUNT] = {
    "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"
};

// registerKeyword LEGAL_REGISTERS[REGISTERS_COUNT] = {
//     {"@r1", 1},
//     {"@r2", 2},
//     {"@r3", 3},
//     {"@r4", 4},
//     {"@r5", 5},
//     {"@r6", 6},
//     {"@r7", 7}
// };

dataTypeInfo LEGAL_DATA_DECLARATIONS[] = {
    {".data", DATA_TYPE},
    {".string", STRING_TYPE}
};

int foo(char *s1, int n1) {
    printf("Inside a foo validator! s1 = %s, n1 = %d\n", s1, n1);
}

int bar(char *s1, int n1) {
    printf("And this is a bar validator! s1 = %s, n1 = %d\n", s1, n1);
}

opCode LEGAL_OP_CODES[OP_CODES_COUNT] = {
    {"mov", MOV, foo},
    {"cmp", CMP, foo},
    {"add", ADD, foo},
    {"sub", SUB, foo},
    {"not", NOT, foo},
    {"clr", CLR, foo},
    {"lea", LEA, foo},
    {"inc", INC, foo},
    {"dec", DEC, foo},
    {"jmp", JMP, foo},
    {"bne", BNE, foo},
    {"red", RED, foo},
    {"prn", PRN, foo},
    {"jsr", JSR, foo},
    {"rts", RTS, foo},
    {"stop", STOP, bar}
};

void printParsedRow(parsedRow *pr) {
    if (pr->hasSymbol) {
        printf("Row has a symbol, called '%s' \n", pr->symbolName);
    }
    printf("Parsed row type = %d\n", pr->rowType);
    // pr->rowMetadata->dataRowMetadata->type;
    if (pr->rowType == DATA_DECLARATION) {
        printf("Parsed row is of type 'data def' ");
        if (pr->rowMetadata.dataRowMetadata.type == DATA_TYPE) {
            printf("with DATA type");
        } else if (pr->rowMetadata.dataRowMetadata.type == STRING_TYPE) {
            printf("with STRING type");
        }

        printf("Raw data = '%s'\n", pr->rowMetadata.dataRowMetadata.rawData);

    } else if (pr->rowType == CODE_INSTRUCTION) {
        printf("Parsed row is of type 'code instruction', instruction type %s with num %d \n", pr->rowMetadata.codeRowMetadata.oc.opCodeName, pr->rowMetadata.codeRowMetadata.oc.opCodeNum);
        if (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND) {
            printf("No source operand ");
        } else {
            printf("Source operand = '%s' (type %d) ", pr->rowMetadata.codeRowMetadata.srcOperand, pr->rowMetadata.codeRowMetadata.srcOperandType);
        }
        printf(" | ");
        if (pr->rowMetadata.codeRowMetadata.destOperandType == NO_OPERAND) {
            printf("No dest operand ");
        } else {
            printf("Destination operand = '%s' (type %d) ", pr->rowMetadata.codeRowMetadata.destOperand, pr->rowMetadata.codeRowMetadata.destOperandType);
        }
    }

    printf("\n");
    
}


int isSymbolDefinition(char *inputRow) {
    if (strchr(inputRow, ':') == NULL) {
        return 0;
    }
    return 1;
} 

void getSymbolName(char *inputRow, char *symbolName) {
    char *inputRowStart = inputRow;

    /* Locate the symbol name */
    while (*inputRowStart != 0 && !isspace(*inputRowStart) && *inputRow != ':') {
        *symbolName = *inputRowStart;
        symbolName++;
        inputRowStart++;
    }

    *(--symbolName) = '\0'; /* We extracted the ':' part as well, so get rid of it before terminating the symb name */
    
    /* Remove the symbol name from inputRow */
    while (*inputRowStart != 0) {
        *inputRow = *inputRowStart;
        inputRow++;
        inputRowStart++; 
    }
    *inputRow = '\0';
}

int getDataDefType(char *dataDef) {
    if (strcmp(dataDef, ".string") == 0) {
        return STRING_TYPE;
    }
    return DATA_TYPE;
}

void getRowType(char *inputRow, parsedRow *pr) {
    int i = 0;
    char firstKeyword[MAX_RESERVED_KEYWORD_SIZE];
    char *inputRowStart = inputRow;
    int detectedRowType = 0;

    while (*inputRowStart != 0 && !isspace(*inputRowStart)) {
        if (i >= MAX_RESERVED_KEYWORD_SIZE) {
            printf("Whoopsie, first keyword is longer than the max reserved keyword. HANDLE THISSSS\n");
            return;
        }
        firstKeyword[i++] = *inputRowStart;
        inputRowStart++;
    }
    firstKeyword[i] = '\0'; // Terminate the first keyword

    /* Remove the first keyword from inputRow */
    while (*inputRowStart != 0) {
        *inputRow = *inputRowStart;
        inputRow++;
        inputRowStart++; 
    }
    *inputRow = '\0';

    printf("Initial detected keyword = '%s'\n", firstKeyword);

    // What row type do we have?!
    for (i = 0; i < DATA_DECLARATION_TYPES_COUNT; i++) {
        if (strcmp(firstKeyword, LEGAL_DATA_DECLARATIONS[i].dataDefName) == 0) {
            pr->rowType = DATA_DECLARATION;
            detectedRowType = 1;
            printf("Detected DATA DEFINITION\n");
            pr->rowMetadata.dataRowMetadata.type = LEGAL_DATA_DECLARATIONS[i].dataDefCodeNum;
            break;
        }
    }

    if (!detectedRowType) {
        if (strcmp(firstKeyword, ".extern") == 0) {
            pr->rowType = EXTERNAL_DECLARATION;
            detectedRowType = 1;
            printf("Detected EXTERNAL DECLARATION\n");
        } else if (strcmp(firstKeyword, ".entry") == 0) {
            pr->rowType = ENTRY_DECLARATION;
            detectedRowType = 1;
            printf("Detected ENTRY DECLARATION\n");
        }
    }

    if (!detectedRowType) {
        for (i = 0; i < OP_CODES_COUNT; i++) {
            if (strcmp(firstKeyword, LEGAL_OP_CODES[i].opCodeName) == 0) {
                printf("Detected code instruction with opcode %s with num %d\n", LEGAL_OP_CODES[i].opCodeName, LEGAL_OP_CODES[i].opCodeNum);
                pr->rowType = CODE_INSTRUCTION;
                pr->rowMetadata.codeRowMetadata.oc.opCodeName = LEGAL_OP_CODES[i].opCodeName;
                pr->rowMetadata.codeRowMetadata.oc.opCodeNum = LEGAL_OP_CODES[i].opCodeNum;
                detectedRowType = 1;
                break;
            }
        }
        
    }    

    if (!detectedRowType) {
        pr->isValidRow = 0;
        printf("Undefined row type\n");
    }
}

int operandIsRegister(char *operand) {
    int i;

    for (i = 0; i < REGISTERS_COUNT; i++) {
        // if (strcmp(operand, LEGAL_REGISTERS[i].keyword) == 0) {
        if (strcmp(operand, LEGAL_REGISTERS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// TODO - Handle negative numbers
int operandIsNumber(char *operand) {
    while (*operand != '\0') {
        if (!isdigit(*operand)) {
            return 0;
        }
        operand++;
    } 
    return 1;
}

void getOperandTypes(parsedRow *pr, char *firstOperand, char *secondOperand) {
    if (*secondOperand == '\0' && *firstOperand == '\0') {
        pr->rowMetadata.codeRowMetadata.srcOperandType = NO_OPERAND;

        pr->rowMetadata.codeRowMetadata.destOperandType = NO_OPERAND;

    } else if (*firstOperand != '\0' && *secondOperand == '\0') { /* Only one operand (dest) */
        strcpy(pr->rowMetadata.codeRowMetadata.destOperand, firstOperand);
        if (operandIsRegister(firstOperand)) {
            pr->rowMetadata.codeRowMetadata.destOperandType = REGISTER_MODE;
        } else if (operandIsNumber(firstOperand)) {
            pr->rowMetadata.codeRowMetadata.destOperandType = IMMEDIATE_MODE;
        } else {
            /* Assuming the token exists. If it doesn't, we'll throw an exception later */
            pr->rowMetadata.codeRowMetadata.destOperandType = DIRECT_MODE;
        }

    } else { /* Both operands are set */

        strcpy(pr->rowMetadata.codeRowMetadata.srcOperand, firstOperand);
        strcpy(pr->rowMetadata.codeRowMetadata.destOperand, secondOperand);

        if (operandIsRegister(firstOperand)) {
            pr->rowMetadata.codeRowMetadata.srcOperandType = REGISTER_MODE;
        } else if (operandIsNumber(firstOperand)) {
            pr->rowMetadata.codeRowMetadata.srcOperandType = IMMEDIATE_MODE;
        } else {
            /* Assuming the token exists. If it doesn't, we'll throw an exception later */
            pr->rowMetadata.codeRowMetadata.srcOperandType = DIRECT_MODE;
        }

        if (operandIsRegister(secondOperand)) {
            pr->rowMetadata.codeRowMetadata.destOperandType = REGISTER_MODE;
        } else if (operandIsNumber(secondOperand)) {
            pr->rowMetadata.codeRowMetadata.destOperandType = IMMEDIATE_MODE;
        } else {
            /* Assuming the token exists. If it doesn't, we'll throw an exception later */
            pr->rowMetadata.codeRowMetadata.destOperandType = DIRECT_MODE;
        }
    }
}

void getCodeOperands(char *inputRow, parsedRow *pr) {
    printf("Inside getCodeOperands, extracting operands from '%s'\n", inputRow);
    int i;
    char *inputRowStart = inputRow;
    char firstOperand[MAX_SYMBOL_NAME_LENGTH];
    char secondOperand[MAX_SYMBOL_NAME_LENGTH];


    /* Locate the first operand in the command */
    i = 0;
    while (*inputRowStart != 0 && !isspace(*inputRowStart) && *inputRow != ',') {
        firstOperand[i] = *inputRowStart;
        i++;
        inputRowStart++;
    }
    
    /* Terminate the first operand and get rid of the trailing ',' or ' ' if we picked one */
    if (firstOperand[i-1] == ',' || isspace(firstOperand[i-1])) {
        firstOperand[i-1] = '\0'; 
    } else {
        firstOperand[i] = '\0'; 
    }
    printf("Done with first operand extraction = '%s'!\n", firstOperand);
    trimLeadingWhitespace(inputRowStart);
    printf("Trimmed input row\n");
    /* Locate the second operand, if it exists the command */
    i = 0;
    while (*inputRowStart != 0 && !isspace(*inputRowStart) && *inputRow != ',') {
        secondOperand[i] = *inputRowStart;
        i++;
        inputRowStart++;
    }
    secondOperand[i] = '\0';  /* Terminate the second operand */
    printf("Done with second operand extraction = '%s'!\n", secondOperand);
    printf("Finished extracting operands! first = '%s', second = '%s'\n", firstOperand, secondOperand);
    /* Remove the operands from inputRow */
    while (*inputRowStart != 0) {
        *inputRow = *inputRowStart;
        inputRow++;
        inputRowStart++; 
    }
    *inputRow = '\0';

    getOperandTypes(pr, firstOperand, secondOperand);

}

void addStringRawData(parsedRow *pr, char *rawData) {
    char strippedData[MAX_INSTRUCTION_LENGTH];
    int i;

    i = 0;

    while (*rawData != '\0' && *rawData != '\n') {
        printf("Hello, looking at %c\n", *rawData);
        if (*rawData != '"') {
            strippedData[i] = *rawData;
            i++;
        }
        rawData++;
    }

    printf("String after stripping = '%s'\n", strippedData);
    strcpy(pr->rowMetadata.dataRowMetadata.rawData, strippedData);
}

void parseRow(char *inputRow, parsedRow *pr, int rowNum) {

    printf("1. Parsing '%s' from row number #%d\n", inputRow, rowNum);
    pr->originalLineNum = rowNum;
    pr->isValidRow = 1; // Assume valid row until proving otherwise
    trimLeadingWhitespace(inputRow);

    printf("2. After initial whitespace trimming - '%s'\n", inputRow);

    pr->hasSymbol = isSymbolDefinition(inputRow);
    if (pr->hasSymbol) {
        getSymbolName(inputRow, pr->symbolName);
        trimLeadingWhitespace(inputRow);
    }

    printf("3. Left with '%s' after checking symbol\n", inputRow);

    getRowType(inputRow, pr);
    trimLeadingWhitespace(inputRow);
    printf("4. After getting row type, we're left with - '%s'\n", inputRow);

    if (pr->rowType == DATA_DECLARATION) {
        printf("So we're dealing with a data decl, lets get the data!\n");
        if (pr->rowMetadata.dataRowMetadata.type == STRING_TYPE) {
            addStringRawData(pr, inputRow);
        } else {
            strcpy(pr->rowMetadata.dataRowMetadata.rawData, inputRow);
        }
    } else if (pr->rowType == CODE_INSTRUCTION) {
        printf("So dealing with a code instruction\n");
        getCodeOperands(inputRow, pr);
    } else if (pr->rowType == EXTERNAL_DECLARATION) {
        printf("Dealing with external declaration!\n");
        strcpy(pr->rowMetadata.externRowMetadata.labelName, inputRow);
    } else if (pr->rowType == ENTRY_DECLARATION) {
        printf("Dealing with an entry declaration!\n");
        strcpy(pr->rowMetadata.entryRowMetadata.labelName, inputRow);
    } else {
        printf("This should be an error...\n");
    }
}

void initParsedRowList(parsedRowList *prList) {
    printf("inside initParsedRowList\n");
    prList->head = NULL;
    prList->parsedRowsCounter = 0;
}

void addParsedRowToList(parsedRowList *prList, parsedRow *pr) {
    parsedRowNode *newNode = (parsedRowNode*)malloc(sizeof(parsedRowNode));
    parsedRowNode *temp = (parsedRowNode*)malloc(sizeof(parsedRowNode));

    /* Initialize the new node */
    newNode->pr = *pr;
    newNode->next = NULL;

    if (prList->parsedRowsCounter == 0) { /* Handle empty list */
        prList->head = newNode;
        prList->parsedRowsCounter += 1;
        return;
    }

    temp = prList->head;

    while (temp->next) { /* Go to end of list */
        temp = temp->next;
    }

    temp->next = newNode;
    prList->parsedRowsCounter += 1;
}

void printParsedRowsList(parsedRowList *prList) {
    int i;
    parsedRowNode *temp = (parsedRowNode*)malloc(sizeof(parsedRowNode)); /* TODO - Do I need this? or can I just do parsedRowNode *temp = prList->head */
    temp = prList->head;

    printf("Parsed Rows list has has %d rows\n", prList->parsedRowsCounter);
    for (i = 0; i < prList->parsedRowsCounter; temp = temp->next, i++) {
        printf("Row #%d is of type %d\n", temp->pr.originalLineNum, temp->pr.rowType);
    }
}
