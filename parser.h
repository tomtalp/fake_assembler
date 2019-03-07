#ifndef PARSER_H
#define PARSER_H

#include "dataStructures.h"

void printParsedRow(parsedRow *pr);
void trimLeadingWhitespace(char *source);
int isSymbolDefinition(char *inputRow);
void getSymbolName(char *inputRow, char *symbolName);
int getDataDefType(char *dataDef);
void getRowType(char *inputRow, parsedRow *pr);
int operandIsRegister(char *operand);
int operandIsNumber(char *operand);
void getOperandTypes(parsedRow *pr, char *firstOperand, char *secondOperand);
void getCodeOperands(char *inputRow, parsedRow *pr);
void parseRow(char *inputRow, parsedRow *pr, int rowNum);
void initParsedRowList(parsedRowList *prList);
void addParsedRowToList(parsedRowList *prList, parsedRow *pr);
void printParsedRowsList(parsedRowList *prList);
void printParserError(parsedRow *pr);
void validateIntDataDeclaration(parsedRow *pr, char *rawData);
void validateStringDataDeclaration(parsedRow *pr, char *rawData);
void validateSymbolName(parsedRow *pr);

#endif