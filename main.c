#include <stdio.h>
#include "parser.h"
#include "assembler.h"
#include "errors.h"
#include "utils.h"

int run(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *rowsList) {
    int errorFlag;
    errorFlag = firstIteration(fileName, symbTable, dataTable, codeTable, rowsList);

    if (errorFlag) {
        return 1;
    }

    /* printf("Data table after first iteration - \n");
    printDataTable(dataTable);

    printf("Symbol table after first iteration - \n");
    printSymbolTable(symbTable);

    printf("Code table after first iteration - \n");
    printCodeTable(codeTable);

    printf("######################################\n");
    printParsedRowsList(rowsList);

    printf("######################################\n");
    printf("RELOC TABLE - \n");
    printRelocTable(symbTable->relocTable); */

    errorFlag = secondIteration(fileName, symbTable, dataTable, codeTable, rowsList);
    /*printf("Code table after second iteration - \n");
    printCodeTable(codeTable);*/

    return errorFlag;
}


int main(int argc, char *argv[]) {
    int i;
    parsedRowList rowsList;
    symbolTable symbTable;
    dataDefinitionsTable dataTable;
    codeInstructionsTable codeTable;
    int errorFlag;

    if (argc == 1) {
        printMissingFilesError();
        return 1;
    }

    for (i=1; i < argc; i++) {
        printf("Woring on '%s'\n", argv[i]);
        initSymbolTable(&symbTable);
        initParsedRowList(&rowsList);
        initDataTable(&dataTable);

        errorFlag = run(argv[i], &symbTable, &dataTable, &codeTable, &rowsList);

        if (errorFlag) {
            printGeneralFileError(argv[i]);
            continue;
        } else {
            dumpCode(&codeTable, argv[i], dataTable.dataCounter);
            dumpEntryExternData(&symbTable, argv[i]);
        }
        freeData(&symbTable, &dataTable, &codeTable, &rowsList);
        /* printSymbolTable(&symbTable); */
        /* printCodeTable(&codeTable); */
    }
    return errorFlag;
}