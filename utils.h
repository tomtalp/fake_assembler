#ifndef UTILS_H
#define UTILS_H

#include "dataStructures.h"

#define MAX_KEYWORD_BINARY_LENGTH 12
#define BASE_64_KEYWORD_SIZE 2
#define ASSEMBLY_FILE_EXTENSION ".as"
#define MAIN_OUTPUT_FILE_EXTENSION ".ob"
#define ENTRY_OUTPUT_FILE_EXTENSION ".ent"
#define EXTERN_OUTPUT_FILE_EXTENSION ".ext"

char getBinaryChar(int mask, int value, int isNegative);
void castIntToBinaryString(int n, char *binaryStr, int sizeOfBinaryKeyword);
int isEmptyRow(char *inputRow);
int isCommentRow(char *inputRow);
int strToInt(char *str);
void memKeywordToBinaryString(memKeyword *mem, char *binaryStr);
void appendExtensionToFilename(char *fileName, char *output, char *extension);
void binaryKeywordToBase64(char *keyword, char *base64Keyword);
void dumpCode(codeInstructionsTable *codeTable, char *baseFileName, int dataCount);
void dumpEntryExternData(symbolTable *symbTable, char *baseFileName);
void freeData(symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

#endif