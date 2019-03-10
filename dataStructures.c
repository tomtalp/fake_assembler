#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructures.h"
#include "utils.h"

/*
    Initialize the symbol table by creating the head of the list, setting counter to 0
    and initializing the reloc table that we have inside the symbol table

    @param symbolTable *symbTable - Our symbol table
*/
void initSymbolTable(symbolTable *symbTable) {
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

    for(i = 0; i < tb->symbolsCounter; temp = temp->next, i++)  {   
        printf("Symbol table row #%d = name = %s, addr = %d", i+1, temp->symbolName, temp->memoryAddress);

        printf("\n");
    }
}

/*
    Add a new node to our symbol table.
    If the record already exists in our symbol table, return -1.

    @param symbolTable *tb - A pointer to the symbol table we're working with
    @param char *symbolName - The name of the symbol
    @param int memAddress - The memory address for the given symbol
    @param enum SYMBOL_TYPES symbolType - A flag indicating what kind of symbol we're adding
*/
int addNodeToSymbolTable(symbolTable *tb, char *symbolName, int memAddress, enum SYMBOL_TYPES symbolType) {
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
        return 0;
    }

    temp = tb->head;

    while (temp->next) { /* Go to end of list, while looking for a duplicate node */
        if (strcmp(temp->symbolName, symbolName) == 0) {
            return -1;
        }
        temp = temp->next;
    }

    temp->next = newNode;
    tb->symbolsCounter += 1;  

    return 0;  
}

/*
    Add a new node to our data table

    @param dataDefinitionsTable *tb - A pointer to the data table we're working with
    @param int num - The numeric value we want to encode as binary and add to the data table
*/
void addNodeToDataTable(dataDefinitionsTable *tb, int num) {
    dataDefinitionNode *newNode = (dataDefinitionNode*)malloc(sizeof(dataDefinitionNode));
    dataDefinitionNode *temp = (dataDefinitionNode*)malloc(sizeof(dataDefinitionNode));

    /* Initialize the new node */
    castIntToBinaryString(num, newNode->binaryData, MAX_KEYWORD_BINARY_LENGTH);
    newNode->next = NULL;

    if (tb->dataCounter == 0) { /* Handle empty list */
        tb->head = newNode;
        tb->dataCounter += 1;
        return;
    }

    temp = tb->head;

    while (temp->next) { /* Go to end of list */
        temp = temp->next;
    }

    temp->next = newNode;
    tb->dataCounter += 1;    
}

/*
    Initialize the data table

    @param dataDefinitionsTable *dataTable - Pointer to the data table
*/
void initDataTable(dataDefinitionsTable *dataTable) {
    dataTable->head = NULL;
    dataTable->dataCounter = 0;
}


/*
    Add a new value to the relocations table

    @param relocationTable *relocTable - Pointer to the relocations table
    @param char *varName - The name of the symbol that we'll later relocate
    @param int memAddress - The memory address that needs to be relocated
*/
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
    for(i = 0; i < relocTable->relocationVariablesCounter ; temp = temp->next, i++)  {   
        printf("reloc table row #%d = name = %s, addr = %d\n", i+1, temp->symbolName, temp->memAddress);
    }
}

void printDataTable(dataDefinitionsTable *dataTable) {
    dataDefinitionNode *temp = dataTable->head;

    while (temp != NULL) {
        printf("Data table - '%s'\n", temp->binaryData);
        temp = temp->next;
    }
}

void printCodeTable(codeInstructionsTable *codeTable) {
    int i;

    printf("Code Table has %d rows\n", codeTable->instructionCount);
    for (i = 0; i < codeTable->instructionCount; i++) {
        printf("Code table row #%d mem addr = %s\n", i+BASE_MEM_ADDRESS, codeTable->rows[i]);
    }
}

/*
    Update symbol table memory addresses, based on the calculated instructionCount (so that
    data declarations reside adter the code instructions, in our final data structure)

    @param symbolTable *tb - Pointer to the symbol table
    @param int baseMemAddress - The base memory address in our code, so we can get the mem address to the right place
    @param int instrutionCount - The instruction count that was calculated after initially building the code table

*/
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

/*
    Return a symbolTableNode pointer from a given symbol name.
    If symbol name isn't found, return null!

    @param symbolTable *tb - The symbol table we're working with
    @param char *symbolName - The name of the symbol that needs to be located
*/
symbolTableNode *fetchFromSymbTableByName(symbolTable *tb, char *symbolName) {
    symbolTableNode *temp = tb->head;

    while (temp != NULL) {
        if (strcmp(temp->symbolName, symbolName) == 0) {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}
