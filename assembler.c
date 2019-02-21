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

    if (pr->hasSymbol) {
        addNodeToSymbolTable(symbTable, pr->symbolName , dataTable->dataCounter, 0, 0);
    }

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

        dataTable->rows[dataTable->dataCounter] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));
        castIntToBinaryString(num, dataTable->rows[dataTable->dataCounter]);
        
        dataTable->dataCounter++;
    }
}

void addStringTypeToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    printf("Adding string data. Raw data = %s\n", pr->rowMetadata.dataRowMetadata.rawData);

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
    FILE *fp;
    char *path = "./test.as";
    char inputRow[MAX_INSTRUCTION_LENGTH];
    int rowNum = 0;

    fp = fopen(path, "r");

    if (fp == NULL) {
        printf("Failed opening file '%s'\n", path);
    } else {
        while(fgets(inputRow, MAX_INSTRUCTION_LENGTH, fp) != NULL) {
            rowNum++;
            if (isEmptyRow(inputRow)) {
                continue;
            }
            printf("Got row '%s'\n", inputRow);
            parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
            parseRow(inputRow, pr, rowNum);
            printf("Done parsing row %d\n", rowNum);

            if (!pr->isValidRow) {
                printf("Row number #%d has errors!\n", pr->originalLineNum);
            } else {
                if (pr->rowType == DATA_DECLARATION) {
                    addToDataTable(symbTable, dataTable, pr);
                }
            }

        }
        fclose(fp);
    }
 
}

// void firstIteration(symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable) {
//     char inputRow[] = "XYZ: .data 5, 3, 1";

//     parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
//     parseRow(inputRow, pr, 1);
//     printf("Done parsing row 1\n");

//     if (!pr->isValidRow) {
//         printf("Row number #%d has errors!\n", pr->originalLineNum);
//     } else {
//         if (pr->rowType == DATA_DECLARATION) {
//             addToDataTable(symbTable, dataTable, pr);
//         }
//     }

//     char inputRow2[] = "N1: .data 25";

//     parsedRow *pr2 = (parsedRow*)malloc(sizeof(parsedRow));
//     parseRow(inputRow2, pr2, 2);

//     if (!pr2->isValidRow) {
//         printf("Row number #%d has errors!\n", pr2->originalLineNum);
//     } else {
//         if (pr2->rowType == DATA_DECLARATION) {
//             addToDataTable(symbTable, dataTable, pr2);
//         }
//     }

//     char inputRow3[] = "N2: .data 10";

//     parsedRow *pr3 = (parsedRow*)malloc(sizeof(parsedRow));
//     parseRow(inputRow3, pr3, 3);

//     if (!pr3->isValidRow) {
//         printf("Row number #%d has errors!\n", pr3->originalLineNum);
//     } else {
//         if (pr3->rowType == DATA_DECLARATION) {
//             addToDataTable(symbTable, dataTable, pr3);
//         }
//     }

// }