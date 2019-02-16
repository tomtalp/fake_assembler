#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

char *reservedKeywords[RESERVED_KEYWORDS_COUNT] = {
    "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "mov", "cmp", "add", "sub", "not", "clr", "lea",
    "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    "rts", "stop", "entry", "extern", "string", "data"
};

dataTypeInfo LEGAL_DATA_DECLARATIONS[] = {
    {".data", DATA_TYPE},
    {".string", STRING_TYPE}
};

opCode LEGAL_OP_CODES[OP_CODES_COUNT] = {
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
    } else if (pr->rowType == CODE_INSTRUCTION) {
        printf("Parsed row is of type 'code instruction', instruction type %s with num %d \n", pr->rowMetadata.codeRowMetadata.oc.opCodeName, pr->rowMetadata.codeRowMetadata.oc.opCodeNum);
    }

    printf("\n");
    
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

void parseRow(char *inputRow, parsedRow *pr) {
    printf("1. Parsing '%s'\n", inputRow);
    
    trimLeadingWhitespace(inputRow);

    printf("2. After initial whitespace trimming - '%s'\n", inputRow);

    pr->hasSymbol = isSymbolDefinition(inputRow);
    if (pr->hasSymbol) {
        getSymbolName(inputRow, pr->symbolName);
        trimLeadingWhitespace(inputRow);
    }

    printf("3. Left with '%s' after checking symbol\n", inputRow);

    getRowType(inputRow, pr);
}


int main() {
    char inputRow[] = "XYZ: .data 5, 3, 1";
    parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
    // printf("Parsed row before - \n");
    // printParsedRow(pr);
    parseRow(inputRow, pr);
    printf("\n\n\n");
    printf("Parsed row AFTER - \n");
    printParsedRow(pr);
}