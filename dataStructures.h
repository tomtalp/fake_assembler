#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#define KEYWORD_ENCODING_TYPE_BITS 2
#define KEYWORD_ADDRESSING_MODE_BITS 3
#define KEYWORD_OP_CODE_BITS 4

#define MAX_KEYWORD_BINARY_LENGTH 12
#define MAX_INSTRUCTIONS 1024
#define MAX_SYMBOL_NAME_LENGTH 31
#define BASE_MEM_ADDRESS 100
#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DECLARATION_TYPES_COUNT 2
#define OP_CODES_COUNT 16
#define MAX_INSTRUCTION_LENGTH 80 /* TODO - IS THIS OK? */
#define REGISTERS_COUNT 8
#define REGISTER_KEYWORD_LENGTH 3
#define REGISTER_OPERAND_BINARY_SIZE 5
#define ENCODING_TYPES_MAX_LENGTH 2
#define CODE_INSTRUCTION_KEYWORD_DATA_SIZE 10
#define FILENAME_MAX_LENGTH 80

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

/* Representation of a keyword in INT's */ 
typedef struct memKeyword {
    unsigned int encodingType : KEYWORD_ENCODING_TYPE_BITS;
    unsigned int destAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
    unsigned int opCode : KEYWORD_OP_CODE_BITS;
    unsigned int sourceAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
} memKeyword;

/* Representation of a memKeyword as a String (char array of size 12)*/
typedef struct memKeywordBinaryString{
    char keyword[MAX_KEYWORD_BINARY_LENGTH];
} memKeywordBinaryString;

/* The code instructions section - contains the actual code instructions  */
typedef struct codeInstructionsTable {
    int instructionCount;
    // memKeywordBinaryString *rows[MAX_INSTRUCTIONS];
    char *rows[MAX_INSTRUCTIONS];
} codeInstructionsTable;

/* The data definition section - contains the data counter (DC) and an array of data value
encoded in binary*/
typedef struct dataDefinitionsTables {
    int dataCounter;
    char *rows[MAX_INSTRUCTIONS];
} dataDefinitionsTables;

enum SYMBOL_TYPES {
    DATA_SYMBOL,
    INSTRUCTION_SYMBOL,
    EXTERNAL_SYMBOL,
    ENTRY_SYMBOL
};

typedef struct relocationTableNode {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    int memAddress;
    struct relocationTableNode *next;
} relocationTableNode;

typedef struct relocationTable {
    relocationTableNode *head;
    int relocationVariablesCounter;
} relocationTable;

typedef struct symbolTableNode {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    int memoryAddress;
    enum SYMBOL_TYPES symbolType;
    struct symbolTableNode *next;
} symbolTableNode;

typedef struct symbolTable {
    symbolTableNode *head;
    int symbolsCounter;
    relocationTable *relocTable;
} symbolTable;

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

typedef struct opCode {
    char *opCodeName;
    int opCodeNum;
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

typedef enum {
    NO_ERROR,
    INVALID_OPCODE_ERROR,
    SYMBOL_TOO_LONG,
    SYMBOL_STARTS_WITH_NON_ALPHA,
    SYMBOL_CONTAINS_INVALID_CHAR,
    INVALID_DATA_DEF_TYPE,
    INVALID_REGISTER_NAME,
    INVALID_LABEL_DECLARATION,
    LABEL_DECLARATION_TOO_LONG, /* TODO */
    DATA_STRING_DECLARATION_MISSING_QUOTES,
    EXTRANEOUS_TEXT_AFTER_STRING_QUOTES,
    ILLEGAL_COMMA_IN_DATA_DECLARATION,
    ILLEGAL_POSITIVITY_SIGN_IN_DATA_DECLARATION,
    ILLEGAL_DATA_DECLARATION_CHARACTER,
    ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA,
    ILLEGAL_DATA_DECLARATION_EXTRANEOUS_SPACE,
    ILLEGAL_DATA_DECLARATION_EMPTY_DATA,
    ILLEGAL_CODE_OPERANDS,
    MISSING_COMMA_BETWEEN_OPERANDS,
    MULTIPLE_COMMAS_BETWEEN_OPERANDS,
    NO_TEXT_AFTER_COMMA,
    EXTRANEOS_TEXT_AFTER_OPERANDS,
    EXTRANEOUS_TEXT_AFTER_EXTERN_ENTRY,
    UNKNOWN_ERROR,
} ERROR_TYPES;

typedef struct parsedRow {
    char fileName[FILENAME_MAX_LENGTH];
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

void initSymbolTable(symbolTable *symbTable);
void printSymbolTable(symbolTable *tb);
void addNodeToSymbolTable(symbolTable *tb, char *symbolName, int memAddress, int symbolType);
void printDataTable(dataDefinitionsTables *dataTable);
void printCodeTable(codeInstructionsTable *codeTable);
void updateSymbolTableAddresses(symbolTable *tb, int baseMemAddress, int instructionCount);
void addToRelocationsTable(relocationTable *relocTable, char *varName, int memAddress);
void printRelocTable(relocationTable *relocTable);
// int fetchFromSymbTableByName(symbolTable *symbTable, char *symbolName);
symbolTableNode *fetchFromSymbTableByName(symbolTable *tb, char *symbolName);
#endif