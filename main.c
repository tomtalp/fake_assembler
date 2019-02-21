#include <stdio.h>
#include "parser.h"
#include "assembler.h"


int main() {
    symbolTable symbTable;
    dataDefinitionsTables dataTable;
    codeInstructionsTable codeTable;

    initSymbolTable(&symbTable);

    printf("Data table before first iteration - \n");
    printDataTable(&dataTable);

    firstIteration(&symbTable, &dataTable, &codeTable);

    printf("Data table after first iteration - \n");
    printDataTable(&dataTable);

    printf("Symbol table after first iteration - \n");
    printSymbolTable(&symbTable);

    // char s1[MAX_KEYWORD_BINARY_LENGTH];
    // printf("s1 when starting = %s\n", s1);
    // intToBinaryString(10, s1);
    // printf("s1 = %s \n", s1);
}