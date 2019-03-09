#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include <stdlib.h>
#include <string.h>

static const char base64Constants[] = { 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

/*
    Trim all leading whitespaces in the received source string. This function modifies the original string!

    @param source (*char) - The string to be trimmed
*/
void trimLeadingWhitespace(char *source) {
    char *start = source;
    
    while (isspace(*start)) {
        start++;
    }

    while (*start != 0) {
        *source = *start;
        source++;
        start++;
    }   
    *source = '\0'; /* Terminate the string */
}

/* 
    Get the binary character for a number with a given mask.
    If the negativity number is on, we flip the results

    @param int mask - The mask we're comparing the number with
    @param int value - The original value, in it's absolute value, and +1 if previously negative
    @param int isNegative - Negativity flag
*/
char getBinaryChar(int mask, int value, int isNegative) {
    if (isNegative) { /* Flip the bit of the results */
        if ((mask & value) == 0) {
            return '1';
        } else {
            return '0';
        }
    } else {
        if ((mask & value) == 0) {
            return '0';
        }
    }
    return '1';
    
}

/* 
    Convert an integer into a binary string representation. 
    If the number is negative, we'll flip the bits and add +1

    @param int n - The value to be converted
    @param char *binaryStr - The string to store the results
    @param int sizeOfBinaryKeyword - The amount of bits we have available
*/
void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword) {
    char *copy = binaryStr;
    int isNegative = n < 0 ? 1 : 0;
    int mask, i;

    i = 0;

    if (isNegative) { /* Handle negative numbers - For two's compliment we need to flip the number and add +1. We'll add before flipping since it makes it easier (And the results are the same!) */
        n += 1;
        n *= -1;
    }
    mask = 1 << (sizeOfBinaryKeyword-1);
    while (mask) {
        *binaryStr = getBinaryChar(mask, n, isNegative);
        binaryStr++;
        mask >>= 1;
    }
    *(binaryStr) = '\0';
}

int isEmptyRow(char *inputRow) {
    while (*inputRow != '\0') {
        if (!isspace(*inputRow)) {
            return 0;
        }
        inputRow++;
    }
    return 1;
}

int isCommentRow(char *inputRow) {
    trimLeadingWhitespace(inputRow);
    return *inputRow == ';';
}

int strToInt(char *str) {
    return atoi(str);
}

void memKeywordToBinaryString(memKeyword *mem, char *binaryStr) {
    /* Convert source addressing mode */
    castIntToBinaryString(mem->sourceAddressingMode, binaryStr, KEYWORD_ADDRESSING_MODE_BITS);
    printf("mem->sourceAddressingMode = %d\n", mem->sourceAddressingMode);
    binaryStr = binaryStr + KEYWORD_ADDRESSING_MODE_BITS;

    /* Convert op code */
    castIntToBinaryString(mem->opCode, binaryStr, KEYWORD_OP_CODE_BITS);
    printf("mem->opCode = %d\n", mem->opCode);
    binaryStr = binaryStr + KEYWORD_OP_CODE_BITS;

    /* Convert dest addressing mode*/
    castIntToBinaryString(mem->destAddressingMode, binaryStr, KEYWORD_ADDRESSING_MODE_BITS);
    printf("mem->destAddressingMode = %d\n", mem->destAddressingMode);
    binaryStr = binaryStr + KEYWORD_ADDRESSING_MODE_BITS;

    /* Convert encoding type */
    castIntToBinaryString(mem->encodingType, binaryStr, KEYWORD_ENCODING_TYPE_BITS);
    printf("mem->encodingType = %d\n", mem->encodingType);
    binaryStr = binaryStr + KEYWORD_ENCODING_TYPE_BITS;
}

/*
    Receive a base filename, and append a given extension to the output by concatenating.

    @param char *fileName - The base file name
    @param char *output - The string that will hold the final result
    @param char *extenson - The desired extension to append
*/
void appendExtensionToFilename(char *fileName, char *output, char *extension) {
    strcpy(output, fileName);
    strcat(output, extension);
}

