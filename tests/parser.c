#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../utils.h"
#include "../parser.h"

#define RED_PRINT          "\033[31m"           /* Red format*/
#define RESET_PRINT        "\033[0m"            /* Normal string format print */
#define BOLD_RED_PRINT     "\033[1m\033[31m"    /* Bold Red format */
#define BOLD_WHITE_PRINT   "\033[1m\033[37m"    /* Bold White */
#define BOLD_GREEN_PRINT   "\033[1m\033[32m"    /* Bold green */

void logMessage(char *functionName, char *input, int expectation){
    printf(BOLD_GREEN_PRINT "       # Test passed for %s(\"%s\") == %d\n", functionName, input, expectation);
    printf(RESET_PRINT);
}

void testDataDeclaration(parsedRow *pr, char *dataDecl, int expectedResult) {
    validateIntDataDeclaration(pr, dataDecl);

    if (expectedResult == 0) {
        assert(pr->errorType == 0);
    } else {
        assert(pr->errorType != 0);
    }
    
    logMessage("validateIntDataDeclaration", dataDecl, expectedResult);
    pr->errorType = NO_ERROR; /* reset pr state */
}



int main() {
    printf(BOLD_GREEN_PRINT "######## Fake assembler tests ######## \n");
    printf(BOLD_GREEN_PRINT "   ### validateIntDataDeclaration() \n");
    printf(RESET_PRINT);

    parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
    pr->errorType = NO_ERROR;

    testDataDeclaration(pr, "1, 2, 3", 0);
    testDataDeclaration(pr, ",", 1);
    testDataDeclaration(pr, "1,1", 0);
    testDataDeclaration(pr, "1,1,", 1);
    testDataDeclaration(pr, "1,2,-3", 0);
    testDataDeclaration(pr, "1,2,-3, 4", 0);
}