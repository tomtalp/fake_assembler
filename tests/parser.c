#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../utils.h"
#include "../parser.h"
#include "../errors.h"

/*
 Utility function for nicely printing asserts
*/
void logAssert(char *functionName, char *input){
    printf(BOLD_GREEN_PRINT "       # Testing %s(\"%s\")\n", functionName, input);
    printf(RESET_PRINT);
}

/*
 Utility function for nicely printing assert results
*/
void logResult(char *functionName, char *input, int expectation){
    printf(GREEN_PRINT "           # Test passed for %s(\"%s\") == %d\n", functionName, input, expectation);
    printf(RESET_PRINT);
}

/*
    Test the validateIntDataDeclaration() data declaration (int inputs)
*/
void testDataDeclaration(parsedRow *pr, char *dataDecl, int expectedResult) {
    validateIntDataDeclaration(pr, dataDecl);
    logAssert("validateIntDataDeclaration", dataDecl);
    if (expectedResult == 0) {
        assert(pr->errorType == 0);
    } else {
        assert(pr->errorType != 0);
        printParserError(pr);
    }
    
    logResult("validateIntDataDeclaration", dataDecl, expectedResult);
    pr->errorType = NO_ERROR; /* reset pr state */
}

/*
    Test the validateStringDataDeclaration() function (string inputs)
*/
void testStringDeclaration(parsedRow *pr, char *dataDecl, int expectedResult) {
    validateStringDataDeclaration(pr, dataDecl);
    logAssert("validateStringDataDeclaration", dataDecl);
    if (expectedResult == 0) {
        assert(pr->errorType == 0);
    } else {
        assert(pr->errorType != 0);
        printParserError(pr);
    }
    
    logResult("validateStringDataDeclaration", dataDecl, expectedResult);
    pr->errorType = NO_ERROR; /* reset pr state */
}

/*
    Test validateSymbolName()
*/
void testValidateSymbolName(parsedRow *pr, char *symbName, int expectedResult) {
    strcpy(pr->symbolName, symbName);
    validateSymbolName(pr);
    logAssert("validateSymbolName", symbName);
    if (expectedResult == 0) {
        assert(pr->errorType == 0);
    } else {
        assert(pr->errorType != 0);
        printParserError(pr);
    }
    
    logResult("validateSymbolName", symbName, expectedResult);
    pr->errorType = NO_ERROR; /* reset pr state */
    strcpy(pr->symbolName, "");
}

/*
    Test getCodeOperands()
*/
void testGetCodeOperands(parsedRow *pr, char *instructionArgs, int expectedResult) {
    char instructionArgsCopy[MAX_INSTRUCTION_LENGTH];
    strcpy(instructionArgsCopy, instructionArgs);
    getCodeOperands(instructionArgs, pr);

    logAssert("getCodeOperands", instructionArgsCopy);
    if (expectedResult == 0) {
        assert(pr->errorType == 0);
    } else {
        assert(pr->errorType != 0);
        printParserError(pr);
    }
    
    logResult("validateSymbolName", instructionArgsCopy, expectedResult);
    pr->errorType = NO_ERROR; /* reset pr state */
}

/*
    Test getExternEntryDeclaration()
*/
void testGetExternEntryDeclaration(parsedRow *pr, char *instructionArgs, int expectedResult) {
    char instructionArgsCopy[MAX_INSTRUCTION_LENGTH];
    strcpy(instructionArgsCopy, instructionArgs);

    getExternEntryDeclaration(pr, instructionArgs);

    logAssert("getExternDeclaration", instructionArgsCopy);
    if (expectedResult == 0) {
        assert(pr->errorType == 0);
    } else {
        assert(pr->errorType != 0);
        printParserError(pr);
    }
    
    logResult("getExternDeclaration", instructionArgsCopy, expectedResult);
    pr->errorType = NO_ERROR; /* reset pr state */
}

