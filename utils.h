#ifndef UTILS_H
#define UTILS_H

#include "dataStructures.h"

#define MAX_KEYWORD_BINARY_LENGTH 12
#define BASE_64_KEYWORD_SIZE 2
#define ASSEMBLY_FILE_EXTENSION ".as"
#define MAIN_OUTPUT_FILE_EXTENSION ".ob"
#define ENTRY_OUTPUT_FILE_EXTENSION ".ent"
#define EXTERN_OUTPUT_FILE_EXTENSION ".ext"

/*
    Trim all leading whitespaces in the received source string. This function modifies the original string!

    @param source (*char) - The string to be trimmed
*/
void trimLeadingWhitespace(char *source);

/* 
    Get the binary character for a number with a given mask.
    If the negativity number is on, we flip the results

    @param int mask - The mask we're comparing the number with
    @param int value - The original value, in it's absolute value, and +1 if previously negative
    @param int isNegative - Negativity flag
*/
char getBinaryChar(int mask, int value, int isNegative);

/* 
    Convert an integer into a binary string representation. 
    If the number is negative, we'll flip the bits and add +1

    @param int n - The value to be converted
    @param char *binaryStr - The string to store the results
    @param int sizeOfBinaryKeyword - The amount of bits we have available
*/
void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword);

/*
    Check if a row is a comment, and needs to be ignored

    @param char *inputRow - The raw row string from the file
    @return int - flag representing whether the first character of the row is ';'
*/
int isCommentRow(char *inputRow);

/*
    Convert a string to int, with atoi

    @param char *str - The string to convert
    @return int - The converted number
*/
int strToInt(char *str);

/*
    Create a binary string from our  memKeyword (The keyword holds main code instructions)
    We're converting based on bit fields - each member in the memKeyword has it's designated 
    bit fields, so we take them from the memKeyword and put them in the binary string

    @param memKeyword *mem - Pointer to the memKeyword which has the data we want to convert
    @param char *binaryStr - The destination for our binary string
*/
void memKeywordToBinaryString(memKeyword *mem, char *binaryStr);

/*
    Receive a base filename, and append a given extension to the output by concatenating.

    @param char *fileName - The base file name
    @param char *output - The string that will hold the final result
    @param char *extenson - The desired extension to append
*/
void appendExtensionToFilename(char *fileName, char *output, char *extension);

/* 
    Convert a string with binary data into a base64 keyword - Each half of the string's numeric
    value is converted into a base64 character

    @param char *keyword - The raw binary string
    @param char *base64Keyword - Destination for the converted base64 keyword    
*/
void binaryKeywordToBase64(char *keyword, char *base64Keyword);

/*
    Receive the finalized code table, and write it's content to a file in base 64 encoding.
    The file header needs to be the instructions count & the data count. 

    @param codeInstructionsTable *codeTable - The code table containing all the binary keywords
    @param char *baseFileName - The base filename which will contain the output
    @param int dataCount - The counter of the data declarations
*/
void dumpCode(codeInstructionsTable *codeTable, char *baseFileName, int dataCount);

/*
    Write the entry/extern symbols declared in our code.
    If there are no entry/extern symbols, no file will be created.

    @param symbolTable *symbTable - The symbol table containing the entry/extern declarations
    @param char *baseFileName - The base filename which will contain the output    
*/
void dumpEntryExternData(symbolTable *symbTable, char *baseFileName);

/*
    Go over all our data structures, and free the allocated memory

    @param symbolTable *symbTable - The symbols table
    @param dataDefinitionsTable *dataTable - The data table
    @param codeInstructionsTable *codeTable - The code instruction table
    @param parsedRowList *prList - The list of parsedRows
*/
void freeData(symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

/*
    Check if a row is empty (contains only whitespaces)

    @param char *inputRow - The row to check

    @return int - A flag representing if the string is only whitespaces or not (0=False/1=True)
*/
int isEmptyRow(char *inputRow);

#endif