#include <stdio.h>
#include <ctype.h>
#include "utils.h"

char getBinaryChar(int mask, int value) {
    char val;
    if ((mask & value) == 0) {
        return '0';
    }
    return '1';
}

void castIntToBinaryString(int n, char *binaryStr) {
    int mask, i;
    i = 0;

    mask = 1 << (MAX_KEYWORD_BINARY_LENGTH-1);
    while (mask) {
        *binaryStr = getBinaryChar(mask, n);
        binaryStr++;
        mask >>= 1;
    }
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