void binaryKeywordToBase64(char *keyword, char *base64Keyword) {
    // printf("KW = '%s'\n", keyword);
    int i;
    char firstKeyWordSlice[(MAX_KEYWORD_BINARY_LENGTH/2) + 1];
    char secondKeyWordSlice[(MAX_KEYWORD_BINARY_LENGTH/2) + 1];

    int firstKeyWordNum, secondKeyWordNum;
    // printf("SIZEOF firstKWSLICE = %d\n", (MAX_KEYWORD_BINARY_LENGTH/2) + 1);
    for (i = 0; i < MAX_KEYWORD_BINARY_LENGTH/2; i++) {
        // printf("i = %d, keyword[%d] = %c\n", i, i, keyword[i]);
        firstKeyWordSlice[i] = keyword[i];
        // printf("firstKeyWordSlice[%d] = %c\n", i, firstKeyWordSlice[i]);
    }
    // printf("Setting terminator for %d\n", i);
    firstKeyWordSlice[i] = '\0';
    // printf("FINISHED WITH firstKWSLICE\n");

    for (i = 0; i < MAX_KEYWORD_BINARY_LENGTH/2; i++) {
        // printf("i = %d, keyword[%d] = %c\n", i, (MAX_KEYWORD_BINARY_LENGTH/2) + i, keyword[(MAX_KEYWORD_BINARY_LENGTH/2) + i]);
        secondKeyWordSlice[i] = keyword[(MAX_KEYWORD_BINARY_LENGTH/2) + i];
        // printf("secondKeyWordSlice[%d] = %c\n", i, secondKeyWordSlice[i]);
    }
    secondKeyWordSlice[i] = '\0';

    firstKeyWordNum = strtol(firstKeyWordSlice, NULL, 2);
    secondKeyWordNum = strtol(secondKeyWordSlice, NULL, 2);

    base64Keyword[0] = base64Constants[firstKeyWordNum];
    base64Keyword[1] = base64Constants[secondKeyWordNum];
    base64Keyword[2] = '\0';
    // printf("keyword = '%s', firstKeyWordSlice = '%s', secondKeyWordSlice = '%s' \n", keyword, firstKeyWordSlice, secondKeyWordSlice);
    printf("%s\n", base64Keyword);
    // printf("#######################################\n");
}

/*
    Receive the finalized code table, and write it's content to a file in base 64 encoding.
    The file header needs to be the instructions count & the data count. 

    @param codeInstructionsTable *codeTable - The code table containing all the binary keywords
    @param char *baseFileName - The base filename which will contain the output
    @param int dataCount - The counter of the data declarations
*/
void dumpCode(codeInstructionsTable *codeTable, char *baseFileName, int dataCount) {
    int i;
    char keywordInBase64[BASE_64_KEYWORD_SIZE + 1];
    char fileNameWithExtension[FILENAME_MAX_LENGTH];
    FILE *fp;

    appendExtensionToFilename(baseFileName, fileNameWithExtension, MAIN_OUTPUT_FILE_EXTENSION);

    fp = fopen(fileNameWithExtension, "w");

    fprintf(fp, "%d %d\n", codeTable->instructionCount - dataCount, dataCount); /* Subtract DC from IC, which was added previously in the 2nd iteration */

    for (i = 0; i < codeTable->instructionCount; i++) {
        binaryKeywordToBase64(codeTable->rows[i], keywordInBase64);
        fprintf(fp, "%s\n", keywordInBase64);
    }    

    fclose(fp);
}

/*
    Write the entry/extern symbols declared in our code.
    If there are no entry/extern symbols, no file will be created.

    @param symbolTable *symbTable - The symbol table containing the entry/extern declarations
    @param char *baseFileName - The base filename which will contain the output    
*/
void dumpEntryExternData(symbolTable *symbTable, char *baseFileName) {
    char entryOutputFileName[FILENAME_MAX_LENGTH];
    char externOutputFileName[FILENAME_MAX_LENGTH];
    FILE *fpExtern;
    FILE *fpEntry;
    int i;
    int hasEntryData = 0;
    int hasExternData = 0;
    symbolTableNode *temp = symbTable->head;

    appendExtensionToFilename(baseFileName, entryOutputFileName, ENTRY_OUTPUT_FILE_EXTENSION);
    appendExtensionToFilename(baseFileName, externOutputFileName, EXTERN_OUTPUT_FILE_EXTENSION);

    for(i = 0; i < symbTable->symbolsCounter; temp = temp->next, i++)  {   
        if (temp->symbolType == ENTRY_SYMBOL) {
            if (!hasEntryData) {
                fpEntry = fopen(entryOutputFileName, "w");
                hasEntryData = 1;
            }

            fprintf(fpEntry, "%s    %d\n", temp->symbolName, temp->memoryAddress);
        } else if (temp->symbolType == EXTERNAL_SYMBOL) {
            if (!hasExternData) {
                fpExtern = fopen(externOutputFileName, "w");
                hasExternData = 1;
            }

            fprintf(fpExtern, "%s    %d\n", temp->symbolName, temp->memoryAddress);
        }
    }

    if (hasEntryData) {
        fclose(fpEntry);
    }

    if (hasExternData) {
        fclose(fpExtern);
    }
}
