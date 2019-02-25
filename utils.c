#include <stdio.h>
#include <ctype.h>
#include "utils.h"
// #include "dataStructures.h"

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
    printf("n = %d\n", n);
    mask = 1 << (sizeOfBinaryKeyword-1);
    while (mask) {
        *binaryStr = getBinaryChar(mask, n);
        binaryStr++;
        mask >>= 1;
    }
    // *(binaryStr) = '\0';
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
