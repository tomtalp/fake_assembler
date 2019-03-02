#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "dataStructures.h"


int firstIteration(symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);
void secondIteration(symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

#endif