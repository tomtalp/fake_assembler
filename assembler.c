#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"
#include "errors.h"

/*
    Iterate over the raw data, and parse a single digit or a list of numbers.
    This runs after the validateIntDataDeclaration() func, so we know our data is valid and can be
    legally parsed.

    @param dataDefinitionsTable *dataTable - The data definitions binary table, used to add the parsed numbers in binary
    @param parsedRow *pr - The row we're working with, containing the data to be parsed
*/
void addDataTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr) {
    /* We're supposed to be dealing with a sequence of integers. We'll parse each one 
    and cast into an int, and insert into the data table */
    
    int num, i;
    int isNegative = 0;

    i = 0;

    /* Iterate over the entire raw string. Each for-loop iteration is considered a new number parsing */
    for (i = 0; i < MAX_INSTRUCTION_LENGTH, pr->rowMetadata.dataRowMetadata.rawData[i] != 0;) {
        num = 0;
        isNegative = 0;

        while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0 && !isspace(pr->rowMetadata.dataRowMetadata.rawData[i]) && pr->rowMetadata.dataRowMetadata.rawData[i] != ',') {
            if (pr->rowMetadata.dataRowMetadata.rawData[i] == '-') {
                isNegative = 1;
                i++;
                continue;
            } else if (pr->rowMetadata.dataRowMetadata.rawData[i] == '+') {
                i++;
                continue;
            }
            num = num * 10;
            num += pr->rowMetadata.dataRowMetadata.rawData[i] - '0'; /* Add the character digit as an int */
            i++;
        }

        if (isNegative) {
            num = num * (-1);
        }

        // dataTable->rows[dataTable->dataCounter] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));
        // castIntToBinaryString(num, dataTable->rows[dataTable->dataCounter], MAX_KEYWORD_BINARY_LENGTH);
        addNodeToDataTable(dataTable, num);
        
        // dataTable->dataCounter++;

        /* Get to the next number, if exists, while skipping all spaces or comas */
        while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0 && (isspace(pr->rowMetadata.dataRowMetadata.rawData[i]) || pr->rowMetadata.dataRowMetadata.rawData[i] == ',')) { 
            i++;
        }
    }
}

void addStringTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr) {
    printf("Adding string data. Raw data = %s\n", pr->rowMetadata.dataRowMetadata.rawData);
    int num, i;

    i = 0;
    int asciiVal;

    while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0) {
        asciiVal = (int)pr->rowMetadata.dataRowMetadata.rawData[i];
        addNodeToDataTable(dataTable, asciiVal);
        // dataTable->rows[dataTable->dataCounter] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));
        // castIntToBinaryString(asciiVal, dataTable->rows[dataTable->dataCounter], MAX_KEYWORD_BINARY_LENGTH);
        
        // dataTable->dataCounter++;
        i++;
    }

    /* Add a null terminator at the end*/
    addNodeToDataTable(dataTable, 0);
    // dataTable->rows[dataTable->dataCounter] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));
    // castIntToBinaryString(0, dataTable->rows[dataTable->dataCounter], MAX_KEYWORD_BINARY_LENGTH);
    
    // dataTable->dataCounter++;
}

void addToDataTable(symbolTable *symbTable, dataDefinitionsTable *dataTable, parsedRow *pr) {
    printf("Adding stuff to data table!\n");
    
    if (pr->hasSymbol) {
        if (addNodeToSymbolTable(symbTable, pr->symbolName , dataTable->dataCounter, DATA_SYMBOL) == -1) {
            pr->errorType = DUPLICATE_SYMBOL_DECLARATION;
            return;
        }
    }

    if (pr->rowMetadata.dataRowMetadata.type == DATA_TYPE) {
        addDataTypeToDataTable(dataTable, pr);
    } else if (pr->rowMetadata.dataRowMetadata.type == STRING_TYPE) {
        addStringTypeToDataTable(dataTable, pr);
    }
}

/*
 Receive a register name, and extract the numeric number from it.
 For example, registerName = "@r3", then the numeric value is 3.
*/
int getRegisterValueFromName(char *registerName) {
    int registerValue;

    // TODO - This is kinda nasty..... any better solution?! Couldn't think of anything
    // better w/o actually storing the value of the register in the data structure, but
    // that messes with the previous stuff I did...
    while (*registerName != 0 && (*registerName == '@' || *registerName == 'r')) {
        registerName++;
    }
    registerValue = atoi(registerName);
    return registerValue;
}

