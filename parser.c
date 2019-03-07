#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "parser.h"

// #include "errors.h"

char *LEGAL_REGISTERS[REGISTERS_COUNT] = {
    "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"
};

dataTypeInfo LEGAL_DATA_DECLARATIONS[] = {
    {".data", DATA_TYPE},
    {".string", STRING_TYPE}
};

    // NO_OPERAND = 0,
    // IMMEDIATE_MODE = 1,
    // DIRECT_MODE = 3,
    // REGISTER_MODE = 5


opCode LEGAL_OP_CODES[OP_CODES_COUNT] = {
    {"mov", MOV},
    {"cmp", CMP},
    {"add", ADD},
    {"sub", SUB},
    {"not", NOT},
    {"clr", CLR},
    {"lea", LEA},
    {"inc", INC},
    {"dec", DEC},
    {"jmp", JMP},
    {"bne", BNE},
    {"red", RED},
    {"prn", PRN},
    {"jsr", JSR},
    {"rts", RTS},
    {"stop", STOP}
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

/*
    Evaluate the type of the two operands from the command

    @param parsedRow *pr - A pointer to the parsedRow object, which we'll use to set the operand types
    @param char *firstOperand - The first operand we extracted from the command
    @param char *secondOperand - The second operand we extracted from the command
*/
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

/*
    Receive the raw input row after extracting the operation code, and get the operands

    @param *parsedRow - Parsed Row object pointer
    @param char *inputRow - The string representing the operation arguments

*/
void getCodeOperands(char *inputRow, parsedRow *pr) {
    int i;
    int detectedComma = 0;
    int detectedTextAfterComma = 0;
    char *inputRowStart = inputRow;
    char firstOperand[MAX_SYMBOL_NAME_LENGTH];
    char secondOperand[MAX_SYMBOL_NAME_LENGTH];


    /* Locate the first operand in the command */
    i = 0;
    while (*inputRowStart != 0 && !isspace(*inputRowStart) && *inputRowStart != ',') {
        firstOperand[i] = *inputRowStart;
        i++;
        inputRowStart++;
    }
    
    firstOperand[i] = '\0';

    trimLeadingWhitespace(inputRowStart);

    if (*inputRowStart == ',') {
        detectedComma = 1;
        inputRowStart++;
    } else if (*inputRowStart != '\0') {
        pr->errorType = MISSING_COMMA_BETWEEN_OPERANDS;
        return;
    }
    
    trimLeadingWhitespace(inputRowStart);

    /* Locate the second operand, if it exists in the command */
    i = 0;
    while (*inputRowStart != 0 && !isspace(*inputRowStart)) {
        if (*inputRowStart == ',' && detectedComma) {
            pr->errorType = MULTIPLE_COMMAS_BETWEEN_OPERANDS;
            return;
        }
        detectedTextAfterComma = 1;
        secondOperand[i] = *inputRowStart;
        i++;
        inputRowStart++;
    }

    secondOperand[i] = '\0';  /* Terminate the second operand */

    if (detectedComma && !detectedTextAfterComma) {
        pr->errorType = NO_TEXT_AFTER_COMMA;
        return;
    }

    trimLeadingWhitespace(inputRowStart);

    if (*inputRowStart != '\0') { /* If the string isn't finished after we trimmed whitespaces, then there's extra text after the 2nd operand*/
        pr->errorType = EXTRANEOS_TEXT_AFTER_OPERANDS;
        return;
    }

    

    

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
    int isEmptyString = 1;

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
        } else {
            isEmptyString = 0;
        }
        rawData++;
    }

    if (!hasClosingQuotes) {
        pr->errorType = DATA_STRING_DECLARATION_MISSING_QUOTES;
        return;
    }

    if (isEmptyString) {
        pr->errorType = ILLEGAL_DATA_DECLARATION_EMPTY_DATA;
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
    int positivitySignFlag = 0;

    if (*rawData == '\0') {
        pr->errorType = ILLEGAL_DATA_DECLARATION_EMPTY_DATA;
        return;
    } else if (*rawData == ',') {
        pr->errorType = ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA;
        return;
    } else if (!isdigit(*rawData) && !(*rawData == '-'  || *rawData == '+' )) {
        pr->errorType = ILLEGAL_DATA_DECLARATION_CHARACTER;
        return;
    }

    rawData++;

    while (*rawData != '\0' && *rawData != '\n') {
        if (*rawData == ',') {
            if (commaFlag || (!isspace(*(rawData-1)) && !isdigit(*(rawData-1)))) { /* Check if consecutive commas, or if neighbours aren't a digit or a space*/
                pr->errorType = ILLEGAL_COMMA_IN_DATA_DECLARATION;
                return;
            }
            commaFlag = 1;
            isNumberFlag = 0;
            positivitySignFlag = 0;
        } else if ((*rawData == '-'  || *rawData == '+' )) {
            if (isdigit(*(rawData-1)) || !isdigit(*(rawData+1)) || positivitySignFlag) { /* After a sign we must see a number, and we can't have multiple signs for a number*/
                pr->errorType = ILLEGAL_POSITIVITY_SIGN_IN_DATA_DECLARATION;
                return;
            }
            positivitySignFlag = 1;
        } else if (isdigit(*rawData)) {
            commaFlag = 0;
        } else if (isspace(*rawData)) {
            if (isdigit(*(rawData-1)) && isdigit(*(rawData+1))) {
                pr->errorType = ILLEGAL_DATA_DECLARATION_EXTRANEOUS_SPACE;
                return;
            }
        } else {
            pr->errorType = ILLEGAL_DATA_DECLARATION_CHARACTER;
            return;
        }
        rawData++;
    }

    if (commaFlag || *rawData == ',') {
        pr->errorType = ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA;
        return;
    }
}

