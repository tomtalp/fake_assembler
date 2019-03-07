#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include <stdlib.h>
#include <string.h>

// char base64Constants[] = { 
//     'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
//     'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
//     'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
//     'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
//     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };
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

char getBinaryChar(int mask, int value) {
    char val;
    if ((mask & value) == 0) {
        return '0';
    }
    return '1';
}

void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword) {
    int mask, i;
    i = 0;
    mask = 1 << (sizeOfBinaryKeyword-1);
    while (mask) {
        *binaryStr = getBinaryChar(mask, n);
        binaryStr++;
        mask >>= 1;
    }
    *(binaryStr) = '\0'; // TODO itay - do I need this?
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

// TODO - handle negative numbers
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

void getFileNameWithExtension(char *fileName, char *fileNameWithExtension) {
    strcpy(fileNameWithExtension, fileName);
    strcat(fileNameWithExtension, ASSEMBLER_FILE_EXTENSION);
}

// void keywordToBase64(char *keyword, char *base64Keyword) {
//     int i;
//     char firstKeyWordSlice[MAX_KEYWORD_BINARY_LENGTH/2];
//     char secondKeyWordSlice[MAX_KEYWORD_BINARY_LENGTH/2];

//     for (i = 0; i < MAX_KEYWORD_BINARY_LENGTH; i++) {
//         if (i < MAX_KEYWORD_BINARY_LENGTH/2) {
//             firstKeyWordSlice[i] = keyword[i];
//         } else {
//             secondKeyWordSlice[i - MAX_KEYWORD_BINARY_LENGTH/2] = keyword[i];
//         }
//     }

//     printf("keyword = '%s', firstKeyWordSlice = '%s', secondKeyWordSlice = '%s'\n", keyword, firstKeyWordSlice, secondKeyWordSlice);
// }

// void dumpCode(codeInstructionsTable *codeTable, char *outputFileName) {
//     int i;
//     char keywordInBaste64[2];
//     FILE *fp;

//     fp = fopen("./ps.ob", "w");

//     for (i = 0; i < codeTable->instructionCount; i++) {
//         keywordToBase64(codeTable->rows[i], keywordInBaste64);
//         fprintf(fp, keywordInBaste64);
//         // printf("Code table row #%d mem addr = %s\n", i, codeTable->rows[i]);
//     }    

//     fclose(fp);
// }
