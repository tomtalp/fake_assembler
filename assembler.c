#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "dataStructures.h"
#include "utils.h"

void addDataTypeToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    /* We're supposed to be dealing with a sequence of integers. We'll parse each one 
    and cast into an int, and insert into the data table */

    int num, i;
    char intAsBinaryString[MAX_KEYWORD_BINARY_LENGTH];

    i = 0;

    for (i = 0; i < MAX_INSTRUCTION_LENGTH, pr->rowMetadata.dataRowMetadata.rawData[i] != 0; i++) {
        num = 0;

        while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0 && !isspace(pr->rowMetadata.dataRowMetadata.rawData[i]) && pr->rowMetadata.dataRowMetadata.rawData[i] != ',') {
            num = num * 10;
            if (!isdigit(pr->rowMetadata.dataRowMetadata.rawData[i])) {
                printf("Woopsie, got a non digit in data type parsing! TODO HANDLE THIS\n");
            }
            num += pr->rowMetadata.dataRowMetadata.rawData[i] - '0'; // Add the character digit as an int
            i++;
        }
        i++; /* Skip the comma or space that just made us stop */
        castIntToBinaryString(num, intAsBinaryString);
        printf("Dected number = %d (as binary = '%s'\n", num, intAsBinaryString);
        
    }
}

void addStringTypeToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    printf("Adding string data\n");
}

void addToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    printf("Adding stuff to data table!\n");
    if (pr->rowMetadata.dataRowMetadata.type == DATA_TYPE) {
        addDataTypeToDataTable(symbTable, dataTable, pr);
    } else if (pr->rowMetadata.dataRowMetadata.type == STRING_TYPE) {
        addStringTypeToDataTable(symbTable, dataTable, pr);
    }
}

void firstIteration(symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable) {
    char inputRow[] = "XYZ: .data 5, 3, 1";

    parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
    parseRow(inputRow, pr, 1);

    if (!pr->isValidRow) {
        printf("Row number #%d has errors!\n", pr->originalLineNum);
    } else {
        if (pr->rowType == DATA_DECLARATION) {
            addToDataTable(symbTable, dataTable, pr);
            // pr->rowMetadata.dataRowMetadata.rawData;
        }
    }

}