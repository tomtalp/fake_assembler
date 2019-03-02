#ifndef UTILS_H
#define UTILS_H

#include "dataStructures.h"
#define MAX_KEYWORD_BINARY_LENGTH 12

char getBinaryChar(int mask, int value);
void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword);
int isEmptyRow(char *inputRow);
int isCommentRow(char *inputRow);
int strToInt(char *str);
void memKeywordToBinaryString(memKeyword *mem, char *binaryStr);
// void keywordToBase64(char *keyword, char *base64Keyword);

#endif