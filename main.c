#include <stdio.h>
#include "parser.h"
#include "assembler.h"

int main() {
    parsedRowList rowsList;
    symbolTable symbTable;
    dataDefinitionsTables dataTable;
    codeInstructionsTable codeTable;

    initSymbolTable(&symbTable);
    initParsedRowList(&rowsList);

    printf("Data table before first iteration - \n");
    printDataTable(&dataTable);

    firstIteration(&symbTable, &dataTable, &codeTable, &rowsList);

    printf("Data table after first iteration - \n");
    printDataTable(&dataTable);

    printf("Symbol table after first iteration - \n");
    printSymbolTable(&symbTable);

    printf("Code table after first iteration - \n");
    printCodeTable(&codeTable);

    printf("######################################\n");
    printParsedRowsList(&rowsList);

}