/* 
    Check that a parsedRow object has valid code operands, for their corresponding op code

    @param *parsedRow - Parsed Row object pointer
*/
void validateCodeOperands(parsedRow *pr) {
    int isValid;
    int opCodeNum = pr->rowMetadata.codeRowMetadata.oc.opCodeNum;

    if (opCodeNum == MOV || opCodeNum == ADD || opCodeNum == SUB) {
        isValid = (pr->rowMetadata.codeRowMetadata.srcOperandType == IMMEDIATE_MODE || pr->rowMetadata.codeRowMetadata.srcOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.srcOperandType == REGISTER_MODE)
                    &&
                (pr->rowMetadata.codeRowMetadata.destOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE);
    } else if (opCodeNum == CMP) {
        isValid = (pr->rowMetadata.codeRowMetadata.srcOperandType == IMMEDIATE_MODE || pr->rowMetadata.codeRowMetadata.srcOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.srcOperandType == REGISTER_MODE)
                    &&
                (pr->rowMetadata.codeRowMetadata.destOperandType == IMMEDIATE_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE);
    } else if (opCodeNum == NOT || opCodeNum == CLR || opCodeNum == INC || opCodeNum == DEC 
                || opCodeNum == JMP || opCodeNum == BNE || opCodeNum == RED || opCodeNum == JSR) {
        isValid = (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND)
                    &&
                (pr->rowMetadata.codeRowMetadata.destOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE);
    } else if (opCodeNum == LEA) {
        isValid = (pr->rowMetadata.codeRowMetadata.srcOperandType == DIRECT_MODE)
                    &&
                (pr->rowMetadata.codeRowMetadata.destOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE);
    } else if (opCodeNum == PRN) {
        isValid = (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND)
                    &&
                (pr->rowMetadata.codeRowMetadata.destOperandType == IMMEDIATE_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == DIRECT_MODE || pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE);
    } else {
        isValid = (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND)
                    &&
                (pr->rowMetadata.codeRowMetadata.destOperandType == NO_OPERAND);
    }

    if (!isValid) {
        pr->errorType = ILLEGAL_CODE_OPERANDS;
    }
}

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

        if (pr->errorType != NO_ERROR) {
                return;
        }

        printf("srcOperand = '%s', destOperand = '%s'\n", pr->rowMetadata.codeRowMetadata.srcOperand, pr->rowMetadata.codeRowMetadata.destOperand);
        validateCodeOperands(pr);
        if (pr->errorType != NO_ERROR) {
                return;
        }

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
