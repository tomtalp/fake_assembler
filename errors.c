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
        case DATA_STRING_DECLARATION_MISSING_QUOTES:
            printf(BOLD_WHITE_PRINT "String data declarations must be wrapped in quotes!\n");
            break;
        case EXTRANEOUS_TEXT_AFTER_STRING_QUOTES:
            printf(BOLD_WHITE_PRINT "Extraneous text after quotes\n");
            break;

        case ILLEGAL_COMMA_IN_DATA_DECLARATION:
            printf(BOLD_WHITE_PRINT "Detected illegal comma in data declaration - must include numbers separated by a single comma\n");
            break;
        case ILLEGAL_POSITIVITY_SIGN_IN_DATA_DECLARATION:
            printf(BOLD_WHITE_PRINT "Illegal sign for number - each number can only have one '+' or '-' sign!\n");
            break;
        case ILLEGAL_DATA_DECLARATION_CHARACTER:
            printf(BOLD_WHITE_PRINT "Illegal character for number - can only contain numeric characters, separated by a comma!\n");
            break;
        case ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA:
            printf(BOLD_WHITE_PRINT "Data declaration can't end with a comma! (a comma always has to preceed a number)\n");
            break;
        default:
            printf("Something else\n");
            break;
    } 
    printf(RESET_PRINT);
}
