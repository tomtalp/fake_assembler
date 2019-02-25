#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* TODO - Do new nodes need to be added in a sorted way? */
void addNodeToSymbolTable(symbolTable *tb, char *symbolName, int memAddress, int isExternal, int isInstruction) {
    symbolTableNode *newNode = (symbolTableNode*)malloc(sizeof(symbolTableNode));
    symbolTableNode *temp = (symbolTableNode*)malloc(sizeof(symbolTableNode));

    /* Initialize the new node */
    strcpy(newNode->symbolName, symbolName);
    newNode->memoryAddress = memAddress;
    newNode->isExternal = isExternal;
    newNode->isInstruction = isInstruction;
    newNode->next = NULL;

    if (tb->symbolsCounter == 0) { /* Handle empty list */
        tb->head = newNode;
        tb->symbolsCounter += 1;
        return;
    }

    temp = tb->head;

    while (temp->next) { /* Go to end of list */
        temp = temp->next;
    }

    temp->next = newNode;
    tb->symbolsCounter += 1;

    // temp = tb->head->next;
    // tb->head->next = newNode;
    // newNode->next = temp;
    
}

void printDataTable(dataDefinitionsTables *dataTable) {
    int i;

    printf("Data Table has %d rows\n", dataTable->dataCounter);
    for (i = 0; i < dataTable->dataCounter; i++) {
        printf("Data table row #%d mem addr = %s\n", i, dataTable->rows[i]);
    }
}

void printCodeTable(codeInstructionsTable *codeTable) {
    int i;

    printf("Code Table has %d rows\n", codeTable->instructionCount);
    for (i = 0; i < codeTable->instructionCount; i++) {
        printf("Code table row #%d mem addr = %s\n", i, codeTable->rows[i]);
    }
}