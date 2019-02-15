#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DEFINITIONS_COUNT 2

enum rowTypes {
    CODE_INSTRUCTION,
    DATA_DEFINITION,
    EXTERNAL_DECLARATION,
    ENTRY_DECLARATION
};

enum dataDefType {
    DATA_TYPE,
    STRING_TYPE
};

struct CODE_INSTRUCTION_ROW_METADATA {

};

struct DATA_DEFINITION_ROW_METADATA {
    enum dataDefType type;
};

typedef struct parsedRow {
    int originalLineNum;
    int isValidRow;
    int hasSymbol;
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    enum rowTypes rowType;
    union {
        struct CODE_INSTRUCTION_ROW_METADATA *codeRowMetadata;
        struct DATA_DEFINITION_ROW_METADATA *dataRowMetadata; 
    } *rowMetadata;
} parsedRow;

char *reservedKeywords[RESERVED_KEYWORDS_COUNT] = {
    "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "mov", "cmp", "add", "sub", "not", "clr", "lea",
    "inc", "dec", "jmp", "bne", "red", "prn", "jsr",
    "rts", "stop", "entry", "extern", "string", "data"
};

char *dataDefinitionKeywords[DATA_DEFINITIONS_COUNT] = {
    ".data", ".string"
};

void printParsedRow(parsedRow *pr) {
    if (pr->hasSymbol) {
        printf("Row has a symbol, called '%s' \n", pr->symbolName);
    }
    printf("Parsed row type = %d\n", pr->rowType);
    pr->rowMetadata->dataRowMetadata->type;
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

    while (*inputRowStart != 0 && !isspace(*inputRowStart)) {
        if (i >= MAX_RESERVED_KEYWORD_SIZE) {
            printf("Whoopsie, first keyword is longer than the max reserved keyword. HANDLE THISSSS\n");
            return;
        }
        firstKeyword[i++] = *inputRowStart;
        inputRowStart++;
    }
    firstKeyword[i] = '\0'; // Terminate the first keyword

    // What row type do we have?!
    for (i = 0; i < DATA_DEFINITIONS_COUNT; i++) {
        if (strcmp(firstKeyword, dataDefinitionKeywords[i]) == 0) {
            pr->rowType = DATA_DEFINITION;
            // pr->rowMetadata->dataRowMetadata = (DATA_DEFINITION_ROW_METADATA*)malloc(sizeof(DATA_DEFINITION_ROW_METADATA));
        }
    }

    printf("Initial detected keyword = '%s'\n", firstKeyword);

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

    parseRow(inputRow, pr);
    printf("\n\n\n");
    printParsedRow(pr);
}