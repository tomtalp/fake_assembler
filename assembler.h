#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "dataStructures.h"

void addToDataTable(symbolTable *symbTable, dataDefinitionsTable *dataTable, parsedRow *pr);
void addDataTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr);
void addStringTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr);
void addToCodeTable(symbolTable *symbTable, codeInstructionsTable *codeTable, parsedRow *pr);
void addOperandInBinary(symbolTable *symbTable, codeInstructionsTable *codeTable, enum LEGAL_ADDRESSING_MODES addressingMode, char *operand, int isSrcOperand);
void convertImmediateValueToBinaryKeyword(char *keyword, char *immediateValueStr);
void addEncodingTypeToBinaryKeyword(char *keyword, int encType);
void addRegisterValueToBinaryKeyword(char *keyword, int registerValue, int isSrcOperand);
int firstIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);
void addDataToCodeTable(dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable);
int relocateSymbolAddresses(symbolTable *symbTable, codeInstructionsTable *codeTable);
int setEntrySymbols(parsedRowList *prList, symbolTable *symbTable);
int secondIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

#endif