#include <stdio.h>
#include "errors.h"

void printMissingFilesError() {
    printf(BOLD_RED_PRINT "Error - ");
    printf(RESET_PRINT);
    printf(BOLD_WHITE_PRINT " no files past as an argument!\n");
    printf(RESET_PRINT);
}

void printGeneralFileError(char *fileName) {
    printf(BOLD_RED_PRINT "Failed parsing '%s'\n", fileName);
    printf(RESET_PRINT);
}

void printFailedOpeningFile(char *fileName) {
    printf(BOLD_RED_PRINT "'%s' - ", fileName);
    printf(RESET_PRINT);
    printf(BOLD_WHITE_PRINT " file doesn't exist!\n");
    printf(RESET_PRINT);
}

void printEntryDoesntExist(parsedRow *pr) {
    printf(BOLD_RED_PRINT "Error %s:%d - ", pr->fileName, pr->originalLineNum);
    printf(RESET_PRINT);
    printf(BOLD_WHITE_PRINT " symbol %s doesn't exist!\n", pr->rowMetadata.entryRowMetadata.labelName);
    printf(RESET_PRINT);
}

void printSymbolDoesntExist(char *symbolName) {
    printf(BOLD_RED_PRINT "Error - ");
    printf(RESET_PRINT);
    printf(BOLD_WHITE_PRINT " label %s was used, but no declaration found!\n", symbolName);
    printf(RESET_PRINT);
}

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
            printf(BOLD_WHITE_PRINT "Illegal sign for number - each number can only have one '+' or '-' sign, followed by a number!\n");
            break;
        case ILLEGAL_DATA_DECLARATION_CHARACTER:
            printf(BOLD_WHITE_PRINT "Illegal character for number - can only contain numeric characters, separated by a comma!\n");
            break;
        case ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA:
            printf(BOLD_WHITE_PRINT "Data declaration can't start or end with a comma! (a comma always has to between numbers)\n");
            break;
        case ILLEGAL_DATA_DECLARATION_EXTRANEOUS_SPACE:
            printf(BOLD_WHITE_PRINT "Extraneous whitespace between numbers!\n");
            break;    
        case ILLEGAL_DATA_DECLARATION_EMPTY_DATA:
            printf(BOLD_WHITE_PRINT "Data declaration can't be empty!\n");
            break;
        case ILLEGAL_CODE_OPERANDS:
            printf(BOLD_WHITE_PRINT "Operation code '%s' received illegal arguments!\n", pr->rowMetadata.codeRowMetadata.oc.opCodeName);
            break;
        case MISSING_COMMA_BETWEEN_OPERANDS:
            printf(BOLD_WHITE_PRINT "Operands must be separated between a comma!\n");
            break;
        case MULTIPLE_COMMAS_BETWEEN_OPERANDS:
            printf(BOLD_WHITE_PRINT "Detected more than one comma. Operations can have up to 2 arguments, separated by at most 1 comma\n");
            break;
        case NO_TEXT_AFTER_COMMA:
            printf(BOLD_WHITE_PRINT "Missing text after comma - expecting a second argument\n");
            break;
        case EXTRANEOS_TEXT_AFTER_OPERANDS:
            printf(BOLD_WHITE_PRINT "Extraneous text after second operand\n");
            break;
        case EXTRANEOUS_TEXT_AFTER_EXTERN_ENTRY:
            printf(BOLD_WHITE_PRINT ".entry / .extern declarations can only have a single symbol name\n");
            break;
        case DUPLICATE_SYMBOL_DECLARATION:
            printf(BOLD_WHITE_PRINT "Symbol '%s' already declared - a symbol can only be declared once!\n", pr->symbolName);
            break;
        case INVALID_REGISTER_NAME:
            printf(BOLD_WHITE_PRINT "Invalid register '%s'. Available registers are between @r0 and @r7 \n", pr->rowMetadata.codeRowMetadata.srcOperand);
            break;
        case UNKNOWN_ERROR:
            printf(BOLD_WHITE_PRINT "Encountered an unknown error...\n");
            break;
        default:
            printf(BOLD_WHITE_PRINT "Encountered an unknown error...\n");
            break;
    } 
    printf(RESET_PRINT);
}
