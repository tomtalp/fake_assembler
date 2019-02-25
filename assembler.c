#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"

// #include "dataStructures.h" /* TODO - I get this import from utils.h, is this fine? If I use it - error */


void addDataTypeToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    /* We're supposed to be dealing with a sequence of integers. We'll parse each one 
    and cast into an int, and insert into the data table */

    int num, i;
    char intAsBinaryString[MAX_KEYWORD_BINARY_LENGTH];

    i = 0;

    if (pr->hasSymbol) {
        addNodeToSymbolTable(symbTable, pr->symbolName , dataTable->dataCounter, 0, 0);
    }

    for (i = 0; i < MAX_INSTRUCTION_LENGTH, pr->rowMetadata.dataRowMetadata.rawData[i] != 0; i++) {
        num = 0;

        while (pr->rowMetadata.dataRowMetadata.rawData[i] != 0 && !isspace(pr->rowMetadata.dataRowMetadata.rawData[i]) && pr->rowMetadata.dataRowMetadata.rawData[i] != ',') {
            num = num * 10;
            if (!isdigit(pr->rowMetadata.dataRowMetadata.rawData[i])) {
                printf("Woopsie, got a non digit in data type parsing! TODO HANDLE THIS\n");
            }
            num += pr->rowMetadata.dataRowMetadata.rawData[i] - '0'; // Add the character digit as an int
            i++;
        }
        i++; /* Skip the comma or space that just made us stop */

        dataTable->rows[dataTable->dataCounter] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));
        castIntToBinaryString(num, dataTable->rows[dataTable->dataCounter], MAX_KEYWORD_BINARY_LENGTH);
        
        dataTable->dataCounter++;
    }
}

void addStringTypeToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    printf("Adding string data. Raw data = %s\n", pr->rowMetadata.dataRowMetadata.rawData);

}

void addToDataTable(symbolTable *symbTable, dataDefinitionsTables *dataTable, parsedRow *pr) {
    printf("Adding stuff to data table!\n");
    if (pr->rowMetadata.dataRowMetadata.type == DATA_TYPE) {
        addDataTypeToDataTable(symbTable, dataTable, pr);
    } else if (pr->rowMetadata.dataRowMetadata.type == STRING_TYPE) {
        addStringTypeToDataTable(symbTable, dataTable, pr);
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

void addToCodeTable(codeInstructionsTable *codeTable, parsedRow *pr) {
    memKeyword *mainKeyWord = (memKeyword*)malloc(sizeof(memKeyword));

    mainKeyWord->opCode = pr->rowMetadata.codeRowMetadata.oc.opCodeNum;
    mainKeyWord->sourceAddressingMode = pr->rowMetadata.codeRowMetadata.srcOperandType;
    mainKeyWord->destAddressingMode = pr->rowMetadata.codeRowMetadata.destOperandType;
    mainKeyWord->encodingType = ABSOLUTE_TYPE;

    codeTable->rows[codeTable->instructionCount] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));
    memKeywordToBinaryString(mainKeyWord, codeTable->rows[codeTable->instructionCount]);
    printf("MKB = '%s'\n", codeTable->rows[codeTable->instructionCount]);
    codeTable->instructionCount++;
    

    if (pr->rowMetadata.codeRowMetadata.srcOperandType == NO_OPERAND) {
        if (pr->rowMetadata.codeRowMetadata.destOperandType == NO_OPERAND) {
            printf("Code instruction w/o operands\n");
            return;
        } else {
            /* Allocate a new record (ther's only going to be one since only one operand) */
            codeTable->rows[codeTable->instructionCount] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char));

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
            }
            codeTable->instructionCount++;
        }
    } else { /* Both operands are set, an */
        if (pr->rowMetadata.codeRowMetadata.srcOperandType == REGISTER_MODE && pr->rowMetadata.codeRowMetadata.destOperandType == REGISTER_MODE) {
            printf("Two registers!\n");
            codeTable->rows[codeTable->instructionCount] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char)); /* We'll only need one row, 2 registers fit in 1 kw */
            addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.srcOperand), 1); 
            addRegisterValueToBinaryKeyword(codeTable->rows[codeTable->instructionCount], getRegisterValueFromName(pr->rowMetadata.codeRowMetadata.destOperand), 0);
            addEncodingTypeToBinaryKeyword(codeTable->rows[codeTable->instructionCount], ABSOLUTE_TYPE);

            codeTable->instructionCount++;
        } else {
            codeTable->rows[codeTable->instructionCount] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char)); 
    
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

                case DIRECT_MODE:
                    memcpy(codeTable->rows[codeTable->instructionCount], "zzzzzzzzzzzz", 12);
                default:
                    break;
            }

            codeTable->instructionCount++;
            codeTable->rows[codeTable->instructionCount] = malloc(MAX_KEYWORD_BINARY_LENGTH * sizeof(char)); 

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

                case DIRECT_MODE:
                    memcpy(codeTable->rows[codeTable->instructionCount], "zzzzzzzzzzzz", 12);
                default:
                    break;
            }    
            codeTable->instructionCount++;    
        }
    }

}

void firstIteration(symbolTable *symbTable, dataDefinitionsTables *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList) {
    FILE *fp;
    // char *path = "./test_single_ci.as";
    char *path = "./test.as";
    char inputRow[MAX_INSTRUCTION_LENGTH];
    int rowNum = 0;

    fp = fopen(path, "r");

    if (fp == NULL) {
        printf("Failed opening file '%s'\n", path);
    } else {
        while(fgets(inputRow, MAX_INSTRUCTION_LENGTH, fp) != NULL) {
            rowNum++;
            if (isEmptyRow(inputRow) || isCommentRow(inputRow)) {
                continue;
            }
            printf("Got row '%s'\n", inputRow);
            parsedRow *pr = (parsedRow*)malloc(sizeof(parsedRow));
            parseRow(inputRow, pr, rowNum);
            addParsedRowToList(prList, pr);
            printf("Done parsing row %d\n", rowNum);

            if (!pr->isValidRow) {
                printf("Row number #%d has errors!\n", pr->originalLineNum);
            } else {
                if (pr->rowType == DATA_DECLARATION) {
                    addToDataTable(symbTable, dataTable, pr);
                } else if (pr->rowType == CODE_INSTRUCTION) {
                    addToCodeTable(codeTable, pr);
                }
            }

        }
        fclose(fp);
    }
}

void secondIteration() {

}