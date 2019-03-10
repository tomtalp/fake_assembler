#ifndef ERRORS_H
#define ERRORS_H
#include "dataStructures.h"

#define RED_PRINT          "\033[31m"           /* Red format*/
#define GREEN_PRINT        "\033[32m"           /* Green format*/
#define RESET_PRINT        "\033[0m"            /* Normal string format print */
#define BOLD_RED_PRINT     "\033[1m\033[31m"    /* Bold Red format */
#define BOLD_WHITE_PRINT   "\033[1m\033[37m"    /* Bold White */
#define BOLD_GREEN_PRINT   "\033[1m\033[32m"    /* Bold green */

/*
    Print an error for missing files
*/
void printMissingFilesError();

/*
    Print a general file error (unknown error)
    
    @param char *fileName - String name of the file
*/
void printGeneralFileError(char *fileName);

/*
    Print an error if entry record doesn't exist

    @param parsedRow *pr - Pointer to the parsed row object
*/
void printEntryDoesntExist(parsedRow *pr);

/*
    Print an error for a symbol that was used by doesn't exist

    @param char *symbolName - String name of the symbol
*/
void printSymbolDoesntExist(char *symbolName);

/*
    Print an error from the parser

    @param parsedRow *pr - Pointer to the parsed row that caused the error
*/
void printParserError(parsedRow *pr);

/*
    Print an error when we failed opening a file

    @param char *fileName - The name of the file that caused the error
*/
void printFailedOpeningFile(char *fileName);
#endif