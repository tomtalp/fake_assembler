#ifndef PARSER_H
#define PARSER_H
#include "utils.h"
#include "dataStructures.h"
/*
    Check if an input row has a symbol definition (symbol = label) by looking for a ':'
    This doesn't mean it's a valid label name, we're validating that later

    @param char *inputRow - The raw row from the file
    @return int - A flag representing if we found a symbol or not. 0 = False, 1 = True
*/
int isSymbolDefinition(char *inputRow);

/*
    Extract the symbol name from the input row

    @param char *inputRow - The raw row from the file
    @param char *symbolName - The destination string for extract the symbol name into
*/
void getSymbolName(char *inputRow, char *symbolName);

/*
    Validated that the extracted symbol name is legal - no invalid characters, no spaces, starts with
    an alphabetic character

    @param parsedRow *pr - The parsedRow object ptr which has the symbol name
*/
void validateSymbolName(parsedRow *pr);

/*
    Get the data def type - .string or .data

    @param char *dataDef - The raw data definition from the file
    @return int - The numeric value we've assigned for the data def
*/
int getDataDefType(char *dataDef);

/*
    Receive an input row that was read from the file, and determine the row type - data declaration,
    code instruction, extern/entry

    @param char *inputRow - The raw string from the file
    @param parsedRow *pr - The parsedRow object pointer
*/
void getRowType(char *inputRow, parsedRow *pr);

/*
    Check if an operand is a valid register

    @param char *operand - string representing a registr
    @ return int - 0 for false, 1 for true
*/
int operandIsRegister(char *operand);

/*
    Check if an operand is a valid number, which might also contain a +/- sign

    @param char *operand - string representing a number
    @ return int - 0 for false, 1 for true
*/
int operandIsNumber(char *operand);

/*
    Receive an operand, determine it's type and put it in the parsedRow object

    @param parsedRow *pr - Pointer to the parsed row object we're working with
    @param char *operand - The raw operand name extracted from the raw file row
    @param int isSrcOperand - A flag indicating whether this is a source operand or not
*/
void setSingleOperandType(parsedRow *pr, char *operand, int isSrcOperand);

/*
    Evaluate the type of the two operands from the command

    @param parsedRow *pr - A pointer to the parsedRow object, which we'll use to set the operand types
    @param char *firstOperand - The first operand we extracted from the command
    @param char *secondOperand - The second operand we extracted from the command
*/
void getOperandTypes(parsedRow *pr, char *firstOperand, char *secondOperand);

/*
    Receive the raw input row after extracting the operation code, and get the operands

    @param *parsedRow - Parsed Row object pointer
    @param char *inputRow - The string representing the operation arguments

*/
void getCodeOperands(char *inputRow, parsedRow *pr);

/*
    Add raw data to a parsed row, after stripping quotes and ignoring redundant spaces

    @param parsedRow *pr - The parsed row we're adding to
    @param char *rawData - The string representing the raw data
*/
void addStringRawData(parsedRow *pr, char *rawData);

/* 
    Receive a string that's supposed to contain the data declaration, and make sure the string is valid.
    We check that the string is wrapped in " ", and doesn't have any text after " "

    @param *parsedRow - Parsed Row object pointer
    @param *rawData - String containing the expected data declaration

*/
void validateStringDataDeclaration(parsedRow *pr, char *rawData);

/*
    Receive a parsedRow object and the raw data, and extract the extern/entry symbol name.

    @param parsedRow *pr - Parsed Row object pointer
    @param char *rawData - String containing the expected external/entry declaration
*/
void getExternEntryDeclaration(parsedRow *pr, char *rawData);

/* 
    Receive a string that's supposed to represent an int data declaration, and make sure the string is valid.
    We check that the numbers are separated by commas, and that there are no illegal characters

    @param parsedRow *pr - Parsed Row object pointer
    @param char *rawData - String containing the expected data declaration

*/
void validateIntDataDeclaration(parsedRow *pr, char *rawData);

/* 
    Check that a parsedRow object has valid code operands, for their corresponding op code

    @param *parsedRow - Parsed Row object pointer
*/
void validateCodeOperands(parsedRow *pr);

/*
    Receive a raw input row, and turn into a parsed row object

    @param char *inputRow - The raw input row received from the file
    @param parsedRow *pr - The pointer to the parsed row object we're editing
    @param int rowNum - original row number from file
*/
void parseRow(char *inputRow, parsedRow *pr, int rowNum);

/*
    Initialized a parsedRow linked list

    @param parsedRowList *prList - The pointer to the list we're initializing
*/
void initParsedRowList(parsedRowList *prList);

/*
    Add a parsedRow object into the parsedRow linked list
    We're adding it in a sorted manner since the order of the rows is crucial

    @param parsedRowList *prList - Pointer to the list we're adding to
    @param parsedRow *pr - Pointer to the row we're adding
*/
void addParsedRowToList(parsedRowList *prList, parsedRow *pr);

#endif