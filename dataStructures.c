#include <stdio.h>
#include <stdlib.h>
#include "dataStructures.h"

void initSymbolTable(symbolTable *symbTable) {
    printf("inside initSymbolTable\n");
    symbTable->head = NULL;
    symbTable->symbolsCounter = 0;
}

void printSymbolTable(symbolTable *tb) {
    int i;
    symbolTableNode *temp = tb->head;

    printf("Symbol Table has %d rows\n", tb->symbolsCounter);
    // for(i = 1; temp ; temp = temp->next, i++)  {   
    for(i = 0; i < tb->symbolsCounter; temp = temp->next, i++)  {   
        printf("Symbol table row #%d = name = %s, addr = %d\n", i+1, temp->symbolName, temp->memoryAddress);
    }
}

void addNodeToSymbolTable(symbolTable *tb, symbolTableNode *newNode) {
    symbolTableNode *temp = (symbolTableNode*)malloc(sizeof(symbolTableNode));

    if (tb->symbolsCounter == 0) {
        tb->head = newNode;
        tb->symbolsCounter += 1;
        return;
    }

    tb->head->next = newNode;
    newNode->next = temp;
    tb->symbolsCounter += 1;
}

void printDataTable(dataDefinitionsTables *dataTable) {
    int i;

    printf("Data Table has %d rows\n", dataTable->dataCounter);
    for (i = 0; i < dataTable->dataCounter; i++) {
        printf("Data table row #%d mem addr = %s\n", i, dataTable->rows[i]->keyword);
    }
}
