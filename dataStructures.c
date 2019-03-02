#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructures.h"

void initSymbolTable(symbolTable *symbTable) {
    printf("inside initSymbolTable\n");
    symbTable->head = NULL;
    symbTable->symbolsCounter = 0;

    symbTable->relocTable = (relocationTable*)malloc(sizeof(relocationTable));
    symbTable->relocTable->relocationVariablesCounter = 0;
    symbTable->relocTable->head = NULL;
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

void addNodeToSymbolTable(symbolTable *tb, char *symbolName, int memAddress, int symbolType) {
    symbolTableNode *newNode = (symbolTableNode*)malloc(sizeof(symbolTableNode));
    symbolTableNode *temp = (symbolTableNode*)malloc(sizeof(symbolTableNode));

    /* Initialize the new node */
    strcpy(newNode->symbolName, symbolName);
    newNode->memoryAddress = memAddress;
    newNode->symbolType = symbolType;
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

void addToRelocationsTable(relocationTable *relocTable, char *varName, int memAddress) {
    relocationTableNode *newNode = (relocationTableNode*)malloc(sizeof(relocationTableNode));
    relocationTableNode *temp = (relocationTableNode*)malloc(sizeof(relocationTableNode));
    
    memcpy(newNode->symbolName, varName, MAX_SYMBOL_NAME_LENGTH);
    newNode->memAddress = memAddress + BASE_MEM_ADDRESS;
    newNode->next = NULL;

    if (relocTable->relocationVariablesCounter == 0) { /* Handle empty list */
        relocTable->head = newNode;
        relocTable->relocationVariablesCounter += 1;
        return;
    }

    temp = relocTable->head;

    while (temp->next) { /* Go to end of list */
        temp = temp->next;
    }

    temp->next = newNode;
    relocTable->relocationVariablesCounter += 1;
}

void printRelocTable(relocationTable *relocTable) {
    int i;
    relocationTableNode *temp = relocTable->head;

    printf("reloc Table has %d rows\n", relocTable->relocationVariablesCounter);
    // for(i = 1; temp ; temp = temp->next, i++)  {   
    for(i = 0; i < relocTable->relocationVariablesCounter ; temp = temp->next, i++)  {   
        printf("reloc table row #%d = name = %s, addr = %d\n", i+1, temp->symbolName, temp->memAddress);
    }
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
        printf("Code table row #%d mem addr = %s\n", i+BASE_MEM_ADDRESS, codeTable->rows[i]);
    }
}

void updateSymbolTableAddresses(symbolTable *tb, int baseMemAddress, int instructionCount) {
    int i;
    symbolTableNode *temp = tb->head;

    for(i = 0; i < tb->symbolsCounter; temp = temp->next, i++)  {  
        if (temp->symbolType == DATA_SYMBOL) {
            temp->memoryAddress = temp->memoryAddress + baseMemAddress + instructionCount;
        } else {
            temp->memoryAddress += baseMemAddress;
        }
    }
}


int fetchFromSymbTableByName(symbolTable *tb, char *symbolName) {
    int i;
    symbolTableNode *temp = tb->head;

    for(i = 0; i < tb->symbolsCounter; temp = temp->next, i++)  {  
        if (strcmp(temp->symbolName, symbolName) == 0) {
            return temp->memoryAddress;
        }
    }
    return -1;
}
