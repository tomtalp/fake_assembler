#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "dataStructures.h"


int firstIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);
int secondIteration(symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

#endif