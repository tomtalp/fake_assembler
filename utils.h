#ifndef UTILS_H
#define UTILS_H

#include "dataStructures.h"

#define MAX_KEYWORD_BINARY_LENGTH 12
#define ASSEMBLER_FILE_EXTENSION ".as"

char getBinaryChar(int mask, int value, int isNegative);
void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword);
int isEmptyRow(char *inputRow);
int isCommentRow(char *inputRow);
int strToInt(char *str);
void memKeywordToBinaryString(memKeyword *mem, char *binaryStr);
void getFileNameWithExtension(char *fileName, char *fileNameWithExtension);
// void keywordToBase64(char *keyword, char *base64Keyword);

#endif