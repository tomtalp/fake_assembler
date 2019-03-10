#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "dataStructures.h"

/*
    Add a parsed row which contains a data declaration into our data table (and create a symbol table if there's one)

    @param symbolTable *symbTable - The symbols table we're using
    @param dataDefinitionsTable *dataTable - The data table, which we'll add the data to
    @param parsedRow *pr - The parsed row object pointer
*/
void addToDataTable(symbolTable *symbTable, dataDefinitionsTable *dataTable, parsedRow *pr);

/*
    Iterate over the raw data, and parse a single digit or a list of numbers.
    This runs after the validateIntDataDeclaration() func, so we know our data is valid and can be
    legally parsed.

    @param dataDefinitionsTable *dataTable - The data definitions binary table, used to add the parsed numbers in binary
    @param parsedRow *pr - The row we're working with, containing the data to be parsed
*/
void addDataTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr);

/*
    Iterate over the raw data, and convert every character into it's ASCII value.
    This runs after the validateStringDataDeclaration() func, so we know our data is valid and can be
    legally parsed.

    @param dataDefinitionsTable *dataTable - The data definitions binary table, used to add the parsed numbers in binary
    @param parsedRow *pr - The row we're working with, containing the data to be parsed
*/
void addStringTypeToDataTable(dataDefinitionsTable *dataTable, parsedRow *pr);

/*
    Receive a register name, and extract the numeric number from it.
    For example, registerName = "@r3", then the numeric value is 3.

    @param char *registerName - The name of the register we're converting
*/
int getRegisterValueFromName(char *registerName);

/*
    Add the register value to a binary keyword.
    Since registers can take up half a keyword, we'll add it in a different location based on 
    whether it's a source or a dest operand

    @param char *keyword - The destination of the binary keyword
    @param char *registerValue - The string representing the register
    @param int isSrcOperand - A flag representing whether the string is a source operand or not.

*/
void addRegisterValueToBinaryKeyword(char *keyword, int registerValue, int isSrcOperand);

/* 
    Convert an encoding type integer flag into a binary keyword. We plant the binary representation
    in the first 2 bits of the binary string

    @param char *keyword - The destination of the binary keyword
    @param int encType - The encoding value
*/
void addEncodingTypeToBinaryKeyword(char *keyword, int encType);

/*
    Convert a string representing an immediate value, into a binary keyword

    @param char *keyword - The destination of our binary keyword
    @param char *immediateValueStr - The string representing the immediate value
*/
void convertImmediateValueToBinaryKeyword(char *keyword, char *immediateValueStr);

/*
    Received a parsed row's operand and convert it to a binary value, and add to code instructions table

    @param symbolTable *symbTable - The symbols table we're using
    @param codeInstructionsTable *codeTable - The code table, which we'll add our binary code
    @param enum LEGAL_ADDRESSING_MODES addressingMode - The addressing mode of the operand
    @param char *operand - The operand string value
*/
void addOperandInBinary(symbolTable *symbTable, codeInstructionsTable *codeTable, enum LEGAL_ADDRESSING_MODES addressingMode, char *operand, int isSrcOperand);

/*
    Convert a parsed row which is a code instruction into it's binary value, and add to
    the code table.

    @param symbolTable *symbTable - The symbols table we're using
    @param codeInstructionsTable *codeTable - The code table, which we'll add our binary code
    @param parsedRow *pr - The parsed row object pointer

*/
void addToCodeTable(symbolTable *symbTable, codeInstructionsTable *codeTable, parsedRow *pr);

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
int firstIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

/*
    Add the dataTable section that we collected from the first iteration, into the end of the code table
    
    @param dataDefinitionsTable *dataTable - A pointer to the data table
    @param codeInstructionsTable *codeTable - A pointer to the code table
*/
void addDataToCodeTable(dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable);

/*
    Iterate over the relocationTable, which contains data for labels that need to be given
    a value from the symbol table. If we locate a label in the symbol table, we plant the
    value from the symbol table in the code table

    If a label doesn't exist, we throw an error

    @param symbolTable *symbTable - Used to access the relocation table & symbol table
    @param codeInstructionsTable *codeTable - The code table which will be used to plant the new data
*/
int relocateSymbolAddresses(symbolTable *symbTable, codeInstructionsTable *codeTable);

/*
    Iterate over the list of previously parsed rows, and look for variables that were declared as .entry
    and for every one of them, set their "entry" flag in the symbol table

    @param parsedRowList *prList - The list of parsed rows
    @param symbolTable *symbTable - The symbol table used to set entry declarations
*/
int setEntrySymbols(parsedRowList *prList, symbolTable *symbTable);

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
int secondIteration(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *prList);

/*
    Run the first and second iteration, and return an indication whether we failed or succeeded

    @param char *fileName - The original filename we're working with
    @param symbolTable *symbTable - The symbols table
    @param dataDefinitionsTable *dataTable - The data table
    @param codeInstructionsTable *codeTable - The instructions code table
    @param parsedRowList *prList - The list of parsedRows

    @return int - A flag indicating if we found errors (0=False, 1=True)
*/
int run(char *fileName, symbolTable *symbTable, dataDefinitionsTable *dataTable, codeInstructionsTable *codeTable, parsedRowList *rowsList);

#endif