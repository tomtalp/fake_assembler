#include <stdio.h>
#include "parser.h"
#include "assembler.h"

// #define MAX_KEYWORD_BINARY_LENGTH 12

// void keywordToBase64(char *keyword) {
//     int i, j;
//     char firstKeyWordSlice[(MAX_KEYWORD_BINARY_LENGTH/2) + 1];
//     char secondKeyWordSlice[(MAX_KEYWORD_BINARY_LENGTH/2) + 1];

//     printf("MAX_KEYWORD_BINARY_LENGTH/2 = %d\n", MAX_KEYWORD_BINARY_LENGTH/2);

//     for (i = 0; i < MAX_KEYWORD_BINARY_LENGTH/2; i++) {
//         printf("adding %c to first\n", keyword[i]);
//         firstKeyWordSlice[i] = keyword[i];
//     }
//     firstKeyWordSlice[i] = '\0';

//     printf("Done with first - '%s'\n", firstKeyWordSlice);

//     for (j = 0; i < MAX_KEYWORD_BINARY_LENGTH; i++, j++) {
//         printf("adding %c to second\n", keyword[i]);
//         secondKeyWordSlice[j] = keyword[i];
//     }

//     secondKeyWordSlice[j] = '\0';

//     printf("keyword = '%s', firstKeyWordSlice = '%s', secondKeyWordSlice = '%s'\n", keyword, firstKeyWordSlice, secondKeyWordSlice);
// }



int main() {
    // keywordToBase64("123456789123");
    parsedRowList rowsList;
    symbolTable symbTable;
    dataDefinitionsTables dataTable;
    codeInstructionsTable codeTable;
    int errorFlag;

    initSymbolTable(&symbTable);
    initParsedRowList(&rowsList);

    printf("Data table before first iteration - \n");
    printDataTable(&dataTable);

    errorFlag = firstIteration(&symbTable, &dataTable, &codeTable, &rowsList);
    if (errorFlag) {
        printf("FAILED!!!\n");
    } else {
        printf("WOOHOO\n");
    }
    // printf("Data table after first iteration - \n");
    // printDataTable(&dataTable);

    // printf("Symbol table after first iteration - \n");
    // printSymbolTable(&symbTable);

    // printf("Code table after first iteration - \n");
    // printCodeTable(&codeTable);

    // printf("######################################\n");
    // printParsedRowsList(&rowsList);

    // printf("######################################\n");
    // printf("RELOC TABLE - \n");
    // printRelocTable(symbTable.relocTable);

    // secondIteration(&symbTable, &dataTable, &codeTable, &rowsList);
    // printf("Code table after first iteration - \n");
    // printCodeTable(&codeTable);

}