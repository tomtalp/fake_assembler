#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include <stdlib.h>
#include <string.h>

/* Hardcoded base-64 constants. Each character has the numeric value of it's poisition */
static const char base64Constants[] = { 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

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

void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword) {
    int isNegative = n < 0 ? 1 : 0;
    int mask;

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
    binaryStr = binaryStr + KEYWORD_ADDRESSING_MODE_BITS;

    /* Convert op code */
    castIntToBinaryString(mem->opCode, binaryStr, KEYWORD_OP_CODE_BITS);
    binaryStr = binaryStr + KEYWORD_OP_CODE_BITS;

    /* Convert dest addressing mode*/
    castIntToBinaryString(mem->destAddressingMode, binaryStr, KEYWORD_ADDRESSING_MODE_BITS);
    binaryStr = binaryStr + KEYWORD_ADDRESSING_MODE_BITS;

    /* Convert encoding type */
    castIntToBinaryString(mem->encodingType, binaryStr, KEYWORD_ENCODING_TYPE_BITS);
    binaryStr = binaryStr + KEYWORD_ENCODING_TYPE_BITS;
}

void appendExtensionToFilename(char *fileName, char *output, char *extension) {
    strcpy(output, fileName);
    strcat(output, extension);
}

void binaryKeywordToBase64(char *keyword, char *base64Keyword) {
    int i;
    char firstKeyWordSlice[(MAX_KEYWORD_BINARY_LENGTH/2) + 1];
    char secondKeyWordSlice[(MAX_KEYWORD_BINARY_LENGTH/2) + 1];

    int firstKeyWordNum, secondKeyWordNum;

    for (i = 0; i < MAX_KEYWORD_BINARY_LENGTH/2; i++) {
        firstKeyWordSlice[i] = keyword[i];
    }
    
    firstKeyWordSlice[i] = '\0';
    

    for (i = 0; i < MAX_KEYWORD_BINARY_LENGTH/2; i++) {
        secondKeyWordSlice[i] = keyword[(MAX_KEYWORD_BINARY_LENGTH/2) + i];
    }
    secondKeyWordSlice[i] = '\0';

    firstKeyWordNum = strtol(firstKeyWordSlice, NULL, 2);
    secondKeyWordNum = strtol(secondKeyWordSlice, NULL, 2);

    base64Keyword[0] = base64Constants[firstKeyWordNum]; /* First part of the b64 keyword is the first half*/
    base64Keyword[1] = base64Constants[secondKeyWordNum]; /* Second part of the b64 keyword is the second half*/
    base64Keyword[2] = '\0'; /* Terminate */
}

void dumpCode(codeInstructionsTable *codeTable, char *baseFileName, int dataCount) {
    int i;
    char keywordInBase64[BASE_64_KEYWORD_SIZE + 1];
    char fileNameWithExtension[FILENAME_MAX_LENGTH];
    FILE *fp;

    appendExtensionToFilename(baseFileName, fileNameWithExtension, MAIN_OUTPUT_FILE_EXTENSION);

    fp = fopen(fileNameWithExtension, "w");

    fprintf(fp, "%d %d\n", codeTable->instructionCount - dataCount, dataCount); /* Subtract DC from IC, which was added previously in the 2nd iteration */

    /* Write every row in base 64 */
    for (i = 0; i < codeTable->instructionCount; i++) {
        binaryKeywordToBase64(codeTable->rows[i], keywordInBase64);
        fprintf(fp, "%s\n", keywordInBase64);
    }    

    fclose(fp);
}

void dumpEntryExternData(symbolTable *symbTable, char *baseFileName) {
    char entryOutputFileName[FILENAME_MAX_LENGTH];
    char externOutputFileName[FILENAME_MAX_LENGTH];
    FILE *fpExtern;
    FILE *fpEntry;
    int i;
    int hasEntryData = 0;
    int hasExternData = 0;
    symbolTableNode *symbNode = symbTable->head;
    relocationTableNode *relocNode = symbTable->relocTable->head;

    appendExtensionToFilename(baseFileName, entryOutputFileName, ENTRY_OUTPUT_FILE_EXTENSION);
    appendExtensionToFilename(baseFileName, externOutputFileName, EXTERN_OUTPUT_FILE_EXTENSION);

    for (i = 0; i < symbTable->symbolsCounter; symbNode = symbNode->next, i++)  {   
        if (symbNode->symbolType == ENTRY_SYMBOL) {
            if (!hasEntryData) {
                fpEntry = fopen(entryOutputFileName, "w");
                hasEntryData = 1;
            }

            fprintf(fpEntry, "%s    %d\n", symbNode->symbolName, symbNode->memoryAddress);
        }
    }

    while (relocNode != NULL) { /* Go over data that needs to be relocated, and check if it needs to be relocated because it's external */
        if (fetchFromSymbTableByName(symbTable, relocNode->symbolName)->symbolType == EXTERNAL_SYMBOL) {
            if (!hasExternData) {
                fpExtern = fopen(externOutputFileName, "w");
                hasExternData = 1;
            }
            fprintf(fpExtern, "%s    %d\n", relocNode->symbolName, relocNode->memAddress);
        }
        relocNode = relocNode->next;
    }


    /* Close files */
    if (hasEntryData) {
        fclose(fpEntry);
    }

    if (hasExternData) {
        fclose(fpExtern);
    }
}

void freeData(symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList) {
    symbolTableNode *symbNode1, *symbNode2;
    dataDefinitionNode *dataNode1, *dataNode2;
    parsedRowNode *prNode1, *prNode2;

    int i;

    symbNode1 = symbTable->head;
    
    while (symbNode1 != NULL) {
        symbNode2 = symbNode1;
        symbNode1 = symbNode1->next;

        free(symbNode2);
    }

    dataNode1 = dataTable->head;

    while (dataNode1 != NULL) {
        dataNode2 = dataNode1;
        dataNode1 = dataNode1->next;

        free(dataNode2);
    }

    prNode1 = prList->head;

    while (prNode1 != NULL) {
        prNode2 = prNode1;
        prNode1 = prNode1->next;

        free(prNode2);
    }

    for (i = 0; i < codeTable->instructionCount; i++) {
        free(codeTable->rows[i]);
    }
    codeTable->instructionCount = 0;

}
