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

        addNodeToDataTable(dataTable, num);

        /* Get to the next number, if exists, while skipping all spaces or comas */
        while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0 && (isspace(pr->rowMetadata.dataRowMetadata.rawData[i]) || pr->rowMetadata.dataRowMetadata.rawData[i] == ',')) { 
            i++;
        }
    }
}

/*
    Iterate over the raw data, and convert every character into it's ASCII value.
    This runs after the validateStringDataDeclaration() func, so we know our data is valid and can be
    legally parsed.

    @param dataDefinitionsTable *dataTable - The data definitions binary table, used to add the parsed numbers in binary
    @param parsedRow *pr - The row we're working with, containing the data to be parsed
*/
void addStringTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr) {
    int num, i, asciiVal;

    i = 0;
    
    while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0) {
        asciiVal = (int)pr->rowMetadata.dataRowMetadata.rawData[i];
        addNodeToDataTable(dataTable, asciiVal);

        i++;
    }

    /* Add a null terminator at the end*/
    addNodeToDataTable(dataTable, 0);
}

/*
    Add a parsed row which contains a data declaration into our data table (and create a symbol table if there's one)

    @param symbolTable *symbTable - The symbols table we're using
    @param dataDefinitionsTable *dataTable - The data table, which we'll add the data to
    @param parsedRow *pr - The parsed row object pointer
*/
void addToDataTable(symbolTable *symbTable, dataDefinitionsTable *dataTable, parsedRow *pr) {
    if (pr->hasSymbol) {
        if (addNodeToSymbolTable(symbTable, pr->symbolName , dataTable->dataCounter, DATA_SYMBOL) == -1) { /* Detect duplicate symbol declarations as soon as we can */
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

    @param char *registerName - The name of the register we're converting
*/
int getRegisterValueFromName(char *registerName) {
    int registerValue;

    while (*registerName != 0 && (*registerName == '@' || *registerName == 'r')) {
        registerName++;
    }
    registerValue = atoi(registerName);
    return registerValue;
}

/*
    Add the register value to a binary keyword.
    Since registers can take up half a keyword, we'll add it in a different location based on 
    whether it's a source or a dest operand

    @param char *keyword - The destination of the binary keyword
    @param char *registerValue - The string representing the register
    @param int isSrcOperand - A flag representing whether the string is a source operand or not.

*/
void addRegisterValueToBinaryKeyword(char *keyword, int registerValue, int isSrcOperand) {
    char binaryValue[REGISTER_OPERAND_BINARY_SIZE];

    castIntToBinaryString(registerValue, binaryValue, REGISTER_OPERAND_BINARY_SIZE);

    if (isSrcOperand) { /* Start writing from the beginning of the string (which corresponds to bits 7-11) */
        memcpy(keyword, binaryValue, REGISTER_OPERAND_BINARY_SIZE);
    } else { /* Write in the second position (i.e 2-6) */
        memcpy(keyword + REGISTER_OPERAND_BINARY_SIZE, binaryValue, REGISTER_OPERAND_BINARY_SIZE);
    }

}

/* 
    Convert an encoding type integer flag into a binary keyword. We plant the binary representation
    in the first 2 bits of the binary string

    @param char *keyword - The destination of the binary keyword
    @param int encType - The encoding value
*/
void addEncodingTypeToBinaryKeyword(char *keyword, int encType) {
    char encTypeBinary[ENCODING_TYPES_MAX_LENGTH];
    castIntToBinaryString(encType, encTypeBinary, ENCODING_TYPES_MAX_LENGTH);
    memcpy(keyword + CODE_INSTRUCTION_KEYWORD_DATA_SIZE, encTypeBinary, ENCODING_TYPES_MAX_LENGTH);
}

/*
    Convert a string representing an immediate value, into a binary keyword

    @param char *keyword - The destination of our binary keyword
    @param char *immediateValueStr - The string representing the immediate value
*/
void convertImmediateValueToBinaryKeyword(char *keyword, char *immediateValueStr) {
    int immediateVal;
    char binaryVal[CODE_INSTRUCTION_KEYWORD_DATA_SIZE];

    immediateVal = strToInt(immediateValueStr);
    castIntToBinaryString(immediateVal, binaryVal, CODE_INSTRUCTION_KEYWORD_DATA_SIZE);
    memcpy(keyword, binaryVal, CODE_INSTRUCTION_KEYWORD_DATA_SIZE);
}

/*
    Received a parsed row's operand and convert it to a binary value, and add to code instructions table

    @param symbolTable *symbTable - The symbols table we're using
    @param codeInstructionsTable *codeTable - The code table, which we'll add our binary code
    @param enum LEGAL_ADDRESSING_MODES addressingMode - The addressing mode of the operand
    @param char *operand - The operand string value
*/
void addOperandInBinary(symbolTable *symbTable, codeInstructionsTable *codeTable, enum LEGAL_ADDRESSING_MODES addressingMode, char *operand, int isSrcOperand) {
    switch (addressingMode)
        {
            case REGISTER_MODE:
                addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(operand), isSrcOperand);
                addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], 0, !isSrcOperand); /* Add an empty dest register  operand */
                addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
                break;
            
            case IMMEDIATE_MODE:
                convertImmediateValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], operand);
                addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
                break;
            case DIRECT_MODE:
                memcpy(codeTable->rows[codeTable->instructionCount], "000000000000", 12); /* Add an arbitrary place holder. We're overriding this in the 2nd iteration */
                addToRelocationsTable(symbTable->relocTable,operand, codeTable->instructionCount);
                break;
            default:
                break;
        }
}