void addRegisterValueToBinaryKeyword(char *keyword, int registerValue, int isSrcOperand) {
    char binaryValue[REGISTER_OPERAND_BINARY_SIZE];

    castIntToBinaryString(registerValue, binaryValue, REGISTER_OPERAND_BINARY_SIZE);

    if (isSrcOperand) { /* Start writing from the beginning of the string (which corresponds to bits 7-11) */
        memcpy(keyword, binaryValue, REGISTER_OPERAND_BINARY_SIZE);
    } else { /* Write in the second position (i.e 2-6) */
        memcpy(keyword + REGISTER_OPERAND_BINARY_SIZE, binaryValue, REGISTER_OPERAND_BINARY_SIZE);
    }

}

void addEncodingTypeToBinaryKeyword(char *keyword, int encType) {
    char encTypeBinary[ENCODING_TYPES_MAX_LENGTH];
    castIntToBinaryString(encType, encTypeBinary, ENCODING_TYPES_MAX_LENGTH);
    memcpy(keyword + CODE_INSTRUCTION_KEYWORD_DATA_SIZE, encTypeBinary, ENCODING_TYPES_MAX_LENGTH);
}

void addImmediateValueToBinaryKeyword(char *keyword, char *immediateValueStr) {
    int immediateVal;
    char binaryVal[CODE_INSTRUCTION_KEYWORD_DATA_SIZE];

    immediateVal = strToInt(immediateValueStr);
    castIntToBinaryString(immediateVal, binaryVal, CODE_INSTRUCTION_KEYWORD_DATA_SIZE);
    memcpy(keyword, binaryVal, CODE_INSTRUCTION_KEYWORD_DATA_SIZE);
}

void addToCodeTable(symbolTable *symbTable, codeInstructionsTable *codeTable, parsedRow *pr) {
    memKeyword *mainKeyWord = (memKeyword*)malloc(sizeof(memKeyword));

    mainKeyWord->opCode = pr->rowMetadata.codeRowMetadata.oc.opCodeNum;
    mainKeyWord->sourceAddressingMode = pr->rowMetadata.codeRowMetadata.srcOperandType;
    mainKeyWord->destAddressingMode = pr->rowMetadata.codeRowMetadata.destOperandType;
    mainKeyWord->encodingType = ABSOLUTE_TYPE;

    if (pr->hasSymbol) {
        if (addNodeToSymbolTable(symbTable, pr->symbolName, codeTable->instructionCount, INSTRUCTION_SYMBOL) == -1) {
            pr->errorType = DUPLICATE_SYMBOL_DECLARATION;
            return;
        }
        
    }

    codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char));
    memKeywordToBinaryString(mainKeyWord, codeTable->rows[codeTable->instructionCount]);
    printf("MKB = '%s'\n", codeTable->rows[codeTable->instructionCount]);
    codeTable->instructionCount++;
    

    if (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND) {
        if (pr->rowMetadata.codeRowMetadata.destOperandType == NO_OPERAND) {
            printf("Code instruction w/o operands\n");
            return;
        } else {
            /* Allocate a new record (ther's only going to be one since only one operand) */
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char));

            if (pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE) {
                addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], 0, 1); /* Add an empty src operand */
                addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.destOperand), 0); /* Add the dest register  operand */
                addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);

            } else if (pr->rowMetadata.codeRowMetadata.destOperandType == IMMEDIATE_MODE) {
                addImmediateValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], pr->rowMetadata.codeRowMetadata.destOperand);
                addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
            } else { // Direct mode 
                printf("Direct mode\n");
                memcpy(codeTable->rows[codeTable->instructionCount], "zzzzzzzzzzzz", 12);
                addToRelocationsTable(symbTable->relocTable, pr->rowMetadata.codeRowMetadata.destOperand, codeTable->instructionCount);
            }
            codeTable->instructionCount++;
        }
    } else { /* Both operands are set */
        if (pr->rowMetadata.codeRowMetadata.srcOperandType == REGISTER_MODE && pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE) {
            printf("Two registers!\n");
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char)); /* We'll only need one row, 2 registers fit in 1 kw */
            addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.srcOperand), 1); 
            addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.destOperand), 0);
            addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);

            codeTable->instructionCount++;
        } else {
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char)); 
    
            /* Add source */
            switch ( pr->rowMetadata.codeRowMetadata.srcOperandType)
            {
                case REGISTER_MODE:
                    addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.srcOperand), 1);
                    addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], 0, 0); /* Add an empty dest register  operand */
                    addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
                    break;
                
                case IMMEDIATE_MODE:
                    addImmediateValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], pr->rowMetadata.codeRowMetadata.srcOperand);
                    addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
                    break;
                case DIRECT_MODE:
                    memcpy(codeTable->rows[codeTable->instructionCount], "zzzzzzzzzzzz", 12);
                    addToRelocationsTable(symbTable->relocTable, pr->rowMetadata.codeRowMetadata.srcOperand, codeTable->instructionCount);
                    break;
                default:
                    break;
            }

            codeTable->instructionCount++;
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char)); 

            /* Add dest */
            switch ( pr->rowMetadata.codeRowMetadata.destOperandType)
            {
                case REGISTER_MODE:
                    addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.destOperand), 1);
                    addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], 0, 0); /* Add an empty dest register  operand */
                    addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
                    break;
                
                case IMMEDIATE_MODE:
                    addImmediateValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], pr->rowMetadata.codeRowMetadata.destOperand);
                    addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
                    break;

                case DIRECT_MODE:
                    memcpy(codeTable->rows[codeTable->instructionCount], "zzzzzzzzzzzz", 12);
                    addToRelocationsTable(symbTable->relocTable, pr->rowMetadata.codeRowMetadata.destOperand, codeTable->instructionCount);
                    break;
                default:
                    break;
            }    
            codeTable->instructionCount++;    
        }
    }

}

int firstIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList) {
    FILE *fp;

    char fileNameWithExtension[FILENAME_MAX_LENGTH];
    char inputRow[MAX_INSTRUCTION_LENGTH];
    int generalErrorFlag = 0;
    int rowNum = 0;

    appendExtensionToFilename(fileName, fileNameWithExtension, ASSEMBLY_FILE_EXTENSION);

    fp = fopen(fileNameWithExtension, "r");

    if (fp == NULL) {
        printFailedOpeningFile(fileNameWithExtension);
        return 1;
    } else {
        while(fgets(inputRow, MAX_INSTRUCTION_LENGTH, fp) != NULL) {
            rowNum++;
            if (isEmptyRow(inputRow) || isCommentRow(inputRow)) {
                continue;
            }
            printf("Got row '%s'\n", inputRow);
            parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
            strcpy(pr->fileName, fileName);
            parseRow(inputRow, pr, rowNum);
            addParsedRowToList(prList, pr);
            printf("Done parsing row %d\n", rowNum);

            if (pr->errorType != 0) {
                printParserError(pr);
                generalErrorFlag = 1;
            } else {
                if (pr->rowType == DATA_DECLARATION) {
                    addToDataTable(symbTable, dataTable, pr);
                } else if (pr->rowType == CODE_INSTRUCTION) {
                    addToCodeTable(symbTable, codeTable, pr);
                } else if (pr->rowType == EXTERNAL_DECLARATION) {
                    if (addNodeToSymbolTable(symbTable, pr->rowMetadata.externRowMetadata.labelName, codeTable->instructionCount, EXTERNAL_SYMBOL) == -1) {
                        pr->errorType = DUPLICATE_SYMBOL_DECLARATION;
                    }
                } else { 
                    /* Entry delcaration isn't handled in the first iteration */
                    continue;
                }
            }

            if (pr->errorType != 0) {
                printParserError(pr);
                generalErrorFlag = 1;
            }

        }
        fclose(fp);
    }

    if (generalErrorFlag == 0) {
        /* Update symbol table with updated IC, if we didn't encounter any errors so far */
        updateSymbolTableAddresses(symbTable, BASE_MEM_ADDRESS, codeTable->instructionCount);
    }

    return generalErrorFlag;
}

/*
    Add the dataTable section that we collected from the first iteration, into the end of the code table
    
    @param dataDefinitionsTable *dataTable - A pointer to the data table
    @param codeInstructionsTable *codeTable - A pointer to the code table
*/
void addDataToCodeTable(dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable) {
    dataDefinitionNode *temp = dataTable->head;

    while (temp != NULL) {
        codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char));
        
        memcpy(codeTable->rows[codeTable->instructionCount], temp->binaryData, MAX_KEYWORD_BINARY_LENGTH);
        *(codeTable->rows[codeTable->instructionCount] + MAX_KEYWORD_BINARY_LENGTH) = '\0';
        codeTable->instructionCount++;

        temp = temp->next;
    }
}

