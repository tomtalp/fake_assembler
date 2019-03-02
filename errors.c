#include <stdio.h>
#include "parser.h"

void printParserError(parsedRow *pr) {
    printf(BOLD_RED_PRINT "Error %s:%d - ", pr->fileName, pr->originalLineNum);
    printf(RESET_PRINT);
    switch (pr->errorType)
    {
        case INVALID_OPCODE_ERROR:
            printf(BOLD_WHITE_PRINT "Op code '%s' doesn't exist!\n", 
                    pr->rowMetadata.codeRowMetadata.oc.opCodeName);
            break;
        case INVALID_DATA_DEF_TYPE:

            printf(BOLD_WHITE_PRINT "Data declaration type '%s' doesn't exist!\n", pr->rowMetadata.dataRowMetadata.rawData);
            break;
        case SYMBOL_STARTS_WITH_NON_ALPHA:
            printf(BOLD_WHITE_PRINT "Label must start with an alphabet character (A-z). Received '%s'\n",
                    pr->symbolName);
            break;
        case SYMBOL_CONTAINS_INVALID_CHAR:
            printf(BOLD_WHITE_PRINT "Label contains illegal characters (can't contain spaces or non-alphanumeric). Received '%s'\n",
                        pr->symbolName);
            break;

        default:
            printf("Something else\n");
            break;
    } 
    printf(RESET_PRINT);
}