/*
    Convert a parsed row which is a code instruction into it's binary value, and add to
    the code table.

    @param symbolTable *symbTable - The symbols table we're using
    @param codeInstructionsTable *codeTable - The code table, which we'll add our binary code
    @param parsedRow *pr - The parsed row object pointer

*/
void addToCodeTable(symbolTable *symbTable, codeInstructionsTable *codeTable, parsedRow *pr) {
    /* Start by adding the main keyword, which always exists (Regardless of the Operation & it's arguments) */
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
    codeTable->instructionCount++;
    
    if (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND) {
        if (pr->rowMetadata.codeRowMetadata.destOperandType == NO_OPERAND) {
            return; /* Code instruction w/o operands */
        } else {
            /* Allocate a new record (ther's only going to be one since only one operand) */
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char));

            if (pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE) {
                addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], 0, 1); /* Add an empty src operand */
                addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.destOperand), 0); /* Add the dest register  operand */
                addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);

            } else if (pr->rowMetadata.codeRowMetadata.destOperandType == IMMEDIATE_MODE) {
                convertImmediateValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], pr->rowMetadata.codeRowMetadata.destOperand);
                addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);
            } else { /* Direct mode */
                memcpy(codeTable->rows[codeTable->instructionCount], "000000000000", 12);
                addToRelocationsTable(symbTable->relocTable, pr->rowMetadata.codeRowMetadata.destOperand, codeTable->instructionCount);
            }
            codeTable->instructionCount++;
        }
    } else { /* Both operands are set */
        if (pr->rowMetadata.codeRowMetadata.srcOperandType == REGISTER_MODE && pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE) {
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char)); /* We'll only need one row, 2 registers fit in 1 kw */
            addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.srcOperand), 1);

            addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.destOperand), 0);

            addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);

            codeTable->instructionCount++;
        } else {
            /* Add source */
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char)); 
            addOperandInBinary(symbTable, codeTable, pr->rowMetadata.codeRowMetadata.srcOperandType, pr->rowMetadata.codeRowMetadata.srcOperand, 1);
            codeTable->instructionCount++;

            /* Add dest */
            codeTable->rows[codeTable->instructionCount] = malloc((MAX_KEYWORD_BINARY_LENGTH + 1) * sizeof(char)); 
            addOperandInBinary(symbTable, codeTable, pr->rowMetadata.codeRowMetadata.destOperandType, pr->rowMetadata.codeRowMetadata.destOperand, 0);
            codeTable->instructionCount++;    
        }
    }

}

/*
    First iteration of the assembler
    
    In the first iteration we scan the input file for the first time, perform basic parsing while
    populating our tables, and set whatever we can to reduce the workload in the next run

    @param char *fileName - The original filename we're working with
    @param symbolTable *symbTable - The symbols table
    @param dataDefinitionsTable *dataTable - The data table, which will hold all the data definitions we'll find
    @param codeInstructionsTable *codeTable - The initial code table that we'll build
    @param parsedRowList *prList - The list of parsedRows that we'll populate during the iteration
*/
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
        while (fgets(inputRow, MAX_INSTRUCTION_LENGTH, fp) != NULL) {
            rowNum++;
            if (isEmptyRow(inputRow) || isCommentRow(inputRow)) {
                continue;
            }

            parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
            
            strcpy(pr->fileName, fileName);
            parseRow(inputRow, pr, rowNum);
        
            addParsedRowToList(prList, pr);

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
