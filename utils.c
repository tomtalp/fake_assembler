#include <stdio.h>
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
        binaryStr[i++] = getBinaryChar(mask, n);
        mask >>= 1;
    }
}