/*
    Iterate over the relocationTable, which contains data for labels that need to be given
    a value from the symbol table. If we locate a label in the symbol table, we plant the
    value from the symbol table in the code table

    If a label doesn't exist, we throw an error

    @param symbolTable *symbTable - Used to access the relocation table & symbol table
    @param codeInstructionsTable *codeTable - The code table which will be used to plant the new data
*/
int relocateSymbolAddresses(symbolTable *symbTable, codeInstructionsTable *codeTable) {
    relocationTableNode *temp = symbTable->relocTable->head;
    symbolTableNode *symbTableNode;
    int errorFlag = 0;

    while (temp != NULL) {
        symbTableNode = fetchFromSymbTableByName(symbTable, temp->symbolName);
        printf("wattttt = %s\n", temp->symbolName);
        if (symbTableNode == NULL ) {
            printSymbolDoesntExist(temp->symbolName);
            errorFlag = 1;
        } else {
            if (symbTableNode->symbolType == EXTERNAL_SYMBOL) { /* No data for external symbols, only an external type flag*/
                castIntToBinaryString(0, codeTable->rows[temp->memAddress - BASE_MEM_ADDRESS], CODE_INSTRUCTION_KEYWORD_DATA_SIZE);
                addEncodingTypeToBinaryKeyword(codeTable->rows[temp->memAddress - BASE_MEM_ADDRESS], EXTERNAL_TYPE);
            } else { /* Change the value in the code table to the data from the symbol table */
                castIntToBinaryString(symbTableNode->memoryAddress, codeTable->rows[temp->memAddress - BASE_MEM_ADDRESS], CODE_INSTRUCTION_KEYWORD_DATA_SIZE);
                addEncodingTypeToBinaryKeyword(codeTable->rows[temp->memAddress - BASE_MEM_ADDRESS], RELOCATABLE_TYPE);
            }
        }
        temp = temp->next;
    }
    return errorFlag;
}

/*
    Iterate over the list of previously parsed rows, and look for variables that were declared as .entry
    and for every one of them, set their "entry" flag in the symbol table

    @param parsedRowList *prList - The list of parsed rows
    @param symbolTable *symbTable - The symbol table used to set entry declarations
*/
int setEntrySymbols(parsedRowList *prList, symbolTable *symbTable) {
    symbolTableNode *symbTableNode;
    int errorFlag = 0;
    parsedRowNode *prNode = prList->head;

    while (prNode != NULL) {
        if (prNode->pr.rowType == ENTRY_DECLARATION) {
            symbTableNode = fetchFromSymbTableByName(symbTable, prNode->pr.rowMetadata.entryRowMetadata.labelName);
            
            if (symbTableNode == NULL) {
                printEntryDoesntExist(&prNode->pr);
                errorFlag = 1;
            } else {
                symbTableNode->symbolType = ENTRY_DECLARATION;
            }
        }
        
        prNode = prNode->next;
    }
    return errorFlag;
}

/*
    Second iteration of the assembler
    
    Go over the parsed rows & our tables once again and - 
    1. Set symbols as entries, if necessary
    2. Append data section after code section
    3. Plant the symbols in the code table, after retrieving all symbols
    4. Validate that no symbol was declared twice

    @param char *fileName - The original filename we're working with
    @param symbolTable *symbTable - The symbols table
    @param dataDefinitionsTable *dataTable - The data table, containing all the data definitions we found in the first iteartion
    @param codeInstructionsTable *codeTable - The partial code table that was built in the first iteration
    @param parsedRowList *prList - The list of parsedRows, built in the first iteration
*/
int secondIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList) {
    int i, relocMemAddressFromSymbTable;
    
    
    int generalErrorFlag = 0;
    addDataToCodeTable(dataTable, codeTable);
    generalErrorFlag = relocateSymbolAddresses(symbTable, codeTable);

    if (generalErrorFlag > 0) {
        return generalErrorFlag;
    }

    generalErrorFlag = setEntrySymbols(prList, symbTable);

    return generalErrorFlag;

}
