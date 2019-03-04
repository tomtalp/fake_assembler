#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"
// #include "errors.h"

char *LEGAL_REGISTERS[REGISTERS_COUNT] = {
    "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"
};

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

void validateSymbolName(parsedRow *pr) {
    char *temp = pr->symbolName;
    if (!isalpha(*(pr->symbolName))) {
        pr->errorType = SYMBOL_STARTS_WITH_NON_ALPHA;
        return;
    }
    
    while (*temp != '\0') {
        if (!isalnum(*temp)) {
            pr->errorType = SYMBOL_CONTAINS_INVALID_CHAR;
            return;
        }
        temp++;
    }
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
    firstKeyword[i] = '\0'; /* Terminate the first keyword */

    /* Remove the first keyword from inputRow */
    while (*inputRowStart != 0) {
        *inputRow = *inputRowStart;
        inputRow++;
        inputRowStart++; 
    }
    *inputRow = '\0';

    printf("Initial detected keyword = '%s'\n", firstKeyword);

    /* Detect the row type - data decaration, code instruction, extern or entry */

    if (firstKeyword[0] == '.') { /* Expecting a data declaration, or extern/entry */ 
        for (i = 0; i < DATA_DECLARATION_TYPES_COUNT; i++) {
            if (strcmp(firstKeyword, LEGAL_DATA_DECLARATIONS[i].dataDefName) == 0) {
                pr->rowType = DATA_DECLARATION;
                detectedRowType = 1;
                pr->rowMetadata.dataRowMetadata.type = LEGAL_DATA_DECLARATIONS[i].dataDefCodeNum;
                break;
            }
        }

        if (!detectedRowType) { /* Not a data declaration, but perhaps extern/entry  */
            if (strcmp(firstKeyword, ".extern") == 0) {
                pr->rowType = EXTERNAL_DECLARATION;
                detectedRowType = 1;
                printf("Detected EXTERNAL DECLARATION\n");
            } else if (strcmp(firstKeyword, ".entry") == 0) {
                pr->rowType = ENTRY_DECLARATION;
                detectedRowType = 1;
                printf("Detected ENTRY DECLARATION\n");
            } else {
                strcpy(pr->rowMetadata.dataRowMetadata.rawData, firstKeyword); /* Stick the faulty data definition in the raw data so we can print it later*/
            pr->errorType = INVALID_DATA_DEF_TYPE;
            return;
            }
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

    if (!detectedRowType) { /* Undetected row up to here means invalid operation code*/
        /* Put the fauly op code in the parsed row object so we can print it later*/
        pr->rowMetadata.codeRowMetadata.oc.opCodeName = (char *)malloc(MAX_RESERVED_KEYWORD_SIZE * sizeof(char));
        strcpy(pr->rowMetadata.codeRowMetadata.oc.opCodeName, firstKeyword);
        pr->errorType = INVALID_OPCODE_ERROR;
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
        if (*rawData != '"') {
            strippedData[i] = *rawData;
            i++;
        }
        rawData++;
    }

    printf("String after stripping = '%s'\n", strippedData);
    strcpy(pr->rowMetadata.dataRowMetadata.rawData, strippedData);
}

/* 
    Receive a string that's supposed to contain the data declaration, and make sure the string is valid.
    We check that the string is wrapped in " ", and doesn't have any text after " "

    @param *parsedRow - Parsed Row object pointer
    @param *rawData - String containing the expected data declaration

*/
void validateStringDataDeclaration(parsedRow *pr, char *rawData) {
    int hasClosingQuotes = 0;

    if (*rawData != '"') { /* String declaration must start with a quote */
        pr->errorType = DATA_STRING_DECLARATION_MISSING_QUOTES;
        return;
    }
    rawData++;

    while (*rawData != '\0' && *rawData != '\n') {
        if (hasClosingQuotes && !isspace(*rawData)) { /* If we already detected closing quotes, we're not supposed to see additional text*/
            pr->errorType = EXTRANEOUS_TEXT_AFTER_STRING_QUOTES;
            return;
        }

        if (*rawData == '"') {
            hasClosingQuotes = 1;
        }
        rawData++;
    }

    if (!hasClosingQuotes) {
        pr->errorType = DATA_STRING_DECLARATION_MISSING_QUOTES;
        return;
    }
}

/* 
    Receive a string that's supposed to represent an int data declaration, and make sure the string is valid.
    We check that the numbers are separated by commas, and that there are no illegal characters

    @param *parsedRow - Parsed Row object pointer
    @param *rawData - String containing the expected data declaration

*/
void validateIntDataDeclaration(parsedRow *pr, char *rawData) {
    int isNumberFlag = 0;
    int whiteSpaceFlag = 0;
    int commaFlag = 0;
    
    while (*rawData != '\0' && *rawData != '\n') {
        if (*rawData == ',') { /* Encountered a comma, we turn of the isNumberFlag */
            if (!isNumberFlag) {
                pr->errorType = ILLEGAL_COMMA_IN_DATA_DECLARATION;
                return;
            }
            /* handle flags = forget the numbers & commas and remember we saw a comma */
            isNumberFlag = 0;
            whiteSpaceFlag = 0;
            commaFlag = 1;
        } else if (isspace(*rawData)) {
            whiteSpaceFlag = 1;
        }
        else {
            if (isdigit(*rawData)) {
                if (whiteSpaceFlag && isNumberFlag) {
                    pr->errorType = ILLEGAL_DATA_DECLARATION_CHARACTER;
                    return;
                } else {
                    isNumberFlag = 1;
                    commaFlag = 0;
                }
            } else {
                pr->errorType = ILLEGAL_DATA_DECLARATION_CHARACTER;
                return;
            }
        }
        rawData++;
    }

    if (commaFlag) {
        pr->errorType = ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA;
        return;
    }

}
// void validateIntDataDeclaration(parsedRow *pr, char *rawData) {
//     int legalNumFlag = 0;
//     int positivitySignFlag = 0;
//     int legalNumAfterComma = 0;

//     while (*rawData != '\0' && *rawData != '\n') {
//         if (isspace(*rawData)) { /* A space is only legal after we detected a valid number, and before a comma */

//         } else if (*rawData == ',') { /* Encountered a comma, so the legalNumFlag needs to be true */
//             if (!legalNumFlag) {
//                 pr->errorType = ILLEGAL_COMMA_IN_DATA_DECLARATION;
//                 return;
//             }
//             /* Reset flag */
//             legalNumFlag = 0;
//             positivitySignFlag = 0;
//         } else {
//             if (*rawData == '-'  || *rawData == '+' ) {
//                 if (positivitySignFlag) { /* Already encountered a positivity sign */
//                     pr->errorType = ILLEGAL_POSITIVITY_SIGN_IN_DATA_DECLARATION;
//                     return;
//                 } else {
//                     positivitySignFlag = 1;
//                 }
//             } else {
//                 if (isdigit(*rawData)) {
//                     legalNumFlag = 1;
//                 } else {
//                     pr->errorType = ILLEGAL_DATA_DECLARATION_CHARACTER;
//                     return;
//                 }
//             }
//         }

//     rawData++;
//     }
// }

void parseRow(char *inputRow, parsedRow *pr, int rowNum) {

    printf("1. Parsing '%s' from row number #%d\n", inputRow, rowNum);
    pr->originalLineNum = rowNum;
    pr->errorType = NO_ERROR; // Assume valid row until proven otherwise
    trimLeadingWhitespace(inputRow);

    printf("2. After initial whitespace trimming - '%s'\n", inputRow);

    pr->hasSymbol = isSymbolDefinition(inputRow);
    if (pr->hasSymbol) {
        getSymbolName(inputRow, pr->symbolName);
        validateSymbolName(pr);
        trimLeadingWhitespace(inputRow);

        if (pr->errorType != NO_ERROR) {
            return;
        }
    }

    printf("3. Left with '%s' after checking symbol\n", inputRow);

    getRowType(inputRow, pr);
    if (pr->errorType != NO_ERROR) {
        return;
    }
    
    
    trimLeadingWhitespace(inputRow);
    printf("4. After getting row type, we're left with - '%s'\n", inputRow);

    if (pr->rowType == DATA_DECLARATION) {
        printf("So we're dealing with a data decl, lets get the data!\n");
        if (pr->rowMetadata.dataRowMetadata.type == STRING_TYPE) {
            validateStringDataDeclaration(pr, inputRow);

            if (pr->errorType != NO_ERROR) {
                return;
            }

            addStringRawData(pr, inputRow);
        } else {
            validateIntDataDeclaration(pr, inputRow);
            if (pr->errorType != NO_ERROR) {
                return;
            }
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