int main() {
    printf(BOLD_GREEN_PRINT "######## Fake assembler tests ######## \n");

    parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
    pr->errorType = NO_ERROR;

    printf(BOLD_GREEN_PRINT "   ### 1. validateIntDataDeclaration() \n");
    printf(RESET_PRINT);

    testDataDeclaration(pr, "1, 2, 3", 0);
    testDataDeclaration(pr, ",", 1);
    testDataDeclaration(pr, "1,1", 0);
    testDataDeclaration(pr, "1,1,", 1);
    testDataDeclaration(pr, "1,2,-3", 0);
    testDataDeclaration(pr, "1,2,-3, 4", 0);
    testDataDeclaration(pr, "1,2,-3, +4", 0);
    testDataDeclaration(pr, "1,2,-3, +4+", 1);
    testDataDeclaration(pr, "1 3 5", 1);
    testDataDeclaration(pr, "1 ,        3", 0);
    testDataDeclaration(pr, "1 ,        3   ", 0);
    testDataDeclaration(pr, "1, 3, 5,   7 a", 1);
    testDataDeclaration(pr, "tom", 1);

    printf(BOLD_GREEN_PRINT "   ### 2. validateStringDataDeclaration() \n");
    printf(RESET_PRINT);

    testStringDeclaration(pr, "hello", 1);
    testStringDeclaration(pr, "\"hello\"", 0);
    testStringDeclaration(pr, "\"hello asdasd", 1);
    testStringDeclaration(pr, "\" \"", 0);
    testStringDeclaration(pr, "\"\"", 1);
    testStringDeclaration(pr, "\" my name is tom \"", 0);

    printf(BOLD_GREEN_PRINT "   ### 3. validateSymbolName() \n");
    printf(RESET_PRINT);

    testValidateSymbolName(pr, "HELLO", 0);
    testValidateSymbolName(pr, "1TEST", 1);
    testValidateSymbolName(pr, "hEllO123", 0);
    testValidateSymbolName(pr, "H0l@", 1);
    testValidateSymbolName(pr, "12345", 1);
    testValidateSymbolName(pr, "label name", 1);

    printf(BOLD_GREEN_PRINT "   ### 4. testGetCodeOperands() \n");
    printf(RESET_PRINT);

    char instruction[MAX_INSTRUCTION_LENGTH] = "@r1, @r2";
    testGetCodeOperands(pr, instruction, 0);

    strcpy(instruction, "@r3  ,   @r5   ");
    testGetCodeOperands(pr, instruction, 0);

    strcpy(instruction, "@r3  ,   ");
    testGetCodeOperands(pr, instruction, 1);

    strcpy(instruction, "@r3  ,  @r2, ");
    testGetCodeOperands(pr, instruction, 1);

    strcpy(instruction, "@r3  , , @r2 ");
    testGetCodeOperands(pr, instruction, 1);

    strcpy(instruction, "@r3  @r2 ");
    testGetCodeOperands(pr, instruction, 1);

    strcpy(instruction, "@r1 ");
    testGetCodeOperands(pr, instruction, 0);

    strcpy(instruction, "");
    testGetCodeOperands(pr, instruction, 0);

    strcpy(instruction, "@r1, @r2 @r3");
    testGetCodeOperands(pr, instruction, 1);

    printf(BOLD_GREEN_PRINT "   ### 5. testGetExternEntryDeclaration() \n");
    printf(RESET_PRINT);

    char externVarName[MAX_INSTRUCTION_LENGTH] = "ABC";
    testGetExternEntryDeclaration(pr, externVarName, 0);

    strcpy(externVarName, "ABC DEFG");
    testGetExternEntryDeclaration(pr, externVarName, 1);
    
    strcpy(externVarName, "ABC          ");
    testGetExternEntryDeclaration(pr, externVarName, 0);

    printf(BOLD_GREEN_PRINT "##### ALL TESTS PASSED! ##### \n");
    printf(RESET_PRINT);
}