#ifndef ERRORS_H
#define ERRORS_H
#include "dataStructures.h"

#define RED_PRINT          "\033[31m"           /* Red format*/
#define GREEN_PRINT        "\033[32m"           /* Green format*/
#define RESET_PRINT        "\033[0m"            /* Normal string format print */
#define BOLD_RED_PRINT     "\033[1m\033[31m"    /* Bold Red format */
#define BOLD_WHITE_PRINT   "\033[1m\033[37m"    /* Bold White */
#define BOLD_GREEN_PRINT   "\033[1m\033[32m"    /* Bold green */

void printMissingFilesError();
void printParserError(parsedRow *pr);
void printGeneralFileError(char *fileName);
void printFailedOpeningFile(char *fileName);
void printEntryDoesntExist(parsedRow *pr);
#endif