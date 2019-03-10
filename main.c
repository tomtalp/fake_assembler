#include <stdio.h>
#include "parser.h"
#include "assembler.h"
#include "errors.h"
#include "utils.h"

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
        printf(BOLD_WHITE_PRINT "Woring on '%s'\n", argv[i]);
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
        printf(BOLD_GREEN_PRINT "Successfully compiled '%s'\n", argv[i]);
    }
    return errorFlag;
}