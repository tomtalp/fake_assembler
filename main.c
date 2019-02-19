#include <stdio.h>
#include "parser.h"
#include "assembler.h"

int main() {
    symbolTable symbTable;
    dataDefinitionsTables dataTable;
    codeInstructionsTable codeTable;

    initSymbolTable(&symbTable);

    firstIteration(&symbTable, &dataTable, &codeTable);
}