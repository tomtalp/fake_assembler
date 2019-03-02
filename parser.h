#ifndef PARSER_H
#define PARSER_H

#include "errors.h"

#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DECLARATION_TYPES_COUNT 2
#define OP_CODES_COUNT 16
#define MAX_INSTRUCTION_LENGTH 256 /* TODO - IS THIS OK? */
#define REGISTERS_COUNT 8
#define REGISTER_KEYWORD_LENGTH 3
#define REGISTER_OPERAND_BINARY_SIZE 5
#define ENCODING_TYPES_MAX_LENGTH 2
#define CODE_INSTRUCTION_KEYWORD_DATA_SIZE 10

#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15

enum ROW_TYPES {
    CODE_INSTRUCTION,
    DATA_DECLARATION,
    EXTERNAL_DECLARATION,
    ENTRY_DECLARATION
};

enum LEGAL_ADDRESSING_MODES {
    NO_OPERAND = 0,
    IMMEDIATE_MODE = 1,
    DIRECT_MODE = 3,
    REGISTER_MODE = 5
};

enum DATA_DEF_TYPES {
    DATA_TYPE,
    STRING_TYPE
};

enum ENCODING_TYPES {
    ABSOLUTE_TYPE = 0,
    EXTERNAL_TYPE = 1,
    RELOCATABLE_TYPE = 2
};

typedef struct dataTypeInfo {
    char *dataDefName;
    int dataDefCodeNum;
} dataTypeInfo;

typedef struct registerKeyword {
    char keyword[REGISTER_KEYWORD_LENGTH];
    int value;
} registerKeyword;

// typedef struct opCodeLegalArgs {

// };

typedef int (*opCodeValidator)(char *, int);

typedef struct opCode {
    char *opCodeName;
    int opCodeNum;
    opCodeValidator validator;
} opCode;

struct CODE_INSTRUCTION_ROW_METADATA {
    opCode oc;
    char srcOperand[MAX_SYMBOL_NAME_LENGTH];
    enum LEGAL_ADDRESSING_MODES srcOperandType;
    char destOperand[MAX_SYMBOL_NAME_LENGTH];
    enum LEGAL_ADDRESSING_MODES destOperandType;
};

struct DATA_DECLARATION_ROW_METADATA {
    enum DATA_DEF_TYPES type;
    char rawData[MAX_INSTRUCTION_LENGTH];
};

struct EXTERN_ENTRY_DECLARATION_ROW_METADATA {
    char labelName[MAX_SYMBOL_NAME_LENGTH];
};

typedef struct parsedRow {
    char fileName[FILENAME_MAX];
    int originalLineNum;
    ERROR_TYPES errorType;
    int hasSymbol;
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    enum ROW_TYPES rowType;
    union {
        struct CODE_INSTRUCTION_ROW_METADATA codeRowMetadata;
        struct DATA_DECLARATION_ROW_METADATA dataRowMetadata; 
        struct EXTERN_ENTRY_DECLARATION_ROW_METADATA externRowMetadata;
        struct EXTERN_ENTRY_DECLARATION_ROW_METADATA entryRowMetadata;
    } rowMetadata;
} parsedRow;

typedef struct parsedRowNode {
    parsedRow pr;
    struct parsedRowNode *next;
} parsedRowNode;

typedef struct parsedRowList {
    parsedRowNode *head;
    int parsedRowsCounter;
} parsedRowList;

void printParsedRow(parsedRow *pr);
void trimLeadingWhitespace(char *source);
int isSymbolDefinition(char *inputRow);
void getSymbolName(char *inputRow, char *symbolName);
int getDataDefType(char *dataDef);
void getRowType(char *inputRow, parsedRow *pr);
int operandIsRegister(char *operand);
int operandIsNumber(char *operand);
void getOperandTypes(parsedRow *pr, char *firstOperand, char *secondOperand);
void getCodeOperands(char *inputRow, parsedRow *pr);
void parseRow(char *inputRow, parsedRow *pr, int rowNum);
void initParsedRowList(parsedRowList *prList);
void addParsedRowToList(parsedRowList *prList, parsedRow *pr);
void printParsedRowsList(parsedRowList *prList);
void printParserError(parsedRow *pr);

#endif